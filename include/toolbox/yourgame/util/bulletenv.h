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
#ifndef YOURGAME_BULLETENV_H
#define YOURGAME_BULLETENV_H

#if defined(YOURGAME_EXTPROJ_bullet) || defined(YG_DOXYGEN)

#include <map>
#include <string>
#include "btBulletDynamicsCommon.h"

namespace yourgame
{
    namespace util
    {
        /** \brief helper class for bullet physics */
        class BulletEnv
        {
        public:
            BulletEnv();
            ~BulletEnv();
            btDiscreteDynamicsWorld *m_dynamicsWorld;
            std::map<std::string, btCollisionShape *> m_shapes;

        private:
            btDefaultCollisionConfiguration *m_collisionCfg;
            btCollisionDispatcher *m_collisionDispatcher;
            btDbvtBroadphase *m_broadphase;
            btSequentialImpulseConstraintSolver *m_constraintSolver;
        };
    }
} // namespace yourgame

#endif

#endif
