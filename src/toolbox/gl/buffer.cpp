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
#include "yourgame/gl_include.h"
#include "yourgame/gl/buffer.h"

namespace yourgame
{
    namespace gl
    {
        Buffer *Buffer::make(GLenum target, GLsizeiptr size, const GLvoid *data, GLenum usage)
        {
            GLuint handle;
            glGenBuffers(1, &handle);
            glBindBuffer(target, handle);
            glBufferData(target, size, data, usage);

            GLint checkSize = -1;
            glGetBufferParameteriv(target, GL_BUFFER_SIZE, &checkSize);
            if (size != checkSize)
            {
                glBindBuffer(target, 0);
                glDeleteBuffers(1, &handle);
                return nullptr;
            }
            else
            {
                Buffer *newBuf = new Buffer();
                newBuf->m_target = target;
                newBuf->m_handle = handle;
                newBuf->m_usage = usage;
                return newBuf;
            }
        }

        Buffer::~Buffer()
        {
            glDeleteBuffers(1, &m_handle);
        }

        void Buffer::bind()
        {
            glBindBuffer(m_target, m_handle);
        }

        void Buffer::unbindTarget()
        {
            glBindBuffer(m_target, 0);
        }

        bool Buffer::bufferData(GLsizeiptr size, const GLvoid *data)
        {
            glBindBuffer(m_target, m_handle);
            glBufferData(m_target, size, data, m_usage);

            GLint checkSize = -1;
            glGetBufferParameteriv(m_target, GL_BUFFER_SIZE, &checkSize);
            return (size == checkSize);
        }
    }
} // namespace yourgame
