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
#ifndef YOURGAME_GLTEXTUREATLAS_H
#define YOURGAME_GLTEXTUREATLAS_H

#include <vector>
#include <map>
#include <string>
#include "yourgame/gltexture2d.h"

namespace yourgame
{
    class GLTextureAtlas
    {
    public:
        struct Coords
        {
            float uMin;
            float uMax;
            float vMin;
            float vMax;
            yourgame::GLTexture2D *tex;
        };

        GLTextureAtlas();

        /* deleting the copy constructor and the copy assignment operator
        prevents copying (and moving) of the object. */
        GLTextureAtlas(GLTextureAtlas const &) = delete;
        GLTextureAtlas &operator=(GLTextureAtlas const &) = delete;

        ~GLTextureAtlas();

        void pushTexture(yourgame::GLTexture2D *newTex);
        void pushCoords(std::string name, float uMin, float uMax, float vMin, float vMax);
        bool getCoords(std::string name, Coords &dst);

    private:
        std::vector<yourgame::GLTexture2D *> m_textures;
        std::map<std::string, Coords> m_coords;
    };
} // namespace yourgame

#endif
