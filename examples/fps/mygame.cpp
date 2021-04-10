#include <map>
#include <string>
#include <vector>
#include "btBulletDynamicsCommon.h"
#include "yourgame/yourgame.h"
#include "yourgame/toolbox.h"
#include "yourgame/bulletenv.h"
#include "yourgame/audioplayer.h"
#include "imgui.h"

namespace yg = yourgame; // convenience

namespace mygame
{
    struct Player
    {
        btRigidBody *body;
    };

    struct Box
    {
        btRigidBody *body;
    };

    yg::AssetManager g_assets;
    yg::BulletEnv g_bullet;
    yg::Camera g_camera;
    yg::Trafo g_camTrafoFltr;
    Player g_player;
    std::vector<Box> g_boxes;
    yg::GLLightsource g_light;

    void init()
    {
        auto ctx = yg::getCtx();

        yg::audioInit(2, 44100, 5);
        yg::audioStoreFile("a//laserSmall_000.ogg");

        g_light.setPosition({4.07625f, 5.90386f, -1.00545f});
        g_camera.setPerspective(65.0f, ctx.winAspectRatio, 0.2f, 100.0f);

        // load license info file
        {
            std::vector<uint8_t> licFileData;
            yg::readFile("a//LICENSE_web.txt", licFileData);
            std::string *licStr = new std::string(licFileData.begin(), licFileData.end());
            g_assets.insert("licenseStr", licStr);
        }

        g_assets.insert("geoCube", yg::loadGeometry("a//cube.obj", nullptr));
        g_assets.insert("geoGrid", yg::loadGeometry("a//grid.obj", nullptr));
        g_assets.insert("geoCross", yg::loadGeometry("a//cross.obj", nullptr));
        g_assets.insert("geoBlaster", yg::loadGeometry("a//blasterD.obj", "a//blasterD.mtl"));

        g_assets.insert("shaderDiffuseColor", yg::loadShader({{GL_VERTEX_SHADER, "a//default.vert"},
                                                              {GL_FRAGMENT_SHADER, "a//diffusecolor.frag"}}));

        g_assets.insert("shaderSimpleColor", yg::loadShader({{GL_VERTEX_SHADER, "a//default.vert"},
                                                             {GL_FRAGMENT_SHADER, "a//simplecolor.frag"}}));

        g_assets.insert("shaderDiffuseTex", yg::loadShader({{GL_VERTEX_SHADER, "a//default_instanced.vert"},
                                                            {GL_FRAGMENT_SHADER, "a//diffusetex.frag"}}));

        g_assets.insert("texCube", yg::loadTexture("a//cube.png", yg::textureUnitDiffuse, GL_NEAREST, false));

        // for instanced drawing of the cube, add and configure a buffer for per-cube (per draw instance) matrixes:
        GLsizei vec4Size = static_cast<GLsizei>(sizeof(glm::vec4));
        g_assets.get<yg::GLGeometry>("geoCube")->addBuffer("instModelMat", GL_ARRAY_BUFFER, 0, nullptr, GL_DYNAMIC_DRAW);
        g_assets.get<yg::GLGeometry>("geoCube")
            ->addBufferToShape("main",
                               {{yg::attrLocInstModelMatCol0, 4, GL_FLOAT, GL_FALSE, 4 * vec4Size, (void *)0, 1},
                                {yg::attrLocInstModelMatCol1, 4, GL_FLOAT, GL_FALSE, 4 * vec4Size, (void *)(vec4Size), 1},
                                {yg::attrLocInstModelMatCol2, 4, GL_FLOAT, GL_FALSE, 4 * vec4Size, (void *)(2 * vec4Size), 1},
                                {yg::attrLocInstModelMatCol3, 4, GL_FLOAT, GL_FALSE, 4 * vec4Size, (void *)(3 * vec4Size), 1}},
                               "instModelMat");

        glClearColor(0.275f, 0.275f, 0.275f, 1.0f);
        glEnable(GL_DEPTH_TEST);
        //yg::enableVSync(true);
        //yg::enableFullscreen(true);
#ifndef __EMSCRIPTEN__ // todo: we can not initially catch the mouse if the viewport does not have focus (web)
        yg::catchMouse(true);
#endif

        g_bullet.m_dynamicsWorld->setGravity(btVector3(0, -9.81, 0));

        // spawn boxes
        g_bullet.m_shapes["box"] = new btBoxShape(btVector3(0.5, 0.5, 0.5));
        for (int bi = 0; bi <= 10; bi++)
        {
            btTransform trafo;
            btScalar mass(100.0);
            btVector3 inertia(0, 0, 0);
            g_bullet.m_shapes["box"]->calculateLocalInertia(mass, inertia);
            trafo.setIdentity();
            trafo.setOrigin(btVector3(-10 + 2 * bi, 50 - 5 * bi, 10 - 2 * bi));
            btRigidBody::btRigidBodyConstructionInfo rbInfo(mass, new btDefaultMotionState(trafo), g_bullet.m_shapes["box"], inertia);
            rbInfo.m_restitution = 0.2f;
            rbInfo.m_friction = 0.75f;
            auto newBody = new btRigidBody(rbInfo);
            newBody->setSleepingThresholds(0, 0); // never sleep
            g_bullet.m_dynamicsWorld->addRigidBody(newBody);
            g_boxes.push_back({newBody});
        }

        // spawn player
        {
            g_bullet.m_shapes["player"] = new btBoxShape(btVector3(1, 2, 1));
            btTransform trafo;
            btScalar mass(100.0);
            btVector3 inertia(0, 0, 0);
            g_bullet.m_shapes["player"]->calculateLocalInertia(mass, inertia);
            trafo.setIdentity();
            trafo.setOrigin(btVector3(8.0f, 2.0f, 8.0f));
            btRigidBody::btRigidBodyConstructionInfo rbInfo(mass, new btDefaultMotionState(trafo), g_bullet.m_shapes["player"], inertia);
            rbInfo.m_restitution = 0.0f;
            rbInfo.m_friction = 0.75f;
            rbInfo.m_linearDamping = 0.9f;
            auto newBody = new btRigidBody(rbInfo);
            g_player = Player({newBody});
            g_player.body->setAngularFactor(0);         // simple way to lock rotation axes
            g_player.body->setSleepingThresholds(0, 0); // never sleep
            g_bullet.m_dynamicsWorld->addRigidBody(newBody);
        }

        // spawn ground
        {
            g_bullet.m_shapes["ground"] = new btBoxShape(btVector3(btScalar(50.), btScalar(50.), btScalar(50.)));
            btTransform trafo;
            trafo.setIdentity();
            trafo.setOrigin(btVector3(0, -50, 0));
            btScalar mass(0.0);
            btVector3 inertia(0, 0, 0);
            btRigidBody::btRigidBodyConstructionInfo rbInfo(mass, new btDefaultMotionState(trafo), g_bullet.m_shapes["ground"], inertia);
            rbInfo.m_restitution = 1.0f;
            rbInfo.m_friction = 1.0f;
            g_bullet.m_dynamicsWorld->addRigidBody(new btRigidBody(rbInfo));
        }
    }

    void tick()
    {
        auto ctx = yg::getCtx();

        // tick bullet world
        g_bullet.m_dynamicsWorld->stepSimulation(ctx.deltaTimeS, 10);

        // set camera trafo from bullet player body
        btTransform trans;
        g_player.body->getMotionState()->getWorldTransform(trans);
        g_camera.trafo()->setTranslation({trans.getOrigin().getX(), trans.getOrigin().getY(), trans.getOrigin().getZ()});

        // assemble force vector from WASD movement keys
        glm::vec3 forceX = g_camera.trafo()->getAxisLocal(yg::Trafo::AXIS::X);
        glm::vec3 forceZ = g_camera.trafo()->getAxisLocal(yg::Trafo::AXIS::Z);
        forceX[1] = 0.0f;
        forceZ[1] = 0.0f;
        forceX *= (yg::getInput(yg::InputSource::YOURGAME_KEY_D) - yg::getInput(yg::InputSource::YOURGAME_KEY_A));
        forceZ *= (yg::getInput(yg::InputSource::YOURGAME_KEY_S) - yg::getInput(yg::InputSource::YOURGAME_KEY_W));

        glm::vec3 force = forceX + forceZ;

        // apply movement force to player
        if (glm::length(force) > 0.01f)
        {
            force = glm::normalize(force);
            force *= 2500.0f;
            g_player.body->applyCentralForce({force[0], force[1], force[2]});
        }

        // jump
        if (yg::getInputDelta(yg::InputSource::YOURGAME_KEY_SPACE) > 0.0f)
        {
            g_player.body->applyCentralImpulse({0, 400, 0});
        }

        // fire
        if (yg::getInputDelta(yg::InputSource::YOURGAME_MOUSE_BUTTON_1) > 0.0f ||
            yg::getInputDelta(yg::InputSource::YOURGAME_KEY_E) > 0.0f)
        {
            yg::audioPlay("a//laserSmall_000.ogg");

            // cast ray from camera into scene and test what it hits
            static const float rayLength = 100.0f;
            auto eye = g_camera.trafo()->getEye();
            auto dir = -g_camera.trafo()->getAxisLocal(yg::Trafo::AXIS::Z);
            auto to = eye + (rayLength * dir);
            btVector3 btFrom(eye.x, eye.y, eye.z);
            btVector3 btTo(to.x, to.y, to.z);
            btCollisionWorld::ClosestRayResultCallback res(btFrom, btTo);
            g_bullet.m_dynamicsWorld->rayTest(btFrom, btTo, res);
            if (res.hasHit())
            {
                // todo: upcast() returns const btRigidBody* because res.m_collisionObject is const.
                // we cast the constness of the body away to apply impulse
                btRigidBody *body = const_cast<btRigidBody *>(btRigidBody::upcast(res.m_collisionObject));
                if (body)
                {
                    // todo: apply impulse at hit point, not center (0,0,0)
                    body->applyImpulse((btTo - btFrom) * 50.0f, btVector3(0, 0, 0));
                }
            }
        }

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

            ImGui::Text("| mouse delta: %f,%f, player velocity: %f, fps: %f",
                        yg::getInputDelta(yg::InputSource::YOURGAME_MOUSE_X),
                        yg::getInputDelta(yg::InputSource::YOURGAME_MOUSE_Y),
                        g_player.body->getLinearVelocity().length(),
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
        g_camera.trafo()->rotateGlobal(-0.002f * yg::getInputDelta(yg::InputSource::YOURGAME_MOUSE_X), yg::Trafo::AXIS::Y);
        g_camera.trafo()->rotateLocal(-0.002f * yg::getInputDelta(yg::InputSource::YOURGAME_MOUSE_Y), yg::Trafo::AXIS::X);
        g_camera.trafo()->rotateGlobal(-0.001f * yg::getInputDelta(yg::InputSource::YOURGAME_TOUCH_0_X), yg::Trafo::AXIS::Y);
        g_camera.trafo()->rotateLocal(-0.001f * yg::getInputDelta(yg::InputSource::YOURGAME_TOUCH_0_Y), yg::Trafo::AXIS::X);
        g_camera.trafo()->rotateGlobal(static_cast<float>(ctx.deltaTimeS) * 1.0f * yg::getInput(yg::InputSource::YOURGAME_KEY_LEFT), yg::Trafo::AXIS::Y);
        g_camera.trafo()->rotateGlobal(static_cast<float>(ctx.deltaTimeS) * -1.0f * yg::getInput(yg::InputSource::YOURGAME_KEY_RIGHT), yg::Trafo::AXIS::Y);
        g_camera.trafo()->rotateLocal(static_cast<float>(ctx.deltaTimeS) * 1.0f * yg::getInput(yg::InputSource::YOURGAME_KEY_UP), yg::Trafo::AXIS::X);
        g_camera.trafo()->rotateLocal(static_cast<float>(ctx.deltaTimeS) * -1.0f * yg::getInput(yg::InputSource::YOURGAME_KEY_DOWN), yg::Trafo::AXIS::X);

        // prepare diffuse color shader
        auto shdrDiffCol = g_assets.get<yg::GLShader>("shaderDiffuseColor");
        shdrDiffCol->useProgram(&g_light);

        // texture shader
        auto shdrTex = g_assets.get<yg::GLShader>("shaderDiffuseTex");
        shdrTex->useProgram(&g_light, &g_camera);

        // draw boxes
        {
            std::vector<glm::mat4> trafos;
            for (const auto &b : g_boxes)
            {
                yg::Trafo modelTrafo;
                // get box data from bullet
                btTransform trans;
                b.body->getMotionState()->getWorldTransform(trans);
                btQuaternion rot = trans.getRotation();
                modelTrafo.setTranslation({trans.getOrigin().getX(), trans.getOrigin().getY(), trans.getOrigin().getZ()});
                modelTrafo.setRotation({rot.getW(), rot.getX(), rot.getY(), rot.getZ()});
                btBoxShape *bShape = static_cast<btBoxShape *>(b.body->getCollisionShape());
                btVector3 bExtends = bShape->getHalfExtentsWithMargin();
                modelTrafo.setScaleLocal({bExtends.getX(), bExtends.getY(), bExtends.getZ()});
                trafos.push_back(modelTrafo.mat());
            }
            auto geoCube = g_assets.get<yg::GLGeometry>("geoCube");
            geoCube->bufferData("instModelMat", trafos.size() * sizeof(trafos[0]), trafos.data());
            yg::DrawConfig cfg;
            cfg.shader = shdrTex;
            cfg.textures = {g_assets.get<yg::GLTexture2D>("texCube")};
            cfg.camera = &g_camera;
            cfg.instancecount = g_boxes.size();
            yg::drawGeo(geoCube, cfg);
        }

        // draw blaster
        {
            shdrDiffCol->useProgram();

            // filter camera trafo over time for blaster "displacement"
            g_camTrafoFltr.lerp(static_cast<float>(ctx.deltaTimeS) * 30.0f, *g_camera.trafo(), &g_camTrafoFltr);
            // filter rotation only, reset translation to camera eye:
            g_camTrafoFltr.setTranslation(g_camera.trafo()->getEye());

            // blaster Trafo: filtered camera trafo, followed by manual displacement and scale
            yg::Trafo blasterTrafo = g_camTrafoFltr;
            blasterTrafo.translateLocal({0.275f, -0.35f, -0.7f});
            blasterTrafo.setScaleLocal(1.25f);

            yg::DrawConfig cfg;
            cfg.shader = shdrDiffCol;
            cfg.modelMat = blasterTrafo.mat();
            cfg.camera = &g_camera;
            yg::drawGeo(g_assets.get<yg::GLGeometry>("geoBlaster"), cfg);
        }

        // draw grid + crosshair
        {
            auto shdrSimpCol = g_assets.get<yg::GLShader>("shaderSimpleColor");
            shdrSimpCol->useProgram();

            // grid
            yg::DrawConfig cfg;
            cfg.shader = shdrSimpCol;
            cfg.camera = &g_camera;
            yg::drawGeo(g_assets.get<yg::GLGeometry>("geoGrid"), cfg);

            // crosshair
            static const float crossOrthoScale = 50.0f;
            auto crossMvp = glm::ortho(-crossOrthoScale * ctx.winAspectRatio,
                                       crossOrthoScale * ctx.winAspectRatio,
                                       -crossOrthoScale,
                                       crossOrthoScale);
            cfg.camera = nullptr;
            cfg.modelMat = crossMvp;
            yg::drawGeo(g_assets.get<yg::GLGeometry>("geoCross"), cfg);
        }
    }

    void shutdown()
    {
        yg::audioShutdown();
        g_assets.clear();
    }
} // namespace mygame
