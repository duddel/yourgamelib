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
#include "glmesh.h"

GLMesh *GLMesh::make(std::vector<ArrBufferDescr> arDescrs, ElemArrBufferDescr elArDescr)
{
    GLMesh *newMesh = new GLMesh();
    newMesh->m_elArDescr = elArDescr;

    glGenVertexArrays(1, &newMesh->m_vaoHandle);
    glBindVertexArray(newMesh->m_vaoHandle);

    for (const auto &arrDes : arDescrs)
    {
        arrDes.buffer->bind();
        glEnableVertexAttribArray(arrDes.index);
        glVertexAttribPointer(arrDes.index,
                              arrDes.size,
                              arrDes.type,
                              arrDes.normalized,
                              arrDes.stride,
                              arrDes.pointer);
    }

    newMesh->m_elArDescr.buffer->bind();
    glBindVertexArray(0);

    // todo: what about unbinding the buffers (ARRAY, ELEMENT_ARRAY)
    // after creating the VAO?

    return newMesh;
}

GLMesh::~GLMesh()
{
    glDeleteVertexArrays(1, &m_vaoHandle);
}

void GLMesh::draw()
{
    glBindVertexArray(m_vaoHandle);
    glDrawElements(m_elArDescr.drawMode, m_elArDescr.numElements, m_elArDescr.type, 0);
    glBindVertexArray(0);
}
