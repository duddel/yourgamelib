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
#include "doctest.h"
#include "yourgame/yourgame.h"

namespace yg = yourgame; // convenience

TEST_CASE("PhysEnv")
{
    yg::util::PhysEnv env;

    SUBCASE("small simulation")
    {
        env.setGravity(0.0f, -9.81f, 0.0f);

        // shapes
        env.newBoxShape("groundbox", 10.0f, 1.0f, 10.0f);
        env.newBoxShape("box1", 1.0f, 1.0f, 1.0f);

        // ground body
        {
            yg::math::Trafo trafo;
            yg::util::RigidBodyInfo rbInfo;
            rbInfo.mass = 0.0f;
            rbInfo.restitution = 0.5f;
            env.newRigidBody("ground", "groundbox", trafo, rbInfo);
        }

        // box body
        {
            yg::math::Trafo trafo;
            trafo.translateLocal(3.0f, yg::math::Axis::Y);
            yg::util::RigidBodyInfo rbInfo;
            rbInfo.restitution = 0.9f;
            env.newRigidBody("box", "box1", trafo, rbInfo);
        }

        // small simulation. box hits ground
        for (int i = 0; i < 45; i++)
        {
            env.tick(1.f / 30.f);

            auto *boxBody = env.getRigidBody("box");
            if (boxBody)
            {
                yg::log::info("box y: %v", boxBody->getTrafo().getEye().y);
            }
            else
            {
                yg::log::info("ending simulation.");
                break;
            }

            auto cols = env.getCollisions();
            for (const auto &col : cols)
            {
                if (col.involves2("box", "ground"))
                {
                    yg::log::info("%v hit %v, impact: %v. box destroyed.", col.m_body0->m_name, col.m_body1->m_name, col.m_impulse);
                    CHECK(env.deleteRigidBody("box") == true);
                    break;
                }
            }
        }
    }

    SUBCASE("getRigidBodiesStartingWith()")
    {
        env.newBoxShape("box1", 1.0f, 1.0f, 1.0f);

        // add some boxes
        {
            yg::math::Trafo trafo;
            yg::util::RigidBodyInfo rbInfo;
            env.newRigidBody("box1", "box1", trafo, rbInfo);
            env.newRigidBody("box2", "box1", trafo, rbInfo);
            env.newRigidBody("box3", "box1", trafo, rbInfo);
        }

        // query bodies with getRigidBodiesStartingWith()
        CHECK(env.getRigidBodiesStartingWith("box").size() == 3);
        CHECK(env.getRigidBodiesStartingWith("player").empty());
        CHECK(env.getRigidBodiesStartingWith("verylongbodyname").empty());
    }

    SUBCASE("deleteShape()")
    {
        env.newBoxShape("groundbox", 10.0f, 1.0f, 10.0f);
        CHECK(env.deleteShape("groundbox") == true);
        CHECK(env.deleteShape("groundbox") == false);
    }
}
