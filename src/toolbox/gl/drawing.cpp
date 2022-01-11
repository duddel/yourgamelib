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
#include "yourgame/gl/conventions.h"
#include "yourgame/gl/drawing.h"

namespace yourgame
{
    namespace gl
    {
        void drawGeo(const yourgame::gl::Geometry *geo, const yourgame::gl::DrawConfig &cfg)
        {
            if (!geo)
            {
                return;
            }

            // mvp matrix
            if (cfg.shader)
            {
                GLint unif;
                unif = cfg.shader->getUniformLocation(gl::unifNameMvpMatrix);
                if (unif != -1)
                {
                    auto mvp = cfg.camera ? (cfg.camera->pMat() * cfg.camera->vMat() * cfg.modelMat)
                                          : cfg.modelMat;
                    glUniformMatrix4fv(unif, 1, GL_FALSE, glm::value_ptr(mvp));
                }

                // model matrix
                unif = cfg.shader->getUniformLocation(gl::unifNameModelMatrix);
                if (unif != -1)
                {
                    glUniformMatrix4fv(unif, 1, GL_FALSE, glm::value_ptr(cfg.modelMat));
                }

                // normal matrix
                unif = cfg.shader->getUniformLocation(gl::unifNameNormalMatrix);
                if (unif != -1)
                {
                    auto normalMat = glm::inverseTranspose(glm::mat3(cfg.modelMat));
                    glUniformMatrix3fv(unif, 1, GL_FALSE, glm::value_ptr(normalMat));
                }

                // sub texture
                unif = cfg.shader->getUniformLocation(gl::unifNameSubtexture);
                if (unif != -1)
                {
                    // scale u, scale v, offset u, offset v
                    glUniform4f(unif, cfg.subtex[1] - cfg.subtex[0], cfg.subtex[3] - cfg.subtex[2], cfg.subtex[0], cfg.subtex[2]);
                }
            }

            // textures
            for (const auto &t : cfg.textures)
            {
                if (t)
                {
                    t->bind();
                }
            }

            // draw call
            if (cfg.instancecount > 1)
            {
                geo->drawAllInstanced(cfg.instancecount);
            }
            else
            {
                geo->drawAll();
            }
        }
    }
} // namespace yourgame
