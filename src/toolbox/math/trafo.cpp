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
#include "yourgame/math/trafo.h"

namespace yourgame
{
    namespace math
    {
        glm::mat4 Trafo::mat()
        {
            updateMatrixIfInvalidated();
            return m_mat;
        }

        void Trafo::rotateLocal(float angle, math::Axis ax)
        {
            // local rotation: use _global_ axis and this quaternion order:
            m_rot = m_rot * glm::angleAxis(angle, getAxisGlobal(ax));
            m_matInvalidated = true;
        }

        void Trafo::rotateLocal(float angle, glm::vec3 const &ax)
        {
            m_rot = m_rot * glm::angleAxis(angle, glm::normalize(ax));
            m_matInvalidated = true;
        }

        void Trafo::rotateGlobal(float angle, math::Axis ax)
        {
            // global rotation: use this quaternion order:
            m_rot = glm::angleAxis(angle, getAxisGlobal(ax)) * m_rot;
            m_matInvalidated = true;
        }

        void Trafo::rotateGlobal(float angle, glm::vec3 const &ax)
        {
            m_rot = glm::angleAxis(angle, normalize(ax)) * m_rot;
            m_matInvalidated = true;
        }

        void Trafo::translateLocal(float distance, math::Axis ax)
        {
            m_eye += (getAxisLocal(ax) * distance);
            m_matInvalidated = true;
        }

        void Trafo::translateLocal(glm::vec3 const &trans)
        {
            translateLocal(trans.x, math::Axis::X);
            translateLocal(trans.y, math::Axis::Y);
            translateLocal(trans.z, math::Axis::Z);
            m_matInvalidated = true;
        }

        void Trafo::translateGlobal(float distance, math::Axis ax)
        {
            m_eye += (getAxisGlobal(ax) * distance);
            m_matInvalidated = true;
        }

        void Trafo::translateGlobal(glm::vec3 const &trans)
        {
            m_eye += trans;
            m_matInvalidated = true;
        }

        void Trafo::setScaleLocal(glm::vec3 const &scale)
        {
            m_scale = scale;
            m_matInvalidated = true;
        }

        void Trafo::setScaleLocal(float scale)
        {
            m_scale = glm::vec3(scale);
            m_matInvalidated = true;
        }

        void Trafo::setRotation(glm::quat const &rot)
        {
            m_rot = rot;
            m_matInvalidated = true;
        }

        void Trafo::setTranslation(glm::vec3 const &trans)
        {
            m_eye = trans;
            m_matInvalidated = true;
        }

        void Trafo::pointTo(glm::vec3 const &eye, glm::vec3 const &center, glm::vec3 const &up)
        {
            auto dir = glm::normalize(center - eye);
            // LH intentionally used to keep direction (dir) in +z coordinate
            m_rot = glm::quatLookAtLH(dir, up);
            m_eye = eye;
            m_matInvalidated = true;
        }

        void Trafo::lookAt(glm::vec3 const &eye, glm::vec3 const &center, glm::vec3 const &up)
        {
            auto dir = glm::normalize(center - eye);
            m_rot = glm::quatLookAt(dir, up);
            m_eye = eye;
            m_matInvalidated = true;
        }

        void Trafo::lerp(float t, Trafo const &target, Trafo *dst)
        {
            if (dst != nullptr)
            {
                auto t01 = glm::clamp(t, 0.0f, 1.0f);
                dst->m_eye = glm::lerp(m_eye, target.m_eye, t01);
                dst->m_rot = glm::lerp(m_rot, target.m_rot, t01);
                dst->m_scale = glm::lerp(m_scale, target.m_scale, t01);
                dst->m_matInvalidated = true;
            }
        }

        void Trafo::updateMatrixIfInvalidated()
        {
            if (m_matInvalidated)
            {
                m_matRot = glm::mat4(m_rot);
                m_mat = glm::translate(m_eye) * m_matRot * glm::scale(m_scale);
                m_matInvalidated = false;
            }
        }

        void Trafo::setIdentity()
        {
            m_eye = glm::vec3(0.0f, 0.0f, 0.0f);
            m_rot = glm::quat(1.0f, 0.0f, 0.0f, 0.0f);
            m_scale = glm::vec3(1.0f, 1.0f, 1.0f);
            m_matInvalidated = true;
        }

        glm::vec3 Trafo::getAxisLocal(math::Axis ax)
        {
            updateMatrixIfInvalidated();
            switch (ax)
            {
            default:
            case math::Axis::X:
                return m_matRot[0];
                break;
            case math::Axis::Y:
                return m_matRot[1];
                break;
            case math::Axis::Z:
                return m_matRot[2];
                break;
            }
        }

        glm::vec3 Trafo::getAxisGlobal(math::Axis ax) const
        {
            switch (ax)
            {
            default:
            case math::Axis::X:
                return glm::vec3(1.0f, 0.0f, 0.0f);
                break;
            case math::Axis::Y:
                return glm::vec3(0.0f, 1.0f, 0.0f);
                break;
            case math::Axis::Z:
                return glm::vec3(0.0f, 0.0f, 1.0f);
                break;
            }
        }

        glm::vec3 Trafo::getEye() const
        {
            return m_eye;
        }

        glm::quat Trafo::getRotation() const
        {
            return m_rot;
        }

        glm::vec3 Trafo::getScale() const
        {
            return m_scale;
        }
    } // namespace math
} // namespace yourgame
