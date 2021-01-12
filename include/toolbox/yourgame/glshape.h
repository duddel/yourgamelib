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
#ifndef YOURGAME_GLSHAPE_H
#define YOURGAME_GLSHAPE_H

#include <vector>
#include "yourgame/gl_include.h"
#include "glbuffer.h"

namespace yourgame
{
    class GLShape
    {
    public:
        struct ArrBufferDescr
        {
            GLuint index;
            GLint size;
            GLenum type;
            GLboolean normalized;
            GLsizei stride;
            const GLvoid *pointer;
        };

        struct ElemArrBufferDescr
        {
            GLenum type;
            GLenum drawMode;
            GLsizei numElements;
        };

        static GLShape *make(std::vector<ArrBufferDescr> arDescrs,
                             std::vector<GLBuffer *> arBuffers,
                             ElemArrBufferDescr elArDescr,
                             GLBuffer *elArBuffer);
        bool setElArDescr(GLShape::ElemArrBufferDescr elArDescr);
        ~GLShape();
        void draw();

        /* deleting the copy constructor and the copy assignment operator
        prevents copying (and moving) of the object. */
        GLShape(GLShape const &) = delete;
        GLShape &operator=(GLShape const &) = delete;

    private:
        GLShape() {}
        ElemArrBufferDescr m_elArDescr;
        GLuint m_vaoHandle;
    };
} // namespace yourgame

#endif
