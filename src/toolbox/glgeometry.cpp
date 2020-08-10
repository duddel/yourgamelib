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
#include "yourgame/glgeometry.h"

namespace yourgame
{

GLGeometry *GLGeometry::make()
{
    return new GLGeometry();
}

GLGeometry::~GLGeometry()
{
    for (const auto &b : m_buffers)
    {
        delete b.second;
    }

    for (const auto &s : m_shapes)
    {
        delete s.second;
    }
}

bool GLGeometry::addBuffer(std::string name, GLenum target, GLsizeiptr size, const GLvoid *data, GLenum usage)
{
    if (!m_buffers.count(name))
    {
        GLBuffer *newBuffer = GLBuffer::make(target, size, data, usage);
        if (newBuffer)
        {
            m_buffers.insert(std::pair<std::string, GLBuffer *>(name, newBuffer));
            return true;
        }
        return false;
    }
    return false;
}

bool GLGeometry::addShape(std::string name,
                          std::vector<GLShape::ArrBufferDescr> arDescrs,
                          std::vector<std::string> arBufferNames,
                          GLShape::ElemArrBufferDescr elArDescr,
                          std::string elArBufferName)
{
    if (arDescrs.size() != arBufferNames.size())
    {
        return false;
    }

    if (!m_shapes.count(name))
    {
        std::vector<GLBuffer *> arBuffers;
        for (const auto &arBufName : arBufferNames)
        {
            arBuffers.push_back(m_buffers[arBufName]);
        }

        GLShape *newShape = GLShape::make(arDescrs, arBuffers, elArDescr, m_buffers[elArBufferName]);
        if (newShape)
        {
            m_shapes.insert(std::pair<std::string, GLShape *>(name, newShape));
            return true;
        }
        return false;
    }
    return false;
}

void GLGeometry::drawAll()
{
    for (const auto &s : m_shapes)
    {
        s.second->draw();
    }
}

}
