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

    void init(int argc, char *argv[])
    {
        g_camera.trafo()->lookAt(glm::vec3(0.0f, 0.0f, 5.0f),
                                 glm::vec3(0.0f, 0.0f, 0.0f),
                                 glm::vec3(0.0f, 1.0f, 0.0f));
        g_camera.setPerspective(65.0f, yg::input(yg::INPUT::WINDOW_ASPECT_RATIO), 0.2f, 100.0f);

        // load license info file
        {
            std::vector<uint8_t> licFileData;
            yg::readFile("a//LICENSE_web.txt", licFileData);
            std::string *licStr = new std::string(licFileData.begin(), licFileData.end());
            g_assets.insert("licenseStr", licStr);
        }

        // atlas
        {
            yg::TextureConfig cfg;
            cfg.minMagFilter = GL_NEAREST;
            g_assets.insert("atlas", yg::loadTextureAtlasCrunch("a//sprites.json", cfg));
        }

        // geometry
        g_assets.insert("geoGrid", yg::loadGeometry("a//grid.obj"));
        g_assets.insert("geoQuad", yg::loadGeometry("a//quad.obj"));

        // shaders
        g_assets.insert("shaderSimpleColor", yg::loadShader({{GL_VERTEX_SHADER, "a//default.vert"},
                                                             {GL_FRAGMENT_SHADER, "a//simplecolor.frag"}}));

        g_assets.insert("shaderSimpleSubtex", yg::loadShader({{GL_VERTEX_SHADER, "a//default.vert"},
                                                              {GL_FRAGMENT_SHADER, "a//simpletex.frag"}}));

        glClearColor(0.05f, 0.05f, 0.1f, 1.0f);
        glEnable(GL_DEPTH_TEST);
        //yg::enableVSync(true);
        //yg::enableFullscreen(true);
#ifndef __EMSCRIPTEN__ // todo: we can not initially catch the mouse if the viewport does not have focus (web) \
                       //yg::catchMouse(true);
#endif
    }

    void tick()
    {
        // lock/release mouse
        if (yg::inputDelta(yg::INPUT::KEY_M) > 0.0f)
        {
            yg::catchMouse(!yg::mouseCatched);
        }

        // exit
        if (yg::input(yg::INPUT::KEY_ESCAPE))
        {
            yg::notifyShutdown();
        }

        g_camera.setAspect(yg::input(yg::INPUT::WINDOW_ASPECT_RATIO));
        glViewport(0, 0, yg::inputi(yg::INPUT::WINDOW_WIDTH), yg::inputi(yg::INPUT::WINDOW_HEIGHT));
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
                        yg::inputDelta(yg::INPUT::MOUSE_X),
                        yg::inputDelta(yg::INPUT::MOUSE_Y),
                        (float)(1.0 / yg::deltaTimeS));
            ImGui::EndMainMenuBar();
        }

        if (showLicenseWindow)
        {
            ImGui::SetNextWindowSizeConstraints(ImVec2(yg::input(yg::INPUT::WINDOW_WIDTH) * 0.5f,
                                                       yg::input(yg::INPUT::WINDOW_HEIGHT) * 0.5f),
                                                ImVec2(yg::input(yg::INPUT::WINDOW_WIDTH) * 0.8f,
                                                       yg::input(yg::INPUT::WINDOW_HEIGHT) * 0.8f));
            ImGui::Begin("License", &showLicenseWindow, (ImGuiWindowFlags_NoCollapse));
            /* The following procedure allows displaying long wrapped text,
               whereas ImGui::TextWrapped() has a size limit and cuts the content. */
            ImGui::PushTextWrapPos(0.0f);
            ImGui::TextUnformatted(g_assets.get<std::string>("licenseStr")->c_str());
            ImGui::PopTextWrapPos();
            ImGui::End();
        }

        // first-person camera
        if (yg::mouseCatched)
        {
            g_camera.trafo()->rotateGlobal(-0.002f * yg::inputDelta(yg::INPUT::MOUSE_X), yg::Trafo::AXIS::Y);
            g_camera.trafo()->rotateLocal(-0.002f * yg::inputDelta(yg::INPUT::MOUSE_Y), yg::Trafo::AXIS::X);
            g_camera.trafo()->rotateGlobal(-0.001f * yg::inputDelta(yg::INPUT::TOUCH_0_X), yg::Trafo::AXIS::Y);
            g_camera.trafo()->rotateLocal(-0.001f * yg::inputDelta(yg::INPUT::TOUCH_0_Y), yg::Trafo::AXIS::X);
        }
        g_camera.trafo()->rotateGlobal(static_cast<float>(yg::deltaTimeS) * 1.0f * yg::input(yg::INPUT::KEY_LEFT), yg::Trafo::AXIS::Y);
        g_camera.trafo()->rotateGlobal(static_cast<float>(yg::deltaTimeS) * -1.0f * yg::input(yg::INPUT::KEY_RIGHT), yg::Trafo::AXIS::Y);
        g_camera.trafo()->rotateLocal(static_cast<float>(yg::deltaTimeS) * 1.0f * yg::input(yg::INPUT::KEY_UP), yg::Trafo::AXIS::X);
        g_camera.trafo()->rotateLocal(static_cast<float>(yg::deltaTimeS) * -1.0f * yg::input(yg::INPUT::KEY_DOWN), yg::Trafo::AXIS::X);
        g_camera.trafo()->translateLocal(-0.01f * yg::input(yg::INPUT::KEY_W), yg::Trafo::AXIS::Z);
        g_camera.trafo()->translateLocal(0.01f * yg::input(yg::INPUT::KEY_S), yg::Trafo::AXIS::Z);
        g_camera.trafo()->translateLocal(0.01f * yg::input(yg::INPUT::KEY_D), yg::Trafo::AXIS::X);
        g_camera.trafo()->translateLocal(-0.01f * yg::input(yg::INPUT::KEY_A), yg::Trafo::AXIS::X);

        auto atlas = g_assets.get<yg::GLTextureAtlas>("atlas");
        static int frameIdx = 0;
        static bool play = true;
        static float frameRate = 10.0f;
        static bool drawGrid = false;

        ImGui::Begin("Sprite Animation", nullptr, (ImGuiWindowFlags_AlwaysAutoResize));
        // sequence combo
        auto seqNames = atlas->getSequenceNames();
        static int selectedIdx = 0;
        const char *selectedSeqName = seqNames[selectedIdx].c_str();
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
        ImGui::SliderInt("Frame #", &frameIdx, 0, atlas->getSeqFrames(selectedSeqName) - 1);
        ImGui::SliderFloat("Frames/s", &frameRate, 0.0f, 30.0f);
        ImGui::Checkbox("Play", &play);
        ImGui::Checkbox("Draw grid", &drawGrid);
        ImGui::End();

        if (play)
        {
            frameIdx = ((int)(yg::getTime() * frameRate)) % atlas->getSeqFrames(selectedSeqName);
        }

        // draw sprite quad
        {
            auto shdrSimpleTex = g_assets.get<yg::GLShader>("shaderSimpleSubtex");
            shdrSimpleTex->useProgram();
            auto stc = atlas->getCoords(selectedSeqName, frameIdx);
            yg::DrawConfig cfg;
            cfg.shader = shdrSimpleTex;
            cfg.camera = &g_camera;
            cfg.textures = {stc.first};
            cfg.subtex = stc.second;
            yg::drawGeo(g_assets.get<yg::GLGeometry>("geoQuad"), cfg);
        }

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
    }

    void shutdown()
    {
        g_assets.clear();
    }
} // namespace mygame
