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
#include "yourgame/gl/shape.h"

namespace yourgame
{
    namespace gl
    {
        Shape *Shape::make(std::vector<ArrBufferDescr> arDescrs,
                           std::vector<Buffer *> arBuffers,
                           ElemArrBufferDescr elArDescr,
                           Buffer *elArBuffer)
        {
            if (arDescrs.size() != arBuffers.size())
            {
                return nullptr;
            }

            Shape *newShape = new Shape();
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

        bool Shape::setElArDescr(Shape::ElemArrBufferDescr elArDescr)
        {
            m_elArDescr = elArDescr;
            return true;
        }

        bool Shape::addArrBuf(ArrBufferDescr arDescr, Buffer *buf)
        {
            glBindVertexArray(m_vaoHandle);

            if (buf)
            {
                buf->bind();
            }
            glEnableVertexAttribArray(arDescr.index);
            glVertexAttribPointer(arDescr.index,
                                  arDescr.size,
                                  arDescr.type,
                                  arDescr.normalized,
                                  arDescr.stride,
                                  arDescr.pointer);

            if (arDescr.attribDivisor > 0)
            {
                glVertexAttribDivisor(arDescr.index, arDescr.attribDivisor);
            }

            glBindVertexArray(0);
            return true;
        }

        Shape::~Shape()
        {
            glDeleteVertexArrays(1, &m_vaoHandle);
        }

        void Shape::draw()
        {
            glBindVertexArray(m_vaoHandle);
            glDrawElements(m_elArDescr.drawMode, m_elArDescr.numElements, m_elArDescr.type, 0);
            glBindVertexArray(0);
        }

        void Shape::drawInstanced(GLsizei instancecount)
        {
            glBindVertexArray(m_vaoHandle);
            glDrawElementsInstanced(m_elArDescr.drawMode, m_elArDescr.numElements, m_elArDescr.type, 0, instancecount);
            glBindVertexArray(0);
        }
    }
} // namespace yourgame
