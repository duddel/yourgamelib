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
#include "yourgame/gl/framebuffer.h"

namespace yourgame
{
    namespace gl
    {
        Framebuffer *Framebuffer::make(GLsizei width, GLsizei height, std::vector<TextureAttachDescr> texAttachDescrs)
        {
            Framebuffer *newFrameBuf = new Framebuffer();

            glGenFramebuffers(1, &(newFrameBuf->m_handle));
            glBindFramebuffer(GL_FRAMEBUFFER, newFrameBuf->m_handle);

            newFrameBuf->m_texAttachDescrs = texAttachDescrs;

            for (const auto &ta : texAttachDescrs)
            {
                Texture *newTex = Texture::make(GL_TEXTURE_2D, ta.unit, ta.parameteri);
                newTex->updateData(GL_TEXTURE_2D,
                                   0,
                                   ta.internalformat,
                                   width,
                                   height,
                                   0,
                                   ta.format,
                                   ta.type,
                                   nullptr,
                                   false);

                newTex->unbindTarget();

                glFramebufferTexture2D(GL_FRAMEBUFFER, ta.attachment, GL_TEXTURE_2D, newTex->handle(), 0);
                newFrameBuf->m_textureAttachments.push_back(newTex);
            }

            auto status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
            glBindFramebuffer(GL_FRAMEBUFFER, 0);

            if (status != GL_FRAMEBUFFER_COMPLETE)
            {
                delete newFrameBuf;
                return nullptr;
            }

            return newFrameBuf;
        }

        Framebuffer::~Framebuffer()
        {
            for (const auto &t : m_textureAttachments)
            {
                delete t;
            }
            glDeleteFramebuffers(1, &m_handle);
        }

        void Framebuffer::bind()
        {
            glBindFramebuffer(GL_FRAMEBUFFER, m_handle);
        }

        void Framebuffer::unbindTarget()
        {
            glBindFramebuffer(GL_FRAMEBUFFER, 0);
        }

        void Framebuffer::resize(GLsizei width, GLsizei height)
        {
            for (auto i = 0; i < m_textureAttachments.size(); i++)
            {
                m_textureAttachments[i]->updateData(GL_TEXTURE_2D,
                                                    0,
                                                    m_texAttachDescrs[i].internalformat,
                                                    width,
                                                    height,
                                                    0,
                                                    m_texAttachDescrs[i].format,
                                                    m_texAttachDescrs[i].type,
                                                    nullptr,
                                                    false);
            }
        }

        yourgame::gl::Texture *Framebuffer::textureAttachment(int idx)
        {
            try
            {
                return m_textureAttachments.at(idx);
            }
            catch (...)
            {
                return nullptr;
            }
        }
    }
} // namespace yourgame
