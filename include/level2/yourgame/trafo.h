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
#ifndef YOURGAME_TRAFO_H
#define YOURGAME_TRAFO_H

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/ext.hpp>

namespace yourgame
{

class Trafo
{
public:
    enum class AXIS
    {
        X,
        Y,
        Z
    };

    glm::mat4 mat();
    void rotateLocal(float angle, AXIS ax);
    void rotateGlobal(float angle, AXIS ax);
    void translateLocal(float distance, AXIS ax);
    void translateGlobal(float distance, AXIS ax);
    void setScaleLocal(glm::vec3 const &scale);
    void setScaleLocal(float scale);
    void pointTo(glm::vec3 const &eye, glm::vec3 const &center, glm::vec3 const &up);
    void lookAt(glm::vec3 const &eye, glm::vec3 const &center, glm::vec3 const &up);

private:
    glm::vec3 m_eye = glm::vec3(0.0f, 0.0f, 0.0f);
    glm::quat m_rot = glm::quat(1.0f, 0.0f, 0.0f, 0.0f);
    glm::vec3 m_scale = glm::vec3(1.0f, 1.0f, 1.0f);
    glm::mat4 m_mat = glm::mat4(1.0f, 0.0f, 0.0f, 0.0f,
                                0.0f, 1.0f, 0.0f, 0.0f,
                                0.0f, 0.0f, 1.0f, 0.0f,
                                0.0f, 0.0f, 0.0f, 1.0f);
    bool m_matInvalidated = false;
    void updateMatrixIfInvalidated();
    glm::vec3 getAxisLocal(AXIS ax);
    glm::vec3 getAxisGlobal(AXIS ax);
};

}

#endif
