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
#ifndef YOURGAME_GLCONVENTIONS_H
#define YOURGAME_GLCONVENTIONS_H

#include "yourgame/gl_include.h"

namespace yourgame
{
    extern const GLuint attrLocPosition;
    extern const GLuint attrLocNormal;
    extern const GLuint attrLocTexcoords;
    extern const GLuint attrLocColor;
    extern const GLuint attrLocInstModelMatCol0;
    extern const GLuint attrLocInstModelMatCol1;
    extern const GLuint attrLocInstModelMatCol2;
    extern const GLuint attrLocInstModelMatCol3;
    extern const GLchar *unifNameMvpMatrix;
    extern const GLchar *unifNameVpMatrix;
    extern const GLchar *unifNameModelMatrix;
    extern const GLchar *unifNameNormalMatrix;
    extern const GLchar *unifNameCameraPosition;
    extern const GLchar *unifNameCameraTrafo;
    extern const GLchar *unifNameSkyRotationInv;
    extern const GLchar *unifNameTextureDiffuse;
    extern const GLchar *unifNameTextureSky;
    extern const GLchar *unifNameLightAmbient;
    extern const GLchar *unifNameLightDiffuse;
    extern const GLchar *unifNameLightSpecular;
    extern const GLchar *unifNameLightPosition;
    extern const GLint unifValueTextureDiffuse;
    extern const GLint unifValueTextureSky;
    extern const GLenum textureUnitDiffuse;
    extern const GLenum textureUnitSky;
} // namespace yourgame

#endif
