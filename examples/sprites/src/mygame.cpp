#include <map>
#include <string>
#include <vector>
#include "yourgame/yourgame.h"
#include "imgui.h"

namespace yg = yourgame; // convenience

namespace mygame
{
    yg::util::AssetManager g_assets;

    void init(int argc, char *argv[])
    {
        // load license info file
        {
            std::vector<uint8_t> licFileData;
            yg::file::readFile("a//yg_LICENSES.txt", licFileData);
            std::string *licStr = new std::string(licFileData.begin(), licFileData.end());
            g_assets.insert("licenseStr", licStr);
        }

        // atlas
        {
            yg::gl::TextureConfig cfg;
            cfg.minMagFilter = GL_NEAREST;
            g_assets.insert("atlas", yg::gl::loadTexture("a//sprites0.png", "a//sprites.json", cfg));
        }

        glClearColor(0.05f, 0.05f, 0.1f, 1.0f);
        glEnable(GL_DEPTH_TEST);

        yg::control::enableVSync(true);
    }

    void tick()
    {
        // exit
        if (yg::input::get(yg::input::KEY_ESCAPE))
        {
            yg::control::exit();
        }

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

        auto atlas = g_assets.get<yg::gl::Texture>("atlas");
        static int frameIdx = 0;
        static bool play = true;
        static float frameRate = 10.0f;

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
            ImGui::End();

            // Advance animation and draw sprite
            if (play)
            {
                static double timer = 0.0;
                timer += (yg::time::getDelta() * frameRate);
                frameIdx = static_cast<int>(timer) % atlas->getNumFrames(selectedSeqName);
            }

            yourgame::gl::drawSprite(atlas,
                                     atlas->getFrameCoords(selectedSeqName, frameIdx),
                                     100,
                                     100,
                                     100,
                                     0,
                                     0);
        }
    }

    int shutdown()
    {
        g_assets.clear();
        return 0;
    }
} // namespace mygame
