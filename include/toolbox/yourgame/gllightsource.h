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
#ifndef YOURGAME_GLLIGHTSOURCE_H
#define YOURGAME_GLLIGHTSOURCE_H

#include <array>
#include "yourgame/trafo.h"

namespace yourgame
{
    class GLLightsource
    {
    public:
        Trafo *trafo();
        std::array<float, 12> data();
        glm::vec3 ambient();
        glm::vec3 diffuse();
        glm::vec3 specular();
        void setAmbient(glm::vec3 col);
        void setDiffuse(glm::vec3 col);
        void setSpecular(glm::vec3 col);

    private:
        void updateDataIfInvalidated();
        bool m_dataInvalidated = true;
        Trafo m_trafo;
        std::array<float, 12> m_data;
        glm::vec3 m_ambient = glm::vec3(0.0);
        glm::vec3 m_diffuse = glm::vec3(1.0);
        glm::vec3 m_specular = glm::vec3(1.0);
    };
} // namespace yourgame

#endif
