/*
Copyright (c) 2019-2025 Alexander Scholz

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
#include "yourgame/gl/geometry.h"

namespace yourgame
{
    namespace gl
    {
        Geometry *Geometry::make()
        {
            return new Geometry();
        }

        Geometry::~Geometry()
        {
            // Delete VAO
            glBindVertexArray(0);
            glDeleteVertexArrays(1, &m_vaoHandle);

            // Delete Array Buffers
            for (const auto &b : m_arrayBuffers)
            {
                delete b.second.buffer;
            }

            // Delete Element Array Buffer
            if (m_elementArrayBuffer.buffer)
            {
                delete m_elementArrayBuffer.buffer;
            }
        }

        bool Geometry::addArrayBuffer(std::string name,
                                      GLsizeiptr size,
                                      const GLvoid *data,
                                      GLenum usage,
                                      ArrayBufferDescriptor descriptor)
        {
            if (!m_arrayBuffers.count(name))
            {
                Buffer *newBuffer = Buffer::make(GL_ARRAY_BUFFER, size, data, usage);
                if (newBuffer)
                {
                    m_arrayBuffers.insert(std::pair<std::string, ArrayBuffer>(name, {newBuffer, descriptor}));
                    return true;
                }
                return false;
            }
            return false;
        }

        bool Geometry::setElementArrayBuffer(GLsizeiptr size,
                                             const GLvoid *data,
                                             GLenum usage,
                                             ElementArrayBufferDescriptor descriptor)
        {
            // Delete Element Array Buffer it it already exists
            if (m_elementArrayBuffer.buffer)
            {
                delete m_elementArrayBuffer.buffer;
                m_elementArrayBuffer.buffer = nullptr;
            }

            Buffer *newBuffer = Buffer::make(GL_ELEMENT_ARRAY_BUFFER, size, data, usage);
            if (newBuffer)
            {
                m_elementArrayBuffer.buffer = newBuffer;
                m_elementArrayBuffer.descriptor = descriptor;
                return true;
            }
            return false;
        }

        bool Geometry::bufferArrayData(std::string name, GLsizeiptr size, const GLvoid *data)
        {
            auto it = m_arrayBuffers.find(name);
            if (it != m_arrayBuffers.end())
            {
                return it->second.buffer->bufferData(size, data);
            }
            return false;
        }

        bool Geometry::init()
        {
            // Delete VAO if it already exists
            if (m_vaoHandle != 0)
            {
                glDeleteVertexArrays(1, &m_vaoHandle);
                m_vaoHandle = 0;
            }

            glGenVertexArrays(1, &m_vaoHandle);
            glBindVertexArray(m_vaoHandle);

            for (const auto &pair : m_arrayBuffers)
            {
                pair.second.buffer->bind();

                glEnableVertexAttribArray(pair.second.descriptor.index);

                glVertexAttribPointer(pair.second.descriptor.index,
                                      pair.second.descriptor.size,
                                      pair.second.descriptor.type,
                                      pair.second.descriptor.normalized,
                                      pair.second.descriptor.stride,
                                      pair.second.descriptor.pointer);

                if (pair.second.descriptor.attribDivisor > 0)
                {
                    glVertexAttribDivisor(pair.second.descriptor.index, pair.second.descriptor.attribDivisor);
                }
            }

            m_elementArrayBuffer.buffer->bind();

            glBindVertexArray(0);

            return true;
        }

        void Geometry::draw() const
        {
            glBindVertexArray(m_vaoHandle);
            glDrawElements(m_elementArrayBuffer.descriptor.drawMode,
                           m_elementArrayBuffer.descriptor.numElements,
                           m_elementArrayBuffer.descriptor.type,
                           0);
            glBindVertexArray(0);
        }

        void Geometry::drawInstanced(GLsizei instancecount) const
        {
            glBindVertexArray(m_vaoHandle);
            glDrawElementsInstanced(m_elementArrayBuffer.descriptor.drawMode,
                                    m_elementArrayBuffer.descriptor.numElements,
                                    m_elementArrayBuffer.descriptor.type,
                                    0,
                                    instancecount);
            glBindVertexArray(0);
        }

        // Buffer ...
        Geometry::Buffer *Geometry::Buffer::make(GLenum target, GLsizeiptr size, const GLvoid *data, GLenum usage)
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

        Geometry::Buffer::~Buffer()
        {
            glDeleteBuffers(1, &m_handle);
        }

        void Geometry::Buffer::bind()
        {
            glBindBuffer(m_target, m_handle);
        }

        void Geometry::Buffer::unbindTarget()
        {
            glBindBuffer(m_target, 0);
        }

        bool Geometry::Buffer::bufferData(GLsizeiptr size, const GLvoid *data)
        {
            glBindBuffer(m_target, m_handle);
            glBufferData(m_target, size, data, m_usage);

            GLint checkSize = -1;
            glGetBufferParameteriv(m_target, GL_BUFFER_SIZE, &checkSize);
            return (size == checkSize);
        }
    } // namespace gl
} // namespace yourgame
