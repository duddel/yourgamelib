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
#ifndef GLGEOMETRY_H
#define GLGEOMETRY_H

#include <string>
#include <map>
#include <vector>
#include "yourgame/gl_include.h"
#include "glbuffer.h"
#include "glshape.h"

class GLGeometry
{
public:
    static GLGeometry *make();
    ~GLGeometry();
    bool addBuffer(std::string name, GLenum target, GLsizeiptr size, const GLvoid *data, GLenum usage);
    bool addShape(std::string name,
                  std::vector<GLShape::ArrBufferDescr> arDescrs,
                  std::vector<std::string> arBufferNames,
                  GLShape::ElemArrBufferDescr elArDescr,
                  std::string elArBufferName);
    void drawAll();

    /* deleting the copy constructor and the copy assignment operator
    prevents copying (and moving) of the object. */
    GLGeometry(GLGeometry const &) = delete;
    GLGeometry &operator=(GLGeometry const &) = delete;

private:
    GLGeometry() {}
    std::map<std::string, GLBuffer *> m_buffers;
    std::map<std::string, GLShape *> m_shapes;
};

#endif
