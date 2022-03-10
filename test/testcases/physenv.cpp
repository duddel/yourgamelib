/*
Copyright (c) 2019-2022 Alexander Scholz

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
#include <cassert>
#include "yourgame/yourgame.h"

namespace yg = yourgame; // convenience

namespace mygame
{
    void init(int argc, char *argv[]) {}
    void shutdown() {}

    void tick()
    {
        // exit after first cycle
        yg::control::exit();

        // construct, destruct
        {
            yg::util::PhysEnv env;
        }

        yg::util::PhysEnv env;
        env.setGravity(0.0f, -9.81f, 0.0f);

        // add shapes
        env.newBoxShape("groundbox", 10.0f, 1.0f, 10.0f);
        env.newBoxShape("box1", 1.0f, 1.0f, 1.0f);

        // bodies
        env.newRigidBody("ground", 0.0f, 0.0f, 0.0f, 0.0f, "groundbox", 0.5f);
        env.newRigidBody("box", 1.0f, 0.0f, 3.0f, 0.0f, "box1", 0.9f);

        // small simulation. box hits ground
        for (int i = 0; i < 45; i++)
        {
            env.tick(1.f / 30.f);

            auto *boxBody = env.getRigidBody("box");
            if (boxBody)
            {
                yg::log::info("box y: %v", boxBody->getTrafo().getEye().y);
            }

            auto cols = env.getCollisions();
            for (const auto &col : cols)
            {
                if (col.involves("box", "ground"))
                {
                    yg::log::info("%v hit %v, impact: %v. box destroyed.", col.m_body0->m_name, col.m_body1->m_name, col.m_impulse);
                    env.deleteRigidBody("box");
                    break;
                }
            }
        }

        // add some boxes
        env.newRigidBody("box1", 1.0f, 0.0f, 3.0f, 0.0f, "box1", 0.9f);
        env.newRigidBody("box2", 1.0f, 0.0f, 3.0f, 0.0f, "box1", 0.9f);
        env.newRigidBody("box3", 1.0f, 0.0f, 3.0f, 0.0f, "box1", 0.9f);

        // query bodies with getRigidBodiesStartingWith()
        assert(env.getRigidBodiesStartingWith("box").size() == 3);
        assert(env.getRigidBodiesStartingWith("ground").size() == 1);
        assert(env.getRigidBodiesStartingWith("player").empty());
        assert(env.getRigidBodiesStartingWith("verylongbodyname").empty());

        // delete shapes
        assert(env.deleteShape("groundbox") == true);
        assert(env.deleteShape("groundbox") == false);
    }
} // namespace mygame
