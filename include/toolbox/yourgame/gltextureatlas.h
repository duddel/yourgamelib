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
#ifndef YOURGAME_GLTEXTUREATLAS_H
#define YOURGAME_GLTEXTUREATLAS_H

#include <vector>
#include <map>
#include <array>
#include <string>
#include "yourgame/gltexture.h"

namespace yourgame
{
    class GLTextureAtlas
    {
    public:
        GLTextureAtlas(){};

        /* deleting the copy constructor and the copy assignment operator
        prevents copying (and moving) of the object. */
        GLTextureAtlas(GLTextureAtlas const &) = delete;
        GLTextureAtlas &operator=(GLTextureAtlas const &) = delete;

        ~GLTextureAtlas();

        void pushTexture(yourgame::GLTexture *newTex);
        void pushCoords(std::string name, float uMin, float uMax, float vMin, float vMax);

        std::pair<yourgame::GLTexture *, std::array<float, 4>> getCoords(std::string name) const;
        std::pair<yourgame::GLTexture *, std::array<float, 4>> getCoords(std::string seqName, int seqFrame) const;
        int getSeqFrames(std::string seqName) const;
        std::vector<std::string> getSequenceNames() const;
        yourgame::GLTexture *texture(int idx) const;

    private:
        struct Coords
        {
            float uMin;
            float uMax;
            float vMin;
            float vMax;
            yourgame::GLTexture *tex;
        };

        struct Sequence
        {
            // maps sequence frame index to atlas sprite name
            std::map<int, std::string> seqIdxMap;
        };

        std::vector<yourgame::GLTexture *> m_textures;
        std::map<std::string, Coords> m_coords;
        std::map<std::string, Sequence> m_sequences;
    };
} // namespace yourgame

#endif
