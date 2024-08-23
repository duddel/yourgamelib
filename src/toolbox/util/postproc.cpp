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
#include <stdint.h>
#include "yourgame/input.h"
#include "yourgame/gl/conventions.h"
#include "yourgame/gl/drawing.h"
#include "yourgame/gl/framebuffer.h"
#include "yourgame/gl/shader.h"
#include "yourgame_internal/input.h"
#include "yourgame_internal/util/assets.h"

namespace
{
    yourgame::gl::Framebuffer *g_framebuf = nullptr;
    yourgame::gl::Shader *g_postprocShader = nullptr;
    uint32_t g_framebufWidth = 0;
    uint32_t g_framebufHeight = 0;
    uint32_t g_framebufWidthActual = 0;
    uint32_t g_framebufHeightActual = 0;

    void updateFramebufSizeActual()
    {
        if (g_framebufWidth < 1 && g_framebufHeight < 1)
        {
            g_framebufWidthActual = yourgame::input::geti(yourgame::input::WINDOW_WIDTH);
            g_framebufHeightActual = yourgame::input::geti(yourgame::input::WINDOW_HEIGHT);
        }
        else if (g_framebufWidth < 1)
        {
            g_framebufWidthActual = static_cast<uint32_t>(static_cast<float>(g_framebufHeight) *
                                                          yourgame::input::get(yourgame::input::WINDOW_ASPECT_RATIO));
            g_framebufHeightActual = g_framebufHeight;
        }
        else if (g_framebufHeight < 1)
        {
            g_framebufWidthActual = g_framebufWidth;
            g_framebufHeightActual = static_cast<uint32_t>(static_cast<float>(g_framebufWidth) *
                                                           yourgame::input::get(yourgame::input::WINDOW_ASPECT_RATIO_INVERSE));
        }
        else
        {
            g_framebufWidthActual = g_framebufWidth;
            g_framebufHeightActual = g_framebufHeight;
        }

        // Set input sources
        yourgame_internal::input::setInput(yourgame::input::Source::FRAMEBUF_WIDTH,
                                           static_cast<float>(g_framebufWidthActual));
        yourgame_internal::input::setInput(yourgame::input::Source::FRAMEBUF_WIDTH_INVERSE,
                                           1.0f / static_cast<float>(g_framebufWidthActual));

        yourgame_internal::input::setInput(yourgame::input::Source::FRAMEBUF_HEIGHT,
                                           static_cast<float>(g_framebufHeightActual));
        yourgame_internal::input::setInput(yourgame::input::Source::FRAMEBUF_HEIGHT_INVERSE,
                                           1.0f / static_cast<float>(g_framebufHeightActual));

        yourgame_internal::input::setInput(yourgame::input::Source::FRAMEBUF_ASPECT_RATIO,
                                           static_cast<float>(g_framebufWidthActual) /
                                               static_cast<float>(g_framebufHeightActual));

        yourgame_internal::input::setInput(yourgame::input::Source::FRAMEBUF_ASPECT_RATIO_INVERSE,
                                           static_cast<float>(g_framebufHeightActual) /
                                               static_cast<float>(g_framebufWidthActual));

        yourgame_internal::input::setInput(yourgame::input::Source::FRAMEBUF_MATCHES_WINDOW,
                                           (g_framebufWidthActual == yourgame::input::geti(yourgame::input::WINDOW_WIDTH) &&
                                            g_framebufHeightActual == yourgame::input::geti(yourgame::input::WINDOW_HEIGHT))
                                               ? 1.0f
                                               : 0.0f);
    }

    void validateSize()
    {
        static uint32_t g_framebufWidthActualPrevious = 0;
        static uint32_t g_framebufHeightActualPrevious = 0;

        if (g_framebufWidthActualPrevious != g_framebufWidthActual ||
            g_framebufHeightActualPrevious != g_framebufHeightActual)
        {
            g_framebuf->resize(g_framebufWidthActual, g_framebufHeightActual);
            g_framebufWidthActualPrevious = g_framebufWidthActual;
            g_framebufHeightActualPrevious = g_framebufHeightActual;
        }
    }
} // namespace

namespace yourgame_internal
{
    namespace util
    {
        namespace postproc
        {
            void activate()
            {
                if (!g_framebuf)
                {
                    return;
                }

                // If window size changed since last frame, update framebuffer size
                if (yourgame::input::getDelta(yourgame::input::WINDOW_WIDTH) != 0.0f ||
                    yourgame::input::getDelta(yourgame::input::WINDOW_HEIGHT) != 0.0f)
                {
                    updateFramebufSizeActual();
                }

                validateSize();

                g_framebuf->bind();
            }

            void deactivateAndDraw()
            {
                if (!g_framebuf)
                {
                    return;
                }

                g_framebuf->unbindTarget();

                // Framebuffer size == window size
                if (g_framebufWidth < 1 && g_framebufHeight < 1)
                {
                    // framebuffer size matches window size (if auto resize desired)
                    glViewport(0,
                               0,
                               yourgame::input::geti(yourgame::input::WINDOW_WIDTH),
                               yourgame::input::geti(yourgame::input::WINDOW_HEIGHT));
                }
                else
                {
                    // framebuffer size is fixed. draw framebuffer stretched and centered
                    // in window, while maintaining the aspect ratio
                    float aspectFramebuf = (float)g_framebufWidthActual / (float)g_framebufHeightActual;

                    if (yourgame::input::get(yourgame::input::WINDOW_ASPECT_RATIO) > aspectFramebuf)
                    {
                        float viewWidth = (yourgame::input::get(yourgame::input::WINDOW_HEIGHT) * aspectFramebuf);
                        glViewport(
                            (GLint)((yourgame::input::get(yourgame::input::WINDOW_WIDTH) - viewWidth) * 0.5f),
                            0,
                            (GLsizei)viewWidth,
                            (GLsizei)yourgame::input::geti(yourgame::input::WINDOW_HEIGHT));
                    }
                    else
                    {
                        float viewHeight = (yourgame::input::get(yourgame::input::WINDOW_WIDTH) / aspectFramebuf);
                        glViewport(
                            0,
                            (GLint)((yourgame::input::get(yourgame::input::WINDOW_HEIGHT) - viewHeight) * 0.5f),
                            (GLsizei)yourgame::input::geti(yourgame::input::WINDOW_WIDTH),
                            (GLsizei)viewHeight);
                    }
                }

                // clear buffers while maintaining original clear color
                {
                    // get current clear color, set in user code
                    GLfloat clearColorOrg[4];
                    glGetFloatv(GL_COLOR_CLEAR_VALUE, clearColorOrg);

                    // clear with black (0,0,0)
                    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
                    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

                    // restore clear color
                    glClearColor(clearColorOrg[0], clearColorOrg[1], clearColorOrg[2], clearColorOrg[3]);
                }

                // draw framebuffer textures with post processing shader
                {
                    // simple orthographic projection that matches the quad geometry
                    auto pMat = glm::ortho(-1.0f, 1.0f, -1.0f, 1.0f, 1.0f, -1.0f);

                    // use "null" post processing shader by default
                    yourgame::gl::Shader *shader =
                        yourgame_internal::util::assets::manager.get<yourgame::gl::Shader>("shaderPostNull");
                    if (g_postprocShader)
                    {
                        shader = g_postprocShader;
                    }

                    shader->useProgram();
                    yourgame::gl::DrawConfig cfg;
                    cfg.modelMat = pMat;
                    cfg.shader = shader;

                    // hand over color0 and depth texture attachments to draw call
                    cfg.textures.push_back(g_framebuf->textureAttachment(0));
                    cfg.textures.push_back(g_framebuf->textureAttachment(1));

                    yourgame::gl::drawGeo(yourgame_internal::util::assets::manager.get<yourgame::gl::Geometry>("geoQuad"), cfg);
                }
            }
        } // namespace postproc
    } // namespace util
} // namespace yourgame_internal

namespace yourgame
{
    namespace util
    {
        namespace postproc
        {
            void resize(uint32_t width, uint32_t height)
            {
                g_framebufWidth = width;
                g_framebufHeight = height;
                updateFramebufSizeActual();
            }

            bool init(uint32_t width, uint32_t height)
            {
                resize(width, height);

                g_framebuf = yourgame::gl::Framebuffer::make(
                    g_framebufWidthActual,
                    g_framebufHeightActual,
                    {{GL_RGBA8,
                      GL_RGBA,
                      GL_UNSIGNED_BYTE,
                      yourgame::gl::textureUnitBufferColor0,
                      {{GL_TEXTURE_MIN_FILTER, GL_NEAREST},
                       {GL_TEXTURE_MAG_FILTER, GL_NEAREST},
                       {GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE},
                       {GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE}},
                      GL_COLOR_ATTACHMENT0},
                     {GL_DEPTH_COMPONENT16,
                      GL_DEPTH_COMPONENT,
                      GL_UNSIGNED_SHORT,
                      yourgame::gl::textureUnitBufferDepth,
                      {{GL_TEXTURE_MIN_FILTER, GL_NEAREST},
                       {GL_TEXTURE_MAG_FILTER, GL_NEAREST},
                       {GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE},
                       {GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE}},
                      GL_DEPTH_ATTACHMENT}});

                yourgame_internal::input::setInput(yourgame::input::Source::POSTPROC_INITIALIZED, g_framebuf ? 1.0f : 0.0f);

                return (g_framebuf != nullptr);
            }

            void shutdown()
            {
                if (g_framebuf)
                {
                    delete g_framebuf;
                    g_framebuf = nullptr;
                }

                yourgame_internal::input::setInput(yourgame::input::Source::POSTPROC_INITIALIZED, 0.0f);
            }

            bool isInitialized()
            {
                return (g_framebuf != nullptr);
            }

            void use(yourgame::gl::Shader *shader)
            {
                g_postprocShader = shader;
            }
        } // namespace postproc
    } // namespace util
} // namespace yourgame
