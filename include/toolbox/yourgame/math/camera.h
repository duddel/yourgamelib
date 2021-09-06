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
#ifndef YOURGAME_CAMERA_H
#define YOURGAME_CAMERA_H

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include "yourgame/math.h"
#include "yourgame/math/trafo.h"

namespace yourgame
{
    namespace math
    {
        class Camera
        {
        public:
            Trafo *trafo();
            glm::mat4 vMat();
            glm::mat4 pMat();
            glm::mat4 pMat(float zNear, float zFar);
            void setProjection(math::Projection proj);
            void setPerspective(float fovy, float aspect, float zNear, float zFar);
            void setOrthographic(float height, float aspect, float zNear, float zFar);
            void setFovy(float fovy);
            void setHeight(float height);
            void setAspect(float aspect);
            void setZNear(float zNear);
            void setZFar(float zFar);
            /** \brief viewportX,viewportY in [0.0,1.0] */
            void castRay(float viewportX, float viewportY, glm::vec3 &dstOrg, glm::vec3 &dstDir);

        private:
            Trafo m_trafo;
            math::Projection m_proj = math::Projection::PERSPECTIVE;
            float m_fovy = 90.0f;
            float m_height = 1.0f;
            float m_aspect = 1.0f;
            float m_zNear = 0.1f;
            float m_zFar = 10.0f;
            glm::mat4 m_vMat = glm::mat4(1.0f, 0.0f, 0.0f, 0.0f,
                                         0.0f, 1.0f, 0.0f, 0.0f,
                                         0.0f, 0.0f, 1.0f, 0.0f,
                                         0.0f, 0.0f, 0.0f, 1.0f);
            bool m_vMatInvalidated = false;
            void updateVMatIfInvalidated();

            glm::mat4 m_pMat = glm::mat4(1.0f, 0.0f, 0.0f, 0.0f,
                                         0.0f, 1.0f, 0.0f, 0.0f,
                                         0.0f, 0.0f, 1.0f, 0.0f,
                                         0.0f, 0.0f, 0.0f, 1.0f);
            bool m_pMatInvalidated = false;
            void updatePMatIfInvalidated();
            glm::mat4 calcPMat(float zNear, float zFar);
        };
    }
} // namespace yourgame

#endif
