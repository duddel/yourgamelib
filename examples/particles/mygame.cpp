#include <map>
#include <string>
#include <vector>
#include "yourgame/yourgame.h"
#include "imgui.h"

namespace yg = yourgame; // convenience

namespace mygame
{
    yg::util::AssetManager g_assets;
    yg::math::Camera g_camera;

    void init(int argc, char *argv[])
    {
        g_camera.trafo()->lookAt(glm::vec3(7.35889f, 4.95831f, 6.92579f),
                                 glm::vec3(0.0f, 4.0f, 0.0f),
                                 glm::vec3(0.0f, 1.0f, 0.0f));
        g_camera.setPerspective(65.0f, yg::input::get(yg::input::WINDOW_ASPECT_RATIO), 0.2f, 100.0f);

        // load license info file
        {
            std::vector<uint8_t> licFileData;
            yg::file::readFile("a//LICENSE_web.txt", licFileData);
            std::string *licStr = new std::string(licFileData.begin(), licFileData.end());
            g_assets.insert("licenseStr", licStr);
        }

        // particles
        yg::math::Particles::Config partCfg;
        g_assets.insert("particles", yg::gl::Particles::make(partCfg, yg::gl::loadGeometry("a//quad.obj")));

        // geometry
        g_assets.insert("geoGrid", yg::gl::loadGeometry("a//grid.obj"));

        // shaders
        g_assets.insert("shaderSimpleColor", yg::gl::loadShader({{GL_VERTEX_SHADER, "a//default.vert"},
                                                                 {GL_FRAGMENT_SHADER, "a//simplecolor.frag"}}));
        g_assets.insert("shaderParticleColorFade", yg::gl::loadShader({{GL_VERTEX_SHADER, "a//particle_colorfade.vert"},
                                                                       {GL_FRAGMENT_SHADER, "a//simplecolor.frag"}}));

        glEnable(GL_DEPTH_TEST);
        //yg::control::enableVSync(true);
        //yg::control::enableFullscreen(true);
#ifndef __EMSCRIPTEN__ // todo: we can not initially catch the mouse if the viewport does not have focus (web) \
                       //yg::control::catchMouse(true);
#endif
    }

    void tick()
    {
        auto parts = g_assets.get<yg::gl::Particles>("particles");

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
            auto shdrPartsColorFade = g_assets.get<yg::gl::Shader>("shaderParticleColorFade");
            shdrPartsColorFade->useProgram();
            std::array<GLfloat, 5 * 3> colors; // num colors == 5, fixed in shader
            switch (preset)
            {
            case 0: // default
                parts->m_cfg = yg::math::Particles::Config();
                parts->m_cfg.count = 1000;
                parts->reset();
                colors = {1.0f, 1.0f, 0.75f,
                          1.0f, 1.0f, 0.5f,
                          1.0f, 0.75f, 0.0f,
                          1.0f, 0.5f, 0.0f,
                          1.0f, 0.25f, 0.0f};
                break;
            case 1: // snow
                parts->m_cfg = yg::math::Particles::Config();
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
                parts->m_cfg = yg::math::Particles::Config();
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
                parts->m_cfg = yg::math::Particles::Config();
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
        if (yg::input::getDelta(yg::input::KEY_M) > 0.0f)
        {
            yg::control::catchMouse(!yg::input::geti(yg::input::MOUSE_CATCHED));
        }

        // exit
        if (yg::input::get(yg::input::KEY_ESCAPE))
        {
            yg::control::notifyShutdown();
        }

        g_camera.setAspect(yg::input::get(yg::input::WINDOW_ASPECT_RATIO));
        glViewport(0, 0, yg::input::geti(yg::input::WINDOW_WIDTH), yg::input::geti(yg::input::WINDOW_HEIGHT));
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
                        yg::input::getDelta(yg::input::MOUSE_X),
                        yg::input::getDelta(yg::input::MOUSE_Y),
                        (float)(1.0 / yg::time::getDelta()));
            ImGui::EndMainMenuBar();
        }

        if (showLicenseWindow)
        {
            ImGui::SetNextWindowSizeConstraints(ImVec2(yg::input::get(yg::input::WINDOW_WIDTH) * 0.5f,
                                                       yg::input::get(yg::input::WINDOW_HEIGHT) * 0.5f),
                                                ImVec2(yg::input::get(yg::input::WINDOW_WIDTH) * 0.8f,
                                                       yg::input::get(yg::input::WINDOW_HEIGHT) * 0.8f));
            ImGui::Begin("License", &showLicenseWindow, (ImGuiWindowFlags_NoCollapse));
            /* The following procedure allows displaying long wrapped text,
               whereas ImGui::TextWrapped() has a size limit and cuts the content. */
            ImGui::PushTextWrapPos(0.0f);
            ImGui::TextUnformatted(g_assets.get<std::string>("licenseStr")->c_str());
            ImGui::PopTextWrapPos();
            ImGui::End();
        }

        // first-person camera
        if (yg::input::geti(yg::input::MOUSE_CATCHED))
        {
            g_camera.trafo()->rotateGlobal(-0.002f * yg::input::getDelta(yg::input::MOUSE_X), yg::math::Axis::Y);
            g_camera.trafo()->rotateLocal(-0.002f * yg::input::getDelta(yg::input::MOUSE_Y), yg::math::Axis::X);
            g_camera.trafo()->rotateGlobal(-0.001f * yg::input::getDelta(yg::input::TOUCH_0_X), yg::math::Axis::Y);
            g_camera.trafo()->rotateLocal(-0.001f * yg::input::getDelta(yg::input::TOUCH_0_Y), yg::math::Axis::X);
        }
        g_camera.trafo()->rotateGlobal(static_cast<float>(yg::time::getDelta()) * 1.0f * yg::input::get(yg::input::KEY_LEFT), yg::math::Axis::Y);
        g_camera.trafo()->rotateGlobal(static_cast<float>(yg::time::getDelta()) * -1.0f * yg::input::get(yg::input::KEY_RIGHT), yg::math::Axis::Y);
        g_camera.trafo()->rotateLocal(static_cast<float>(yg::time::getDelta()) * 1.0f * yg::input::get(yg::input::KEY_UP), yg::math::Axis::X);
        g_camera.trafo()->rotateLocal(static_cast<float>(yg::time::getDelta()) * -1.0f * yg::input::get(yg::input::KEY_DOWN), yg::math::Axis::X);
        g_camera.trafo()->translateLocal(-0.01f * yg::input::get(yg::input::KEY_W), yg::math::Axis::Z);
        g_camera.trafo()->translateLocal(0.01f * yg::input::get(yg::input::KEY_S), yg::math::Axis::Z);
        g_camera.trafo()->translateLocal(0.01f * yg::input::get(yg::input::KEY_D), yg::math::Axis::X);
        g_camera.trafo()->translateLocal(-0.01f * yg::input::get(yg::input::KEY_A), yg::math::Axis::X);

        // draw grid
        if (drawGrid)
        {
            auto shdrSimpleColor = g_assets.get<yg::gl::Shader>("shaderSimpleColor");
            shdrSimpleColor->useProgram();
            yg::gl::DrawConfig cfg;
            cfg.shader = shdrSimpleColor;
            cfg.camera = &g_camera;
            yg::gl::drawGeo(g_assets.get<yg::gl::Geometry>("geoGrid"), cfg);
        }

        // draw particles
        {
            yg::gl::Particles *parts = g_assets.get<yg::gl::Particles>("particles");
            parts->tick(static_cast<float>(yg::time::getDelta()) * particlesTickRate);
            // model matrix: scaling and billboard-like rotation (from camera trafo)
            auto modelMat = glm::mat4(glm::mat3(g_camera.trafo()->mat()) * glm::mat3(glm::scale(glm::vec3(0.04f))));
            auto shdrPartsColorFade = g_assets.get<yg::gl::Shader>("shaderParticleColorFade");
            shdrPartsColorFade->useProgram(nullptr, &g_camera);
            yg::gl::DrawConfig cfg;
            cfg.shader = shdrPartsColorFade;
            cfg.modelMat = modelMat;
            cfg.camera = &g_camera;
            cfg.instancecount = parts->numParticles();
            yg::gl::drawGeo(parts->geo(), cfg);
        }
    }

    void shutdown()
    {
        g_assets.clear();
    }
} // namespace mygame
