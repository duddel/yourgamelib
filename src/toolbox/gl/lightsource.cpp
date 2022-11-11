/*
Copyright (c) 2019-2023 Alexander Scholz

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
#include "yourgame/gl/lightsource.h"

namespace yourgame
{
    namespace gl
    {
        std::array<float, 12> Lightsource::data()
        {
            return m_data;
        }

        std::array<float, 3> Lightsource::ambient()
        {
            return std::array<float, 3>({m_data[0], m_data[1], m_data[2]});
        }

        std::array<float, 3> Lightsource::diffuse()
        {
            return std::array<float, 3>({m_data[3], m_data[4], m_data[5]});
        }

        std::array<float, 3> Lightsource::specular()
        {
            return std::array<float, 3>({m_data[6], m_data[7], m_data[8]});
        }

        std::array<float, 3> Lightsource::position()
        {
            return std::array<float, 3>({m_data[9], m_data[10], m_data[11]});
        }

        void Lightsource::setAmbient(std::array<float, 3> col)
        {
            m_data[0] = col[0];
            m_data[1] = col[1];
            m_data[2] = col[2];
        }

        void Lightsource::setDiffuse(std::array<float, 3> col)
        {
            m_data[3] = col[0];
            m_data[4] = col[1];
            m_data[5] = col[2];
        }

        void Lightsource::setSpecular(std::array<float, 3> col)
        {
            m_data[6] = col[0];
            m_data[7] = col[1];
            m_data[8] = col[2];
        }

        void Lightsource::setPosition(std::array<float, 3> pos)
        {
            m_data[9] = pos[0];
            m_data[10] = pos[1];
            m_data[11] = pos[2];
        }
    }
} // namespace yourgame
