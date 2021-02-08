/*
Copyright (c) 2019-2021 Alexander Scholz

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
#include "yourgame/bulletenv.h"

namespace yourgame
{
    BulletEnv::BulletEnv()
    {
        m_collisionCfg = new btDefaultCollisionConfiguration();
        m_collisionDispatcher = new btCollisionDispatcher(m_collisionCfg);
        m_broadphase = new btDbvtBroadphase();
        m_constraintSolver = new btSequentialImpulseConstraintSolver();
        m_dynamicsWorld = new btDiscreteDynamicsWorld(m_collisionDispatcher, m_broadphase, m_constraintSolver, m_collisionCfg);
    }

    BulletEnv::~BulletEnv()
    {
        // remove all rigid bodies from world, delete them and their motion state
        for (int i = m_dynamicsWorld->getNumCollisionObjects() - 1; i >= 0; i--)
        {
            auto collObj = m_dynamicsWorld->getCollisionObjectArray()[i];
            btRigidBody *body = btRigidBody::upcast(collObj);
            if (body && body->getMotionState())
            {
                delete body->getMotionState();
            }
            m_dynamicsWorld->removeCollisionObject(collObj);
            delete collObj;
        }

        // delete all shapes
        for (const auto &s : m_shapes)
        {
            delete s.second;
        }
        m_shapes.clear();

        delete m_dynamicsWorld;
        delete m_constraintSolver;
        delete m_broadphase;
        delete m_collisionDispatcher;
        delete m_collisionCfg;
    }
} // namespace yourgame
