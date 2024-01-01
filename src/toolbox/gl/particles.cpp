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
#include <glm/glm.hpp>
#include "yourgame/gl/particles.h"
#include "yourgame/gl/conventions.h"

namespace yourgame
{
    namespace gl
    {
        Particles *Particles::make(const Config cfg, yourgame::gl::Geometry *geo)
        {
            if (!geo)
            {
                return nullptr;
            }

            Particles *newParts = new Particles(cfg, geo);

            GLsizei vec4Size = static_cast<GLsizei>(sizeof(glm::vec4));
            // todo: handle errors during addBuffer()
            newParts->m_geo->addBuffer("instModelPos", GL_ARRAY_BUFFER, 0, nullptr, GL_DYNAMIC_DRAW);
            newParts->m_geo->addBuffer("instProgress", GL_ARRAY_BUFFER, 0, nullptr, GL_DYNAMIC_DRAW);
            newParts->m_geo->addBufferToShape("main", {{gl::attrLocInstModelMatCol3, 4, GL_FLOAT, GL_FALSE, vec4Size, (void *)0, 1}}, "instModelPos");
            newParts->m_geo->addBufferToShape("main", {{gl::attrLocInstProgress, 1, GL_FLOAT, GL_FALSE, sizeof(GLfloat), (void *)0, 1}}, "instProgress");

            return newParts;
        }

        Particles::~Particles()
        {
            delete m_geo;
        }

        void Particles::tick(float dt)
        {
            yourgame::math::Particles::tick(dt);
            m_geo->bufferData("instModelPos", m_positionData.size() * sizeof(m_positionData[0]), m_positionData.data());
            m_geo->bufferData("instProgress", m_progressData.size() * sizeof(m_progressData[0]), m_progressData.data());
        }
        yourgame::gl::Geometry *Particles::geo()
        {
            return m_geo;
        }

        GLsizei Particles::numParticles()
        {
            return m_positionData.size();
        }
    }
} // namespace yourgame
