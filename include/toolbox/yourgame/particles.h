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
#ifndef YOURGAME_PARTICLES_H
#define YOURGAME_PARTICLES_H

#include <vector>
#include <random>
#include <ctime>
#include <glm/glm.hpp>

namespace yourgame
{
    /**
    \todo change count during runtime?
    \todo prevent scatterOnSpawn during spawn() on init
    \todo communicate that the system is eventually "finished" if respawn == false
     */
    class Particles
    {
    public:
        /**
        \brief Particle system configuration

        Should be configured and handed over to Particles() ctor.
        Can be changed during runtime via Particles::m_cfg.
        */
        struct Config
        {
            size_t count = 100;                           /**< \brief number of particles */
            unsigned int seed = std::time(nullptr);       /**< \brief seed for random generator */
            bool scatterOnInit = true;                    /**< \brief if true, particles are scattered in the volume when system is initialized */
            bool scatterOnSpawn = false;                  /**< \brief if true, particles are scattered when they are respawned */
            bool respawn = true;                          /**< \brief if true, particles are respawned after their lifetime ends */
            glm::vec3 origin = {0.0f, 0.0f, 0.0f};        /**< \brief origin of the system */
            glm::vec3 emitterA = {0.0f, 0.0f, 0.0f};      /**< \brief first vector that forms a volume inside particles are spawned */
            glm::vec3 emitterB = {0.0f, 0.0f, 0.0f};      /**< \brief second vector that forms a volume inside particles are spawned */
            glm::vec3 emitterC = {0.0f, 0.0f, 0.0f};      /**< \brief third vector that forms a volume inside particles are spawned */
            glm::vec3 baseDirection = {0.0f, 1.0f, 0.0f}; /**< \brief base direction of particles */
            glm::vec3 noisDirection = {0.2f, 0.2f, 0.2f}; /**< \brief uniform noise for direction */
            float baseVelocity = 1.5f;                    /**< \brief base velocity of particles */
            float noisVelocity = 0.2f;                    /**< \brief uniform noise for velocity */
            float baseLifetime = 3.0f;                    /**< \brief base lifetime of particles */
            float noisLifetime = 0.4f;                    /**< \brief uniform noise for lifetime */
        };

        Particles(const Config cfg = Config());

        /**
        \brief ticks the particle system
        \param dt time delta
         */
        void tick(float dt);
        Config m_cfg;
        std::vector<glm::vec4> m_positionData;
        std::vector<float> m_progressData;

    private:
        struct Part
        {
            glm::vec3 pos;
            glm::vec3 move;
            float lifetime;
            float totalLifeInv;
            float progress;
        };
        std::default_random_engine m_rndgen;
        std::uniform_real_distribution<float> m_unirnd;
        std::vector<Part> m_parts;
        Part spawn();
        float rnd1();
    };
} // namespace yourgame

#endif
