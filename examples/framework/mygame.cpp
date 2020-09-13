/*
Copyright (c) 2019-2020 Alexander Scholz

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
#include <vector>
#include <map>
#include "yourgame/gl_include.h"
#include "yourgame/yourgame.h"
#include "yourgame/toolbox.h"
#include "imgui.h"
#include "box2d/box2d.h"
#include "flecs.h"
extern "C"
{
#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"
}

namespace mygame
{
    namespace
    {
        void loadObj();
        void initGlScene();
        void updateImgui(const yourgame::context &ctx);

        // GL scene
        double g_fade = 0.0;
        float g_modelScale = 0.025f;
        float g_rotation = 0.01f;
        bool g_rotationOn = true;
        ImVec4 g_clearColor = ImVec4(0.4f, 0.6f, 0.8f, 1.00f);
        yourgame::Trafo g_modelTrafo;
        yourgame::Camera g_camera;
        yourgame::GLGeometry *g_geo = nullptr;
        yourgame::GLShader *g_shaderTexture = nullptr;
        yourgame::GLShader *g_shaderNormal = nullptr;
        int g_shaderToUse = 1; // 0: texture, 1: normal
        std::map<std::string, yourgame::GLTexture2D *> g_textures;

        // Flecs components
        struct flecsDistance
        {
            float s;
        };

        struct flecsVelocity
        {
            float v;
        };
    } // namespace

    void init(const yourgame::context &ctx)
    {
        g_camera.trafo()->lookAt(glm::vec3(0.0f, 2.0f, 3.0f),
                                 glm::vec3(0.0f, 0.5f, 0.0f),
                                 glm::vec3(0.0f, 1.0f, 0.0f));
        g_camera.setPerspective(75.0f, ctx.winAspectRatio, 0.1f, 10.0f);

        initGlScene();

        // arbitrary test command
        yourgame::sendCmdToEnv(1, 10, 1024, -15);

#ifdef YOURGAME_PLATFORM_ANDROID
        // arbitrary scale-up on android,
        // todo: DPI awareness
        ImGui::GetStyle().ScaleAllSizes(5.0f);
#endif
    }

    void update(const yourgame::context &ctx)
    {
        g_modelTrafo.setScaleLocal(g_modelScale);
        if (g_rotationOn)
        {
            g_modelTrafo.rotateLocal(g_rotation, yourgame::Trafo::AXIS::Y);
        }
        g_fade += (3.0 * ctx.deltaTimeS);

        if (yourgame::getInputi(yourgame::InputSource::YOURGAME_KEY_ESCAPE))
        {
            yourgame::notifyShutdown();
        }

        updateImgui(ctx);
    }

    void draw(const yourgame::context &ctx)
    {
        g_camera.setAspect(ctx.winAspectRatio);
        glViewport(0, 0, ctx.winWidth, ctx.winHeight);

        float sineFade = (float)((sin(g_fade) * 0.5) + 0.5);
        glClearColor(g_clearColor.x, g_clearColor.y, g_clearColor.z, g_clearColor.w);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // prepare draw call based on selected shader
        auto mvp = g_camera.pMat() * g_camera.vMat() * g_modelTrafo.mat();
        auto modelMat = g_modelTrafo.mat();
        auto normalMat = glm::inverseTranspose(glm::mat3(modelMat));
        if (g_shaderToUse == 1)
        {
            if (g_shaderNormal)
            {
                g_shaderNormal->useProgram();
                glUniform1f(g_shaderNormal->getUniformLocation("fade"), sineFade);
                glUniformMatrix4fv(g_shaderNormal->getUniformLocation(yourgame::unifNameMvpMatrix), 1, GL_FALSE, glm::value_ptr(mvp));
                glUniformMatrix4fv(g_shaderNormal->getUniformLocation(yourgame::unifNameModelMatrix), 1, GL_FALSE, glm::value_ptr(modelMat));
                glUniformMatrix3fv(g_shaderNormal->getUniformLocation(yourgame::unifNameNormalMatrix), 1, GL_FALSE, glm::value_ptr(normalMat));
            }
        }
        else if (g_shaderToUse == 0)
        {
            if (g_shaderTexture)
            {
                g_shaderTexture->useProgram();
                glUniform1f(g_shaderTexture->getUniformLocation("fade"), sineFade);
                glUniformMatrix4fv(g_shaderTexture->getUniformLocation(yourgame::unifNameMvpMatrix), 1, GL_FALSE, glm::value_ptr(mvp));
                glUniformMatrix4fv(g_shaderTexture->getUniformLocation(yourgame::unifNameModelMatrix), 1, GL_FALSE, glm::value_ptr(modelMat));
                glUniformMatrix3fv(g_shaderTexture->getUniformLocation(yourgame::unifNameNormalMatrix), 1, GL_FALSE, glm::value_ptr(normalMat));
            }
            g_textures.at("gradient1.png")->bind();
            g_textures.at("gradient2.jpg")->bind();
        }

        // the actual draw call
        g_geo->drawAll();
    }

    void shutdown(const yourgame::context &ctx)
    {
        if (yourgame::audioIsInitialized())
        {
            yourgame::audioShutdown();
        }
    }

    namespace
    {
        void initGlScene()
        {
            glEnable(GL_DEPTH_TEST);

            g_geo = yourgame::loadGeometry("ship_dark.obj", "ship_dark.mtl");
            g_textures.emplace("gradient1.png", yourgame::loadTexture("gradient1.png", GL_TEXTURE0));
            g_textures.emplace("gradient2.jpg", yourgame::loadTexture("gradient2.jpg", GL_TEXTURE1));

            // Texture shader
            g_shaderTexture = yourgame::loadShader(
#ifdef YOURGAME_GL_API_GLES
                {{GL_VERTEX_SHADER, "simple.es.vert"},
                 {GL_FRAGMENT_SHADER, "simple.es.frag"}},
#else
                {{GL_VERTEX_SHADER, "simple.vert"},
                 {GL_FRAGMENT_SHADER, "simple.frag"}},
#endif
                {{yourgame::attrLocPosition, yourgame::attrNamePosition},
                 {yourgame::attrLocNormal, yourgame::attrNameNormal},
                 {yourgame::attrLocTexcoords, yourgame::attrNameTexcoords}},
                {{0, "color"}});

            g_shaderTexture->useProgram();
            glUniform1i(g_shaderTexture->getUniformLocation(yourgame::unifNameTexture0), 0);
            glUniform1i(g_shaderTexture->getUniformLocation(yourgame::unifNameTexture1), 1);

            // Normal shader
            g_shaderNormal = yourgame::loadShader(
#ifdef YOURGAME_GL_API_GLES
                {{GL_VERTEX_SHADER, "simple.es.vert"},
                 {GL_FRAGMENT_SHADER, "normal.es.frag"}},
#else
                {{GL_VERTEX_SHADER, "simple.vert"},
                 {GL_FRAGMENT_SHADER, "normal.frag"}},
#endif
                {{yourgame::attrLocPosition, yourgame::attrNamePosition},
                 {yourgame::attrLocNormal, yourgame::attrNameNormal},
                 {yourgame::attrLocTexcoords, yourgame::attrNameTexcoords}},
                {{0, "color"}});
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
                ImGui::ColorPicker3("clear color", (float *)&g_clearColor);

                // rotation
                ImGui::Checkbox("", &g_rotationOn);
                ImGui::SameLine();
                ImGui::SliderFloat("rotation", &g_rotation, -0.1f, 0.1f);

                // model scale
                ImGui::InputFloat("model scale", &g_modelScale, 0.001f, 100.0f, "%.3f");

                // shader selector
                ImGui::RadioButton("texture shader", &g_shaderToUse, 0);
                ImGui::SameLine();
                ImGui::RadioButton("normal shader", &g_shaderToUse, 1);

                // camera manipulation
                static int projMode = 0;
                ImGui::RadioButton("perspective", &projMode, 0);
                ImGui::SameLine();
                ImGui::RadioButton("orthographic", &projMode, 1);

                if (projMode == 0)
                {
                    static float f1 = 75.0f;
                    static float f2 = 0.1f;
                    static float f3 = 10.0f;
                    ImGui::DragFloatRange2("zNear/zFar", &f2, &f3, 0.05f, 0.1f, 10.0f);
                    ImGui::SliderFloat("fovy", &f1, 10.0f, 180.0f);
                    g_camera.setPerspective(f1, ctx.winAspectRatio, f2, f3);
                }
                else
                {
                    static float f1 = 5.0f;
                    static float f2 = 0.0f;
                    static float f3 = 10.0f;
                    ImGui::DragFloatRange2("zNear/zFar", &f2, &f3, 0.05f, 0.0f, 10.0f);
                    ImGui::SliderFloat("height", &f1, 0.1f, 30.0f);
                    g_camera.setOrthographic(f1, ctx.winAspectRatio, f2, f3);
                }

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

                ImGui::Begin("Save file", &showSaveFile, (ImGuiWindowFlags_NoCollapse));
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

    } // namespace
} // namespace mygame
