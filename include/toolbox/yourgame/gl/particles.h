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
#ifndef YOURGAME_GLPARTICLES_H
#define YOURGAME_GLPARTICLES_H

#include "yourgame/math/particles.h"
#include "yourgame/gl_include.h"
#include "yourgame/gl/geometry.h"

namespace yourgame
{
    namespace gl
    {
        class Particles : public yourgame::math::Particles
        {
        public:
            static Particles *make(const Config cfg, yourgame::gl::Geometry *geo);
            ~Particles();
            void tick(float dt) override;
            yourgame::gl::Geometry *geo();
            GLsizei numParticles();

        private:
            Particles(const Config cfg, yourgame::gl::Geometry *geo) : yourgame::math::Particles(cfg), m_geo(geo) {}
            yourgame::gl::Geometry *m_geo;
        };
    } // namespace gl
} // namespace yourgame

#endif
