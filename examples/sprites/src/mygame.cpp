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
        g_camera.trafo()->lookAt(glm::vec3(0.0f, 0.0f, 5.0f),
                                 glm::vec3(0.0f, 0.0f, 0.0f),
                                 glm::vec3(0.0f, 1.0f, 0.0f));
        g_camera.setPerspective(65.0f, yg::input::get(yg::input::WINDOW_ASPECT_RATIO), 0.2f, 100.0f);

        // load license info file
        {
            std::vector<uint8_t> licFileData;
            yg::file::readFile("a//LICENSE_web.txt", licFileData);
            std::string *licStr = new std::string(licFileData.begin(), licFileData.end());
            g_assets.insert("licenseStr", licStr);
        }

        // atlas
        {
            yg::gl::TextureConfig cfg;
            cfg.minMagFilter = GL_NEAREST;

            g_assets.insert("atlas", yg::gl::loadTexture("a//sprites0.png", "a//sprites.json", cfg));
        }

        // geometry
        g_assets.insert("geoGrid", yg::gl::loadGeometry("a//grid.obj"));
        g_assets.insert("geoQuad", yg::gl::loadGeometry("a//quad.obj"));

        // shaders
        g_assets.insert("shaderSimpleColor", yg::gl::loadShader({{GL_VERTEX_SHADER, "a//default.vert"},
                                                                 {GL_FRAGMENT_SHADER, "a//simplecolor.frag"}}));

        g_assets.insert("shaderSimpleSubtex", yg::gl::loadShader({{GL_VERTEX_SHADER, "a//default.vert"},
                                                                  {GL_FRAGMENT_SHADER, "a//simpletex.frag"}}));

        glClearColor(0.05f, 0.05f, 0.1f, 1.0f);
        glEnable(GL_DEPTH_TEST);
        // yg::control::enableVSync(true);
        // yg::control::enableFullscreen(true);
#ifndef __EMSCRIPTEN__ // todo: we can not initially catch the mouse if the viewport does not have focus (web) \
                       //yg::control::catchMouse(true);
#endif
    }

    void tick()
    {
        // lock/release mouse
        if (yg::input::getDelta(yg::input::KEY_M) > 0.0f)
        {
            yg::control::catchMouse(!yg::input::geti(yg::input::MOUSE_CATCHED));
        }

        // exit
        if (yg::input::get(yg::input::KEY_ESCAPE))
        {
            yg::control::exit();
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

        auto atlas = g_assets.get<yg::gl::Texture>("atlas");
        static int frameIdx = 0;
        static bool play = true;
        static float frameRate = 10.0f;
        static bool drawGrid = false;

        auto seqNames = atlas->getSequenceNames();
        if (seqNames.size() > 0)
        {
            static int selectedIdx = 0;
            const char *selectedSeqName = seqNames[selectedIdx].c_str();

            ImGui::Begin("Sprite Animation", nullptr, (ImGuiWindowFlags_AlwaysAutoResize));
            if (ImGui::BeginCombo("Sequences", selectedSeqName))
            {
                for (int i = 0; i < seqNames.size(); i++)
                {
                    if (ImGui::Selectable(seqNames[i].c_str(), selectedIdx == i))
                    {
                        selectedIdx = i;
                    }
                }
                ImGui::EndCombo();
            }
            ImGui::SliderInt("Frame #", &frameIdx, 0, atlas->getNumFrames(selectedSeqName) - 1);
            ImGui::SliderFloat("Frames/s", &frameRate, 0.0f, 30.0f);
            ImGui::Checkbox("Play", &play);
            ImGui::Checkbox("Draw grid", &drawGrid);
            ImGui::End();

            if (play)
            {
                frameIdx = ((int)(yg::time::getTime() * frameRate)) % atlas->getNumFrames(selectedSeqName);
            }

            // draw sprite quad
            {
                auto shdrSimpleTex = g_assets.get<yg::gl::Shader>("shaderSimpleSubtex");
                shdrSimpleTex->useProgram();
                auto stc = atlas->getFrameCoords(selectedSeqName, frameIdx);
                yg::gl::DrawConfig cfg;
                cfg.shader = shdrSimpleTex;
                cfg.camera = &g_camera;
                cfg.textures = {atlas};
                cfg.subtex = {stc.uMin, stc.uMax, stc.vMin, stc.vMax};
                yg::gl::drawGeo(g_assets.get<yg::gl::Geometry>("geoQuad"), cfg);
            }
        }

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
    }

    int shutdown()
    {
        g_assets.clear();
        return 0;
    }
} // namespace mygame
