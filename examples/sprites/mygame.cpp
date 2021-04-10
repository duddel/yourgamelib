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

        g_camera.trafo()->lookAt(glm::vec3(0.0f, 0.0f, 5.0f),
                                 glm::vec3(0.0f, 0.0f, 0.0f),
                                 glm::vec3(0.0f, 1.0f, 0.0f));
        g_camera.setPerspective(65.0f, ctx.winAspectRatio, 0.2f, 100.0f);

        // load license info file
        {
            std::vector<uint8_t> licFileData;
            yg::readFile("a//LICENSE_web.txt", licFileData);
            std::string *licStr = new std::string(licFileData.begin(), licFileData.end());
            g_assets.insert("licenseStr", licStr);
        }

        // atlas
        g_assets.insert("atlas", yg::loadTextureAtlasCrunch("a//sprites.json", yg::textureUnitDiffuse, GL_NEAREST));

        // geometry
        g_assets.insert("geoGrid", yg::loadGeometry("a//grid.obj", nullptr));
        g_assets.insert("geoQuad", yg::loadGeometry("a//quad.obj", nullptr));

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
        }
        g_camera.trafo()->rotateGlobal(static_cast<float>(ctx.deltaTimeS) * 1.0f * yg::getInput(yg::InputSource::YOURGAME_KEY_LEFT), yg::Trafo::AXIS::Y);
        g_camera.trafo()->rotateGlobal(static_cast<float>(ctx.deltaTimeS) * -1.0f * yg::getInput(yg::InputSource::YOURGAME_KEY_RIGHT), yg::Trafo::AXIS::Y);
        g_camera.trafo()->rotateLocal(static_cast<float>(ctx.deltaTimeS) * 1.0f * yg::getInput(yg::InputSource::YOURGAME_KEY_UP), yg::Trafo::AXIS::X);
        g_camera.trafo()->rotateLocal(static_cast<float>(ctx.deltaTimeS) * -1.0f * yg::getInput(yg::InputSource::YOURGAME_KEY_DOWN), yg::Trafo::AXIS::X);
        g_camera.trafo()->translateLocal(-0.01f * yg::getInput(yg::InputSource::YOURGAME_KEY_W), yg::Trafo::AXIS::Z);
        g_camera.trafo()->translateLocal(0.01f * yg::getInput(yg::InputSource::YOURGAME_KEY_S), yg::Trafo::AXIS::Z);
        g_camera.trafo()->translateLocal(0.01f * yg::getInput(yg::InputSource::YOURGAME_KEY_D), yg::Trafo::AXIS::X);
        g_camera.trafo()->translateLocal(-0.01f * yg::getInput(yg::InputSource::YOURGAME_KEY_A), yg::Trafo::AXIS::X);

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
