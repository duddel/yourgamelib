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
#ifndef YOURGAME_TRAFO_H
#define YOURGAME_TRAFO_H

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include "yourgame/math.h"

namespace yourgame
{
    namespace math
    {
        class Trafo
        {
        public:
            glm::mat4 mat();
            void rotateLocal(float angle, math::Axis ax);
            void rotateLocal(float angle, glm::vec3 const &ax);
            void rotateGlobal(float angle, math::Axis ax);
            void rotateGlobal(float angle, glm::vec3 const &ax);
            void translateLocal(float distance, math::Axis ax);
            void translateLocal(glm::vec3 const &trans);
            void translateGlobal(float distance, math::Axis ax);
            void translateGlobal(glm::vec3 const &trans);
            void setScaleLocal(glm::vec3 const &scale);
            void setScaleLocal(float scale);
            void setRotation(glm::quat const &rot);
            void setTranslation(glm::vec3 const &trans);
            void resetScale();
            void resetRotation();
            void resetTranslation();
            void pointTo(glm::vec3 const &eye, glm::vec3 const &center, glm::vec3 const &up);
            void lookAt(glm::vec3 const &eye, glm::vec3 const &center, glm::vec3 const &up);
            void lerp(float t, Trafo const &target, Trafo *dst);
            void setIdentity();
            glm::vec3 getAxisLocal(math::Axis ax);
            glm::vec3 getAxisGlobal(math::Axis ax) const;
            glm::vec3 getEye() const;
            glm::quat getRotation() const;
            glm::vec3 getScale() const;

        private:
            glm::vec3 m_eye = glm::vec3(0.0f, 0.0f, 0.0f);
            glm::quat m_rot = glm::quat(1.0f, 0.0f, 0.0f, 0.0f);
            glm::vec3 m_scale = glm::vec3(1.0f, 1.0f, 1.0f);

            // Matrixes generated from the above vectors via updateMatrixIfInvalidated()
            glm::mat4 m_mat = glm::mat4(1.0f);    // identity
            glm::mat4 m_matRot = glm::mat4(1.0f); // identity

            bool m_matInvalidated = false;
            void updateMatrixIfInvalidated();
        };
    } // namespace math
} // namespace yourgame

#endif
