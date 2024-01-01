/*
Copyright (c) 2019-2024 Alexander Scholz

This software is provided 'as-is', without any express or implied
warranty. In no event will the authors be held liable for any damages
arising from the use of this software.

Permission is granted to anyone to use this software for any purpose,
including commercial applications, and to alter it and redistribute it
freely, subject to the following restrictions:

1. The origin of this software must not be misrepresented; you must not
   claim that you wrote the original software. If you use this software
   in a product, an acknowledgment in the product documentation would be
   appreciated but is not required.
2. Altered source versions must be plainly marked as such, and must not be
   misrepresented as being the original software.
3. This notice may not be removed or altered from any source distribution.
*/
#ifdef YOURGAME_EXTPROJ_bullet

#include <memory> // unique_ptr
#include <map>
#include <string>
#include <vector>

#include "btBulletDynamicsCommon.h"
#include "yourgame/util/physenv.h"

namespace yourgame
{
    namespace util
    {
        RigidBody::~RigidBody()
        {
            auto body = static_cast<btRigidBody *>(m_body);
            if (body->getMotionState())
            {
                delete body->getMotionState();
            }
            delete body;
        }

        yourgame::math::Trafo RigidBody::getTrafo()
        {
            yourgame::math::Trafo trafo;

            btTransform trans;
            auto body = static_cast<btRigidBody *>(m_body);
            body->getMotionState()->getWorldTransform(trans);

            auto eye = trans.getOrigin();
            auto rot = trans.getRotation();
            trafo.setTranslation({eye.getX(), eye.getY(), eye.getZ()});
            trafo.setRotation({rot.getW(), rot.getX(), rot.getY(), rot.getZ()});

            // perform shape specific scaling of the Trafo
            btCollisionShape *shape = body->getCollisionShape();
            auto shapeType = shape->getShapeType();
            switch (shapeType)
            {
            // scale the "unit cube" (box with x,y,z [-1,+1])
            // depending on the rigid body shape extends
            case BOX_SHAPE_PROXYTYPE:
            {
                btBoxShape *shape1 = static_cast<btBoxShape *>(shape);
                btVector3 bExtends = shape1->getHalfExtentsWithMargin();
                trafo.setScaleLocal({bExtends.getX(), bExtends.getY(), bExtends.getZ()});
            }
            break;
            case SPHERE_SHAPE_PROXYTYPE:
            {
                btSphereShape *shape1 = static_cast<btSphereShape *>(shape);
                trafo.setScaleLocal(shape1->getRadius());
            }
            break;
            // the default cylinder (btCylinderShape) is y axis aligned.
            // scale "unit cylinder" (y-aligned, y [-1,+1], radius = 1)
            // depending on the rigid body shape extends
            case CYLINDER_SHAPE_PROXYTYPE:
            {
                btCylinderShape *shape1 = static_cast<btCylinderShape *>(shape);
                btVector3 bExtends = shape1->getHalfExtentsWithMargin();
                trafo.setScaleLocal({bExtends.getX(), bExtends.getY(), bExtends.getX()});
            }
            break;
            // the default cone (btConeShape) is y axis aligned.
            // scale "unit cone" (y-aligned, y [-1,+1], radius = 1)
            // depending on the rigid body shape radius and height
            case CONE_SHAPE_PROXYTYPE:
            {
                btConeShape *shape1 = static_cast<btConeShape *>(shape);
                btScalar radius = shape1->getRadius();
                btScalar height = shape1->getHeight() * 0.5f;
                trafo.setScaleLocal({radius, height, radius});
            }
            break;
            default:
                break;
            }

            return trafo;
        }

        void RigidBody::setTrafo(yourgame::math::Trafo trafo)
        {
            auto eye = trafo.getEye();
            auto rot = trafo.getRotation();
            btTransform btTrafo({rot.x, rot.y, rot.z, rot.w},
                                {eye.x, eye.y, eye.z});

            auto body = static_cast<btRigidBody *>(m_body);
            body->getMotionState()->setWorldTransform(btTrafo);
        }

        void RigidBody::setRestitution(float restitution)
        {
            auto body = static_cast<btRigidBody *>(m_body);
            body->setRestitution(restitution);
        }

        void RigidBody::setFriction(float friction)
        {
            auto body = static_cast<btRigidBody *>(m_body);
            body->setFriction(friction);
        }

        void RigidBody::setSleepingThresholds(float linear, float angular)
        {
            auto body = static_cast<btRigidBody *>(m_body);
            body->setSleepingThresholds(linear, angular);
        }

        void RigidBody::setLinearFactors(float x, float y, float z)
        {
            auto body = static_cast<btRigidBody *>(m_body);
            body->setLinearFactor({x, y, z});
        }

        void RigidBody::setAngularFactor(float angFac)
        {
            auto body = static_cast<btRigidBody *>(m_body);
            body->setAngularFactor(angFac);
        }

        void RigidBody::setAngularFactors(float x, float y, float z)
        {
            auto body = static_cast<btRigidBody *>(m_body);
            body->setAngularFactor({x, y, z});
        }

        void RigidBody::applyCentralForce(float x, float y, float z)
        {
            auto body = static_cast<btRigidBody *>(m_body);
            body->applyCentralForce({x, y, z});
        }

        void RigidBody::applyCentralImpulse(float x, float y, float z)
        {
            auto body = static_cast<btRigidBody *>(m_body);
            body->applyCentralImpulse({x, y, z});
        }
    }
}

namespace yourgame
{
    namespace util
    {
        class PhysEnv::impl
        {
        public:
            std::map<std::string, std::unique_ptr<btCollisionShape>> m_shapes;
            std::map<std::string, std::unique_ptr<RigidBody>> m_rigidBodies;
            std::vector<Collision> m_collisions;

            std::unique_ptr<btDefaultCollisionConfiguration> m_collisionCfg;
            std::unique_ptr<btCollisionDispatcher> m_collisionDispatcher;
            std::unique_ptr<btDbvtBroadphase> m_broadphase;
            std::unique_ptr<btSequentialImpulseConstraintSolver> m_constraintSolver;
            std::unique_ptr<btDiscreteDynamicsWorld> m_dynamicsWorld;

            impl() : m_collisionCfg{new btDefaultCollisionConfiguration()},
                     m_collisionDispatcher{new btCollisionDispatcher(m_collisionCfg.get())},
                     m_broadphase{new btDbvtBroadphase()},
                     m_constraintSolver{new btSequentialImpulseConstraintSolver()},
                     m_dynamicsWorld{new btDiscreteDynamicsWorld(m_collisionDispatcher.get(),
                                                                 m_broadphase.get(),
                                                                 m_constraintSolver.get(),
                                                                 m_collisionCfg.get())}
            {
                // setInternalTickCallback() sets this (pointer to this impl instance) as
                // worldUserInfo in the dynamics world. used to retrieve the impl instance
                // in the actual callback impl::tickCallback()
                m_dynamicsWorld->setInternalTickCallback(tickCallback, this);
            }

            static void tickCallback(btDynamicsWorld *world, btScalar timeStep)
            {
                int numManifolds = world->getDispatcher()->getNumManifolds();
                for (int i = 0; i < numManifolds; i++)
                {
                    // the manifold stores contact points of a pair of objects
                    btPersistentManifold *manifold = world->getDispatcher()->getManifoldByIndexInternal(i);

                    // for each contact point of the manifold, add up the applied impulse
                    int numContacts = manifold->getNumContacts();
                    float totalImpulse = 0.0f;
                    for (int j = 0; j < numContacts; j++)
                    {
                        btManifoldPoint &pt = manifold->getContactPoint(j);
                        totalImpulse += pt.m_appliedImpulse;
                    }

                    // if a total impulse exists (> 0), create an actual Collision between object pair
                    if (totalImpulse > 0.0f)
                    {
                        auto *body0 = static_cast<const btCollisionObject *>(manifold->getBody0());
                        auto *body1 = static_cast<const btCollisionObject *>(manifold->getBody1());

                        auto implPtr = static_cast<PhysEnv::impl *>(world->getWorldUserInfo());
                        implPtr->m_collisions.emplace_back(static_cast<RigidBody *>(body0->getUserPointer()),
                                                           static_cast<RigidBody *>(body1->getUserPointer()),
                                                           totalImpulse);
                    }
                }
            }
        };

        PhysEnv::PhysEnv() : pimpl{new impl{}} {}

        // we need to define the (default) destructor here, after impl is complete
        // (defined above) to make the destructor work, as it needs to know impl
        // in order to destruct impl.
        PhysEnv::~PhysEnv() = default;

        void PhysEnv::setGravity(float x, float y, float z)
        {
            pimpl->m_dynamicsWorld->setGravity({x, y, z});
        }

        bool PhysEnv::newBoxShape(const std::string &name, float halfX, float halfY, float halfZ)
        {
            auto emplaceRet = pimpl->m_shapes.emplace(name, new btBoxShape(btVector3(halfX, halfY, halfZ)));
            return emplaceRet.second;
        }

        bool PhysEnv::newSphereShape(const std::string &name, float radius)
        {
            auto emplaceRet = pimpl->m_shapes.emplace(name, new btSphereShape(radius));
            return emplaceRet.second;
        }

        bool PhysEnv::newCylinderShape(const std::string &name, float radius, float height)
        {
            auto emplaceRet = pimpl->m_shapes.emplace(name, new btCylinderShape(btVector3(radius, height, radius)));
            return emplaceRet.second;
        }

        bool PhysEnv::newConeShape(const std::string &name, float radius, float height)
        {
            auto emplaceRet = pimpl->m_shapes.emplace(name, new btConeShape(radius, height));
            return emplaceRet.second;
        }

        bool PhysEnv::deleteShape(const std::string &name)
        {
            return pimpl->m_shapes.erase(name) > 0;
        }

        size_t PhysEnv::clearShapes()
        {
            auto num = pimpl->m_shapes.size();
            pimpl->m_shapes.clear();
            return num;
        }

        size_t PhysEnv::numShapes()
        {
            return pimpl->m_shapes.size();
        }

        bool PhysEnv::newRigidBody(const std::string &name,
                                   const std::string &shapeName,
                                   const yourgame::math::Trafo &trafo,
                                   const RigidBodyInfo &info)
        {
            if (pimpl->m_rigidBodies.find(name) == pimpl->m_rigidBodies.end() &&
                pimpl->m_shapes.find(shapeName) != pimpl->m_shapes.end())
            {
                // kinematic bodies must have zero mass
                float mass = info.kinematic ? 0.0f : info.mass;

                btVector3 inertia(0, 0, 0);
                pimpl->m_shapes[shapeName].get()->calculateLocalInertia(mass, inertia);

                auto eye = trafo.getEye();
                auto rot = trafo.getRotation();
                btTransform btTrafo({rot.x, rot.y, rot.z, rot.w},
                                    {eye.x, eye.y, eye.z});

                btRigidBody::btRigidBodyConstructionInfo rbInfo(mass,
                                                                new btDefaultMotionState(btTrafo),
                                                                pimpl->m_shapes[shapeName].get(),
                                                                inertia);

                rbInfo.m_linearDamping = info.linearDamping;
                rbInfo.m_angularDamping = info.angularDamping;
                rbInfo.m_friction = info.friction;
                rbInfo.m_rollingFriction = info.rollingFriction;
                rbInfo.m_spinningFriction = info.spinningFriction;
                rbInfo.m_restitution = info.restitution;
                rbInfo.m_linearSleepingThreshold = info.linearSleepingThreshold;
                rbInfo.m_angularSleepingThreshold = info.angularSleepingThreshold;

                auto newBody = new btRigidBody(rbInfo);

                if (info.disableDeactivation | info.kinematic)
                {
                    newBody->setActivationState(DISABLE_DEACTIVATION);
                }

                if (info.kinematic)
                {
                    newBody->setCollisionFlags(newBody->getCollisionFlags() |
                                               btCollisionObject::CF_KINEMATIC_OBJECT);
                }

                auto newRigidBody = new RigidBody(name, newBody);
                newBody->setUserPointer(static_cast<void *>(newRigidBody));

                pimpl->m_dynamicsWorld->addRigidBody(newBody);
                pimpl->m_rigidBodies[name] = std::unique_ptr<RigidBody>(newRigidBody);

                return true;
            }
            return false;
        }

        bool PhysEnv::deleteRigidBody(const std::string &name)
        {
            auto it = pimpl->m_rigidBodies.find(name);
            if (it != pimpl->m_rigidBodies.end())
            {
                auto body = static_cast<btRigidBody *>(it->second->m_body);

                pimpl->m_dynamicsWorld->removeRigidBody(body);
                pimpl->m_rigidBodies.erase(it);

                return true;
            }

            return false;
        }

        RigidBody *PhysEnv::getRigidBody(const std::string &name)
        {
            auto it = pimpl->m_rigidBodies.find(name);
            if (it != pimpl->m_rigidBodies.end())
            {
                return {it->second.get()};
            }
            return nullptr;
        }

        std::vector<RigidBody *> PhysEnv::getRigidBodiesStartingWith(const std::string &name)
        {
            std::vector<RigidBody *> bodies;

            for (const auto &b : pimpl->m_rigidBodies)
            {
                if (b.first.substr(0, name.size()) == name)
                {
                    bodies.push_back(b.second.get());
                }
            }

            return bodies;
        }

        size_t PhysEnv::clearRigidBodies()
        {
            auto num = pimpl->m_rigidBodies.size();

            for (int i = pimpl->m_dynamicsWorld->getNumCollisionObjects() - 1; i >= 0; i--)
            {
                auto collObj = pimpl->m_dynamicsWorld->getCollisionObjectArray()[i];
                btRigidBody *body = btRigidBody::upcast(collObj);
                pimpl->m_dynamicsWorld->removeCollisionObject(collObj);
            }

            pimpl->m_rigidBodies.clear();
            return num;
        }

        size_t PhysEnv::numRigidBodies()
        {
            return pimpl->m_rigidBodies.size();
        }

        void PhysEnv::tick(float dt)
        {
            pimpl->m_collisions.clear();
            pimpl->m_dynamicsWorld->stepSimulation(dt, 10);
        }

        std::vector<Collision> PhysEnv::getCollisions()
        {
            return pimpl->m_collisions;
        }

        RayTestResult PhysEnv::rayTest(float eyeX, float eyeY, float eyeZ, float targetX, float targetY, float targetZ)
        {
            btVector3 btFrom(eyeX, eyeY, eyeZ);
            btVector3 btTo(targetX, targetY, targetZ);

            btCollisionWorld::ClosestRayResultCallback res(btFrom, btTo);
            pimpl->m_dynamicsWorld->rayTest(btFrom, btTo, res);

            RigidBody *hitBody = nullptr;
            if (res.hasHit())
            {
                // find our RigidBody wrapping the underlying body that was hit
                for (const auto &b : pimpl->m_rigidBodies)
                {
                    auto *collObj = const_cast<btCollisionObject *>(res.m_collisionObject);
                    if (static_cast<void *>(collObj) == static_cast<void *>(b.second->m_body))
                    {
                        hitBody = b.second.get();
                        break;
                    }
                }
            }

            return RayTestResult(hitBody);
        }
    }
} // namespace yourgame

#endif
