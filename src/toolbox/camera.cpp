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
#include <glm/glm.hpp>
#include "yourgame/camera.h"

namespace yourgame{

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
        if (m_proj == PROJECTION::PERSPECTIVE)
        {
            m_pMat = glm::perspective(m_fovy * 0.01745329251f, m_aspect, m_zNear, m_zFar);
        }
        else
        {
            float heightHalf = m_height * 0.5f;
            float widthHalf = heightHalf * m_aspect;
            m_pMat = glm::ortho(-widthHalf, widthHalf, -heightHalf, heightHalf, m_zNear, m_zFar);
        }
        m_pMatInvalidated = false;
    }
}

}