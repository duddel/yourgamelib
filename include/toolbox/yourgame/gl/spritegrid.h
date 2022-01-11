/*
Copyright (c) 2019-2022 Alexander Scholz

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
#ifndef YOURGAME_GLSPRITEGRID_H
#define YOURGAME_GLSPRITEGRID_H

#include <string>
#include <vector>
#include "yourgame/gl/geometry.h"
#include "yourgame/gl/textureatlas.h"

namespace yourgame
{
    namespace gl
    {
        class SpriteGrid
        {
        public:
            SpriteGrid();

            /* deleting the copy constructor and the copy assignment operator
            prevents copying (and moving) of the object. */
            SpriteGrid(SpriteGrid const &) = delete;
            SpriteGrid &operator=(SpriteGrid const &) = delete;

            ~SpriteGrid();

            /**
            \brief creates grid geometry in x-y plane (z = 0)

            - planar grid in x-y plane (z=0), 2 ccw trianlges per tile
            - drawing: bind texture manually and get geometry for drawing via geo()
            - aspect ratio taken from first tile
            - atlas is only used for geometry creation, pointer is not kept
            - atlas should have only 1 texture

            \param atlas atlas, the coordinates are taken from
            \param tiles names of tiles in atlas, in row-major order
            \param width number of tiles per row. if = 0, all tiles in 1 row
            \param gridWidth desired width of entire grid. if <= 0.0f, aspect ratio and gridHeight (if > 0.0f) used
            \param gridHeight desired height of entire grid. if <= 0.0f, aspect ratio and gridWidth (if > 0.0f) used
            \return error (0 on success)
            */
            int make(const yourgame::gl::TextureAtlas *atlas,
                     const std::vector<std::string> &tiles,
                     unsigned int width,
                     float gridWidth,
                     float gridHeight);

            /** \return pointer to geometry object (m_geo) */
            Geometry *geo() const { return m_geo; }

        private:
            Geometry *m_geo = nullptr;
        };
    }
} // namespace yourgame

#endif
