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
#ifndef YOURGAME_GLTEXTURE2D_H
#define YOURGAME_GLTEXTURE2D_H

#include <vector>
#include <utility> // pair
#include "yourgame/gl_include.h"

namespace yourgame
{

class GLTexture2D
{
public:
    static GLTexture2D *make(GLint level,
                             GLint internalformat,
                             GLsizei width,
                             GLsizei height,
                             GLint border,
                             GLenum format,
                             GLenum type,
                             const void *data,
                             GLenum unit,
                             std::vector<std::pair<GLenum, GLint>> parameteri,
                             bool generateMipmap);

    /* deleting the copy constructor and the copy assignment operator
    prevents copying (and moving) of the object. */
    GLTexture2D(GLTexture2D const &) = delete;
    GLTexture2D &operator=(GLTexture2D const &) = delete;

    ~GLTexture2D();
    void bind();
    void unbindTarget();

    GLsizei m_width;
    GLsizei m_height;

private:
    GLTexture2D() {}
    GLuint m_handle;
    GLenum m_unit;
};

}

#endif
