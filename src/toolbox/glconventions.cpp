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
#include <string>
#include "yourgame/gl_include.h"

#include "yourgame/glconventions.h" // ToDo: why is this include required?

namespace yourgame
{
    const GLuint attrLocPosition = 0;
    const GLuint attrLocNormal = 1;
    const GLuint attrLocTexcoords = 2;
    const GLuint attrLocColor = 3;
    const std::string attrNamePosition = "inPosition";
    const std::string attrNameNormal = "inNormal";
    const std::string attrNameTexcoords = "inTexcoords";
    const std::string attrNameColor = "inColor";
    const GLchar *unifNameMvpMatrix = "mvpMat";
    const GLchar *unifNameModelMatrix = "modelMat";
    const GLchar *unifNameTexture0 = "texture0";
    const GLchar *unifNameTexture1 = "texture1";
}