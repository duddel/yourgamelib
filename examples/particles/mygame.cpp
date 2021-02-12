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
                                 glm::vec3(0.0f, 0.0f, 0.0f),
                                 glm::vec3(0.0f, 1.0f, 0.0f));
        g_camera.setPerspective(65.0f, ctx.winAspectRatio, 0.2f, 100.0f);

        // load license info file
        {
            std::vector<uint8_t> licFileData;
            yg::readAssetFile("LICENSE_web.txt", licFileData);
            std::string *licStr = new std::string(licFileData.begin(), licFileData.end());
            g_assets.insert("licenseStr", licStr);
        }

        g_assets.insert("geoGrid", yg::loadGeometry("grid.obj", nullptr));

        g_assets.insert("shaderSimpleColor", yg::loadShader({{GL_VERTEX_SHADER, "default.vert"},
                                                             {GL_FRAGMENT_SHADER, "simplecolor.frag"}}));

        glClearColor(0.275f, 0.275f, 0.275f, 1.0f);
        glEnable(GL_DEPTH_TEST);
        //yg::enableVSync(true);
        //yg::enableFullscreen(true);
#ifndef __EMSCRIPTEN__ // todo: we can not initially catch the mouse if the viewport does not have focus (web)
        yg::catchMouse(true);
#endif
    }

    void tick()
    {
        auto ctx = yg::getCtx();

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
            g_camera.trafo()->rotateGlobal(static_cast<float>(ctx.deltaTimeS) * 1.0f * yg::getInput(yg::InputSource::YOURGAME_KEY_LEFT), yg::Trafo::AXIS::Y);
            g_camera.trafo()->rotateGlobal(static_cast<float>(ctx.deltaTimeS) * -1.0f * yg::getInput(yg::InputSource::YOURGAME_KEY_RIGHT), yg::Trafo::AXIS::Y);
            g_camera.trafo()->rotateLocal(static_cast<float>(ctx.deltaTimeS) * 1.0f * yg::getInput(yg::InputSource::YOURGAME_KEY_UP), yg::Trafo::AXIS::X);
            g_camera.trafo()->rotateLocal(static_cast<float>(ctx.deltaTimeS) * -1.0f * yg::getInput(yg::InputSource::YOURGAME_KEY_DOWN), yg::Trafo::AXIS::X);
            g_camera.trafo()->translateLocal(-0.01f * yg::getInput(yg::InputSource::YOURGAME_KEY_W), yg::Trafo::AXIS::Z);
            g_camera.trafo()->translateLocal(0.01f * yg::getInput(yg::InputSource::YOURGAME_KEY_S), yg::Trafo::AXIS::Z);
            g_camera.trafo()->translateLocal(0.01f * yg::getInput(yg::InputSource::YOURGAME_KEY_D), yg::Trafo::AXIS::X);
            g_camera.trafo()->translateLocal(-0.01f * yg::getInput(yg::InputSource::YOURGAME_KEY_A), yg::Trafo::AXIS::X);
        }

        // draw grid
        {
            auto shdrSimpCol = g_assets.get<yg::GLShader>("shaderSimpleColor");
            shdrSimpCol->useProgram();

            // grid
            yg::drawGeo(g_assets.get<yg::GLGeometry>("geoGrid"), shdrSimpCol, {}, glm::mat4(1), &g_camera);
        }
    }

    void shutdown()
    {
        yg::audioShutdown();
        g_assets.clear();
    }
} // namespace mygame
