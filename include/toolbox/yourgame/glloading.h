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
#ifndef YOURGAME_GLLOADING_H
#define YOURGAME_GLLOADING_H

#include <string>
#include <vector>
#include "yourgame/gl_include.h"
#include "yourgame/glgeometry.h"
#include "yourgame/glshader.h"
#include "yourgame/gltexture2d.h"
#include "yourgame/gltextureatlas.h"

namespace yourgame
{
    GLTexture2D *loadTexture(const std::string &filename,
                             GLenum unit,
                             GLint minMaxFilter = GL_LINEAR,
                             bool generateMipmap = false);

    GLTextureAtlas *loadTextureAtlasCrunch(const std::string &filename,
                                           GLenum unit,
                                           GLint minMaxFilter = GL_LINEAR,
                                           bool generateMipmap = false);

    GLTextureAtlas *loadTextureAtlasGrid(const std::string &filename,
                                         int tilesWidth,
                                         int tilesHeight,
                                         GLenum unit,
                                         GLint minMaxFilter = GL_LINEAR,
                                         bool generateMipmap = false);

    GLTexture2D *loadTexture(const std::string &filename,
                             GLenum unit,
                             const std::vector<std::pair<GLenum, GLint>> &parameteri,
                             bool generateMipmap,
                             bool premultiplyAlpha = true);

    /**
    \brief loads images and generates cubemap texture

    \param filenames this order: POSITIVE_X, NEGATIVE_X, POSITIVE_Y, NEGATIVE_Y, POSITIVE_Z, NEGATIVE_Z
    */
    GLTexture2D *loadCubemap(const std::vector<std::string> &filenames,
                             GLenum unit,
                             const std::vector<std::pair<GLenum, GLint>> &parameteri,
                             bool generateMipmap);

    GLTextureAtlas *loadTextureAtlasCrunch(const std::string &filename,
                                           GLenum unit,
                                           const std::vector<std::pair<GLenum, GLint>> &parameteri,
                                           bool generateMipmap);

    GLTextureAtlas *loadTextureAtlasGrid(const std::string &filename,
                                         int tilesWidth,
                                         int tilesHeight,
                                         GLenum unit,
                                         const std::vector<std::pair<GLenum, GLint>> &parameteri,
                                         bool generateMipmap);

    GLShader *loadShader(const std::vector<std::pair<GLenum, std::string>> &shaderFilenames,
                         const std::vector<std::pair<GLuint, std::string>> &attrLocs = {},
                         const std::vector<std::pair<GLuint, std::string>> &fragDataLocs = {});

    GLGeometry *loadGeometry(const std::string &objFilename,
                             const std::string &mtlFilename = "");
} // namespace yourgame

#endif
