#include "yourgame/yourgame.h"
#include "mygame_version.h"

namespace yg = yourgame; // convenience

namespace mygame
{
    yg::util::AssetManager g_assets;
    yg::math::Camera g_camera;
    yg::math::Trafo g_modelTrafo;
    yg::gl::Lightsource g_light;

    void init(int argc, char *argv[])
    {
        yg::log::info("project: %v (%v)", mygame::version::PROJECT_NAME, mygame::version::git_commit);
        yg::log::info("based on: %v (%v)", yg::version::PROJECT_NAME, yg::version::git_commit);

        g_camera.trafo()->lookAt(glm::vec3(7.35889f, 4.95831f, 6.92579f),
                                 glm::vec3(0.0f, 0.0f, 0.0f),
                                 glm::vec3(0.0f, 1.0f, 0.0f));
        g_camera.setPerspective(40.0f, yg::input::get(yg::input::WINDOW_ASPECT_RATIO), 1.0f, 100.0f);

        g_light.setPosition({4.07625f, 5.90386f, -1.00545f});
        g_light.setDiffuse({1.0f, 1.0f, 1.0f});

        g_assets.insert("geoCube", yg::gl::loadGeometry("a//yg_cube_outside.obj"));
        g_assets.insert("geoGrid", yg::gl::loadGeometry("a//yg_grid.obj"));

        g_assets.insert("shaderDiffuseColor",
                        yg::gl::loadShader({{GL_VERTEX_SHADER, "a//yg_default.vert"},
                                            {GL_FRAGMENT_SHADER, "a//yg_diffusecolor.frag"}}));

        g_assets.insert("shaderSimpleColor",
                        yg::gl::loadShader({{GL_VERTEX_SHADER, "a//yg_default.vert"},
                                            {GL_FRAGMENT_SHADER, "a//yg_simplecolor.frag"}}));

        glClearColor(0.275f, 0.275f, 0.275f, 1.0f);
        glEnable(GL_DEPTH_TEST);
    }

    void tick()
    {
        g_camera.setAspect(yg::input::get(yg::input::WINDOW_ASPECT_RATIO));
        glViewport(0,
                   0,
                   yg::input::geti(yg::input::WINDOW_WIDTH),
                   yg::input::geti(yg::input::WINDOW_HEIGHT));
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        if (yg::input::get(yg::input::KEY_ESCAPE))
        {
            yg::control::exit();
        }

        // first-person camera
        g_camera.trafo()->rotateGlobal(static_cast<float>(yg::time::getDelta()) * 0.75f * yg::input::get(yg::input::KEY_LEFT), yg::math::Axis::Y);
        g_camera.trafo()->rotateGlobal(static_cast<float>(yg::time::getDelta()) * -0.75f * yg::input::get(yg::input::KEY_RIGHT), yg::math::Axis::Y);
        g_camera.trafo()->rotateLocal(static_cast<float>(yg::time::getDelta()) * 0.75f * yg::input::get(yg::input::KEY_UP), yg::math::Axis::X);
        g_camera.trafo()->rotateLocal(static_cast<float>(yg::time::getDelta()) * -0.75f * yg::input::get(yg::input::KEY_DOWN), yg::math::Axis::X);
        g_camera.trafo()->translateLocal(static_cast<float>(yg::time::getDelta()) * -5.0f * yg::input::get(yg::input::KEY_W), yg::math::Axis::Z);
        g_camera.trafo()->translateLocal(static_cast<float>(yg::time::getDelta()) * 5.0f * yg::input::get(yg::input::KEY_S), yg::math::Axis::Z);
        g_camera.trafo()->translateLocal(static_cast<float>(yg::time::getDelta()) * 5.0f * yg::input::get(yg::input::KEY_D), yg::math::Axis::X);
        g_camera.trafo()->translateLocal(static_cast<float>(yg::time::getDelta()) * -5.0f * yg::input::get(yg::input::KEY_A), yg::math::Axis::X);

        // fade light
        static float lightT = 0.0f;
        float light = std::cos(lightT) * 0.5f + 0.5f;
        g_light.setDiffuse({light, light, light});
        lightT += (static_cast<float>(yg::time::getDelta()) * 0.65f);

        // draw cube
        {
            auto shdrDiffCol = g_assets.get<yg::gl::Shader>("shaderDiffuseColor");
            shdrDiffCol->useProgram(&g_light, &g_camera);
            yg::gl::DrawConfig cfg;
            cfg.camera = &g_camera;
            cfg.modelMat = g_modelTrafo.mat();
            cfg.shader = shdrDiffCol;
            yg::gl::drawGeo(g_assets.get<yg::gl::Geometry>("geoCube"), cfg);
        }

        // draw grid
        {
            auto shdrSimpCol = g_assets.get<yg::gl::Shader>("shaderSimpleColor");
            shdrSimpCol->useProgram(nullptr, &g_camera);
            yg::gl::DrawConfig cfg;
            cfg.camera = &g_camera;
            cfg.shader = shdrSimpCol;
            yg::gl::drawGeo(g_assets.get<yg::gl::Geometry>("geoGrid"), cfg);
        }
    }

    int shutdown()
    {
        g_assets.clear();
        return 0;
    }

} // namespace mygame
