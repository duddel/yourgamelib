#include "yourgame/yourgame.h"
#include "yourgame/toolbox.h"
#include "yourgame/gl_include.h"

namespace yg = yourgame; // convenience

namespace mygame
{
    struct Lightsource
    {
        glm::vec3 position;
        glm::vec3 diffuse;
    };

    yg::AssetManager g_assets;
    yg::Camera g_camera;
    yg::Trafo g_modelTrafo;
    Lightsource g_light{{4.07625f, 5.90386f, -1.00545f}, {1.0f, 1.0f, 1.0f}};

    void init()
    {
        auto ctx = yg::getCtx();

        g_camera.trafo()->lookAt(glm::vec3(7.35889f, 4.95831f, 6.92579f),
                                 glm::vec3(0.0f, 0.0f, 0.0f),
                                 glm::vec3(0.0f, 1.0f, 0.0f));
        g_camera.setPerspective(40.0f, ctx.winAspectRatio, 1.0f, 100.0f);

        g_assets.insert("geoCube", yg::loadGeometry("cube.obj", nullptr));
        g_assets.insert("geoGrid", yg::loadGeometry("grid.obj", nullptr));

        g_assets.insert("shaderDiffuseColor", yg::loadShader({{GL_VERTEX_SHADER, "default.vert"},
                                                              {GL_FRAGMENT_SHADER, "diffusecolor.frag"}},
                                                             {}, {}));

        g_assets.insert("shaderSimpleColor", yg::loadShader({{GL_VERTEX_SHADER, "default.vert"},
                                                             {GL_FRAGMENT_SHADER, "simplecolor.frag"}},
                                                            {}, {}));

        glClearColor(0.275f, 0.275f, 0.275f, 1.0f);
        glEnable(GL_DEPTH_TEST);
        //yg::enableVSync(false);
        //yg::enableFullscreen(true);
        //yg::catchMouse(true);
    }

    void tick()
    {
        auto ctx = yg::getCtx();

        g_camera.setAspect(ctx.winAspectRatio);
        glViewport(0, 0, ctx.winWidth, ctx.winHeight);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        if (yg::getInput(yg::InputSource::YOURGAME_KEY_ESCAPE))
        {
            yg::notifyShutdown();
        }

        // first-person camera
        g_camera.trafo()->rotateGlobal(static_cast<float>(ctx.deltaTimeS) * 0.75f * yg::getInput(yg::InputSource::YOURGAME_KEY_LEFT), yg::Trafo::AXIS::Y);
        g_camera.trafo()->rotateGlobal(static_cast<float>(ctx.deltaTimeS) * -0.75f * yg::getInput(yg::InputSource::YOURGAME_KEY_RIGHT), yg::Trafo::AXIS::Y);
        g_camera.trafo()->rotateLocal(static_cast<float>(ctx.deltaTimeS) * 0.75f * yg::getInput(yg::InputSource::YOURGAME_KEY_UP), yg::Trafo::AXIS::X);
        g_camera.trafo()->rotateLocal(static_cast<float>(ctx.deltaTimeS) * -0.75f * yg::getInput(yg::InputSource::YOURGAME_KEY_DOWN), yg::Trafo::AXIS::X);
        g_camera.trafo()->translateLocal(static_cast<float>(ctx.deltaTimeS) * -5.0f * yg::getInput(yg::InputSource::YOURGAME_KEY_W), yg::Trafo::AXIS::Z);
        g_camera.trafo()->translateLocal(static_cast<float>(ctx.deltaTimeS) * 5.0f * yg::getInput(yg::InputSource::YOURGAME_KEY_S), yg::Trafo::AXIS::Z);
        g_camera.trafo()->translateLocal(static_cast<float>(ctx.deltaTimeS) * 5.0f * yg::getInput(yg::InputSource::YOURGAME_KEY_D), yg::Trafo::AXIS::X);
        g_camera.trafo()->translateLocal(static_cast<float>(ctx.deltaTimeS) * -5.0f * yg::getInput(yg::InputSource::YOURGAME_KEY_A), yg::Trafo::AXIS::X);

        // fade light
        static float lightT = 0.0f;
        g_light.diffuse = glm::vec3(std::cos(lightT) * 0.5f + 0.5f);
        lightT += (static_cast<float>(ctx.deltaTimeS) * 0.65f);

        // prepare matrixes for shaders
        auto modelMat = g_modelTrafo.mat();
        auto mvp = g_camera.pMat() * g_camera.vMat() * modelMat;
        auto normalMat = glm::inverseTranspose(glm::mat3(modelMat));

        // draw cube
        auto shdrDiffCol = g_assets.get<yg::GLShader>("shaderDiffuseColor");
        shdrDiffCol->useProgram();
        glUniform3fv(shdrDiffCol->getUniformLocation("lightPos"), 1, &g_light.position[0]);
        glUniform3fv(shdrDiffCol->getUniformLocation("lightDiffuse"), 1, &g_light.diffuse[0]);
        glUniformMatrix4fv(shdrDiffCol->getUniformLocation(yg::unifNameMvpMatrix), 1, GL_FALSE, glm::value_ptr(mvp));
        glUniformMatrix4fv(shdrDiffCol->getUniformLocation(yg::unifNameModelMatrix), 1, GL_FALSE, glm::value_ptr(modelMat));
        glUniformMatrix3fv(shdrDiffCol->getUniformLocation(yg::unifNameNormalMatrix), 1, GL_FALSE, glm::value_ptr(normalMat));
        g_assets.get<yg::GLGeometry>("geoCube")->drawAll();

        // draw grid
        auto shdrSimpCol = g_assets.get<yg::GLShader>("shaderSimpleColor");
        shdrSimpCol->useProgram();
        glUniformMatrix4fv(shdrSimpCol->getUniformLocation(yg::unifNameMvpMatrix), 1, GL_FALSE, glm::value_ptr(mvp));
        g_assets.get<yg::GLGeometry>("geoGrid")->drawAll();
    }

    void shutdown()
    {
        g_assets.clear();
    }

} // namespace mygame
