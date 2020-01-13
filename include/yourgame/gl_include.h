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

/*
usage:
make sure _exactly_ one of these macros is defined
  YOURGAME_GL_INCLUDE_GLES1
  YOURGAME_GL_INCLUDE_GLES2
  YOURGAME_GL_INCLUDE_GLES3
  YOURGAME_GL_INCLUDE_GLES31
  YOURGAME_GL_INCLUDE_GLES32
  YOURGAME_GL_INCLUDE_GLEW
  YOURGAME_GL_INCLUDE_GL3W
  YOURGAME_GL_INCLUDE_GLAD

wherever gl is required:
  #include "gl_include.h"

do not include any other gl headers before or after gl_include.h
*/
#ifndef YOURGAME_GL_INCLUDE_H
#define YOURGAME_GL_INCLUDE_H

#if defined(GL_TRUE) // assuming GL_TRUE is (only) defined in some gl header
  #error gl header is already included
#elif defined(YOURGAME_GL_INCLUDE_GLES1)
  #include <GLES/gl.h>
#elif defined(YOURGAME_GL_INCLUDE_GLES2)
  #include <GLES2/gl2.h>
#elif defined(YOURGAME_GL_INCLUDE_GLES3)
  #include <GLES3/gl3.h>
#elif defined(YOURGAME_GL_INCLUDE_GLES31)
  #include <GLES3/gl31.h>
#elif defined(YOURGAME_GL_INCLUDE_GLES32)
  #include <GLES3/gl32.h>
#elif defined(YOURGAME_GL_INCLUDE_GLEW)
  #include <GL/glew.h>
#elif defined(YOURGAME_GL_INCLUDE_GL3W)
  #include <GL/gl3w.h>
#elif defined(YOURGAME_GL_INCLUDE_GLAD)
  #include <glad/glad.h>
#else
  #error no gl header included
#endif

#endif
