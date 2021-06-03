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
#include <glm/glm.hpp>
#include "yourgame/camera.h"

namespace yourgame
{
    glm::mat4 Camera::vMat()
    {
        updateVMatIfInvalidated();
        return m_vMat;
    }

    glm::mat4 Camera::pMat()
    {
        updatePMatIfInvalidated();
        return m_pMat;
    }

    glm::mat4 Camera::pMat(float zNear, float zFar)
    {
        return calcPMat(zNear, zFar);
    }

    Trafo *Camera::trafo()
    {
        m_vMatInvalidated = true; // assuming it gets changed
        return &m_trafo;
    }

    void Camera::setProjection(PROJECTION proj)
    {
        if (proj != m_proj)
        {
            m_proj = proj;
            m_pMatInvalidated = true;
        }
    }

    void Camera::setPerspective(float fovy, float aspect, float zNear, float zFar)
    {
        m_fovy = fovy;
        m_aspect = aspect;
        m_zNear = zNear;
        m_zFar = zFar;
        m_proj = PROJECTION::PERSPECTIVE;
        m_pMatInvalidated = true;
    }

    void Camera::setOrthographic(float height, float aspect, float zNear, float zFar)
    {
        m_height = height;
        m_aspect = aspect;
        m_zNear = zNear;
        m_zFar = zFar;
        m_proj = PROJECTION::ORTHOGRAPHIC;
        m_pMatInvalidated = true;
    }

    void Camera::setFovy(float fovy)
    {
        m_fovy = fovy;
        if (m_proj == PROJECTION::PERSPECTIVE)
        {
            m_pMatInvalidated = true;
        }
    }

    void Camera::setHeight(float height)
    {
        m_height = height;
        if (m_proj == PROJECTION::ORTHOGRAPHIC)
        {
            m_pMatInvalidated = true;
        }
    }

    void Camera::setAspect(float aspect)
    {
        m_aspect = aspect;
        m_pMatInvalidated = true;
    }

    void Camera::setZNear(float zNear)
    {
        m_zNear = zNear;
        m_pMatInvalidated = true;
    }

    void Camera::setZFar(float zFar)
    {
        m_zFar = zFar;
        m_pMatInvalidated = true;
    }

    void Camera::castRay(float viewportX, float viewportY, glm::vec3 &dstOrg, glm::vec3 &dstDir)
    {
        float viewX = ((viewportX * 2.0f) - 1.0f);
        float viewY = (1.0f - (viewportY * 2.0f));

        if (m_proj == PROJECTION::PERSPECTIVE)
        {
            float tanFovyHalf = std::tan(m_fovy * 0.00872664625f); // pi/180/2 = 0.00872664625
            float dirX = tanFovyHalf * viewX * m_aspect;
            float dirY = tanFovyHalf * viewY;
            float dirZ = -1.0f;
            dstDir = glm::normalize(glm::vec3(m_trafo.mat() * glm::vec4(dirX, dirY, dirZ, 0.0f)));
            dstOrg = m_trafo.getEye();
        }
        else
        {
            float orgX = viewX * m_height * 0.5f * m_aspect;
            float orgY = viewY * m_height * 0.5f;
            float orgZ = 0.0f;
            dstOrg = glm::normalize(glm::vec3(m_trafo.mat() * glm::vec4(orgX, orgY, orgZ, 1.0f)));
            dstDir = -m_trafo.getAxisLocal(yourgame::Trafo::AXIS::Z);
        }
    }

    void Camera::updateVMatIfInvalidated()
    {
        if (m_vMatInvalidated)
        {
            m_vMat = glm::inverse(m_trafo.mat());
            m_vMatInvalidated = false;
        }
    }

    void Camera::updatePMatIfInvalidated()
    {
        if (m_pMatInvalidated)
        {
            m_pMat = calcPMat(m_zNear, m_zFar);
            m_pMatInvalidated = false;
        }
    }

    glm::mat4 Camera::calcPMat(float zNear, float zFar)
    {
        if (m_proj == PROJECTION::PERSPECTIVE)
        {
            return glm::perspective(m_fovy * 0.01745329251f, m_aspect, zNear, zFar);
        }
        else
        {
            float heightHalf = m_height * 0.5f;
            float widthHalf = heightHalf * m_aspect;
            return glm::ortho(-widthHalf, widthHalf, -heightHalf, heightHalf, zNear, zFar);
        }
    }
} // namespace yourgame
