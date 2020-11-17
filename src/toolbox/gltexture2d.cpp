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
#include "yourgame/gl_include.h"
#include "yourgame/gltexture2d.h"

namespace yourgame
{
    GLTexture2D *GLTexture2D::make(GLint level,
                                   GLint internalformat,
                                   GLsizei width,
                                   GLsizei height,
                                   GLint border,
                                   GLenum format,
                                   GLenum type,
                                   const void *data,
                                   GLenum unit,
                                   std::vector<std::pair<GLenum, GLint>> parameteri,
                                   bool generateMipmap)
    {
        GLuint handle;

        glGenTextures(1, &handle);
        glBindTexture(GL_TEXTURE_2D, handle);

        for (const auto &par : parameteri)
        {
            glTexParameteri(GL_TEXTURE_2D, par.first, par.second);
        }

        glTexImage2D(GL_TEXTURE_2D,
                     level,
                     internalformat,
                     width,
                     height,
                     border,
                     format,
                     type,
                     data);

        if (generateMipmap)
        {
            glGenerateMipmap(GL_TEXTURE_2D);
        }

        GLTexture2D *newTexture = new GLTexture2D();
        newTexture->m_unit = unit;
        newTexture->m_handle = handle;
        newTexture->m_width = width;
        newTexture->m_height = height;

        return newTexture;
    }

    GLTexture2D::~GLTexture2D()
    {
        glDeleteTextures(1, &m_handle);
    }

    void GLTexture2D::bind()
    {
        glActiveTexture(m_unit);
        glBindTexture(GL_TEXTURE_2D, m_handle);
    }

    void GLTexture2D::unbindTarget()
    {
        glActiveTexture(m_unit);
        glBindTexture(GL_TEXTURE_2D, 0);
    }
} // namespace yourgame
