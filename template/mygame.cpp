#include "yourgame/yourgame.h"
#include "yourgame/toolbox.h"
#include "yourgame/gl_include.h"
#include "mygame_version.h"

namespace yg = yourgame; // convenience

namespace mygame
{
    yg::AssetManager g_assets;
    yg::Camera g_camera;
    yg::Trafo g_modelTrafo;
    yg::GLLightsource g_light;

    void init(int argc, char *argv[])
    {
        auto ctx = yg::getCtx();

        yg::logi("project: %v (%v)", mygame::version::PROJECT_NAME, mygame::version::git_commit);
        yg::logi("based on: %v (%v)", yg::version::PROJECT_NAME, yg::version::git_commit);

        g_camera.trafo()->lookAt(glm::vec3(7.35889f, 4.95831f, 6.92579f),
                                 glm::vec3(0.0f, 0.0f, 0.0f),
                                 glm::vec3(0.0f, 1.0f, 0.0f));
        g_camera.setPerspective(40.0f, ctx.winAspectRatio, 1.0f, 100.0f);

        g_light.setPosition({4.07625f, 5.90386f, -1.00545f});
        g_light.setDiffuse({1.0f, 1.0f, 1.0f});

        g_assets.insert("geoCube", yg::loadGeometry("a//cube.obj"));
        g_assets.insert("geoGrid", yg::loadGeometry("a//grid.obj"));

        g_assets.insert("shaderDiffuseColor", yg::loadShader({{GL_VERTEX_SHADER, "a//default.vert"},
                                                              {GL_FRAGMENT_SHADER, "a//diffusecolor.frag"}}));

        g_assets.insert("shaderSimpleColor", yg::loadShader({{GL_VERTEX_SHADER, "a//default.vert"},
                                                             {GL_FRAGMENT_SHADER, "a//simplecolor.frag"}}));

        glClearColor(0.275f, 0.275f, 0.275f, 1.0f);
        glEnable(GL_DEPTH_TEST);
        //yg::enableVSync(true);
        //yg::enableFullscreen(true);
        //yg::catchMouse(true);
    }

    void tick()
    {
        auto ctx = yg::getCtx();

        g_camera.setAspect(ctx.winAspectRatio);
        glViewport(0, 0, ctx.winWidth, ctx.winHeight);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        if (yg::input(yg::INPUT::KEY_ESCAPE))
        {
            yg::notifyShutdown();
        }

        // first-person camera
        g_camera.trafo()->rotateGlobal(static_cast<float>(ctx.deltaTimeS) * 0.75f * yg::input(yg::INPUT::KEY_LEFT), yg::Trafo::AXIS::Y);
        g_camera.trafo()->rotateGlobal(static_cast<float>(ctx.deltaTimeS) * -0.75f * yg::input(yg::INPUT::KEY_RIGHT), yg::Trafo::AXIS::Y);
        g_camera.trafo()->rotateLocal(static_cast<float>(ctx.deltaTimeS) * 0.75f * yg::input(yg::INPUT::KEY_UP), yg::Trafo::AXIS::X);
        g_camera.trafo()->rotateLocal(static_cast<float>(ctx.deltaTimeS) * -0.75f * yg::input(yg::INPUT::KEY_DOWN), yg::Trafo::AXIS::X);
        g_camera.trafo()->translateLocal(static_cast<float>(ctx.deltaTimeS) * -5.0f * yg::input(yg::INPUT::KEY_W), yg::Trafo::AXIS::Z);
        g_camera.trafo()->translateLocal(static_cast<float>(ctx.deltaTimeS) * 5.0f * yg::input(yg::INPUT::KEY_S), yg::Trafo::AXIS::Z);
        g_camera.trafo()->translateLocal(static_cast<float>(ctx.deltaTimeS) * 5.0f * yg::input(yg::INPUT::KEY_D), yg::Trafo::AXIS::X);
        g_camera.trafo()->translateLocal(static_cast<float>(ctx.deltaTimeS) * -5.0f * yg::input(yg::INPUT::KEY_A), yg::Trafo::AXIS::X);

        // fade light
        static float lightT = 0.0f;
        float light = std::cos(lightT) * 0.5f + 0.5f;
        g_light.setDiffuse({light, light, light});
        lightT += (static_cast<float>(ctx.deltaTimeS) * 0.65f);

        // draw cube
        {
            auto shdrDiffCol = g_assets.get<yg::GLShader>("shaderDiffuseColor");
            shdrDiffCol->useProgram(&g_light, &g_camera);
            yg::DrawConfig cfg;
            cfg.camera = &g_camera;
            cfg.modelMat = g_modelTrafo.mat();
            cfg.shader = shdrDiffCol;
            yg::drawGeo(g_assets.get<yg::GLGeometry>("geoCube"), cfg);
        }

        // draw grid
        {
            auto shdrSimpCol = g_assets.get<yg::GLShader>("shaderSimpleColor");
            shdrSimpCol->useProgram(nullptr, &g_camera);
            yg::DrawConfig cfg;
            cfg.camera = &g_camera;
            cfg.shader = shdrSimpCol;
            yg::drawGeo(g_assets.get<yg::GLGeometry>("geoGrid"), cfg);
        }
    }

    void shutdown()
    {
        g_assets.clear();
    }

} // namespace mygame
