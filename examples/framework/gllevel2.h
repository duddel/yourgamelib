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
#ifndef GLLEVEL2_H
#define GLLEVEL2_H

#include "yourgame/gl_include.h"
#include "gltexture2d.h"
#include "glshader.h"
#include "glgeometry.h"

namespace mygame
{
    extern const GLuint attrLocPosition;
    extern const GLuint attrLocNormal;
    extern const GLuint attrLocTexcoords;
    extern const GLuint attrLocColor;
    extern const std::string attrNamePosition;
    extern const std::string attrNameNormal;
    extern const std::string attrNameTexcoords;
    extern const std::string attrNameColor;
    extern const GLchar *unifNameMvpMatrix;
    extern const GLchar *unifNameModelMatrix;
    extern const GLchar *unifNameTexture0;
    extern const GLchar *unifNameTexture1;

    GLTexture2D *loadTexture(const char *filename, GLenum unit);

    GLShader *loadShader(std::vector<std::pair<GLenum, std::string>> shaderFilenames,
                         std::vector<std::pair<GLuint, std::string>> attrLocs,
                         std::vector<std::pair<GLuint, std::string>> fragDataLocs);

    GLGeometry *loadGeometry(const char *objFilename, const char *mtlFilename);
} // namespace mygame

#endif
