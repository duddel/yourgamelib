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
#include "yourgame/gllightsource.h"

namespace yourgame
{
    Trafo *GLLightsource::trafo()
    {
        m_dataInvalidated = true; // assuming it gets changed
        return &m_trafo;
    }

    std::array<float, 12> GLLightsource::data()
    {
        updateDataIfInvalidated();
        return m_data;
    }

    glm::vec3 GLLightsource::ambient()
    {
        return m_ambient;
    }

    glm::vec3 GLLightsource::diffuse()
    {
        return m_diffuse;
    }

    glm::vec3 GLLightsource::specular()
    {
        return m_specular;
    }

    void GLLightsource::setAmbient(glm::vec3 col)
    {
        m_ambient = col;
    }

    void GLLightsource::setDiffuse(glm::vec3 col)
    {
        m_diffuse = col;
    }

    void GLLightsource::setSpecular(glm::vec3 col)
    {
        m_specular = col;
    }

    void GLLightsource::updateDataIfInvalidated()
    {
        if (m_dataInvalidated)
        {
            auto eye = m_trafo.getEye();
            for (int i = 0; i < 3; i++)
            {
                m_data[i] = m_ambient[i];
                m_data[3 + i] = m_diffuse[i];
                m_data[6 + i] = m_specular[i];
                m_data[9 + i] = eye[i];
            }
            m_dataInvalidated = false;
        }
    }
} // namespace yourgame
