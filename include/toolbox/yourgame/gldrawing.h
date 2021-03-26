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
#ifndef YOURGAME_GLDRAWING_H
#define YOURGAME_GLDRAWING_H

#include <array>
#include <vector>
#include "yourgame/camera.h"
#include "yourgame/glgeometry.h"
#include "yourgame/glshader.h"
#include "yourgame/gltexture2d.h"

namespace yourgame
{
    struct DrawConfig
    {
        yourgame::GLShader *shader = nullptr;
        yourgame::Camera *camera = nullptr;
        glm::mat4 modelMat = glm::mat4(1);
        std::vector<yourgame::GLTexture2D *> textures = {};
        std::array<float, 4> subtex = {0.0f, 1.0f, 0.0f, 1.0f};
        GLsizei instancecount = 1;
    };

    void drawGeo(const yourgame::GLGeometry *geo, const yourgame::DrawConfig &cfg);
} // namespace yourgame

#endif
