/*
Copyright (c) 2019-2021 Alexander Scholz

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
#include "yourgame/gl_include.h"
#include "yourgame/yourgame.h"
#include "yourgame/toolbox.h"
#include "imgui.h"
#include "box2d/box2d.h"
#include "flecs.h"
#include "choreograph/Choreograph.h"
#include "q3.h"
extern "C"
{
#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"
}

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
    void updateImgui(const yourgame::context &ctx);

    // GL scene
    float g_modelScale = 0.025f;
    float g_rotation = 0.01f;
    bool g_rotationOn = true;
    ImVec4 g_clearColor = ImVec4(0.4f, 0.6f, 0.8f, 1.00f);
    bool g_drawSkybox = true;
    float g_skyboxRotation = 0.001f;
    bool g_skyboxRotationOn = true;
    yourgame::Trafo g_modelTrafo;
    yourgame::Trafo g_skyboxTrafo;
    yourgame::Camera g_camera;
    yourgame::Camera g_skyboxCamera;
    std::map<std::string, yourgame::GLGeometry *> g_geos;
    std::string g_geoName = "ship_dark";
    float g_shaderColorMix = 0.5f;
    int g_shaderToUse = 0;     // 0: color shader, 1: normal shader
    int g_framebufDisplay = 0; // 0: default color, 1: framebuffer color 0, framebuffer depth
    yourgame::AssetManager g_assets;

    void init(const yourgame::context &ctx)
    {
        g_camera.trafo()->lookAt(glm::vec3(0.0f, 2.0f, 3.0f),
                                 glm::vec3(0.0f, 0.5f, 0.0f),
                                 glm::vec3(0.0f, 1.0f, 0.0f));
        g_camera.setPerspective(75.0f, ctx.winAspectRatio, 1.0f, 10.0f);
        g_skyboxCamera.setPerspective(75.0f, ctx.winAspectRatio, 0.1f, 2.0f);

        // Skybox
        g_assets.insert("skybox",
                        yourgame::loadCubemap(
                            {"sky_right.png", "sky_left.png", "sky_top.png", "sky_bottom.png", "sky_front.png", "sky_back.png"},
                            yourgame::textureUnitSky,
                            {{GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR},
                             {GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR},
                             {GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE},
                             {GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE},
                             {GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE}},
                            true));

        g_assets.insert("cube",
                        yourgame::loadGeometry("cube.obj", nullptr));

        // Skybox shader
        g_assets.insert("shaderSkybox",
                        yourgame::loadShader(
#ifdef YOURGAME_GL_API_GLES
                            {{GL_VERTEX_SHADER, "skybox.es.vert"},
                             {GL_FRAGMENT_SHADER, "skybox.es.frag"}},
#else
                            {{GL_VERTEX_SHADER, "skybox.vert"},
                             {GL_FRAGMENT_SHADER, "skybox.frag"}},
#endif
                            {{yourgame::attrLocPosition, yourgame::attrNamePosition}},
                            {{0, "color"}}));
        g_assets.get<yourgame::GLShader>("shaderSkybox")->useProgram();
        glUniform1i(g_assets.get<yourgame::GLShader>("shaderSkybox")->getUniformLocation(yourgame::unifNameTextureSky),
                    yourgame::unifValueTextureSky);

        // Normal shader
        g_assets.insert("shaderNormal",
                        yourgame::loadShader(
#ifdef YOURGAME_GL_API_GLES
                            {{GL_VERTEX_SHADER, "simple.es.vert"},
                             {GL_FRAGMENT_SHADER, "simplenormal.es.frag"}},
#else
                            {{GL_VERTEX_SHADER, "simple.vert"},
                             {GL_FRAGMENT_SHADER, "simplenormal.frag"}},
#endif
                            {{yourgame::attrLocPosition, yourgame::attrNamePosition},
                             {yourgame::attrLocNormal, yourgame::attrNameNormal}},
                            {{0, "color"}}));

        // Color shader
        g_assets.insert("shaderColor",
                        yourgame::loadShader(
#ifdef YOURGAME_GL_API_GLES
                            {{GL_VERTEX_SHADER, "simple.es.vert"},
                             {GL_FRAGMENT_SHADER, "simplecolor.es.frag"}},
#else
                            {{GL_VERTEX_SHADER, "simple.vert"},
                             {GL_FRAGMENT_SHADER, "simplecolor.frag"}},
#endif
                            {{yourgame::attrLocPosition, yourgame::attrNamePosition},
                             {yourgame::attrLocColor, yourgame::attrNameColor}},
                            {{0, "color"}}));
        g_assets.get<yourgame::GLShader>("shaderColor")->useProgram();
        glUniform1i(g_assets.get<yourgame::GLShader>("shaderColor")->getUniformLocation(yourgame::unifNameTextureSky),
                    yourgame::unifValueTextureSky);

        // Texture shader
        g_assets.insert("shaderTexture",
                        yourgame::loadShader(
#ifdef YOURGAME_GL_API_GLES
                            {{GL_VERTEX_SHADER, "simple.es.vert"},
                             {GL_FRAGMENT_SHADER, "simpletex.es.frag"}},
#else
                            {{GL_VERTEX_SHADER, "simple.vert"},
                             {GL_FRAGMENT_SHADER, "simpletex.frag"}},
#endif
                            {{yourgame::attrLocPosition, yourgame::attrNamePosition},
                             {yourgame::attrLocTexcoords, yourgame::attrNameTexcoords}},
                            {{0, "color"}}));
        g_assets.get<yourgame::GLShader>("shaderTexture")->useProgram();
        glUniform1i(g_assets.get<yourgame::GLShader>("shaderTexture")->getUniformLocation(yourgame::unifNameTextureDiffuse),
                    yourgame::unifValueTextureDiffuse);

        // Depth Texture shader
        g_assets.insert("shaderTextureDepth",
                        yourgame::loadShader(
#ifdef YOURGAME_GL_API_GLES
                            {{GL_VERTEX_SHADER, "simple.es.vert"},
                             {GL_FRAGMENT_SHADER, "simpletexdepth.es.frag"}},
#else
                            {{GL_VERTEX_SHADER, "simple.vert"},
                             {GL_FRAGMENT_SHADER, "simpletexdepth.frag"}},
#endif
                            {{yourgame::attrLocPosition, yourgame::attrNamePosition},
                             {yourgame::attrLocTexcoords, yourgame::attrNameTexcoords}},
                            {{0, "color"}}));
        g_assets.get<yourgame::GLShader>("shaderTextureDepth")->useProgram();
        glUniform1i(g_assets.get<yourgame::GLShader>("shaderTextureDepth")->getUniformLocation(yourgame::unifNameTextureDiffuse),
                    yourgame::unifValueTextureDiffuse);

        // quad geometry
        g_assets.insert("quadGeo", yourgame::loadGeometry("quad.obj", nullptr));

        // framebuffer
        // sampling the depth texture with texture() (in glsl) is unreliable on GL ES platforms:
        // texture parameters (GL_NEAREST, GL_CLAMP_TO_EDGE) may be required to do so
        g_assets.insert("framebuf",
                        yourgame::GLFramebuffer::make(ctx.winWidth, ctx.winHeight,
                                                      {{GL_RGBA8,
                                                        GL_RGBA,
                                                        GL_UNSIGNED_BYTE,
                                                        yourgame::textureUnitDiffuse,
                                                        {{GL_TEXTURE_MIN_FILTER, GL_NEAREST},
                                                         {GL_TEXTURE_MAG_FILTER, GL_NEAREST},
                                                         {GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE},
                                                         {GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE}},
                                                        GL_COLOR_ATTACHMENT0},
                                                       {GL_DEPTH_COMPONENT16,
                                                        GL_DEPTH_COMPONENT,
                                                        GL_UNSIGNED_SHORT,
                                                        yourgame::textureUnitDiffuse,
                                                        {{GL_TEXTURE_MIN_FILTER, GL_NEAREST},
                                                         {GL_TEXTURE_MAG_FILTER, GL_NEAREST},
                                                         {GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE},
                                                         {GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE}},
                                                        GL_DEPTH_ATTACHMENT}}));

        // arbitrary test command
        yourgame::sendCmdToEnv(1, 10, 1024, -15);

#ifdef YOURGAME_PLATFORM_ANDROID
        // arbitrary scale-up on android,
        // todo: DPI awareness
        ImGui::GetStyle().ScaleAllSizes(5.0f);
#endif
    }

    void tick(const yourgame::context &ctx)
    {
        g_modelTrafo.setScaleLocal(g_modelScale);
        if (g_rotationOn)
        {
            g_modelTrafo.rotateLocal(g_rotation, yourgame::Trafo::AXIS::Y);
        }

        if (g_skyboxRotationOn)
        {
            g_skyboxTrafo.rotateLocal(g_skyboxRotation, yourgame::Trafo::AXIS::Y);
        }

        if (yourgame::getInputi(yourgame::InputSource::YOURGAME_KEY_ESCAPE))
        {
            yourgame::notifyShutdown();
        }

        g_camera.setAspect(ctx.winAspectRatio);
        g_skyboxCamera.setAspect(ctx.winAspectRatio);

        // prepare draw call based on selected shader
        auto mvp = g_camera.pMat() * g_camera.vMat() * g_modelTrafo.mat();
        auto modelMat = g_modelTrafo.mat();
        auto normalMat = glm::inverseTranspose(glm::mat3(modelMat));

        // load geometry on demand
        if (g_geos.find(g_geoName) == g_geos.end())
        {
            g_geos[g_geoName] = yourgame::loadGeometry(std::string(g_geoName + ".obj").c_str(), std::string(g_geoName + ".mtl").c_str());
        }

        // drawing
        glEnable(GL_DEPTH_TEST);

        // prepare and bind framebuffer, if requested
        if (g_assets.get<yourgame::GLFramebuffer>("framebuf") && (g_framebufDisplay != 0))
        {
            static uint32_t lastWinWidth = 0;
            static uint32_t lastWinHeight = 0;
            if (lastWinWidth != ctx.winWidth || lastWinHeight != ctx.winHeight)
            {
                g_assets.get<yourgame::GLFramebuffer>("framebuf")->resize(ctx.winWidth, ctx.winHeight);
                yourgame::logi("framebuffer resized to %v,%v", ctx.winWidth, ctx.winHeight);
                lastWinWidth = ctx.winWidth;
                lastWinHeight = ctx.winHeight;
            }
            g_assets.get<yourgame::GLFramebuffer>("framebuf")->bind();
        }

        // the actual drawing
        glClearColor(g_clearColor.x, g_clearColor.y, g_clearColor.z, g_clearColor.w);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glViewport(0, 0, ctx.winWidth, ctx.winHeight);

        // draw skybox
        if (g_drawSkybox)
        {
            g_assets.get<yourgame::GLShader>("shaderSkybox")->useProgram();
            auto mvpSky = g_skyboxCamera.pMat() *                    // skybox camera projection
                          glm::mat4(glm::mat3(g_camera.vMat())) *    // rotation part of main camera view matrix
                          glm::mat4(glm::mat3(g_skyboxTrafo.mat())); // rotation part of skybox transformation
            glUniformMatrix4fv(g_assets.get<yourgame::GLShader>("shaderSkybox")->getUniformLocation(yourgame::unifNameMvpMatrix), 1, GL_FALSE, glm::value_ptr(mvpSky));
            g_assets.get<yourgame::GLTexture2D>("skybox")->bind();
            glDepthMask(GL_FALSE);
            g_assets.get<yourgame::GLGeometry>("cube")->drawAll();
            glDepthMask(GL_TRUE);
        }

        // set selected shader
        if (g_shaderToUse == 0)
        {
            auto shaderColor = g_assets.get<yourgame::GLShader>("shaderColor");
            if (shaderColor)
            {
                shaderColor->useProgram();
                glUniformMatrix4fv(shaderColor->getUniformLocation(yourgame::unifNameMvpMatrix), 1, GL_FALSE, glm::value_ptr(mvp));
                glUniformMatrix4fv(shaderColor->getUniformLocation(yourgame::unifNameModelMatrix), 1, GL_FALSE, glm::value_ptr(modelMat));
                glUniformMatrix3fv(shaderColor->getUniformLocation(yourgame::unifNameNormalMatrix), 1, GL_FALSE, glm::value_ptr(normalMat));
                glm::vec3 camEye = glm::vec3(g_camera.trafo()->mat()[3]);
                glUniform3fv(shaderColor->getUniformLocation(yourgame::unifNameCameraPosition), 1, glm::value_ptr(camEye));
                auto skyRotInv = glm::transpose((glm::mat3(g_skyboxTrafo.mat())));
                glUniformMatrix3fv(shaderColor->getUniformLocation(yourgame::unifNameSkyRotationInv), 1, GL_FALSE, glm::value_ptr(skyRotInv));
                glUniform3fv(shaderColor->getUniformLocation(yourgame::unifNameCameraPosition), 1, glm::value_ptr(camEye));
                glUniform1f(shaderColor->getUniformLocation("colorMix"), g_shaderColorMix);
            }
        }
        else
        {
            if (g_assets.get<yourgame::GLShader>("shaderNormal"))
            {
                g_assets.get<yourgame::GLShader>("shaderNormal")->useProgram();
                glUniformMatrix4fv(g_assets.get<yourgame::GLShader>("shaderNormal")->getUniformLocation(yourgame::unifNameMvpMatrix), 1, GL_FALSE, glm::value_ptr(mvp));
                glUniformMatrix4fv(g_assets.get<yourgame::GLShader>("shaderNormal")->getUniformLocation(yourgame::unifNameModelMatrix), 1, GL_FALSE, glm::value_ptr(modelMat));
                glUniformMatrix3fv(g_assets.get<yourgame::GLShader>("shaderNormal")->getUniformLocation(yourgame::unifNameNormalMatrix), 1, GL_FALSE, glm::value_ptr(normalMat));
            }
        }

        // bind skybox texture
        g_assets.get<yourgame::GLTexture2D>("skybox")->bind();

        // draw object
        g_geos[g_geoName]->drawAll();

        // unbind framebuffer and draw framebuffer color or depth texture attachment, if requested
        if (g_assets.get<yourgame::GLFramebuffer>("framebuf") && (g_framebufDisplay != 0))
        {
            g_assets.get<yourgame::GLFramebuffer>("framebuf")->unbindTarget();
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            glViewport(0, 0, ctx.winWidth, ctx.winHeight);

            // simple orthographic projection that matches the quad geometry
            auto pMat = glm::ortho(-0.5f, 0.5f, -0.5f, 0.5f, 1.0f, -1.0f);

            // render color- or depth texture attachment of the framebuffer to a quad
            if (g_framebufDisplay == 1) // color
            {
                g_assets.get<yourgame::GLShader>("shaderTexture")->useProgram();
                glUniformMatrix4fv(g_assets.get<yourgame::GLShader>("shaderTexture")->getUniformLocation(yourgame::unifNameMvpMatrix), 1, GL_FALSE, glm::value_ptr(pMat));
                g_assets.get<yourgame::GLFramebuffer>("framebuf")->textureAttachment(0)->bind();
            }
            else if (g_framebufDisplay == 2) // depth
            {
                g_assets.get<yourgame::GLShader>("shaderTextureDepth")->useProgram();
                glUniformMatrix4fv(g_assets.get<yourgame::GLShader>("shaderTextureDepth")->getUniformLocation(yourgame::unifNameMvpMatrix), 1, GL_FALSE, glm::value_ptr(pMat));
                g_assets.get<yourgame::GLFramebuffer>("framebuf")->textureAttachment(1)->bind();
            }
            g_assets.get<yourgame::GLGeometry>("quadGeo")->drawAll();
        }

        updateImgui(ctx);
    }

    void shutdown(const yourgame::context &ctx)
    {
        if (yourgame::audioIsInitialized())
        {
            yourgame::audioShutdown();
        }
    }

    void updateImgui(const yourgame::context &ctx)
    {
        static bool showDemoAudio = false;
        static bool showDemoGl = true;
        static bool showLicense = false;
        static bool showImguiDemo = false;
        static bool showSaveFile = false;
        static bool showBox2d = false;
        static bool showFlecs = false;
        static bool showLua = false;
        static bool showChoreograph = false;
        static bool showSpriteGrid = false;
        static bool showQu3e = false;
        static bool showGamepadInput = false;

        // Main Menu Bar
        if (ImGui::BeginMainMenuBar())
        {
            if (ImGui::BeginMenu("File"))
            {
                if (ImGui::MenuItem("Exit"))
                {
                    yourgame::notifyShutdown();
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
                if (ImGui::MenuItem("Save file", "", &showSaveFile))
                {
                    showSaveFile = true;
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
                if (ImGui::MenuItem("Choreograph", "", &showChoreograph))
                {
                    showChoreograph = true;
                }
                if (ImGui::MenuItem("SpriteGrid", "", &showSpriteGrid))
                {
                    showSpriteGrid = true;
                }
                if (ImGui::MenuItem("qu3e", "", &showQu3e))
                {
                    showQu3e = true;
                }
                if (ImGui::MenuItem("Gamepad input", "", &showGamepadInput))
                {
                    showGamepadInput = true;
                }
                ImGui::EndMenu();
            }
            if (ImGui::BeginMenu("Options"))
            {
                if (ImGui::MenuItem("Fullscreen", "", yourgame::getCtx().winIsFullscreen))
                {
                    yourgame::enableFullscreen(!yourgame::getCtx().winIsFullscreen);
                }
                if (ImGui::MenuItem("VSync", "", yourgame::getCtx().vsyncEnabled))
                {
                    yourgame::enableVSync(!yourgame::getCtx().vsyncEnabled);
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
            ImGui::Text("| fps: %f", (float)(1.0 / yourgame::getCtx().deltaTimeS));
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
                g_camera.setPerspective(f1, ctx.winAspectRatio, f2, f3);
                g_skyboxCamera.setPerspective(f1, ctx.winAspectRatio, 0.1f, 2.0f);
            }
            else
            {
                static float f1 = 5.0f;
                static float f2 = 0.0f;
                static float f3 = 10.0f;
                ImGui::DragFloatRange2("zNear/zFar", &f2, &f3, 0.05f, 0.0f, 10.0f);
                ImGui::SliderFloat("height", &f1, 0.1f, 30.0f);
                g_camera.setOrthographic(f1, ctx.winAspectRatio, f2, f3);
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

                if (!yourgame::audioIsInitialized())
                {
                    yourgame::audioInit(2, 44100, numAudioSources);
                    yourgame::audioStoreFile("jingles_SAX07_mono_11025.ogg");
                    yourgame::audioStoreFile("jingles_PIZZI00.ogg");
                }

                ImGui::Begin("Audio", &showDemoAudio, (ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_AlwaysAutoResize));
                if (ImGui::Button("Play"))
                {
                    yourgame::audioPlay("jingles_PIZZI00.ogg");
                }
                ImGui::SameLine();

                if (ImGui::Button("Play Loop"))
                {
                    yourgame::audioPlay("jingles_SAX07_mono_11025.ogg", true);
                }

                static float audioBalance[numAudioSources] = {0.0f};
                static bool audioPause[numAudioSources] = {false};
                // manipulate source
                for (int i = 0; i < numAudioSources; i++)
                {
                    // gain slider
                    ImGui::SliderFloat(("balance " + std::to_string(i)).c_str(), &audioBalance[i], -1.0f, 1.0f);
                    yourgame::audioSetChannelGains(i,
                                                   {std::fmin(1.0f - audioBalance[i], 1.0f),
                                                    std::fmin(1.0f + audioBalance[i], 1.0f)});
                    ImGui::SameLine();

                    // pause/unpause
                    ImGui::Checkbox(("pause " + std::to_string(i)).c_str(), &audioPause[i]);
                    yourgame::audioPause(i, audioPause[i]);
                    ImGui::SameLine();

                    // stop
                    if (ImGui::Button(("stop " + std::to_string(i)).c_str()))
                    {
                        yourgame::audioStop(i);
                    }
                }

                ImGui::End();
            }
            else if (yourgame::audioIsInitialized())
            {
                yourgame::audioShutdown();
            }
        }

        // save file demo window
        if (showSaveFile)
        {
            static int testValue = 0;

            ImGui::Begin("Save file", &showSaveFile, (ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_AlwaysAutoResize));
            if (ImGui::Button("load"))
            {
                std::vector<uint8_t> fileData;
                // try to read save file
                if (!yourgame::readSaveFile("testValue.bin", fileData))
                {
                    testValue = *((int *)fileData.data());
                }
            }
            ImGui::SameLine();

            if (ImGui::Button("save"))
            {
                yourgame::writeSaveFile("testValue.bin", &testValue, sizeof(testValue));
            }

            ImGui::SliderInt("persistent integer", &testValue, -100, 100);
            ImGui::End();
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
            box2dWorld->Step(ctx.deltaTimeS, 6, 2);

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
                flecsWorld->system<flecsDistance, flecsVelocity>().each([](flecs::entity e, flecsDistance &dist, flecsVelocity &vel) {
                    float newS = dist.s + vel.v * e.delta_time();
                    dist.s = newS > 10.0f ? newS - 10.0f : newS;
                });

                // add drawing system
                flecsWorld->system<flecsDistance>().each([](flecs::entity e, flecsDistance &dist) {
                    // indicate entity components via sliders
                    float entityVal = dist.s;
                    ImGui::SliderFloat(("entity " + std::to_string(e.id())).c_str(), &entityVal, 0.0f, 10.0f);
                });

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

        // Choreograph demo window
        static bool choreographInitialized = false;
        static choreograph::Timeline *choreoTimeline;
        static choreograph::Output<float> *choreoTarget1;
        static choreograph::Output<float> *choreoTarget2;
        static choreograph::Output<float> *choreoTarget3;
        if (showChoreograph)
        {
            if (!choreographInitialized)
            {
                choreoTimeline = new choreograph::Timeline();
                choreoTarget1 = new choreograph::Output<float>();
                choreoTarget2 = new choreograph::Output<float>();
                choreoTarget3 = new choreograph::Output<float>();
                choreograph::PhraseRef<float> choreoPhrase1 = choreograph::makeRamp<float>(-5.0f, 10.0f, 2.0, choreograph::EaseNone());
                choreograph::PhraseRef<float> choreoPhrase2 = choreograph::makeRamp<float>(-5.0f, 10.0f, 2.0, choreograph::EaseInOutQuad());
                choreograph::PhraseRef<float> choreoPhrase3 = choreograph::makeRamp<float>(-5.0f, 10.0f, 2.0, choreograph::EaseInOutExpo());
                choreoTimeline->apply(choreoTarget1).then(choreograph::makePingPong(choreoPhrase1, 10.0f));
                choreoTimeline->apply(choreoTarget2).then(choreograph::makePingPong(choreoPhrase2, 10.0f));
                choreoTimeline->apply(choreoTarget3).then(choreograph::makePingPong(choreoPhrase3, 10.0f));

                choreographInitialized = true;
            }

            choreoTimeline->step(ctx.deltaTimeS);

            ImGui::Begin("Choreograph", &showChoreograph, (ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_AlwaysAutoResize));
            ImGui::SliderFloat("linear", choreoTarget1->valuePtr(), -5.0f, 10.0f);
            ImGui::SliderFloat("quad", choreoTarget2->valuePtr(), -5.0f, 10.0f);
            ImGui::SliderFloat("expo", choreoTarget3->valuePtr(), -5.0f, 10.0f);
            ImGui::End();
        }
        else if (choreographInitialized)
        {
            delete choreoTimeline;
            delete choreoTarget1;
            delete choreoTarget2;
            delete choreoTarget3;
            choreographInitialized = false;
        }

        // SpriteGrid demo window
        static bool spriteGridInitialized = false;
        static yourgame::GLTextureAtlas *spriteGridAtlas;
        static yourgame::GLSpriteGrid *spriteGrid;
        static yourgame::Trafo *spriteGridTrafo;
        if (showSpriteGrid)
        {
            static const int tilesWide = 48; // match kenney_1bitpack_colored_packed.png
            static const int tilesHigh = 22; // match kenney_1bitpack_colored_packed.png
            static const int maxNumTiles = tilesWide * tilesHigh;
            if (!spriteGridInitialized)
            {
                spriteGridAtlas = yourgame::loadTextureAtlasGrid("kenney_1bitpack_colored_packed.png",
                                                                 tilesWide,
                                                                 tilesHigh,
                                                                 yourgame::textureUnitDiffuse,
                                                                 GL_NEAREST,
                                                                 false);
                spriteGrid = new yourgame::GLSpriteGrid();
                spriteGridTrafo = new yourgame::Trafo();
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
            ImGui::InputFloat("x offset 0.1", &xOffset, 0.1f, 0.5f, 3);
            ImGui::InputFloat("y offset 0.1", &yOffset, 0.1f, 0.5f, 3);
            ImGui::InputFloat("x offset 1.0", &xOffset, 1.0f, 1.0f, 3);
            ImGui::InputFloat("y offset 1.0", &yOffset, 1.0f, 1.0f, 3);
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
            spriteGrid->make(spriteGridAtlas, tiles, tilesWide, (float)(spriteGridAtlas->texture(0)->width()), -1.0f);

            // gl drawing
            spriteGridTrafo->pointTo({0.0f + xOffset, (float)ctx.winHeight + yOffset, 0.0f},
                                     {0.0f + xOffset, (float)ctx.winHeight + yOffset, 1.0f},
                                     {0.0f, 1.0f, 0.0f});
            auto pMat = glm::ortho(0.0f, (float)ctx.winWidth, 0.0f, (float)ctx.winHeight, 1.0f, -1.0f);
            auto mvp = pMat * spriteGridTrafo->mat();
            g_assets.get<yourgame::GLShader>("shaderTexture")->useProgram();
            glUniformMatrix4fv(g_assets.get<yourgame::GLShader>("shaderTexture")->getUniformLocation(yourgame::unifNameMvpMatrix), 1, GL_FALSE, glm::value_ptr(mvp));
            spriteGridAtlas->texture(0)->bind();
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
            delete spriteGridAtlas;
            delete spriteGrid;
            delete spriteGridTrafo;
            spriteGridInitialized = false;
        }

        // qu3e demo window
        static bool qu3eInitialized = false;
        static q3Scene *qu3eScene;
        if (showQu3e)
        {
            static q3Body *qu3eBodyGround;
            static q3Body *qu3eBodyDynamic;
            static float gravity = -9.81f;

            if (!qu3eInitialized)
            {
                qu3eScene = new q3Scene(1.0 / 60.0);

                // static body with 2 boxes: bottom and top
                q3BodyDef groundDef;
                groundDef.bodyType = eStaticBody;
                qu3eBodyGround = qu3eScene->CreateBody(groundDef);
                q3BoxDef groundBoxDef;
                q3Transform localSpace;
                q3Identity(localSpace);
                localSpace.position.Set(0.0, -5.0, 0.0);
                groundBoxDef.Set(localSpace, q3Vec3(10.0, 10.0, 10.0));
                qu3eBodyGround->AddBox(groundBoxDef);
                localSpace.position.Set(0.0, 15.0, 0.0);
                groundBoxDef.Set(localSpace, q3Vec3(10.0, 10.0, 10.0));
                qu3eBodyGround->AddBox(groundBoxDef);

                // dynamic body: move between static boxes
                q3BodyDef dynDef;
                dynDef.bodyType = eDynamicBody;
                dynDef.allowSleep = false;
                dynDef.lockAxisX = true; // lock rotations
                dynDef.lockAxisY = true;
                dynDef.lockAxisZ = true;
                dynDef.position.Set(0.0, 10.0, 0.0);
                qu3eBodyDynamic = qu3eScene->CreateBody(dynDef);
                q3BoxDef dynBoxDef;
                dynBoxDef.SetRestitution(0.5);
                q3Identity(localSpace);
                localSpace.position.Set(0.0, 0.0, 0.0);
                dynBoxDef.Set(localSpace, q3Vec3(1.0, 1.0, 1.0));
                qu3eBodyDynamic->AddBox(dynBoxDef);

                qu3eInitialized = true;
            }

            // todo: the dynamic body falls asleep even though allowSleep was set to false
            qu3eBodyDynamic->SetToAwake();

            qu3eScene->SetGravity({0.0f, gravity, 0.0f});

            // step qu3e scene
            qu3eScene->Step();

            // the vertical slider gets updated with qu3eTestHeight every frame,
            // indicating the height of the dropping qu3e body
            float qu3eTestHeight = (float)(qu3eBodyDynamic->GetTransform().position.y);
            ImGui::Begin("qu3e", &showQu3e, (ImGuiWindowFlags_NoCollapse));
            ImGui::VSliderFloat("box", ImVec2(50, 150), &qu3eTestHeight, 0.0f, 10.0f);
            ImGui::SameLine();
            ImGui::VSliderFloat("gravity", ImVec2(50, 150), &gravity, -20.0f, 20.0f);
            ImGui::End();
        }
        else if (qu3eInitialized)
        {
            delete qu3eScene;
            qu3eInitialized = false;
        }

        // Input demo window
        if (showGamepadInput)
        {
            ImGui::Begin("Input", &showGamepadInput, (ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_AlwaysAutoResize));
            ImGui::BeginGroup();
            ImGui::Text("GAMEPAD_0");
            {
                bool connected = yourgame::getInputi(yourgame::InputSource::YOURGAME_GAMEPAD_0_CONNECTED);
                bool button0 = yourgame::getInputi(yourgame::InputSource::YOURGAME_GAMEPAD_0_BUTTON_A);
                bool button1 = yourgame::getInputi(yourgame::InputSource::YOURGAME_GAMEPAD_0_BUTTON_B);
                bool button2 = yourgame::getInputi(yourgame::InputSource::YOURGAME_GAMEPAD_0_BUTTON_X);
                bool button3 = yourgame::getInputi(yourgame::InputSource::YOURGAME_GAMEPAD_0_BUTTON_Y);
                bool button4 = yourgame::getInputi(yourgame::InputSource::YOURGAME_GAMEPAD_0_BUTTON_LEFT_BUMPER);
                bool button5 = yourgame::getInputi(yourgame::InputSource::YOURGAME_GAMEPAD_0_BUTTON_RIGHT_BUMPER);
                bool button6 = yourgame::getInputi(yourgame::InputSource::YOURGAME_GAMEPAD_0_BUTTON_BACK);
                bool button7 = yourgame::getInputi(yourgame::InputSource::YOURGAME_GAMEPAD_0_BUTTON_START);
                bool button8 = yourgame::getInputi(yourgame::InputSource::YOURGAME_GAMEPAD_0_BUTTON_GUIDE);
                bool button9 = yourgame::getInputi(yourgame::InputSource::YOURGAME_GAMEPAD_0_BUTTON_LEFT_THUMB);
                bool button10 = yourgame::getInputi(yourgame::InputSource::YOURGAME_GAMEPAD_0_BUTTON_RIGHT_THUMB);
                bool button11 = yourgame::getInputi(yourgame::InputSource::YOURGAME_GAMEPAD_0_BUTTON_DPAD_UP);
                bool button12 = yourgame::getInputi(yourgame::InputSource::YOURGAME_GAMEPAD_0_BUTTON_DPAD_RIGHT);
                bool button13 = yourgame::getInputi(yourgame::InputSource::YOURGAME_GAMEPAD_0_BUTTON_DPAD_DOWN);
                bool button14 = yourgame::getInputi(yourgame::InputSource::YOURGAME_GAMEPAD_0_BUTTON_DPAD_LEFT);
                float axis0 = yourgame::getInputf(yourgame::InputSource::YOURGAME_GAMEPAD_0_AXIS_LEFT_X);
                float axis1 = yourgame::getInputf(yourgame::InputSource::YOURGAME_GAMEPAD_0_AXIS_LEFT_Y);
                float axis2 = yourgame::getInputf(yourgame::InputSource::YOURGAME_GAMEPAD_0_AXIS_RIGHT_X);
                float axis3 = yourgame::getInputf(yourgame::InputSource::YOURGAME_GAMEPAD_0_AXIS_RIGHT_Y);
                float axis4 = yourgame::getInputf(yourgame::InputSource::YOURGAME_GAMEPAD_0_AXIS_LEFT_TRIGGER);
                float axis5 = yourgame::getInputf(yourgame::InputSource::YOURGAME_GAMEPAD_0_AXIS_RIGHT_TRIGGER);
                ImGui::Checkbox("CONNECTED", &connected);
                ImGui::Checkbox("A", &button0);
                ImGui::Checkbox("B", &button1);
                ImGui::Checkbox("X", &button2);
                ImGui::Checkbox("Y", &button3);
                ImGui::Checkbox("LEFT_BUMPER", &button4);
                ImGui::Checkbox("RIGHT_BUMPER", &button5);
                ImGui::Checkbox("BACK", &button6);
                ImGui::Checkbox("START", &button7);
                ImGui::Checkbox("GUIDE", &button8);
                ImGui::Checkbox("LEFT_THUMB", &button9);
                ImGui::Checkbox("RIGHT_THUMB", &button10);
                ImGui::Checkbox("DPAD_UP", &button11);
                ImGui::Checkbox("DPAD_RIGHT", &button12);
                ImGui::Checkbox("DPAD_DOWN", &button13);
                ImGui::Checkbox("DPAD_LEFT", &button14);
                ImGui::SliderFloat("LEFT_X", &axis0, -1.0f, 1.0f);
                ImGui::SliderFloat("LEFT_Y", &axis1, -1.0f, 1.0f);
                ImGui::SliderFloat("RIGHT_X", &axis2, -1.0f, 1.0f);
                ImGui::SliderFloat("RIGHT_Y", &axis3, -1.0f, 1.0f);
                ImGui::SliderFloat("LEFT_TRIGGER", &axis4, -1.0f, 1.0f);
                ImGui::SliderFloat("RIGHT_TRIGGER", &axis5, -1.0f, 1.0f);
            }
            ImGui::EndGroup();
            ImGui::SameLine();
            ImGui::BeginGroup();
            ImGui::Text("GAMEPAD_1");
            {
                bool connected = yourgame::getInputi(yourgame::InputSource::YOURGAME_GAMEPAD_1_CONNECTED);
                bool button0 = yourgame::getInputi(yourgame::InputSource::YOURGAME_GAMEPAD_1_BUTTON_A);
                bool button1 = yourgame::getInputi(yourgame::InputSource::YOURGAME_GAMEPAD_1_BUTTON_B);
                bool button2 = yourgame::getInputi(yourgame::InputSource::YOURGAME_GAMEPAD_1_BUTTON_X);
                bool button3 = yourgame::getInputi(yourgame::InputSource::YOURGAME_GAMEPAD_1_BUTTON_Y);
                bool button4 = yourgame::getInputi(yourgame::InputSource::YOURGAME_GAMEPAD_1_BUTTON_LEFT_BUMPER);
                bool button5 = yourgame::getInputi(yourgame::InputSource::YOURGAME_GAMEPAD_1_BUTTON_RIGHT_BUMPER);
                bool button6 = yourgame::getInputi(yourgame::InputSource::YOURGAME_GAMEPAD_1_BUTTON_BACK);
                bool button7 = yourgame::getInputi(yourgame::InputSource::YOURGAME_GAMEPAD_1_BUTTON_START);
                bool button8 = yourgame::getInputi(yourgame::InputSource::YOURGAME_GAMEPAD_1_BUTTON_GUIDE);
                bool button9 = yourgame::getInputi(yourgame::InputSource::YOURGAME_GAMEPAD_1_BUTTON_LEFT_THUMB);
                bool button10 = yourgame::getInputi(yourgame::InputSource::YOURGAME_GAMEPAD_1_BUTTON_RIGHT_THUMB);
                bool button11 = yourgame::getInputi(yourgame::InputSource::YOURGAME_GAMEPAD_1_BUTTON_DPAD_UP);
                bool button12 = yourgame::getInputi(yourgame::InputSource::YOURGAME_GAMEPAD_1_BUTTON_DPAD_RIGHT);
                bool button13 = yourgame::getInputi(yourgame::InputSource::YOURGAME_GAMEPAD_1_BUTTON_DPAD_DOWN);
                bool button14 = yourgame::getInputi(yourgame::InputSource::YOURGAME_GAMEPAD_1_BUTTON_DPAD_LEFT);
                float axis0 = yourgame::getInputf(yourgame::InputSource::YOURGAME_GAMEPAD_1_AXIS_LEFT_X);
                float axis1 = yourgame::getInputf(yourgame::InputSource::YOURGAME_GAMEPAD_1_AXIS_LEFT_Y);
                float axis2 = yourgame::getInputf(yourgame::InputSource::YOURGAME_GAMEPAD_1_AXIS_RIGHT_X);
                float axis3 = yourgame::getInputf(yourgame::InputSource::YOURGAME_GAMEPAD_1_AXIS_RIGHT_Y);
                float axis4 = yourgame::getInputf(yourgame::InputSource::YOURGAME_GAMEPAD_1_AXIS_LEFT_TRIGGER);
                float axis5 = yourgame::getInputf(yourgame::InputSource::YOURGAME_GAMEPAD_1_AXIS_RIGHT_TRIGGER);
                ImGui::Checkbox("CONNECTED", &connected);
                ImGui::Checkbox("A", &button0);
                ImGui::Checkbox("B", &button1);
                ImGui::Checkbox("X", &button2);
                ImGui::Checkbox("Y", &button3);
                ImGui::Checkbox("LEFT_BUMPER", &button4);
                ImGui::Checkbox("RIGHT_BUMPER", &button5);
                ImGui::Checkbox("BACK", &button6);
                ImGui::Checkbox("START", &button7);
                ImGui::Checkbox("GUIDE", &button8);
                ImGui::Checkbox("LEFT_THUMB", &button9);
                ImGui::Checkbox("RIGHT_THUMB", &button10);
                ImGui::Checkbox("DPAD_UP", &button11);
                ImGui::Checkbox("DPAD_RIGHT", &button12);
                ImGui::Checkbox("DPAD_DOWN", &button13);
                ImGui::Checkbox("DPAD_LEFT", &button14);
                ImGui::SliderFloat("LEFT_X", &axis0, -1.0f, 1.0f);
                ImGui::SliderFloat("LEFT_Y", &axis1, -1.0f, 1.0f);
                ImGui::SliderFloat("RIGHT_X", &axis2, -1.0f, 1.0f);
                ImGui::SliderFloat("RIGHT_Y", &axis3, -1.0f, 1.0f);
                ImGui::SliderFloat("LEFT_TRIGGER", &axis4, -1.0f, 1.0f);
                ImGui::SliderFloat("RIGHT_TRIGGER", &axis5, -1.0f, 1.0f);
            }
            ImGui::EndGroup();
            ImGui::End();
        }

        // license window
        if (showLicense)
        {
#if defined(YOURGAME_PLATFORM_DESKTOP)
            static const char licFilename[] = "LICENSE_desktop.txt";
#elif defined(YOURGAME_PLATFORM_ANDROID)
            static const char licFilename[] = "LICENSE_android.txt";
#elif defined(YOURGAME_PLATFORM_WEB)
            static const char licFilename[] = "LICENSE_web.txt";
#endif

            // todo: do not load license on every frame!
            std::vector<uint8_t> licFileData;
            yourgame::readAssetFile(licFilename, licFileData);
            std::string licStr(licFileData.begin(), licFileData.end());
            ImGui::SetNextWindowSizeConstraints(ImVec2((float)(ctx.winWidth) * 0.5f,
                                                       (float)(ctx.winHeight) * 0.5f),
                                                ImVec2((float)(ctx.winWidth),
                                                       (float)(ctx.winHeight)));
            ImGui::Begin("License", &showLicense, (ImGuiWindowFlags_NoCollapse));
            /* The following procedure allows displaying long wrapped text,
                whereas ImGui::TextWrapped() has a size limit and cuts the content. */
            ImGui::PushTextWrapPos(0.0f);
            ImGui::TextUnformatted(licStr.c_str());
            ImGui::PopTextWrapPos();
            ImGui::End();
        }
    }
} // namespace mygame
