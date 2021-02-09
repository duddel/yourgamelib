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

namespace yourgame
{
    class GLLightsource
    {
    public:
        std::array<float, 12> data();
        std::array<float, 3> ambient();
        std::array<float, 3> diffuse();
        std::array<float, 3> specular();
        std::array<float, 3> position();
        void setAmbient(std::array<float, 3> col);
        void setDiffuse(std::array<float, 3> col);
        void setSpecular(std::array<float, 3> col);
        void setPosition(std::array<float, 3> pos);

    private:
        // ambient, diffuse, specular, position
        std::array<float, 12> m_data = {0.0f, 0.0f, 0.0f,
                                        1.0f, 1.0f, 1.0f,
                                        1.0f, 1.0f, 1.0f,
                                        0.0f, 0.0f, 0.0f};
    };
} // namespace yourgame

#endif
