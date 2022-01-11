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
#ifndef YOURGAME_GLGEOMETRY_H
#define YOURGAME_GLGEOMETRY_H

#include <string>
#include <map>
#include <vector>
#include "yourgame/gl_include.h"
#include "yourgame/gl/buffer.h"
#include "yourgame/gl/shape.h"

namespace yourgame
{
    namespace gl
    {
        class Geometry
        {
        public:
            static Geometry *make();
            ~Geometry();
            bool addBuffer(std::string name, GLenum target, GLsizeiptr size, const GLvoid *data, GLenum usage);
            bool bufferData(std::string name, GLsizeiptr size, const GLvoid *data);
            bool addShape(std::string name,
                          std::vector<Shape::ArrBufferDescr> arDescrs,
                          std::vector<std::string> arBufferNames,
                          Shape::ElemArrBufferDescr elArDescr,
                          std::string elArBufferName);
            bool addBufferToShape(std::string shapeName,
                                  std::vector<Shape::ArrBufferDescr> arDescrs,
                                  std::string bufferName);
            bool setShapeElArDescr(std::string name,
                                   Shape::ElemArrBufferDescr elArDescr);
            void drawAll() const;
            void drawAllInstanced(GLsizei instancecount) const;

            /* deleting the copy constructor and the copy assignment operator
            prevents copying (and moving) of the object. */
            Geometry(Geometry const &) = delete;
            Geometry &operator=(Geometry const &) = delete;

        private:
            Geometry() {}
            std::map<std::string, Buffer *> m_buffers;
            std::map<std::string, Shape *> m_shapes;
        };
    }
} // namespace yourgame

#endif
