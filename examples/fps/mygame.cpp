#include <map>
#include <string>
#include <vector>
#include "btBulletDynamicsCommon.h"
#include "yourgame/yourgame.h"
#include "yourgame/toolbox.h"
#include "imgui.h"
#include "yourgame/gl_include.h"

namespace yg = yourgame; // convenience

namespace mygame
{
    struct Lightsource
    {
        glm::vec3 position;
        glm::vec3 diffuse;
    };

    struct Player
    {
        btRigidBody *body;
    };

    struct Box
    {
        btRigidBody *body;
    };

    struct BulletEnv
    {
        btDefaultCollisionConfiguration *collisionCfg;
        btCollisionDispatcher *collisionDispatcher;
        btDbvtBroadphase *broadphase;
        btSequentialImpulseConstraintSolver *constraintSolver;
        btDiscreteDynamicsWorld *dynamicsWorld;
        std::map<std::string, btCollisionShape *> shapes;

        BulletEnv()
        {
            collisionCfg = new btDefaultCollisionConfiguration();
            collisionDispatcher = new btCollisionDispatcher(collisionCfg);
            broadphase = new btDbvtBroadphase();
            constraintSolver = new btSequentialImpulseConstraintSolver();
            dynamicsWorld = new btDiscreteDynamicsWorld(collisionDispatcher, broadphase, constraintSolver, collisionCfg);
        }

        ~BulletEnv()
        {
            // remove all rigid bodies from world, delete them and their motion state
            for (int i = dynamicsWorld->getNumCollisionObjects() - 1; i >= 0; i--)
            {
                auto collObj = dynamicsWorld->getCollisionObjectArray()[i];
                btRigidBody *body = btRigidBody::upcast(collObj);
                if (body && body->getMotionState())
                {
                    delete body->getMotionState();
                }
                dynamicsWorld->removeCollisionObject(collObj);
                delete collObj;
            }

            // delete all shapes
            for (const auto &s : shapes)
            {
                delete s.second;
            }
            shapes.clear();

            delete dynamicsWorld;
            delete constraintSolver;
            delete broadphase;
            delete collisionDispatcher;
            delete collisionCfg;
        }
    };

    yg::AssetManager g_assets;
    BulletEnv g_bullet;
    yg::Camera g_camera;
    yg::Trafo g_camTrafoFltr;
    Player g_player;
    std::vector<Box> g_boxes;
    Lightsource g_light{{4.07625f, 5.90386f, -1.00545f}, {1.0f, 1.0f, 1.0f}};

    void drawGeo(const yg::GLGeometry *geo,
                 yg::GLShader *shader,
                 std::vector<const yg::GLTexture2D *> textures,
                 const glm::mat4 &modelMat = glm::mat4(1),
                 const yg::Camera *camera = nullptr)
    {
        if (!geo)
            return;

        // mvp matrix
        if (shader)
        {
            GLint unif;
            unif = shader->getUniformLocation(yg::unifNameMvpMatrix);
            if (unif != -1)
            {
                auto mvp = camera ? (g_camera.pMat() * g_camera.vMat() * modelMat)
                                  : modelMat;
                glUniformMatrix4fv(unif, 1, GL_FALSE, glm::value_ptr(mvp));
            }

            // model matrix
            unif = shader->getUniformLocation(yg::unifNameModelMatrix);
            if (unif != -1)
            {
                glUniformMatrix4fv(unif, 1, GL_FALSE, glm::value_ptr(modelMat));
            }

            // normal matrix
            unif = shader->getUniformLocation(yg::unifNameNormalMatrix);
            if (unif != -1)
            {
                auto normalMat = glm::inverseTranspose(glm::mat3(modelMat));
                glUniformMatrix3fv(unif, 1, GL_FALSE, glm::value_ptr(normalMat));
            }
        }

        // textures
        for (const auto &t : textures)
        {
            if (t)
                t->bind();
        }

        geo->drawAll();
    }

    void init()
    {
        auto ctx = yg::getCtx();

        yg::audioInit(2, 44100, 5);
        yg::audioStoreFile("laserSmall_000.ogg");

        g_camera.setPerspective(65.0f, ctx.winAspectRatio, 0.2f, 100.0f);

        g_assets.insert("geoCube", yg::loadGeometry("cube.obj", nullptr));
        g_assets.insert("geoGrid", yg::loadGeometry("grid.obj", nullptr));
        g_assets.insert("geoCross", yg::loadGeometry("cross.obj", nullptr));
        g_assets.insert("geoBlaster", yg::loadGeometry("blasterD.obj", "blasterD.mtl"));

        g_assets.insert("shaderDiffuseColor", yg::loadShader({{GL_VERTEX_SHADER, "default.vert"},
                                                              {GL_FRAGMENT_SHADER, "diffusecolor.frag"}},
                                                             {}, {}));

        g_assets.insert("shaderSimpleColor", yg::loadShader({{GL_VERTEX_SHADER, "default.vert"},
                                                             {GL_FRAGMENT_SHADER, "simplecolor.frag"}},
                                                            {}, {}));

        g_assets.insert("shaderSimpleTex", yg::loadShader({{GL_VERTEX_SHADER, "default.vert"},
                                                           {GL_FRAGMENT_SHADER, "simpletex.frag"}},
                                                          {}, {}));

        g_assets.insert("texCube", yg::loadTexture("cube.png", yg::textureUnitDiffuse, GL_NEAREST, false));

        glClearColor(0.275f, 0.275f, 0.275f, 1.0f);
        glEnable(GL_DEPTH_TEST);
        //yg::enableVSync(true);
        //yg::enableFullscreen(true);
#ifndef __EMSCRIPTEN__ // todo: we can not initially catch the mouse if the viewport does not have focus (web)
        yg::catchMouse(true);
#endif

        g_bullet.dynamicsWorld->setGravity(btVector3(0, -9.81, 0));

        // spawn boxes
        g_bullet.shapes["box"] = new btBoxShape(btVector3(0.5, 0.5, 0.5));
        for (int bi = 0; bi <= 10; bi++)
        {
            btTransform trafo;
            btScalar mass(100.0);
            btVector3 inertia(0, 0, 0);
            g_bullet.shapes["box"]->calculateLocalInertia(mass, inertia);
            trafo.setIdentity();
            trafo.setOrigin(btVector3(-10 + 2 * bi, 50 - 5 * bi, 10 - 2 * bi));
            btRigidBody::btRigidBodyConstructionInfo rbInfo(mass, new btDefaultMotionState(trafo), g_bullet.shapes["box"], inertia);
            rbInfo.m_restitution = 0.2f;
            rbInfo.m_friction = 0.75f;
            auto newBody = new btRigidBody(rbInfo);
            newBody->setSleepingThresholds(0, 0); // never sleep
            g_bullet.dynamicsWorld->addRigidBody(newBody);
            g_boxes.push_back({newBody});
        }

        // spawn player
        {
            g_bullet.shapes["player"] = new btBoxShape(btVector3(1, 2, 1));
            btTransform trafo;
            btScalar mass(100.0);
            btVector3 inertia(0, 0, 0);
            g_bullet.shapes["player"]->calculateLocalInertia(mass, inertia);
            trafo.setIdentity();
            trafo.setOrigin(btVector3(8.0f, 2.0f, 8.0f));
            btRigidBody::btRigidBodyConstructionInfo rbInfo(mass, new btDefaultMotionState(trafo), g_bullet.shapes["player"], inertia);
            rbInfo.m_restitution = 0.0f;
            rbInfo.m_friction = 0.75f;
            rbInfo.m_linearDamping = 0.9f;
            auto newBody = new btRigidBody(rbInfo);
            g_player = Player({newBody});
            g_player.body->setAngularFactor(0);         // simple way to lock rotation axes
            g_player.body->setSleepingThresholds(0, 0); // never sleep
            g_bullet.dynamicsWorld->addRigidBody(newBody);
        }

        // spawn ground
        {
            g_bullet.shapes["ground"] = new btBoxShape(btVector3(btScalar(50.), btScalar(50.), btScalar(50.)));
            btTransform trafo;
            trafo.setIdentity();
            trafo.setOrigin(btVector3(0, -50, 0));
            btScalar mass(0.0);
            btVector3 inertia(0, 0, 0);
            btRigidBody::btRigidBodyConstructionInfo rbInfo(mass, new btDefaultMotionState(trafo), g_bullet.shapes["ground"], inertia);
            rbInfo.m_restitution = 1.0f;
            rbInfo.m_friction = 1.0f;
            g_bullet.dynamicsWorld->addRigidBody(new btRigidBody(rbInfo));
        }
    }

    void tick()
    {
        auto ctx = yg::getCtx();

        // tick bullet world
        g_bullet.dynamicsWorld->stepSimulation(ctx.deltaTimeS, 10);

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
            yg::audioPlay("laserSmall_000.ogg");

            // cast ray from camera into scene and test what it hits
            static const float rayLength = 100.0f;
            auto eye = g_camera.trafo()->getEye();
            auto dir = -g_camera.trafo()->getAxisLocal(yg::Trafo::AXIS::Z);
            auto to = eye + (rayLength * dir);
            btVector3 btFrom(eye.x, eye.y, eye.z);
            btVector3 btTo(to.x, to.y, to.z);
            btCollisionWorld::ClosestRayResultCallback res(btFrom, btTo);
            g_bullet.dynamicsWorld->rayTest(btFrom, btTo, res);
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

        if (ImGui::BeginMainMenuBar())
        {
            if (ImGui::BeginMenu("Help"))
            {
                if (ImGui::MenuItem("License"))
                {
                    //showLicense = true;
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
        // todo: GLShader::useProgram() should get references to objects,
        // to set uniforms once per frame (lightsource)
        auto shdrDiffCol = g_assets.get<yg::GLShader>("shaderDiffuseColor");
        shdrDiffCol->useProgram();
        glUniform3fv(shdrDiffCol->getUniformLocation("lightPos"), 1, &g_light.position[0]);
        glUniform3fv(shdrDiffCol->getUniformLocation("lightDiffuse"), 1, &g_light.diffuse[0]);

        // texture shader
        auto shdrTex = g_assets.get<yg::GLShader>("shaderSimpleTex");
        shdrTex->useProgram();

        // draw boxes
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

            drawGeo(g_assets.get<yg::GLGeometry>("geoCube"), shdrTex, {g_assets.get<yg::GLTexture2D>("texCube")}, modelTrafo.mat(), &g_camera);
        }

        // draw blaster
        {
            shdrDiffCol->useProgram();

            // blaster trafo relative to camera eye
            yg::Trafo blasterTrafo;
            blasterTrafo.setTranslation({0.275f, -0.35f, -0.7f});
            blasterTrafo.setScaleLocal(1.25f);

            // filter camera trafo over time for blaster "displacement"
            g_camTrafoFltr.lerp(static_cast<float>(ctx.deltaTimeS) * 30.0f, *g_camera.trafo(), &g_camTrafoFltr);
            // filter rotation only, reset translation to camera eye:
            g_camTrafoFltr.setTranslation(g_camera.trafo()->getEye());

            auto modelMat = g_camTrafoFltr.mat() * blasterTrafo.mat();
            drawGeo(g_assets.get<yg::GLGeometry>("geoBlaster"), shdrDiffCol, {}, modelMat, &g_camera);
        }

        // draw grid + crosshair
        {
            auto shdrSimpCol = g_assets.get<yg::GLShader>("shaderSimpleColor");
            shdrSimpCol->useProgram();

            // grid
            drawGeo(g_assets.get<yg::GLGeometry>("geoGrid"), shdrSimpCol, {}, glm::mat4(1), &g_camera);

            // crosshair
            static const float crossOrthoScale = 50.0f;
            static const auto crossMvp = glm::ortho(-crossOrthoScale * ctx.winAspectRatio,
                                                    crossOrthoScale * ctx.winAspectRatio,
                                                    -crossOrthoScale,
                                                    crossOrthoScale);
            drawGeo(g_assets.get<yg::GLGeometry>("geoCross"), shdrSimpCol, {}, crossMvp);
        }
    }

    void shutdown()
    {
        yg::audioShutdown();
        g_assets.clear();
    }

} // namespace mygame
