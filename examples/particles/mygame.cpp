#include <map>
#include <string>
#include <vector>
#include "yourgame/yourgame.h"
#include "yourgame/toolbox.h"
#include "imgui.h"

namespace yg = yourgame; // convenience

namespace mygame
{
    yg::AssetManager g_assets;
    yg::Camera g_camera;

    void init()
    {
        auto ctx = yg::getCtx();

        g_camera.trafo()->lookAt(glm::vec3(7.35889f, 4.95831f, 6.92579f),
                                 glm::vec3(0.0f, 4.0f, 0.0f),
                                 glm::vec3(0.0f, 1.0f, 0.0f));
        g_camera.setPerspective(65.0f, ctx.winAspectRatio, 0.2f, 100.0f);

        // load license info file
        {
            std::vector<uint8_t> licFileData;
            yg::readFile("a//LICENSE_web.txt", licFileData);
            std::string *licStr = new std::string(licFileData.begin(), licFileData.end());
            g_assets.insert("licenseStr", licStr);
        }

        // particles
        yg::Particles::Config partCfg;
        g_assets.insert("particles", yg::GLParticles::make(partCfg, yg::loadGeometry("a//quad.obj")));

        // geometry
        g_assets.insert("geoGrid", yg::loadGeometry("a//grid.obj"));

        // shaders
        g_assets.insert("shaderSimpleColor", yg::loadShader({{GL_VERTEX_SHADER, "a//default.vert"},
                                                             {GL_FRAGMENT_SHADER, "a//simplecolor.frag"}}));
        g_assets.insert("shaderParticleColorFade", yg::loadShader({{GL_VERTEX_SHADER, "a//particle_colorfade.vert"},
                                                                   {GL_FRAGMENT_SHADER, "a//simplecolor.frag"}}));

        glEnable(GL_DEPTH_TEST);
        //yg::enableVSync(true);
        //yg::enableFullscreen(true);
#ifndef __EMSCRIPTEN__ // todo: we can not initially catch the mouse if the viewport does not have focus (web) \
                       //yg::catchMouse(true);
#endif
    }

    void tick()
    {
        auto ctx = yg::getCtx();
        auto parts = g_assets.get<yg::GLParticles>("particles");

        ImGui::Begin("Particles", nullptr, (ImGuiWindowFlags_AlwaysAutoResize));
        static int preset = 0;
        static int presetLast = -1;
        ImGui::Combo("Preset", &preset, "fountain (default)\0snow\0glitter\0sparkler\0\0");
        ImGui::Checkbox("scatterOnSpawn", &parts->m_cfg.scatterOnSpawn);
        ImGui::Checkbox("respawn", &parts->m_cfg.respawn);
        ImGui::SliderFloat("origin.x", &parts->m_cfg.origin.x, -10.0f, 10.0f);
        ImGui::SliderFloat("origin.y", &parts->m_cfg.origin.y, -10.0f, 10.0f);
        ImGui::SliderFloat("origin.z", &parts->m_cfg.origin.z, -10.0f, 10.0f);
        ImGui::SliderFloat("emitterA.x", &parts->m_cfg.emitterA.x, 0.0f, 10.0f);
        ImGui::SliderFloat("emitterB.y", &parts->m_cfg.emitterB.y, 0.0f, 10.0f);
        ImGui::SliderFloat("emitterC.z", &parts->m_cfg.emitterC.z, 0.0f, 10.0f);
        ImGui::SliderFloat("baseDirection.x", &parts->m_cfg.baseDirection.x, -10.0f, 10.0f);
        ImGui::SliderFloat("baseDirection.y", &parts->m_cfg.baseDirection.y, -10.0f, 10.0f);
        ImGui::SliderFloat("baseDirection.z", &parts->m_cfg.baseDirection.z, -10.0f, 10.0f);
        ImGui::SliderFloat("noisDirection.x", &parts->m_cfg.noisDirection.x, -10.0f, 10.0f);
        ImGui::SliderFloat("noisDirection.y", &parts->m_cfg.noisDirection.y, -10.0f, 10.0f);
        ImGui::SliderFloat("noisDirection.z", &parts->m_cfg.noisDirection.z, -10.0f, 10.0f);
        ImGui::SliderFloat("baseVelocity", &parts->m_cfg.baseVelocity, 0.0f, 10.0f);
        ImGui::SliderFloat("noisVelocity", &parts->m_cfg.noisVelocity, 0.0f, 10.0f);
        ImGui::SliderFloat("baseLifetime", &parts->m_cfg.baseLifetime, 0.0f, 10.0f);
        ImGui::SliderFloat("noisLifetime", &parts->m_cfg.noisLifetime, 0.0f, 10.0f);
        ImGui::Separator();
        static bool drawGrid = true;
        ImGui::Checkbox("Draw grid", &drawGrid);
        static float particlesTickRate = 1.0f;
        ImGui::SliderFloat("Tick rate", &particlesTickRate, 0.0f, 1.0f);
        static ImVec4 clearColor = ImVec4(0.05f, 0.05f, 0.1f, 1.0f);
        ImGui::ColorPicker3("Clear color", (float *)&clearColor, ImGuiColorEditFlags_NoInputs);
        ImGui::End();

        glClearColor(clearColor.x, clearColor.y, clearColor.z, clearColor.w);

        if (preset != presetLast)
        {
            auto shdrPartsColorFade = g_assets.get<yg::GLShader>("shaderParticleColorFade");
            shdrPartsColorFade->useProgram();
            std::array<GLfloat, 5 * 3> colors; // num colors == 5, fixed in shader
            switch (preset)
            {
            case 0: // default
                parts->m_cfg = yg::Particles::Config();
                parts->m_cfg.count = 1000;
                parts->reset();
                colors = {1.0f, 1.0f, 0.75f,
                          1.0f, 1.0f, 0.5f,
                          1.0f, 0.75f, 0.0f,
                          1.0f, 0.5f, 0.0f,
                          1.0f, 0.25f, 0.0f};
                break;
            case 1: // snow
                parts->m_cfg = yg::Particles::Config();
                parts->m_cfg.count = 1000;
                parts->m_cfg.origin.y = 10.0f;
                parts->m_cfg.emitterA.x = 5.0f;
                parts->m_cfg.emitterC.z = 5.0f;
                parts->m_cfg.baseDirection.y = -1.0f;
                parts->m_cfg.noisDirection = {0.35f, 0.0f, 0.35f};
                parts->m_cfg.baseVelocity = 5.0f;
                parts->m_cfg.noisVelocity = 2.0f;
                parts->m_cfg.baseLifetime = 2.0f;
                parts->m_cfg.noisLifetime = 0.0f;
                parts->reset();
                colors = {1.0f, 1.0f, 1.0f,
                          1.0f, 1.0f, 1.0f,
                          1.0f, 1.0f, 1.0f,
                          1.0f, 1.0f, 1.0f,
                          1.0f, 1.0f, 1.0f};
                break;
            case 2: // glitter
                parts->m_cfg = yg::Particles::Config();
                parts->m_cfg.count = 1000;
                parts->m_cfg.scatterOnSpawn = true;
                parts->m_cfg.origin = {0.0f, 5.0f, 0.0f};
                parts->m_cfg.emitterA.x = 5.0f;
                parts->m_cfg.emitterB.y = 5.0f;
                parts->m_cfg.emitterC.z = 5.0f;
                parts->m_cfg.baseDirection = {0.0f, 0.0f, 0.0f};
                parts->m_cfg.noisDirection = {0.5f, 0.5f, 0.5f};
                parts->m_cfg.baseVelocity = 0.3f;
                parts->m_cfg.noisVelocity = 0.1f;
                parts->m_cfg.baseLifetime = 0.35f;
                parts->m_cfg.noisLifetime = 0.0f;
                parts->reset();
                colors = {1.0f, 0.0f, 1.0f,
                          1.0f, 0.0f, 1.0f,
                          0.5f, 0.2f, 0.75f,
                          0.25f, 0.25f, 1.0f,
                          1.0f, 0.85f, 0.0f};
                break;
            case 3: // sparkler
                parts->m_cfg = yg::Particles::Config();
                parts->m_cfg.count = 1000;
                parts->m_cfg.origin = {0.0f, 5.0f, 0.0f};
                parts->m_cfg.baseDirection = {0.0f, 0.0f, 0.0f};
                parts->m_cfg.noisDirection = {0.5f, 0.5f, 0.5f};
                parts->m_cfg.baseVelocity = 3.5f;
                parts->m_cfg.noisVelocity = 0.25f;
                parts->m_cfg.baseLifetime = 0.35f;
                parts->m_cfg.noisLifetime = 0.15f;
                parts->reset();
                colors = {1.0f, 1.0f, 0.75f,
                          1.0f, 1.0f, 0.5f,
                          1.0f, 0.75f, 0.0f,
                          1.0f, 0.5f, 0.0f,
                          1.0f, 0.25f, 0.0f};
                break;
            }
            presetLast = preset;
            glUniform3fv(shdrPartsColorFade->getUniformLocation("fadeColors"), colors.size(), (GLfloat *)colors.data());
        }

        // lock/release mouse
        if (yg::getInputDelta(yg::InputSource::YOURGAME_KEY_M) > 0.0f)
        {
            yg::catchMouse(!ctx.mouseCatched);
        }

        // exit
        if (yg::getInput(yg::InputSource::YOURGAME_KEY_ESCAPE))
        {
            yg::notifyShutdown();
        }

        g_camera.setAspect(ctx.winAspectRatio);
        glViewport(0, 0, ctx.winWidth, ctx.winHeight);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        static bool showLicenseWindow = false;
        if (ImGui::BeginMainMenuBar())
        {
            if (ImGui::BeginMenu("Help"))
            {
                if (ImGui::MenuItem("License"))
                {
                    showLicenseWindow = true;
                }
                ImGui::EndMenu();
            }

            ImGui::Text("| mouse delta: %f,%f, fps: %f",
                        yg::getInputDelta(yg::InputSource::YOURGAME_MOUSE_X),
                        yg::getInputDelta(yg::InputSource::YOURGAME_MOUSE_Y),
                        (float)(1.0 / ctx.deltaTimeS));
            ImGui::EndMainMenuBar();
        }

        if (showLicenseWindow)
        {
            ImGui::SetNextWindowSizeConstraints(ImVec2((float)(ctx.winWidth) * 0.5f,
                                                       (float)(ctx.winHeight) * 0.5f),
                                                ImVec2((float)(ctx.winWidth) * 0.8f,
                                                       (float)(ctx.winHeight) * 0.8f));
            ImGui::Begin("License", &showLicenseWindow, (ImGuiWindowFlags_NoCollapse));
            /* The following procedure allows displaying long wrapped text,
               whereas ImGui::TextWrapped() has a size limit and cuts the content. */
            ImGui::PushTextWrapPos(0.0f);
            ImGui::TextUnformatted(g_assets.get<std::string>("licenseStr")->c_str());
            ImGui::PopTextWrapPos();
            ImGui::End();
        }

        // first-person camera
        if (ctx.mouseCatched)
        {
            g_camera.trafo()->rotateGlobal(-0.002f * yg::getInputDelta(yg::InputSource::YOURGAME_MOUSE_X), yg::Trafo::AXIS::Y);
            g_camera.trafo()->rotateLocal(-0.002f * yg::getInputDelta(yg::InputSource::YOURGAME_MOUSE_Y), yg::Trafo::AXIS::X);
            g_camera.trafo()->rotateGlobal(-0.001f * yg::getInputDelta(yg::InputSource::YOURGAME_TOUCH_0_X), yg::Trafo::AXIS::Y);
            g_camera.trafo()->rotateLocal(-0.001f * yg::getInputDelta(yg::InputSource::YOURGAME_TOUCH_0_Y), yg::Trafo::AXIS::X);
        }
        g_camera.trafo()->rotateGlobal(static_cast<float>(ctx.deltaTimeS) * 1.0f * yg::getInput(yg::InputSource::YOURGAME_KEY_LEFT), yg::Trafo::AXIS::Y);
        g_camera.trafo()->rotateGlobal(static_cast<float>(ctx.deltaTimeS) * -1.0f * yg::getInput(yg::InputSource::YOURGAME_KEY_RIGHT), yg::Trafo::AXIS::Y);
        g_camera.trafo()->rotateLocal(static_cast<float>(ctx.deltaTimeS) * 1.0f * yg::getInput(yg::InputSource::YOURGAME_KEY_UP), yg::Trafo::AXIS::X);
        g_camera.trafo()->rotateLocal(static_cast<float>(ctx.deltaTimeS) * -1.0f * yg::getInput(yg::InputSource::YOURGAME_KEY_DOWN), yg::Trafo::AXIS::X);
        g_camera.trafo()->translateLocal(-0.01f * yg::getInput(yg::InputSource::YOURGAME_KEY_W), yg::Trafo::AXIS::Z);
        g_camera.trafo()->translateLocal(0.01f * yg::getInput(yg::InputSource::YOURGAME_KEY_S), yg::Trafo::AXIS::Z);
        g_camera.trafo()->translateLocal(0.01f * yg::getInput(yg::InputSource::YOURGAME_KEY_D), yg::Trafo::AXIS::X);
        g_camera.trafo()->translateLocal(-0.01f * yg::getInput(yg::InputSource::YOURGAME_KEY_A), yg::Trafo::AXIS::X);

        // draw grid
        if (drawGrid)
        {
            auto shdrSimpleColor = g_assets.get<yg::GLShader>("shaderSimpleColor");
            shdrSimpleColor->useProgram();
            yg::DrawConfig cfg;
            cfg.shader = shdrSimpleColor;
            cfg.camera = &g_camera;
            yg::drawGeo(g_assets.get<yg::GLGeometry>("geoGrid"), cfg);
        }

        // draw particles
        {
            yg::GLParticles *parts = g_assets.get<yg::GLParticles>("particles");
            parts->tick(static_cast<float>(ctx.deltaTimeS) * particlesTickRate);
            // model matrix: scaling and billboard-like rotation (from camera trafo)
            auto modelMat = glm::mat4(glm::mat3(g_camera.trafo()->mat()) * glm::mat3(glm::scale(glm::vec3(0.04f))));
            auto shdrPartsColorFade = g_assets.get<yg::GLShader>("shaderParticleColorFade");
            shdrPartsColorFade->useProgram(nullptr, &g_camera);
            yg::DrawConfig cfg;
            cfg.shader = shdrPartsColorFade;
            cfg.modelMat = modelMat;
            cfg.camera = &g_camera;
            cfg.instancecount = parts->numParticles();
            yg::drawGeo(parts->geo(), cfg);
        }
    }

    void shutdown()
    {
        g_assets.clear();
    }
} // namespace mygame
