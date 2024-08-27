/*
Copyright (c) 2019-2024 Alexander Scholz

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
#include "yourgame_internal/util/assets.h"
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

            // check uniforms
            if (cfg.shader)
            {
                GLint unif;

                // mvp matrix
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

        std::array<float, 4> drawSprite(yourgame::gl::Texture *texture,
                                        const yourgame::gl::TextureCoords &coords,
                                        float x,
                                        float y,
                                        float width,
                                        float height,
                                        float angle)
        {
            yourgame::gl::DrawConfig cfg;

            cfg.shader = yourgame_internal::util::assets::manager.get<yourgame::gl::Shader>("shaderSprite");
            if (cfg.shader)
            {
                cfg.shader->useProgram();
            }

            cfg.textures.push_back(texture);

            float width_f = width;
            float height_f = height;
            if (texture)
            {
                cfg.subtex = {coords.uMin, coords.uMax, coords.vMin, coords.vMax};

                if (std::fpclassify(width) == FP_ZERO && std::fpclassify(height) == FP_ZERO)
                {
                    width_f = static_cast<float>(coords.xMaxPixel - coords.xMinPixel);
                    height_f = static_cast<float>(coords.yMaxPixel - coords.yMinPixel);
                }
                else if (std::fpclassify(width) == FP_ZERO)
                {
                    width_f = height_f * coords.aspectRatioPixel;
                }
                else if (std::fpclassify(height) == FP_ZERO)
                {
                    height_f = width_f * coords.aspectRatioPixelInverse;
                }
            }

            std::array<float, 4> screenPos;
            {
                GLint viewport[4];
                glGetIntegerv(GL_VIEWPORT, viewport);

                float windowWidth = static_cast<float>(viewport[2]);
                float windowHeight = static_cast<float>(viewport[3]);

                // 1. make a transform to position the sprite quad in the "world", where the
                //    length units match screen space pixels.
                yourgame::math::Trafo trafo;
                trafo.translateGlobal({x, -y, 0.0f});
                trafo.rotateGlobal(angle, yourgame::math::Axis::Z);
                trafo.setScaleLocal({width_f * 0.5f, height_f * 0.5f, 1.0f});

                // 2. make an orthographic projection that projects the (x+,y-) "quadrant" from
                //    world space (where the quad gets positioned by trafo) onto screen space.
                // this is actually the MVP matrix (or P * V, as the view matrix is identity).
                // because we do not pass a camera to the draw call, we apply the projection
                // matrix to cfg.modelMat (P * M) directly
                cfg.modelMat = glm::ortho(0.0f, windowWidth, -windowHeight, 0.0f, -1.0f, 1.0f) *
                               trafo.mat();

                // 3. calculate the screen position of the resulting srpite quad, as if it
                //    was not rotated
                auto quadUpLeftModel = trafo.mat() * glm::vec4(-1.0f, 1.0f, 0.0f, 1.0f);
                auto quadLowRightModel = trafo.mat() * glm::vec4(1.0f, -1.0f, 0.0f, 1.0f);
                // xmin, xmax, ymin, ymax
                screenPos = {quadUpLeftModel[0], quadLowRightModel[0], -quadUpLeftModel[1], -quadLowRightModel[1]};
            }

            yourgame::gl::drawGeo(yourgame_internal::util::assets::manager.get<yourgame::gl::Geometry>("geoQuad"), cfg);

            return screenPos;
        }

        void drawSky(yourgame::gl::Texture *texture,
                     yourgame::math::Camera *camera,
                     std::array<float, 3> tint,
                     yourgame::math::Trafo *trafo)
        {
            if (!texture || !camera)
            {
                return;
            }

            bool isCubemap = (texture->getTarget() == GL_TEXTURE_CUBE_MAP);

            yourgame::gl::Geometry *geo = isCubemap ? yourgame_internal::util::assets::manager.get<yourgame::gl::Geometry>("cubeInside")
                                                    : yourgame_internal::util::assets::manager.get<yourgame::gl::Geometry>("sphereInside");

            yourgame::gl::DrawConfig cfg;
            cfg.shader = isCubemap ? yourgame_internal::util::assets::manager.get<yourgame::gl::Shader>("ambientCubemap")
                                   : yourgame_internal::util::assets::manager.get<yourgame::gl::Shader>("ambientTexture");

            cfg.textures.push_back(texture);

            {
                yourgame::gl::Lightsource light;
                light.setAmbient(tint);
                cfg.shader->useProgram(&light, camera);
            }

            // we do not pass camera to the draw config (cfg) and
            // build the model matrix (used as mvp) manually:
            if (trafo)
            {
                cfg.modelMat = camera->pMat(0.1f, 2.0f) *             // "custom" zNear, zFar for skybox camera projection
                               glm::mat4(glm::mat3(camera->vMat())) * // rotation part of camera view matrix
                               glm::mat4(glm::mat3(trafo->mat()));    // rotation part of skybox transformation
            }
            else
            {
                cfg.modelMat = camera->pMat(0.1f, 2.0f) *            // "custom" zNear, zFar for skybox camera projection
                               glm::mat4(glm::mat3(camera->vMat())); // rotation part of camera view matrix
            }

            // Draw geometry while preserving current depth mask state
            {
                GLboolean depthMask;
                glGetBooleanv(GL_DEPTH_WRITEMASK, &depthMask);

                if (depthMask == GL_TRUE)
                {
                    glDepthMask(GL_FALSE);
                }

                yourgame::gl::drawGeo(geo, cfg);

                if (depthMask == GL_TRUE)
                {
                    glDepthMask(GL_TRUE);
                }
            }
        }
    } // namespace gl
} // namespace yourgame
