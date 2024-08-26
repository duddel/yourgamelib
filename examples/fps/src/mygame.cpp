#include <map>
#include <string>
#include <vector>
#include "yourgame/yourgame.h"
#include "imgui.h"

namespace yg = yourgame; // convenience

namespace mygame
{
    yg::util::AssetManager g_assets;
    yg::util::PhysEnv g_physEnv;
    yg::math::Camera g_camera;
    yg::math::Trafo g_camTrafoFltr;
    yg::gl::Lightsource g_light;

    const float g_boxScale = 0.5f;

    void init(int argc, char *argv[])
    {
        yg::audio::init(5, 0, 0);
        yg::audio::storeFile("a//laserSmall_000.ogg");

        g_light.setPosition({4.07625f, 5.90386f, -1.00545f});
        g_camera.setPerspective(65.0f, yg::input::get(yg::input::WINDOW_ASPECT_RATIO), 0.2f, 100.0f);

        // load license info file
        {
            std::vector<uint8_t> licFileData;
            yg::file::readFile("a//yg_LICENSES.txt", licFileData);
            std::string *licStr = new std::string(licFileData.begin(), licFileData.end());
            g_assets.insert("licenseStr", licStr);
        }

        g_assets.insert("geoCube", yg::gl::loadGeometry("a//yg_cube_outside.obj"));
        g_assets.insert("geoGrid", yg::gl::loadGeometry("a//yg_grid.obj"));
        g_assets.insert("geoCross", yg::gl::loadGeometry("a//yg_cross.obj"));
        g_assets.insert("geoBlaster", yg::gl::loadGeometry("a//blasterD.obj", "a//blasterD.mtl"));

        g_assets.insert("shaderDiffuseColor", yg::gl::loadShader({{GL_VERTEX_SHADER, "a//yg_default.vert"},
                                                                  {GL_FRAGMENT_SHADER, "a//yg_diffusecolor.frag"}}));

        g_assets.insert("shaderSimpleColor", yg::gl::loadShader({{GL_VERTEX_SHADER, "a//yg_default.vert"},
                                                                 {GL_FRAGMENT_SHADER, "a//yg_simplecolor.frag"}}));

        g_assets.insert("shaderDiffuseTex", yg::gl::loadShader({{GL_VERTEX_SHADER, "a//default_instanced.vert"},
                                                                {GL_FRAGMENT_SHADER, "a//yg_diffusetex.frag"}}));

        {
            yg::gl::TextureConfig cfg;
            cfg.minMagFilter = GL_NEAREST;
            g_assets.insert("texCube", yg::gl::loadTexture("a//cube.png", "", cfg));
        }

        // for instanced drawing of the cube, add and configure a buffer for per-cube (per draw instance) matrixes:
        GLsizei vec4Size = static_cast<GLsizei>(sizeof(glm::vec4));
        g_assets.get<yg::gl::Geometry>("geoCube")->addBuffer("instModelMat", GL_ARRAY_BUFFER, 0, nullptr, GL_DYNAMIC_DRAW);
        g_assets.get<yg::gl::Geometry>("geoCube")
            ->addBufferToShape("main",
                               {{yg::gl::attrLocInstModelMatCol0, 4, GL_FLOAT, GL_FALSE, 4 * vec4Size, (void *)0, 1},
                                {yg::gl::attrLocInstModelMatCol1, 4, GL_FLOAT, GL_FALSE, 4 * vec4Size, (void *)(vec4Size), 1},
                                {yg::gl::attrLocInstModelMatCol2, 4, GL_FLOAT, GL_FALSE, 4 * vec4Size, (void *)(2 * vec4Size), 1},
                                {yg::gl::attrLocInstModelMatCol3, 4, GL_FLOAT, GL_FALSE, 4 * vec4Size, (void *)(3 * vec4Size), 1}},
                               "instModelMat");

        glClearColor(0.275f, 0.275f, 0.275f, 1.0f);
        glEnable(GL_DEPTH_TEST);
        // yg::control::enableVSync(true);
        // yg::control::enableFullscreen(true);
#ifndef __EMSCRIPTEN__ // todo: we can not initially catch the mouse if the viewport does not have focus (web)
        yg::control::catchMouse(true);
#endif

        g_physEnv.setGravity(0.0f, -9.81f, 0.0f);

        // spawn boxes
        g_physEnv.newBoxShape("box", g_boxScale, g_boxScale, g_boxScale);
        for (int bi = 0; bi <= 10; bi++)
        {
            yg::math::Trafo trafo;
            trafo.setTranslation({-10 + 2 * bi, 50 - 5 * bi, 10 - 2 * bi});
            yg::util::RigidBodyInfo rbInfo;
            rbInfo.mass = 100.0f;
            rbInfo.restitution = 0.2f;
            rbInfo.friction = 0.75f;
            rbInfo.disableDeactivation = true;
            g_physEnv.newRigidBody("box" + std::to_string(bi), "box", trafo, rbInfo);
        }

        // spawn player
        {
            g_physEnv.newBoxShape("player", 1.0f, 2.0f, 1.0f);

            yg::math::Trafo trafo;
            trafo.setTranslation({8.0f, 2.0f, 8.0f});
            yg::util::RigidBodyInfo rbInfo;
            rbInfo.mass = 100.0f;
            rbInfo.restitution = 0.0f;
            rbInfo.friction = 0.75f;
            rbInfo.linearDamping = 0.9f;
            rbInfo.disableDeactivation = true;
            g_physEnv.newRigidBody("player", "player", trafo, rbInfo);

            auto playerBody = g_physEnv.getRigidBody("player");
            playerBody->setAngularFactor(0);         // simple way to lock rotation axes
            playerBody->setSleepingThresholds(0, 0); // never sleep
        }

        // spawn ground
        {
            g_physEnv.newBoxShape("ground", 50.0f, 50.0f, 50.0f);

            yg::math::Trafo trafo;
            trafo.setTranslation({0.0f, -50.0f, 0.0f});
            yg::util::RigidBodyInfo rbInfo;
            rbInfo.mass = 0.0f;
            rbInfo.restitution = 1.0f;
            rbInfo.friction = 1.0f;
            rbInfo.linearDamping = 0.0f;
            g_physEnv.newRigidBody("ground", "ground", trafo, rbInfo);
        }
    }

    void tick()
    {
        // tick physics world
        g_physEnv.tick(yg::time::getDelta());

        // set camera trafo from physics player body
        g_camera.trafo()->setTranslation(g_physEnv.getRigidBody("player")->getTrafo().getEye());

        // assemble force vector from WASD movement keys
        glm::vec3 forceX = g_camera.trafo()->getAxisLocal(yg::math::Axis::X);
        glm::vec3 forceZ = g_camera.trafo()->getAxisLocal(yg::math::Axis::Z);
        forceX[1] = 0.0f;
        forceZ[1] = 0.0f;
        forceX *= (yg::input::get(yg::input::KEY_D) - yg::input::get(yg::input::KEY_A));
        forceZ *= (yg::input::get(yg::input::KEY_S) - yg::input::get(yg::input::KEY_W));

        glm::vec3 force = forceX + forceZ;

        // apply movement force to player
        if (glm::length(force) > 0.01f)
        {
            force = glm::normalize(force);
            force *= 2500.0f;
            g_physEnv.getRigidBody("player")->applyCentralForce(force[0], force[1], force[2]);
        }

        // jump
        if (yg::input::getDelta(yg::input::KEY_SPACE) > 0.0f)
        {
            g_physEnv.getRigidBody("player")->applyCentralImpulse(0.0f, 400.0f, 0.0f);
        }

        // fire
        if (yg::input::getDelta(yg::input::MOUSE_BUTTON_1) > 0.0f ||
            yg::input::getDelta(yg::input::KEY_E) > 0.0f)
        {
            yg::audio::play("a//laserSmall_000.ogg");

            // cast ray from camera into scene and test what it hits
            static const float rayLength = 100.0f;
            auto eye = g_camera.trafo()->getEye();
            auto dir = -g_camera.trafo()->getAxisLocal(yg::math::Axis::Z);
            auto to = eye + (rayLength * dir);

            auto rayRes = g_physEnv.rayTest(eye.x, eye.y, eye.z, to.x, to.y, to.z);

            if (rayRes.m_rigidBody)
            {
                float impulseFactor = 50.0f;
                rayRes.m_rigidBody->applyCentralImpulse((to.x - eye.x) * impulseFactor,
                                                        (to.y - eye.y) * impulseFactor,
                                                        (to.z - eye.z) * impulseFactor);
            }
        }

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

        // Rotate camera from input
        float yawFromInput = -0.0005f * yg::input::getDelta(yg::input::MOUSE_X) +
                             -0.0005f * yg::input::getDelta(yg::input::TOUCH_0_X) +
                             (yg::input::get(yg::input::KEY_LEFT) - yg::input::get(yg::input::KEY_RIGHT)) * static_cast<float>(yg::time::getDelta());

        float pitchFromInput = -0.0005f * yg::input::getDelta(yg::input::MOUSE_Y) +
                               -0.0005f * yg::input::getDelta(yg::input::TOUCH_0_Y) +
                               (yg::input::get(yg::input::KEY_UP) - yg::input::get(yg::input::KEY_DOWN)) * static_cast<float>(yg::time::getDelta());

        g_camera.rotateFirstPerson(yawFromInput, pitchFromInput);

        // prepare diffuse color shader
        auto shdrDiffCol = g_assets.get<yg::gl::Shader>("shaderDiffuseColor");
        shdrDiffCol->useProgram(&g_light);

        // texture shader
        auto shdrTex = g_assets.get<yg::gl::Shader>("shaderDiffuseTex");
        shdrTex->useProgram(&g_light, &g_camera);

        // draw boxes
        {
            std::vector<glm::mat4> trafos;
            auto bodies = g_physEnv.getRigidBodiesStartingWith("box");
            for (const auto &b : bodies)
            {
                auto trafo = b->getTrafo();
                trafo.setScaleLocal(g_boxScale);
                trafos.push_back(trafo.mat());
            }

            auto geoCube = g_assets.get<yg::gl::Geometry>("geoCube");
            geoCube->bufferData("instModelMat", trafos.size() * sizeof(trafos[0]), trafos.data());
            yg::gl::DrawConfig cfg;
            cfg.shader = shdrTex;
            cfg.textures = {g_assets.get<yg::gl::Texture>("texCube")};
            cfg.camera = &g_camera;
            cfg.instancecount = bodies.size();
            yg::gl::drawGeo(geoCube, cfg);
        }

        // draw blaster
        {
            shdrDiffCol->useProgram();

            // filter camera trafo over time for blaster "displacement"
            g_camTrafoFltr.lerp(static_cast<float>(yg::time::getDelta()) * 30.0f, *g_camera.trafo(), &g_camTrafoFltr);
            // filter rotation only, reset translation to camera eye:
            g_camTrafoFltr.setTranslation(g_camera.trafo()->getEye());

            // blaster Trafo: filtered camera trafo, followed by manual displacement and scale
            yg::math::Trafo blasterTrafo = g_camTrafoFltr;
            blasterTrafo.translateLocal({0.275f, -0.35f, -0.7f});
            blasterTrafo.setScaleLocal(1.25f);

            yg::gl::DrawConfig cfg;
            cfg.shader = shdrDiffCol;
            cfg.modelMat = blasterTrafo.mat();
            cfg.camera = &g_camera;
            yg::gl::drawGeo(g_assets.get<yg::gl::Geometry>("geoBlaster"), cfg);
        }

        // draw grid + crosshair
        {
            auto shdrSimpCol = g_assets.get<yg::gl::Shader>("shaderSimpleColor");
            shdrSimpCol->useProgram();

            // grid
            yg::gl::DrawConfig cfg;
            cfg.shader = shdrSimpCol;
            cfg.camera = &g_camera;
            yg::gl::drawGeo(g_assets.get<yg::gl::Geometry>("geoGrid"), cfg);

            // crosshair
            static const float crossOrthoScale = 50.0f;
            auto crossMvp = glm::ortho(-crossOrthoScale * yg::input::get(yg::input::WINDOW_ASPECT_RATIO),
                                       crossOrthoScale * yg::input::get(yg::input::WINDOW_ASPECT_RATIO),
                                       -crossOrthoScale,
                                       crossOrthoScale);
            cfg.camera = nullptr;
            cfg.modelMat = crossMvp;
            yg::gl::drawGeo(g_assets.get<yg::gl::Geometry>("geoCross"), cfg);
        }
    }

    int shutdown()
    {
        yg::audio::shutdown();
        g_assets.clear();
        return 0;
    }
} // namespace mygame
