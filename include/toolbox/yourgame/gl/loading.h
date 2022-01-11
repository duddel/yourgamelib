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
#ifndef YOURGAME_GLLOADING_H
#define YOURGAME_GLLOADING_H

#include <string>
#include <vector>
#include "yourgame/gl_include.h"
#include "yourgame/gl/geometry.h"
#include "yourgame/gl/shader.h"
#include "yourgame/gl/texture.h"
#include "yourgame/gl/textureatlas.h"
#include "yourgame/gl/conventions.h"

namespace yourgame
{
    namespace gl
    {
        struct TextureConfig
        {
            GLenum unit = gl::textureUnitDiffuse;
            GLint minMagFilter = GL_LINEAR;
            GLint wrapMode = GL_REPEAT;
            bool generateMipmap = false;
            bool premultiplyAlpha = true;
            std::vector<std::pair<GLenum, GLint>> parameteri = {};
        };

        Texture *loadTexture(const std::string &filename,
                             const yourgame::gl::TextureConfig &cfg);

        TextureAtlas *loadTextureAtlasCrunch(const std::string &filename,
                                             const yourgame::gl::TextureConfig &cfg);

        TextureAtlas *loadTextureAtlasGrid(const std::string &filename,
                                           int tilesWidth,
                                           int tilesHeight,
                                           const yourgame::gl::TextureConfig &cfg);

        /**
        \brief loads images and generates cubemap texture

        \param filenames this order: POSITIVE_X, NEGATIVE_X, POSITIVE_Y, NEGATIVE_Y, POSITIVE_Z, NEGATIVE_Z
        */
        Texture *loadCubemap(const std::vector<std::string> &filenames,
                             const yourgame::gl::TextureConfig &cfg);

        Shader *loadShader(const std::vector<std::pair<GLenum, std::string>> &shaderFilenames,
                           const std::vector<std::pair<GLuint, std::string>> &attrLocs = {},
                           const std::vector<std::pair<GLuint, std::string>> &fragDataLocs = {});

        Shader *loadShaderFromStrings(const std::vector<std::pair<GLenum, std::string>> &shaderCodes,
                                      const std::vector<std::pair<GLuint, std::string>> &attrLocs = {},
                                      const std::vector<std::pair<GLuint, std::string>> &fragDataLocs = {});

        Geometry *loadGeometry(const std::string &objFilename,
                               const std::string &mtlFilename = "");
    }
} // namespace yourgame

#endif
