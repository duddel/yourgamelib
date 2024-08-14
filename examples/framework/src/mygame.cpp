/*
Copyright (c) 2019-2024 Alexander Scholz

This software is provided 'as-is', without any express or implied
warranty. In no event will the authors be held liable for any damages
arising from the use of this software.

Permission is granted to anyone to use this software for any purpose,
including commercial applications, and to alter it and redistribute it
freely, subject to the following restrictions:

1. The origin of this software must not be misrepresented; you must not
   claim that you wrote the original software. If you use this software
   in a product, an acknowledgment in the product documentation would be
   appreciated but is not required.
2. Altered source versions must be plainly marked as such, and must not be
   misrepresented as being the original software.
3. This notice may not be removed or altered from any source distribution.
*/
#include <cmath>
#include <string>
#include <vector>
#include <map>
#include <memory> // unique_ptr
#include "yourgame/yourgame.h"
#include "imgui.h"
#include "box2d/box2d.h"
#include "flecs.h"
extern "C"
{
#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"
}
#include "LuaBridge/LuaBridge.h"
#include "LuaBridge/Vector.h"

namespace yg = yourgame; // convenience

namespace mygame
{
    // Flecs components
    struct flecsDistance
    {
        float s;
    };

    struct flecsVelocity
    {
        float v;
    };

    // forward declarations
    void updateImgui();

    // GL scene
    float g_modelScale = 0.025f;
    float g_rotation = 0.01f;
    bool g_rotationOn = true;
    ImVec4 g_clearColor = ImVec4(0.4f, 0.6f, 0.8f, 1.00f);
    bool g_drawSkybox = true;
    float g_skyboxRotation = 0.001f;
    bool g_skyboxRotationOn = true;
    yg::math::Trafo g_modelTrafo;
    yg::math::Trafo g_skyboxTrafo;
    yg::math::Camera g_camera;
    yg::math::Camera g_skyboxCamera;
    std::map<std::string, yg::gl::Geometry *> g_geos;
    std::string g_geoName = "ship_dark";
    float g_shaderColorMix = 0.5f;
    int g_shaderToUse = 0;     // 0: color shader, 1: normal shader
    int g_framebufDisplay = 0; // 0: default color, 1: framebuffer color 0, framebuffer depth
    yg::util::AssetManager g_assets;

    void init(int argc, char *argv[])
    {
        // load license info file
        {
#if defined(YOURGAME_PLATFORM_DESKTOP)
            static const std::string licFilename("a//LICENSE_desktop.txt");
#elif defined(YOURGAME_PLATFORM_ANDROID)
            static const std::string licFilename("a//LICENSE_android.txt");
#elif defined(YOURGAME_PLATFORM_WEB)
            static const std::string licFilename("a//LICENSE_web.txt");
#endif
            std::vector<uint8_t> licFileData;
            yg::file::readFile(licFilename, licFileData);
            std::string *licStr = new std::string(licFileData.begin(), licFileData.end());
            g_assets.insert("licenseStr", licStr);
        }

        g_camera.trafo()->lookAt(glm::vec3(0.0f, 2.0f, 3.0f),
                                 glm::vec3(0.0f, 0.5f, 0.0f),
                                 glm::vec3(0.0f, 1.0f, 0.0f));
        g_camera.setPerspective(75.0f, yg::input::get(yg::input::WINDOW_ASPECT_RATIO), 1.0f, 10.0f);
        g_skyboxCamera.setPerspective(75.0f, yg::input::get(yg::input::WINDOW_ASPECT_RATIO), 0.1f, 2.0f);

        // Skybox texture
        {
            yg::gl::TextureConfig cfg;
            cfg.unit = yg::gl::textureUnitSkyCube;
            cfg.minMagFilter = GL_LINEAR_MIPMAP_LINEAR;
            cfg.wrapMode = GL_CLAMP_TO_EDGE;
            cfg.generateMipmap = true;
            cfg.premultiplyAlpha = false;
            g_assets.insert("skybox", yg::gl::loadCubemap({"a//sky_right.png", "a//sky_left.png", "a//sky_top.png", "a//sky_bottom.png", "a//sky_front.png", "a//sky_back.png"},
                                                          cfg));
        }

        // Skybox shader
        g_assets.insert("shaderSkybox", yg::gl::loadShader(
                                            {{GL_VERTEX_SHADER, "a//skybox.vert"},
                                             {GL_FRAGMENT_SHADER, "a//skybox.frag"}},
                                            {}, {}));
        g_assets.get<yg::gl::Shader>("shaderSkybox")->useProgram();

        // Normal shader
        g_assets.insert("shaderNormal", yg::gl::loadShader(
                                            {{GL_VERTEX_SHADER, "a//simple.vert"},
                                             {GL_FRAGMENT_SHADER, "a//simplenormal.frag"}},
                                            {}, {}));

        // Color shader
        g_assets.insert("shaderColor", yg::gl::loadShader(
                                           {{GL_VERTEX_SHADER, "a//simple.vert"},
                                            {GL_FRAGMENT_SHADER, "a//simplecolor.frag"}},
                                           {}, {}));
        g_assets.get<yg::gl::Shader>("shaderColor")->useProgram();

        // Texture shader
        g_assets.insert("shaderTexture", yg::gl::loadShader(
                                             {{GL_VERTEX_SHADER, "a//simple.vert"},
                                              {GL_FRAGMENT_SHADER, "a//simpletex.frag"}},
                                             {}, {}));
        g_assets.get<yg::gl::Shader>("shaderTexture")->useProgram();

        // Post processing Depth Texture shader
        g_assets.insert("shaderTextureDepth", yg::gl::loadShader(
                                                  {{GL_VERTEX_SHADER, "a//simple.vert"},
                                                   {GL_FRAGMENT_SHADER, "a//postdepth.frag"}},
                                                  {}, {}));
        g_assets.get<yg::gl::Shader>("shaderTextureDepth")->useProgram();

        // Post processing Color0 shader
        g_assets.insert("shaderPostColor0", yg::gl::loadShader(
                                             {{GL_VERTEX_SHADER, "a//simple.vert"},
                                              {GL_FRAGMENT_SHADER, "a//postcolor0.frag"}},
                                             {}, {}));
        g_assets.get<yg::gl::Shader>("shaderPostColor0")->useProgram();

        // geometry
        g_assets.insert("quadGeo", yg::gl::loadGeometry("a//quad.obj"));
        g_assets.insert("cube", yg::gl::loadGeometry("a//cube.obj"));

        // framebuffer
        // sampling the depth texture with texture() (in glsl) is unreliable on GL ES platforms:
        // texture parameters (GL_NEAREST, GL_CLAMP_TO_EDGE) may be required to do so
        g_assets.insert("framebuf",
                        yg::gl::Framebuffer::make(yg::input::geti(yg::input::WINDOW_WIDTH),
                                                  yg::input::geti(yg::input::WINDOW_HEIGHT),
                                                  {{GL_RGBA8,
                                                    GL_RGBA,
                                                    GL_UNSIGNED_BYTE,
                                                    yg::gl::textureUnitBufferColor0,
                                                    {{GL_TEXTURE_MIN_FILTER, GL_NEAREST},
                                                     {GL_TEXTURE_MAG_FILTER, GL_NEAREST},
                                                     {GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE},
                                                     {GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE}},
                                                    GL_COLOR_ATTACHMENT0},
                                                   {GL_DEPTH_COMPONENT16,
                                                    GL_DEPTH_COMPONENT,
                                                    GL_UNSIGNED_SHORT,
                                                    yg::gl::textureUnitBufferDepth,
                                                    {{GL_TEXTURE_MIN_FILTER, GL_NEAREST},
                                                     {GL_TEXTURE_MAG_FILTER, GL_NEAREST},
                                                     {GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE},
                                                     {GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE}},
                                                    GL_DEPTH_ATTACHMENT}}));

        // arbitrary test command
        yg::control::sendCmdToEnv(1, 10, 1024, -15);

#ifdef YOURGAME_PLATFORM_ANDROID
        // arbitrary scale-up on android,
        // todo: DPI awareness
        ImGui::GetStyle().ScaleAllSizes(5.0f);
#endif
    }

    void tick()
    {
        g_modelTrafo.setScaleLocal(g_modelScale);
        if (g_rotationOn)
        {
            g_modelTrafo.rotateLocal(g_rotation, yg::math::Axis::Y);
        }

        if (g_skyboxRotationOn)
        {
            g_skyboxTrafo.rotateLocal(g_skyboxRotation, yg::math::Axis::Y);
        }

        if (yg::input::getDelta(yg::input::KEY_ESCAPE) > 0.0f)
        {
            yg::control::catchMouse(!yg::input::geti(yg::input::MOUSE_CATCHED));
        }

        // first-person camera
        if (yg::input::geti(yg::input::MOUSE_CATCHED))
        {
            g_camera.trafo()->rotateGlobal(-0.001f * yg::input::getDelta(yg::input::MOUSE_X), yg::math::Axis::Y);
            g_camera.trafo()->rotateLocal(-0.001f * yg::input::getDelta(yg::input::MOUSE_Y), yg::math::Axis::X);
            g_camera.trafo()->translateLocal(-0.01f * yg::input::get(yg::input::KEY_W), yg::math::Axis::Z);
            g_camera.trafo()->translateLocal(0.01f * yg::input::get(yg::input::KEY_S), yg::math::Axis::Z);
            g_camera.trafo()->translateLocal(0.01f * yg::input::get(yg::input::KEY_D), yg::math::Axis::X);
            g_camera.trafo()->translateLocal(-0.01f * yg::input::get(yg::input::KEY_A), yg::math::Axis::X);
        }

        g_camera.setAspect(yg::input::get(yg::input::WINDOW_ASPECT_RATIO));
        g_skyboxCamera.setAspect(yg::input::get(yg::input::WINDOW_ASPECT_RATIO));

        // prepare draw call based on selected shader
        auto mvp = g_camera.pMat() * g_camera.vMat() * g_modelTrafo.mat();
        auto modelMat = g_modelTrafo.mat();
        auto normalMat = glm::inverseTranspose(glm::mat3(modelMat));

        // load geometry on demand
        if (g_geos.find(g_geoName) == g_geos.end())
        {
            g_geos[g_geoName] = yg::gl::loadGeometry(std::string("a//") + g_geoName + ".obj",
                                                     std::string("a//") + g_geoName + ".mtl");
        }

        // drawing
        glEnable(GL_DEPTH_TEST);

        // prepare and bind framebuffer, if requested
        if (g_assets.get<yg::gl::Framebuffer>("framebuf") && (g_framebufDisplay != 0))
        {
            static uint32_t lastWinWidth = 0;
            static uint32_t lastWinHeight = 0;
            if (lastWinWidth != yg::input::geti(yg::input::WINDOW_WIDTH) ||
                lastWinHeight != yg::input::geti(yg::input::WINDOW_HEIGHT))
            {
                g_assets.get<yg::gl::Framebuffer>("framebuf")
                    ->resize(yg::input::geti(yg::input::WINDOW_WIDTH), yg::input::geti(yg::input::WINDOW_HEIGHT));
                yg::log::info("framebuffer resized to %v,%v", yg::input::geti(yg::input::WINDOW_WIDTH), yg::input::geti(yg::input::WINDOW_HEIGHT));
                lastWinWidth = yg::input::geti(yg::input::WINDOW_WIDTH);
                lastWinHeight = yg::input::geti(yg::input::WINDOW_HEIGHT);
            }
            g_assets.get<yg::gl::Framebuffer>("framebuf")->bind();
        }

        // the actual drawing
        glClearColor(g_clearColor.x, g_clearColor.y, g_clearColor.z, g_clearColor.w);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glViewport(0, 0, yg::input::geti(yg::input::WINDOW_WIDTH), yg::input::geti(yg::input::WINDOW_HEIGHT));

        // draw skybox
        if (g_drawSkybox)
        {
            yg::gl::DrawConfig cfg;
            cfg.shader = g_assets.get<yg::gl::Shader>("shaderSkybox");
            cfg.textures = {g_assets.get<yg::gl::Texture>("skybox")};
            cfg.modelMat = g_skyboxCamera.pMat() *                    // skybox camera projection
                           glm::mat4(glm::mat3(g_camera.vMat())) *    // rotation part of main camera view matrix
                           glm::mat4(glm::mat3(g_skyboxTrafo.mat())); // rotation part of skybox transformation
            glDepthMask(GL_FALSE);
            g_assets.get<yg::gl::Shader>("shaderSkybox")->useProgram();
            yg::gl::drawGeo(g_assets.get<yg::gl::Geometry>("cube"), cfg);
            glDepthMask(GL_TRUE);
        }

        // set selected shader
        if (g_shaderToUse == 0)
        {
            auto shaderColor = g_assets.get<yg::gl::Shader>("shaderColor");
            if (shaderColor)
            {
                shaderColor->useProgram();
                glUniformMatrix4fv(shaderColor->getUniformLocation(yg::gl::unifNameMvpMatrix), 1, GL_FALSE, glm::value_ptr(mvp));
                glUniformMatrix4fv(shaderColor->getUniformLocation(yg::gl::unifNameModelMatrix), 1, GL_FALSE, glm::value_ptr(modelMat));
                glUniformMatrix3fv(shaderColor->getUniformLocation(yg::gl::unifNameNormalMatrix), 1, GL_FALSE, glm::value_ptr(normalMat));
                glm::vec3 camEye = glm::vec3(g_camera.trafo()->mat()[3]);
                glUniform3fv(shaderColor->getUniformLocation(yg::gl::unifNameCameraPosition), 1, glm::value_ptr(camEye));
                auto skyRotInv = glm::transpose((glm::mat3(g_skyboxTrafo.mat())));
                glUniformMatrix3fv(shaderColor->getUniformLocation(yg::gl::unifNameSkyRotationInv), 1, GL_FALSE, glm::value_ptr(skyRotInv));
                glUniform3fv(shaderColor->getUniformLocation(yg::gl::unifNameCameraPosition), 1, glm::value_ptr(camEye));
                glUniform1f(shaderColor->getUniformLocation("colorMix"), g_shaderColorMix);
            }
        }
        else
        {
            if (g_assets.get<yg::gl::Shader>("shaderNormal"))
            {
                g_assets.get<yg::gl::Shader>("shaderNormal")->useProgram();
                glUniformMatrix4fv(g_assets.get<yg::gl::Shader>("shaderNormal")->getUniformLocation(yg::gl::unifNameMvpMatrix), 1, GL_FALSE, glm::value_ptr(mvp));
                glUniformMatrix4fv(g_assets.get<yg::gl::Shader>("shaderNormal")->getUniformLocation(yg::gl::unifNameModelMatrix), 1, GL_FALSE, glm::value_ptr(modelMat));
                glUniformMatrix3fv(g_assets.get<yg::gl::Shader>("shaderNormal")->getUniformLocation(yg::gl::unifNameNormalMatrix), 1, GL_FALSE, glm::value_ptr(normalMat));
            }
        }

        // bind skybox texture
        g_assets.get<yg::gl::Texture>("skybox")->bind();

        // draw object
        g_geos[g_geoName]->drawAll();

        // unbind framebuffer and draw framebuffer color or depth texture attachment, if requested
        if (g_assets.get<yg::gl::Framebuffer>("framebuf") && (g_framebufDisplay != 0))
        {
            g_assets.get<yg::gl::Framebuffer>("framebuf")->unbindTarget();
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            glViewport(0, 0, yg::input::geti(yg::input::WINDOW_WIDTH), yg::input::geti(yg::input::WINDOW_HEIGHT));

            // simple orthographic projection that matches the quad geometry
            auto pMat = glm::ortho(-0.5f, 0.5f, -0.5f, 0.5f, 1.0f, -1.0f);

            // render color- or depth texture attachment of the framebuffer to a quad
            if (g_framebufDisplay == 1) // color
            {
                g_assets.get<yg::gl::Shader>("shaderPostColor0")->useProgram();
                glUniformMatrix4fv(g_assets.get<yg::gl::Shader>("shaderPostColor0")->getUniformLocation(yg::gl::unifNameMvpMatrix), 1, GL_FALSE, glm::value_ptr(pMat));
                g_assets.get<yg::gl::Framebuffer>("framebuf")->textureAttachment(0)->bind();
            }
            else if (g_framebufDisplay == 2) // depth
            {
                g_assets.get<yg::gl::Shader>("shaderTextureDepth")->useProgram();
                glUniformMatrix4fv(g_assets.get<yg::gl::Shader>("shaderTextureDepth")->getUniformLocation(yg::gl::unifNameMvpMatrix), 1, GL_FALSE, glm::value_ptr(pMat));
                g_assets.get<yg::gl::Framebuffer>("framebuf")->textureAttachment(1)->bind();
            }
            g_assets.get<yg::gl::Geometry>("quadGeo")->drawAll();
        }

        // if mouse is catched, first-person camera movement is enabled
        // (see above), and imgui menu drawing is not desired
        if (!yg::input::geti(yg::input::MOUSE_CATCHED))
        {
            updateImgui();
        }
    }

    int shutdown()
    {
        if (yg::audio::isInitialized())
        {
            yg::audio::shutdown();
        }
        return 0;
    }

    void updateImgui()
    {
        static bool showDemoAudio = false;
        static bool showDemoGl = true;
        static bool showLicense = false;
        static bool showImguiDemo = false;
        static bool showBox2d = false;
        static bool showFlecs = false;
        static bool showLua = false;
        static bool showMotion = false;
        static bool showSpriteGrid = false;
        static bool showGamepadInput = false;
        static bool showLs = false;

        // Main Menu Bar
        if (ImGui::BeginMainMenuBar())
        {
            if (ImGui::BeginMenu("File"))
            {
                if (ImGui::MenuItem("Open"))
                {
                }
                if (ImGui::MenuItem("Exit"))
                {
                    yg::control::exit();
                }
                ImGui::EndMenu();
            }
            if (ImGui::BeginMenu("Demo"))
            {
                if (ImGui::MenuItem("GL", "", &showDemoGl))
                {
                    showDemoGl = true;
                }
                if (ImGui::MenuItem("Audio", "", &showDemoAudio))
                {
                    showDemoAudio = true;
                }
                if (ImGui::MenuItem("Imgui", "", &showImguiDemo))
                {
                    showImguiDemo = true;
                }
                if (ImGui::MenuItem("Box2D", "", &showBox2d))
                {
                    showBox2d = true;
                }
                if (ImGui::MenuItem("Flecs", "", &showFlecs))
                {
                    showFlecs = true;
                }
                if (ImGui::MenuItem("Lua", "", &showLua))
                {
                    showLua = true;
                }
                if (ImGui::MenuItem("Motion", "", &showMotion))
                {
                    showMotion = true;
                }
                if (ImGui::MenuItem("SpriteGrid", "", &showSpriteGrid))
                {
                    showSpriteGrid = true;
                }
                if (ImGui::MenuItem("Gamepad input", "", &showGamepadInput))
                {
                    showGamepadInput = true;
                }
                if (ImGui::MenuItem("ls", "", &showLs))
                {
                    showLs = true;
                }
                ImGui::EndMenu();
            }
            if (ImGui::BeginMenu("Options"))
            {
                if (ImGui::MenuItem("Fullscreen", "", yg::input::geti(yg::input::WINDOW_FULLSCREEN)))
                {
                    yg::control::enableFullscreen(!yg::input::geti(yg::input::WINDOW_FULLSCREEN));
                }
                if (ImGui::MenuItem("VSync", "", yg::input::geti(yg::input::VSYNC_ON)))
                {
                    yg::control::enableVSync(!yg::input::geti(yg::input::VSYNC_ON));
                }
                ImGui::EndMenu();
            }
            if (ImGui::BeginMenu("Help"))
            {
                if (ImGui::MenuItem("License"))
                {
                    showLicense = true;
                }
                ImGui::EndMenu();
            }
            // camera ray casting
            glm::vec3 org, dir;
            g_camera.castRay(yg::input::get(yg::input::MOUSE_X) / yg::input::get(yg::input::WINDOW_WIDTH),
                             yg::input::get(yg::input::MOUSE_Y) / yg::input::get(yg::input::WINDOW_HEIGHT),
                             org, dir);

            ImGui::Text("| fps: %f, mouse delta: %f,%f | camera ray: (%3f,%3f,%3f)->(%3f,%3f,%3f)",
                        (float)(1.0 / yg::time::getDelta()),
                        yg::input::getDelta(yg::input::MOUSE_X),
                        yg::input::getDelta(yg::input::MOUSE_Y),
                        org.x, org.y, org.z, dir.x, dir.y, dir.z);

            ImGui::EndMainMenuBar();
        }

        // Imgui demo window
        if (showImguiDemo)
        {
            ImGui::ShowDemoWindow(&showImguiDemo);
        }

        // GL demo window
        if (showDemoGl)
        {
            ImGui::Begin("GL", &showDemoGl, (ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_AlwaysAutoResize));
            ImGui::BeginGroup();
            ImGui::TextColored(ImVec4(0.4f, 0.6f, 0.8f, 1.0f), "skybox");
            ImGui::Checkbox("draw skybox", &g_drawSkybox);
            ImGui::SameLine();
            ImGui::Checkbox("skybox rotation on", &g_skyboxRotationOn);
            ImGui::SliderFloat("skybox rotation", &g_skyboxRotation, -0.1f, 0.1f);
            ImGui::ColorPicker3("clear color", (float *)&g_clearColor);

            // geometry
            const char *geoNames[] = {"ship_dark", "tower", "sphere"};
            static int geoId = 0;
            int numGeos = sizeof(geoNames) / sizeof(geoNames[0]);
            ImGui::ListBox("geometry", &geoId, geoNames, numGeos, numGeos);
            g_geoName = geoNames[geoId];

            ImGui::EndGroup();
            ImGui::SameLine();
            ImGui::BeginGroup();

            ImGui::TextColored(ImVec4(0.4f, 0.6f, 0.8f, 1.0f), "transform and projection");
            // model scale
            ImGui::InputFloat("model scale", &g_modelScale, 0.001f, 100.0f, "%.3f");

            // rotation
            ImGui::Checkbox("geometry rotation on", &g_rotationOn);
            ImGui::SliderFloat("geometry rotation", &g_rotation, -0.1f, 0.1f);

            // camera manipulation
            static int projMode = 0;
            ImGui::RadioButton("perspective", &projMode, 0);
            ImGui::SameLine();
            ImGui::RadioButton("orthographic", &projMode, 1);

            if (projMode == 0)
            {
                static float f1 = 75.0f;
                static float f2 = 1.0f;
                static float f3 = 10.0f;
                ImGui::DragFloatRange2("zNear/zFar", &f2, &f3, 0.05f, 0.1f, 10.0f);
                ImGui::SliderFloat("fovy", &f1, 10.0f, 180.0f);
                g_camera.setPerspective(f1, yg::input::get(yg::input::WINDOW_ASPECT_RATIO), f2, f3);
                g_skyboxCamera.setPerspective(f1, yg::input::get(yg::input::WINDOW_ASPECT_RATIO), 0.1f, 2.0f);
            }
            else
            {
                static float f1 = 5.0f;
                static float f2 = 0.0f;
                static float f3 = 10.0f;
                ImGui::DragFloatRange2("zNear/zFar", &f2, &f3, 0.05f, 0.0f, 10.0f);
                ImGui::SliderFloat("height", &f1, 0.1f, 30.0f);
                g_camera.setOrthographic(f1, yg::input::get(yg::input::WINDOW_ASPECT_RATIO), f2, f3);
                // skybox camera always perspective (see above)
            }

            // shader
            ImGui::Separator();
            ImGui::TextColored(ImVec4(0.4f, 0.6f, 0.8f, 1.0f), "shader");
            ImGui::SliderFloat("color shader mix", &g_shaderColorMix, 0.0f, 1.0f);
            ImGui::RadioButton("color shader", &g_shaderToUse, 0);
            ImGui::RadioButton("normal shader", &g_shaderToUse, 1);

            // framebuffer
            ImGui::Separator();
            ImGui::TextColored(ImVec4(0.4f, 0.6f, 0.8f, 1.0f), "framebuffer");
            ImGui::RadioButton("default color buffer", &g_framebufDisplay, 0);
            ImGui::RadioButton("framebuffer color 0", &g_framebufDisplay, 1);
            ImGui::RadioButton("framebuffer depth", &g_framebufDisplay, 2);

            ImGui::EndGroup();
            ImGui::End();

            // Audio demo window
            if (showDemoAudio)
            {
                const int numAudioSources = 5;

                if (!yg::audio::isInitialized())
                {
                    yg::audio::init(numAudioSources, 0, 0);
                    yg::audio::storeFile("a//jingles_SAX07_mono_11025.ogg");
                    yg::audio::storeFile("a//jingles_PIZZI00.ogg");
                }

                ImGui::Begin("Audio", &showDemoAudio, (ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_AlwaysAutoResize));
                if (ImGui::Button("Play"))
                {
                    yg::audio::play("a//jingles_PIZZI00.ogg");
                }
                ImGui::SameLine();

                if (ImGui::Button("Play Loop"))
                {
                    yg::audio::play("a//jingles_SAX07_mono_11025.ogg", true);
                }

                static float audioBalance[numAudioSources] = {0.0f};
                static bool audioPause[numAudioSources] = {false};
                // manipulate source
                for (int i = 0; i < numAudioSources; i++)
                {
                    // gain slider
                    ImGui::SliderFloat(("balance " + std::to_string(i)).c_str(), &audioBalance[i], -1.0f, 1.0f);
                    yg::audio::setChannelGains(i,
                                               {std::fmin(1.0f - audioBalance[i], 1.0f),
                                                std::fmin(1.0f + audioBalance[i], 1.0f)});
                    ImGui::SameLine();

                    // pause/unpause
                    ImGui::Checkbox(("pause " + std::to_string(i)).c_str(), &audioPause[i]);
                    yg::audio::pause(i, audioPause[i]);
                    ImGui::SameLine();

                    // stop
                    if (ImGui::Button(("stop " + std::to_string(i)).c_str()))
                    {
                        yg::audio::stop(i);
                    }
                }

                ImGui::End();
            }
            else if (yg::audio::isInitialized())
            {
                yg::audio::shutdown();
            }
        }

        // Box2D demo window
        static bool box2dInitialized = false;
        static b2World *box2dWorld;
        if (showBox2d)
        {
            static b2Body *box2dDynBody;

            if (!box2dInitialized)
            {
                box2dWorld = new b2World(b2Vec2(0.0f, -10.0f));

                // ground body
                b2BodyDef box2dGroundBodyDef;
                box2dGroundBodyDef.position.Set(0.0f, -10.0f);
                b2Body *box2dGroundBody = box2dWorld->CreateBody(&box2dGroundBodyDef);
                b2PolygonShape box2dGroundBodyShape;
                box2dGroundBodyShape.SetAsBox(50.0f, 9.0f);
                box2dGroundBody->CreateFixture(&box2dGroundBodyShape, 0.0f);

                // dynamic box
                b2BodyDef box2dDynBodyDef;
                box2dDynBodyDef.type = b2_dynamicBody;
                box2dDynBodyDef.position.Set(0.0f, 8.0f);
                box2dDynBody = box2dWorld->CreateBody(&box2dDynBodyDef);
                b2PolygonShape box2dDynBodyShape;
                box2dDynBodyShape.SetAsBox(1.0f, 1.0f);
                b2FixtureDef box2dDynBodyFixtureDef;
                box2dDynBodyFixtureDef.shape = &box2dDynBodyShape;
                box2dDynBodyFixtureDef.density = 1.0f;
                box2dDynBodyFixtureDef.restitution = 0.75f;
                box2dDynBody->CreateFixture(&box2dDynBodyFixtureDef);

                box2dInitialized = true;
            }

            // simulate Box2D world
            box2dWorld->Step(yg::time::getDelta(), 6, 2);

            // the vertical slider gets updated with box2dTestHeight every frame,
            // indicating the height of the dropping Box2D body
            float box2dTestHeight = (float)box2dDynBody->GetPosition().y;
            ImGui::Begin("Box2D", &showBox2d, (ImGuiWindowFlags_NoCollapse));
            ImGui::VSliderFloat("", ImVec2(50, 150), &box2dTestHeight, 0.0f, 10.0f);
            ImGui::End();
        }
        else if (box2dInitialized)
        {
            delete box2dWorld;
            box2dInitialized = false;
        }

        // Flecs demo window
        static bool flecsInitialized = false;
        static flecs::world *flecsWorld;
        if (showFlecs)
        {
            ImGui::Begin("Flecs", &showFlecs, (ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_AlwaysAutoResize));

            if (!flecsInitialized)
            {
                flecsWorld = new flecs::world();

                // register components
                flecsWorld->component<flecsDistance>();
                flecsWorld->component<flecsVelocity>();

                // add movement system
                flecsWorld->system<flecsDistance, flecsVelocity>().each([](flecs::entity e, flecsDistance &dist, flecsVelocity &vel)
                                                                        {
                                                                            float newS = dist.s + vel.v * e.delta_time();
                                                                            dist.s = newS > 10.0f ? newS - 10.0f : newS; });

                // add drawing system
                flecsWorld->system<flecsDistance>().each([](flecs::entity e, flecsDistance &dist)
                                                         {
                                                             // indicate entity components via sliders
                                                             float entityVal = dist.s;
                                                             ImGui::SliderFloat(("entity " + std::to_string(e.id())).c_str(), &entityVal, 0.0f, 10.0f); });

                // add some entities
                for (int i = 1; i < 11; i++)
                {
                    flecsWorld->entity().set<flecsDistance>({5.0}).set<flecsVelocity>({(float)i});
                }

                flecsInitialized = true;
            }

            flecsWorld->progress();

            ImGui::End();
        }
        else if (flecsInitialized)
        {
            delete flecsWorld;
            flecsInitialized = false;
        }

        // Lua demo window
        static bool luaInitialized = false;
        static lua_State *luaState;
        if (showLua)
        {
            static char luaSource[1024] =
                "function f(x)\n"
                "    return math.cos(x*2)\n"
                "end\n";

            if (!luaInitialized)
            {
                // create a Lua state and open standard libraries
                luaState = luaL_newstate();
                luaL_openlibs(luaState);
                luaL_dostring(luaState, luaSource);
                luaInitialized = true;
            }

            // input/output of Lua function f() that is called below
            static float outVal = 0.0f;
            static float inVal = 0.0f;
            inVal = inVal > 6.28318530718f ? 0.0f : inVal + 0.02f;

            // call Lua function f() and get result
            lua_getglobal(luaState, "f");
            lua_pushnumber(luaState, inVal);
            lua_call(luaState, 1, 1);
            outVal = lua_tonumber(luaState, 1);
            lua_pop(luaState, 1);

            // indicate input and output of Lua function f() with sliders
            ImGui::Begin("Lua", &showLua, (ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_AlwaysAutoResize));
            ImGui::SliderFloat("x", &inVal, 0.0f, 6.28318530718f);
            ImGui::InputTextMultiline("Lua", luaSource, IM_ARRAYSIZE(luaSource));
            ImGui::SliderFloat("f(x)", &outVal, -1.0f, 1.0f);
            if (ImGui::Button("Compile"))
            {
                luaL_dostring(luaState, luaSource);
            }
            ImGui::SameLine();
            ImGui::Text("Beware! No errors get catched!");
            ImGui::End();
        }
        else if (luaInitialized)
        {
            lua_close(luaState);
            luaInitialized = false;
        }

        // Motion demo window
        static bool motionInitialized = false;
        static std::vector<std::unique_ptr<yg::util::Motion>> *motions;

        if (showMotion)
        {
            static std::vector<std::string> motionNames = {"linear",
                                                           "SQUARE",
                                                           "CUBE",
                                                           "SQUAREDOWN",
                                                           "CUBEDOWN",
                                                           "SINE",
                                                           "SINEDOWN",
                                                           "SMOOTH",
                                                           "SMOOTH2",
                                                           "sequence"};

            if (!motionInitialized)
            {
                motions = new std::vector<std::unique_ptr<yg::util::Motion>>();

                motions->emplace_back(new yg::util::Motion(yg::util::Motion::FlowType::PINGPONG));
                motions->back()->addRamp(2.0f, -5.0f, 10.0f, yg::util::Motion::SegmentEase::NONE);

                motions->emplace_back(new yg::util::Motion(yg::util::Motion::FlowType::PINGPONG));
                motions->back()->addRamp(2.0f, -5.0f, 10.0f, yg::util::Motion::SegmentEase::SQUARE);

                motions->emplace_back(new yg::util::Motion(yg::util::Motion::FlowType::PINGPONG));
                motions->back()->addRamp(2.0f, -5.0f, 10.0f, yg::util::Motion::SegmentEase::CUBE);

                motions->emplace_back(new yg::util::Motion(yg::util::Motion::FlowType::PINGPONG));
                motions->back()->addRamp(2.0f, -5.0f, 10.0f, yg::util::Motion::SegmentEase::SQUAREDOWN);

                motions->emplace_back(new yg::util::Motion(yg::util::Motion::FlowType::PINGPONG));
                motions->back()->addRamp(2.0f, -5.0f, 10.0f, yg::util::Motion::SegmentEase::CUBEDOWN);

                motions->emplace_back(new yg::util::Motion(yg::util::Motion::FlowType::PINGPONG));
                motions->back()->addRamp(2.0f, -5.0f, 10.0f, yg::util::Motion::SegmentEase::SINE);

                motions->emplace_back(new yg::util::Motion(yg::util::Motion::FlowType::PINGPONG));
                motions->back()->addRamp(2.0f, -5.0f, 10.0f, yg::util::Motion::SegmentEase::SINEDOWN);

                motions->emplace_back(new yg::util::Motion(yg::util::Motion::FlowType::PINGPONG));
                motions->back()->addRamp(2.0f, -5.0f, 10.0f, yg::util::Motion::SegmentEase::SMOOTH);

                motions->emplace_back(new yg::util::Motion(yg::util::Motion::FlowType::PINGPONG));
                motions->back()->addRamp(2.0f, -5.0f, 10.0f, yg::util::Motion::SegmentEase::SMOOTH2);

                motions->emplace_back(new yg::util::Motion(yg::util::Motion::FlowType::ONCE));
                motions->back()->addRamp(2.0f, -5.0f, 10.0f, yg::util::Motion::SegmentEase::NONE).addIdle(0.5f, 10.0f).addRamp(1.0f, 10.0f, -5.0f, yg::util::Motion::SegmentEase::SQUAREDOWN).addIdle(0.5f, -5.0f);

                motionInitialized = true;
            }

            ImGui::Begin("Motion", &showMotion, (ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_AlwaysAutoResize));

            int mI = 0;
            for (auto &m : *motions)
            {
                ImGui::PushID(&m);
                m->tick(yg::time::getDelta());
                float val = m->val();
                ImGui::SliderFloat(motionNames.at(mI++).c_str(), &val, -5.0f, 10.0f);
                ImGui::SameLine();
                if (ImGui::Button("restart"))
                {
                    m->restart();
                }
                ImGui::SameLine();
                if (ImGui::Button("pause"))
                {
                    m->pause(!m->isPaused());
                }
                ImGui::SameLine();
                if (ImGui::Button("direction"))
                {
                    m->setDirection(!m->getDirection());
                }
                if (m->isFinished())
                {
                    ImGui::SameLine();
                    ImGui::Text("finished");
                }
                ImGui::PopID();
            }

            ImGui::End();
        }
        else if (motionInitialized)
        {
            delete motions;
            motionInitialized = false;
        }

        // SpriteGrid demo window
        static bool spriteGridInitialized = false;
        static yg::gl::Texture *spriteTexture;
        static yg::gl::SpriteGrid *spriteGrid;
        static yg::math::Trafo *spriteGridTrafo;
        if (showSpriteGrid)
        {
            static const int tilesWide = 48;  // match kenney_1bitpack_colored_packed.png
            static const int tilesHigh = 22;  // match kenney_1bitpack_colored_packed.png
            static const int tileWidth = 16;  // match kenney_1bitpack_colored_packed.png
            static const int tileHeight = 16; // match kenney_1bitpack_colored_packed.png

            static const int maxNumTiles = tilesWide * tilesHigh;
            if (!spriteGridInitialized)
            {
                yg::gl::TextureConfig cfg;
                cfg.minMagFilter = GL_NEAREST;
                spriteTexture = yg::gl::loadTexture("a//kenney_1bitpack_colored_packed.png", "", cfg);

                // ToDo: SpriteGrid could use some rework

                // for each sprite in the grid, get the coords via getGridCoords() and save them
                // back into the texture with insertCoords() with sprite index as name
                for (int h = 0; h < tilesHigh; h++)
                {
                    for (int w = 0; w < tilesWide; w++)
                    {
                        int spriteIndex = h * tilesWide + w;
                        auto coords = spriteTexture->getGridCoords(tilesWide, tilesHigh, spriteIndex);
                        spriteTexture->insertCoords(std::to_string(spriteIndex),
                                                    coords.xMinPixel,
                                                    coords.yMinPixel,
                                                    coords.xMaxPixel - coords.xMinPixel,
                                                    coords.yMaxPixel - coords.yMinPixel,
                                                    false);
                    }
                }

                spriteGrid = new yg::gl::SpriteGrid();
                spriteGridTrafo = new yg::math::Trafo();
                spriteGridInitialized = true;
            }

            static int startTile = 0;
            static int numTiles = maxNumTiles;
            static float xOffset = 0.0f;
            static float yOffset = 0.0f;
            static int texFilterLast = 0;
            static int texFilter = 0;
            static bool moveGrid = false;
            ImGui::Begin("SpriteGrid", &showSpriteGrid, (ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_AlwaysAutoResize));
            ImGui::SliderInt("start tile", &startTile, 0, maxNumTiles - 1);
            numTiles = (numTiles > maxNumTiles - startTile) ? (maxNumTiles - startTile) : numTiles;
            ImGui::SliderInt("num tiles", &numTiles, 1, maxNumTiles - startTile);
            ImGui::InputFloat("x offset 0.1", &xOffset, 0.1f, 0.5f);
            ImGui::InputFloat("y offset 0.1", &yOffset, 0.1f, 0.5f);
            ImGui::InputFloat("x offset 1.0", &xOffset, 1.0f, 1.0f);
            ImGui::InputFloat("y offset 1.0", &yOffset, 1.0f, 1.0f);
            ImGui::RadioButton("GL_NEAREST", &texFilter, 0);
            ImGui::SameLine();
            ImGui::RadioButton("GL_LINEAR", &texFilter, 1);
            ImGui::Checkbox("move", &moveGrid);
            if (moveGrid)
            {
                xOffset += 2.0f;
                yOffset -= 1.0f;
            }
            ImGui::End();

            // reconstruct the grid
            std::vector<std::string> tiles;
            for (int i = startTile; i < (startTile + numTiles); i++)
            {
                tiles.push_back(std::to_string(i));
            }
            spriteGrid->make(spriteTexture, tiles, tilesWide, static_cast<float>(spriteTexture->getWidth()), -1.0f);

            // gl drawing
            spriteGridTrafo->pointTo({0.0f + xOffset, yg::input::get(yg::input::WINDOW_HEIGHT) + yOffset, 0.0f},
                                     {0.0f + xOffset, yg::input::get(yg::input::WINDOW_HEIGHT) + yOffset, 1.0f},
                                     {0.0f, 1.0f, 0.0f});
            auto pMat = glm::ortho(0.0f, yg::input::get(yg::input::WINDOW_WIDTH), 0.0f, yg::input::get(yg::input::WINDOW_HEIGHT), 1.0f, -1.0f);
            auto mvp = pMat * spriteGridTrafo->mat();
            g_assets.get<yg::gl::Shader>("shaderTexture")->useProgram();
            glUniformMatrix4fv(g_assets.get<yg::gl::Shader>("shaderTexture")->getUniformLocation(yg::gl::unifNameMvpMatrix), 1, GL_FALSE, glm::value_ptr(mvp));
            spriteTexture->bind();
            if (texFilter != texFilterLast) // change texture mode filter if requested
            {
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, (texFilter == 0) ? GL_NEAREST : GL_LINEAR);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, (texFilter == 0) ? GL_NEAREST : GL_LINEAR);
                texFilterLast = texFilter;
            }
            spriteGrid->geo()->drawAll();
        }
        else if (spriteGridInitialized)
        {
            delete spriteTexture;
            delete spriteGrid;
            delete spriteGridTrafo;
            spriteGridInitialized = false;
        }

        // Input demo window
        if (showGamepadInput)
        {
            ImGui::Begin("Input", &showGamepadInput, (ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_AlwaysAutoResize));
            ImGui::BeginGroup();
            ImGui::Text("GAMEPAD_0");
            {
                bool connected = yg::input::get(yg::input::GAMEPAD_0_CONNECTED);
                bool button0 = yg::input::get(yg::input::GAMEPAD_0_BUTTON_A);
                bool button1 = yg::input::get(yg::input::GAMEPAD_0_BUTTON_B);
                bool button2 = yg::input::get(yg::input::GAMEPAD_0_BUTTON_X);
                bool button3 = yg::input::get(yg::input::GAMEPAD_0_BUTTON_Y);
                bool button4 = yg::input::get(yg::input::GAMEPAD_0_BUTTON_LEFT_BUMPER);
                bool button5 = yg::input::get(yg::input::GAMEPAD_0_BUTTON_RIGHT_BUMPER);
                bool button6 = yg::input::get(yg::input::GAMEPAD_0_BUTTON_BACK);
                bool button7 = yg::input::get(yg::input::GAMEPAD_0_BUTTON_START);
                bool button8 = yg::input::get(yg::input::GAMEPAD_0_BUTTON_GUIDE);
                bool button9 = yg::input::get(yg::input::GAMEPAD_0_BUTTON_LEFT_THUMB);
                bool button10 = yg::input::get(yg::input::GAMEPAD_0_BUTTON_RIGHT_THUMB);
                bool button11 = yg::input::get(yg::input::GAMEPAD_0_BUTTON_DPAD_UP);
                bool button12 = yg::input::get(yg::input::GAMEPAD_0_BUTTON_DPAD_RIGHT);
                bool button13 = yg::input::get(yg::input::GAMEPAD_0_BUTTON_DPAD_DOWN);
                bool button14 = yg::input::get(yg::input::GAMEPAD_0_BUTTON_DPAD_LEFT);
                float axis0 = yg::input::get(yg::input::GAMEPAD_0_AXIS_LEFT_X);
                float axis1 = yg::input::get(yg::input::GAMEPAD_0_AXIS_LEFT_Y);
                float axis2 = yg::input::get(yg::input::GAMEPAD_0_AXIS_RIGHT_X);
                float axis3 = yg::input::get(yg::input::GAMEPAD_0_AXIS_RIGHT_Y);
                float axis4 = yg::input::get(yg::input::GAMEPAD_0_AXIS_LEFT_TRIGGER);
                float axis5 = yg::input::get(yg::input::GAMEPAD_0_AXIS_RIGHT_TRIGGER);
                ImGui::Checkbox("CONNECTED_0", &connected);
                ImGui::Checkbox("A_0", &button0);
                ImGui::Checkbox("B_0", &button1);
                ImGui::Checkbox("X_0", &button2);
                ImGui::Checkbox("Y_0", &button3);
                ImGui::Checkbox("LEFT_BUMPER_0", &button4);
                ImGui::Checkbox("RIGHT_BUMPER_0", &button5);
                ImGui::Checkbox("BACK_0", &button6);
                ImGui::Checkbox("START_0", &button7);
                ImGui::Checkbox("GUIDE_0", &button8);
                ImGui::Checkbox("LEFT_THUMB_0", &button9);
                ImGui::Checkbox("RIGHT_THUMB_0", &button10);
                ImGui::Checkbox("DPAD_UP_0", &button11);
                ImGui::Checkbox("DPAD_RIGHT_0", &button12);
                ImGui::Checkbox("DPAD_DOWN_0", &button13);
                ImGui::Checkbox("DPAD_LEFT_0", &button14);
                ImGui::SliderFloat("LEFT_X_0", &axis0, -1.0f, 1.0f);
                ImGui::SliderFloat("LEFT_Y_0", &axis1, -1.0f, 1.0f);
                ImGui::SliderFloat("RIGHT_X_0", &axis2, -1.0f, 1.0f);
                ImGui::SliderFloat("RIGHT_Y_0", &axis3, -1.0f, 1.0f);
                ImGui::SliderFloat("LEFT_TRIGGER_0", &axis4, -1.0f, 1.0f);
                ImGui::SliderFloat("RIGHT_TRIGGER_0", &axis5, -1.0f, 1.0f);
            }
            ImGui::EndGroup();
            ImGui::SameLine();
            ImGui::BeginGroup();
            ImGui::Text("GAMEPAD_1");
            {
                bool connected = yg::input::get(yg::input::GAMEPAD_1_CONNECTED);
                bool button0 = yg::input::get(yg::input::GAMEPAD_1_BUTTON_A);
                bool button1 = yg::input::get(yg::input::GAMEPAD_1_BUTTON_B);
                bool button2 = yg::input::get(yg::input::GAMEPAD_1_BUTTON_X);
                bool button3 = yg::input::get(yg::input::GAMEPAD_1_BUTTON_Y);
                bool button4 = yg::input::get(yg::input::GAMEPAD_1_BUTTON_LEFT_BUMPER);
                bool button5 = yg::input::get(yg::input::GAMEPAD_1_BUTTON_RIGHT_BUMPER);
                bool button6 = yg::input::get(yg::input::GAMEPAD_1_BUTTON_BACK);
                bool button7 = yg::input::get(yg::input::GAMEPAD_1_BUTTON_START);
                bool button8 = yg::input::get(yg::input::GAMEPAD_1_BUTTON_GUIDE);
                bool button9 = yg::input::get(yg::input::GAMEPAD_1_BUTTON_LEFT_THUMB);
                bool button10 = yg::input::get(yg::input::GAMEPAD_1_BUTTON_RIGHT_THUMB);
                bool button11 = yg::input::get(yg::input::GAMEPAD_1_BUTTON_DPAD_UP);
                bool button12 = yg::input::get(yg::input::GAMEPAD_1_BUTTON_DPAD_RIGHT);
                bool button13 = yg::input::get(yg::input::GAMEPAD_1_BUTTON_DPAD_DOWN);
                bool button14 = yg::input::get(yg::input::GAMEPAD_1_BUTTON_DPAD_LEFT);
                float axis0 = yg::input::get(yg::input::GAMEPAD_1_AXIS_LEFT_X);
                float axis1 = yg::input::get(yg::input::GAMEPAD_1_AXIS_LEFT_Y);
                float axis2 = yg::input::get(yg::input::GAMEPAD_1_AXIS_RIGHT_X);
                float axis3 = yg::input::get(yg::input::GAMEPAD_1_AXIS_RIGHT_Y);
                float axis4 = yg::input::get(yg::input::GAMEPAD_1_AXIS_LEFT_TRIGGER);
                float axis5 = yg::input::get(yg::input::GAMEPAD_1_AXIS_RIGHT_TRIGGER);
                ImGui::Checkbox("CONNECTED_1", &connected);
                ImGui::Checkbox("A_1", &button0);
                ImGui::Checkbox("B_1", &button1);
                ImGui::Checkbox("X_1", &button2);
                ImGui::Checkbox("Y_1", &button3);
                ImGui::Checkbox("LEFT_BUMPER_1", &button4);
                ImGui::Checkbox("RIGHT_BUMPER_1", &button5);
                ImGui::Checkbox("BACK_1", &button6);
                ImGui::Checkbox("START_1", &button7);
                ImGui::Checkbox("GUIDE_1", &button8);
                ImGui::Checkbox("LEFT_THUMB_1", &button9);
                ImGui::Checkbox("RIGHT_THUMB_1", &button10);
                ImGui::Checkbox("DPAD_UP_1", &button11);
                ImGui::Checkbox("DPAD_RIGHT_1", &button12);
                ImGui::Checkbox("DPAD_DOWN_1", &button13);
                ImGui::Checkbox("DPAD_LEFT_1", &button14);
                ImGui::SliderFloat("LEFT_X_1", &axis0, -1.0f, 1.0f);
                ImGui::SliderFloat("LEFT_Y_1", &axis1, -1.0f, 1.0f);
                ImGui::SliderFloat("RIGHT_X_1", &axis2, -1.0f, 1.0f);
                ImGui::SliderFloat("RIGHT_Y_1", &axis3, -1.0f, 1.0f);
                ImGui::SliderFloat("LEFT_TRIGGER_1", &axis4, -1.0f, 1.0f);
                ImGui::SliderFloat("RIGHT_TRIGGER_1", &axis5, -1.0f, 1.0f);
            }
            ImGui::EndGroup();
            ImGui::End();
        }

        if (showLs)
        {
            static char lsPattern[256] = "a//*.png";
            static std::string lsResultText("");

            ImGui::Begin("ls", &showLs, (ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_AlwaysAutoResize));
            ImGui::InputText("ls pattern", lsPattern, 256);
            if (ImGui::Button("call ls()"))
            {
                lsResultText.clear();
                auto lsResult = yg::file::ls(lsPattern);
                yg::log::info("ls(\"%v\"): %v results:", lsPattern, lsResult.size());

                for (const auto &s : lsResult)
                {
                    yg::log::info("%v", s);
                    lsResultText += (s + "\n");
                }
            }
            ImGui::Text("%s", lsResultText.c_str());
            ImGui::End();
        }

        // license window
        if (showLicense)
        {
            ImGui::SetNextWindowSizeConstraints(ImVec2(yg::input::get(yg::input::WINDOW_WIDTH) * 0.5f,
                                                       yg::input::get(yg::input::WINDOW_HEIGHT) * 0.5f),
                                                ImVec2(yg::input::get(yg::input::WINDOW_WIDTH),
                                                       yg::input::get(yg::input::WINDOW_HEIGHT)));
            ImGui::Begin("License", &showLicense, (ImGuiWindowFlags_NoCollapse));
            /* The following procedure allows displaying long wrapped text,
                whereas ImGui::TextWrapped() has a size limit and cuts the content. */
            ImGui::PushTextWrapPos(0.0f);
            ImGui::TextUnformatted(g_assets.get<std::string>("licenseStr")->c_str());
            ImGui::PopTextWrapPos();
            ImGui::End();
        }
    }
} // namespace mygame
