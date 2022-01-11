/*
Copyright (c) 2019-2022 Alexander Scholz

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
            for (const auto &b : m_buffers)
            {
                delete b.second;
            }

            for (const auto &s : m_shapes)
            {
                delete s.second;
            }
        }

        bool Geometry::addBuffer(std::string name, GLenum target, GLsizeiptr size, const GLvoid *data, GLenum usage)
        {
            if (!m_buffers.count(name))
            {
                Buffer *newBuffer = Buffer::make(target, size, data, usage);
                if (newBuffer)
                {
                    m_buffers.insert(std::pair<std::string, Buffer *>(name, newBuffer));
                    return true;
                }
                return false;
            }
            return false;
        }

        bool Geometry::bufferData(std::string name, GLsizeiptr size, const GLvoid *data)
        {
            auto it = m_buffers.find(name);
            if (it != m_buffers.end())
            {
                return it->second->bufferData(size, data);
            }
            return false;
        }

        bool Geometry::addShape(std::string name,
                                std::vector<Shape::ArrBufferDescr> arDescrs,
                                std::vector<std::string> arBufferNames,
                                Shape::ElemArrBufferDescr elArDescr,
                                std::string elArBufferName)
        {
            if (arDescrs.size() != arBufferNames.size())
            {
                return false;
            }

            if (!m_shapes.count(name))
            {
                std::vector<Buffer *> arBuffers;
                for (const auto &arBufName : arBufferNames)
                {
                    arBuffers.push_back(m_buffers[arBufName]);
                }

                Shape *newShape = Shape::make(arDescrs, arBuffers, elArDescr, m_buffers[elArBufferName]);
                if (newShape)
                {
                    m_shapes.insert(std::pair<std::string, Shape *>(name, newShape));
                    return true;
                }
                return false;
            }
            return false;
        }

        bool Geometry::addBufferToShape(std::string shapeName, std::vector<Shape::ArrBufferDescr> arDescrs, std::string bufferName)
        {
            auto it = m_shapes.find(shapeName);
            if (it != m_shapes.end())
            {
                // the actual buffer only needs to be passed once to GLShape::addArrBuf()
                auto numArDescrs = arDescrs.size();
                if (numArDescrs > 0)
                {
                    it->second->addArrBuf(arDescrs[0], m_buffers[bufferName]);
                }
                for (auto i = 1; i < numArDescrs; i++)
                {
                    it->second->addArrBuf(arDescrs[i], nullptr);
                }
            }
            return false;
        }

        bool Geometry::setShapeElArDescr(std::string name,
                                         Shape::ElemArrBufferDescr elArDescr)
        {
            auto it = m_shapes.find(name);
            if (it != m_shapes.end())
            {
                return it->second->setElArDescr(elArDescr);
            }
            return false;
        }

        void Geometry::drawAll() const
        {
            for (const auto &s : m_shapes)
            {
                s.second->draw();
            }
        }

        void Geometry::drawAllInstanced(GLsizei instancecount) const
        {
            for (const auto &s : m_shapes)
            {
                s.second->drawInstanced(instancecount);
            }
        }
    }
} // namespace yourgame
