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
#include "yourgame/glshape.h"

namespace yourgame
{
    GLShape *GLShape::make(std::vector<ArrBufferDescr> arDescrs,
                           std::vector<GLBuffer *> arBuffers,
                           ElemArrBufferDescr elArDescr,
                           GLBuffer *elArBuffer)
    {
        if (arDescrs.size() != arBuffers.size())
        {
            return nullptr;
        }

        GLShape *newShape = new GLShape();
        newShape->m_elArDescr = elArDescr;

        glGenVertexArrays(1, &newShape->m_vaoHandle);
        glBindVertexArray(newShape->m_vaoHandle);

        for (auto i = 0; i < arDescrs.size(); i++)
        {
            arBuffers[i]->bind();
            glEnableVertexAttribArray(arDescrs[i].index);
            glVertexAttribPointer(arDescrs[i].index,
                                  arDescrs[i].size,
                                  arDescrs[i].type,
                                  arDescrs[i].normalized,
                                  arDescrs[i].stride,
                                  arDescrs[i].pointer);
        }

        elArBuffer->bind();
        glBindVertexArray(0);

        // todo: what about unbinding the buffers (ARRAY, ELEMENT_ARRAY)
        // after creating the VAO?

        return newShape;
    }

    bool GLShape::setElArDescr(GLShape::ElemArrBufferDescr elArDescr)
    {
        m_elArDescr = elArDescr;
        return true;
    }

    GLShape::~GLShape()
    {
        glDeleteVertexArrays(1, &m_vaoHandle);
    }

    void GLShape::draw()
    {
        glBindVertexArray(m_vaoHandle);
        glDrawElements(m_elArDescr.drawMode, m_elArDescr.numElements, m_elArDescr.type, 0);
        glBindVertexArray(0);
    }
} // namespace yourgame
