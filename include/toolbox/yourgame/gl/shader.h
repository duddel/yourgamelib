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
#ifndef YOURGAME_GLSHADER_H
#define YOURGAME_GLSHADER_H

#include <string>
#include <vector>
#include <map>
#include "yourgame/gl_include.h"
#include "yourgame/gl/lightsource.h"
#include "yourgame/math/camera.h"

namespace yourgame
{
    namespace gl
    {
        class Shader
        {
        public:
            static Shader *make(const std::vector<std::pair<GLenum, std::string>> &shaderCodes,
                                const std::vector<std::pair<GLuint, std::string>> &attrLocs,
                                const std::vector<std::pair<GLuint, std::string>> &fragDataLocs,
                                std::string &errorLog);
            ~Shader();
            void useProgram(Lightsource *lightsource = nullptr, yourgame::math::Camera *camera = nullptr);
            GLint getUniformLocation(const GLchar *name);

            /* deleting the copy constructor and the copy assignment operator
    prevents copying (and moving) of the object. */
            Shader(Shader const &) = delete;
            Shader &operator=(Shader const &) = delete;

        private:
            Shader() {}
            GLuint m_programHandle;
            std::map<std::string, GLint> m_uniformLocations;
        };
    }
} // namespace yourgame

#endif
