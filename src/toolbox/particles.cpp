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
#include "yourgame/particles.h"

namespace yourgame
{
    Particles::Particles(const Config cfg) : m_cfg(cfg)
    {
        m_rndgen.seed(m_cfg.seed);
        m_unirnd = std::uniform_real_distribution<float>(-1.0f, 1.0f);

        reset();
    }

    void Particles::tick(float dt)
    {
        m_positionData.clear();
        m_progressData.clear();

        for (auto &p : m_parts)
        {
            p.lifetime -= dt;
            p.progress = 1.0f - (p.totalLifeInv * p.lifetime);
            if (p.lifetime <= 0.0f)
            {
                if (m_cfg.respawn)
                {
                    p = spawn();
                    m_positionData.push_back(glm::vec4(p.pos, 0.0f));
                    m_progressData.push_back(p.progress);
                }
            }
            else
            {
                p.pos += (dt * p.move);
                m_positionData.push_back(glm::vec4(p.pos, 0.0f));
                m_progressData.push_back(p.progress);
            }
        }
    }

    void Particles::reset()
    {
        m_parts.clear();
        m_parts.reserve(m_cfg.count);
        for (size_t i = 0; i < m_cfg.count; i++)
        {
            m_parts.push_back(spawn());
        }

        if (m_cfg.scatterOnInit)
        {
            for (auto &p : m_parts)
            {
                float dt = (rnd1() * 0.5f + 0.5f) * p.lifetime;
                p.pos += (dt * p.move);
                p.lifetime -= dt;
            }
        }
    }

    Particles::Part Particles::spawn()
    {
        Part p;
        p.pos = m_cfg.origin + (rnd1() * m_cfg.emitterA) + (rnd1() * m_cfg.emitterB) + (rnd1() * m_cfg.emitterC);
        // todo check if we normalize a 0,0,0-vector here:
        p.move = glm::normalize(m_cfg.baseDirection + (glm::vec3(rnd1(), rnd1(), rnd1()) * m_cfg.noisDirection)) *
                 (m_cfg.baseVelocity + (rnd1() * m_cfg.noisVelocity));
        p.lifetime = m_cfg.baseLifetime + (rnd1() * m_cfg.noisLifetime);

        p.totalLifeInv = 1.0f / p.lifetime;
        p.progress = 0.0f;

        if (m_cfg.scatterOnSpawn)
        {
            auto dt = (rnd1() * 0.5f + 0.5f) * p.lifetime;
            p.pos += (dt * p.move);
            p.lifetime -= dt;
            p.progress = 1.0f - (p.totalLifeInv * p.lifetime);
        }

        return p;
    }

    float Particles::rnd1()
    {
        return m_unirnd(m_rndgen);
    }
} // namespace yourgame
