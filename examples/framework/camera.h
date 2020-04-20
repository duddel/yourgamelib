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
#ifndef CAMERA_H
#define CAMERA_H

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include "trafo.h"

class Camera
{
public:
    enum class PROJECTION
    {
        PERSPECTIVE,
        ORTHOGRAPHIC
    };

    Trafo *trafo();
    glm::mat4 vMat();
    glm::mat4 pMat();
    void setProjection(PROJECTION proj);
    void setPerspective(float fovy, float aspect, float zNear, float zFar);
    void setOrthographic(float height, float aspect, float zNear, float zFar);
    void setFovy(float fovy);
    void setHeight(float height);
    void setAspect(float aspect);
    void setZNear(float zNear);
    void setZFar(float zFar);

private:
    Trafo m_trafo;
    PROJECTION m_proj = PROJECTION::PERSPECTIVE;
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
};

#endif
