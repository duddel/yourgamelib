/*
Copyright (c) 2019-2020 Alexander Scholz

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
#include "yourgame/trafo.h"

namespace yourgame{

glm::mat4 Trafo::mat()
{
    updateMatrixIfInvalidated();
    return m_mat;
}

void Trafo::rotateLocal(float angle, AXIS ax)
{
    // local rotation: use _global_ axis and this quaternion order:
    m_rot = m_rot * glm::angleAxis(angle, getAxisGlobal(ax));
    m_matInvalidated = true;
}

void Trafo::rotateGlobal(float angle, AXIS ax)
{
    // global rotation: use this quaternion order:
    m_rot = glm::angleAxis(angle, getAxisGlobal(ax)) * m_rot;
    m_matInvalidated = true;
}

void Trafo::translateLocal(float distance, AXIS ax)
{
    m_eye += (getAxisLocal(ax) * distance);
    m_matInvalidated = true;
}

void Trafo::translateGlobal(float distance, AXIS ax)
{
    m_eye += (getAxisGlobal(ax) * distance);
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

void Trafo::updateMatrixIfInvalidated()
{
    if (m_matInvalidated)
    {
        m_mat = glm::translate(m_eye) * glm::mat4(m_rot) * glm::scale(m_scale);
        m_matInvalidated = false;
    }
}

glm::vec3 Trafo::getAxisLocal(AXIS ax)
{
    updateMatrixIfInvalidated();
    switch (ax)
    {
    case AXIS::X:
        return m_mat[0];
        break;
    case AXIS::Y:
        return m_mat[1];
        break;
    case AXIS::Z:
        return m_mat[2];
        break;
    }
}

glm::vec3 Trafo::getAxisGlobal(AXIS ax)
{
    switch (ax)
    {
    case AXIS::X:
        return glm::vec3(1.0f, 0.0f, 0.0f);
        break;
    case AXIS::Y:
        return glm::vec3(0.0f, 1.0f, 0.0f);
        break;
    case AXIS::Z:
        return glm::vec3(0.0f, 0.0f, 1.0f);
        break;
    }
}

}
