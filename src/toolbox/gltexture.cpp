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
#include "yourgame/gl_include.h"
#include "yourgame/gltexture.h"

namespace yourgame
{
    GLTexture *GLTexture::make(GLenum target,
                                   GLenum unit,
                                   const std::vector<std::pair<GLenum, GLint>> &parameteri)
    {
        GLuint handle;

        glGenTextures(1, &handle);
        glBindTexture(target, handle);

        for (const auto &par : parameteri)
        {
            glTexParameteri(target, par.first, par.second);
        }

        GLTexture *newTexture = new GLTexture();
        newTexture->m_unit = unit;
        newTexture->m_target = target;
        newTexture->m_handle = handle;

        return newTexture;
    }

    GLTexture::~GLTexture()
    {
        glDeleteTextures(1, &m_handle);
    }

    void GLTexture::bind() const
    {
        glActiveTexture(m_unit);
        glBindTexture(m_target, m_handle);
    }

    void GLTexture::unbindTarget() const
    {
        glActiveTexture(m_unit);
        glBindTexture(m_target, 0);
    }

    void GLTexture::updateData(GLenum target,
                                 GLint level,
                                 GLint internalformat,
                                 GLsizei width,
                                 GLsizei height,
                                 GLint border,
                                 GLenum format,
                                 GLenum type,
                                 const void *data,
                                 bool generateMipmap)
    {
        glActiveTexture(m_unit);
        glBindTexture(m_target, m_handle);

        glTexImage2D(target, // target can be != m_target (cubemaps...)
                     level,
                     internalformat,
                     width,
                     height,
                     border,
                     format,
                     type,
                     data);

        // todo: does this make sense _here_ for cubemaps?
        if (generateMipmap)
        {
            glGenerateMipmap(m_target);
        }

        glBindTexture(m_target, 0);

        m_width = width;
        m_height = height;
    }
} // namespace yourgame
