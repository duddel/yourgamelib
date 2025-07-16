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
#ifndef YOURGAME_GLGEOMETRY_H
#define YOURGAME_GLGEOMETRY_H

#include <string>
#include <map>
#include <vector>
#include "yourgame/gl_include.h"

namespace yourgame
{
    namespace gl
    {
        class Geometry
        {
        public:
            struct ArrayBufferDescriptor
            {
                GLuint index;
                GLint size;
                GLenum type;
                GLboolean normalized;
                GLsizei stride;
                const GLvoid *pointer;
                GLuint attribDivisor;
            };

            struct ElementArrayBufferDescriptor
            {
                GLenum type;
                GLenum drawMode;
                GLsizei numElements;
            };

            static Geometry *make();
            ~Geometry();

            bool addArrayBuffer(std::string name,
                                GLsizeiptr size,
                                const GLvoid *data,
                                GLenum usage,
                                ArrayBufferDescriptor descriptor);

            bool bufferArrayData(std::string name, GLsizeiptr size, const GLvoid *data);

            bool setElementArrayBuffer(GLsizeiptr size,
                                       const GLvoid *data,
                                       GLenum usage,
                                       ElementArrayBufferDescriptor descriptor);

            bool init();

            void draw() const;
            void drawInstanced(GLsizei instancecount) const;

            /* deleting the copy constructor and the copy assignment operator
            prevents copying (and moving) of the object. */
            Geometry(Geometry const &) = delete;
            Geometry &operator=(Geometry const &) = delete;

        private:
            class Buffer
            {
            public:
                static Buffer *make(GLenum target, GLsizeiptr size, const GLvoid *data, GLenum usage);
                ~Buffer();
                void bind();
                void unbindTarget();
                bool bufferData(GLsizeiptr size, const GLvoid *data);

                /* deleting the copy constructor and the copy assignment operator
                prevents copying (and moving) of the object. */
                Buffer(Buffer const &) = delete;
                Buffer &operator=(Buffer const &) = delete;

            private:
                Buffer() {}
                GLenum m_target;
                GLuint m_handle;
                GLenum m_usage;
            };

            struct ArrayBuffer
            {
                Buffer *buffer = nullptr;
                ArrayBufferDescriptor descriptor;
            };

            struct ElementArrayBuffer
            {
                Buffer *buffer = nullptr;
                ElementArrayBufferDescriptor descriptor;
            };

            Geometry() {}
            std::map<std::string, ArrayBuffer> m_arrayBuffers;
            ElementArrayBuffer m_elementArrayBuffer;
            GLuint m_vaoHandle = 0;
        };
    } // namespace gl
} // namespace yourgame

#endif
