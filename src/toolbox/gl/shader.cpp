/*
Copyright (c) 2019-2024 Alexander Scholz

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
#include <string>
#include "yourgame/gl/shader.h"
#include "yourgame/gl/conventions.h"

namespace yourgame
{
    namespace gl
    {
        namespace
        {
            int compileShader(GLenum type, const GLchar *source, GLuint *handle, std::string &errorLog)
            {
                const GLchar *glsrc = source;
                *handle = glCreateShader(type);
                glShaderSource(*handle, 1, &glsrc, NULL);
                glCompileShader(*handle);

                GLint status;
                glGetShaderiv(*handle, GL_COMPILE_STATUS, &status);
                if (status == GL_FALSE)
                {
                    GLchar infoLog[1024];
                    glGetShaderInfoLog(*handle, 1024, NULL, infoLog);
                    errorLog += infoLog;
                    glDeleteShader(*handle);
                    return -1;
                }

                return 0;
            }
        } // namespace

        Shader *Shader::make(const std::vector<std::pair<GLenum, std::string>> &shaderCodes,
                             const std::vector<std::pair<GLuint, std::string>> &attrLocs,
                             const std::vector<std::pair<GLuint, std::string>> &fragDataLocs,
                             std::string &errorLog)
        {
            std::vector<GLuint> shdrHandles;

            for (const auto &shdrDes : shaderCodes)
            {
                GLuint handle;
                if (compileShader(shdrDes.first, shdrDes.second.c_str(), &handle, errorLog) != 0)
                {
                    for (const auto &shdrHandle : shdrHandles)
                    {
                        glDeleteShader(shdrHandle);
                    }
                    return nullptr;
                }
                shdrHandles.push_back(handle);
            }

            GLuint progHandle = glCreateProgram();

            for (const auto &shdrHandle : shdrHandles)
            {
                glAttachShader(progHandle, shdrHandle);
            }

            for (const auto &attr : attrLocs)
            {
                glBindAttribLocation(progHandle, attr.first, attr.second.c_str());
            }

#ifndef YOURGAME_GL_API_GLES
            for (const auto &frag : fragDataLocs)
            {
                glBindFragDataLocation(progHandle, frag.first, frag.second.c_str());
            }
#endif

            glLinkProgram(progHandle);

            GLint linkStatus;
            glGetProgramiv(progHandle, GL_LINK_STATUS, &linkStatus);
            if (linkStatus == GL_FALSE)
            {
                GLchar infoLog[1024];
                glGetProgramInfoLog(progHandle, 1024, NULL, infoLog);
                errorLog += infoLog;
            }

            for (const auto &shdrHandle : shdrHandles)
            {
                glDetachShader(progHandle, shdrHandle);
                glDeleteShader(shdrHandle);
            }

            if (linkStatus == GL_FALSE)
            {
                glDeleteProgram(progHandle);
                return nullptr;
            }

            // set fixed uniform values for texture units
            {
                glUseProgram(progHandle);
                GLint unif;

                // diffuse texture
                unif = glGetUniformLocation(progHandle, gl::unifNameTextureDiffuse);
                if (unif != -1)
                {
                    glUniform1i(unif, gl::unifValueTextureDiffuse);
                }

                // sky cube texture
                unif = glGetUniformLocation(progHandle, gl::unifNameTextureSkyCube);
                if (unif != -1)
                {
                    glUniform1i(unif, gl::unifValueTextureSkyCube);
                }

                // sky texture
                unif = glGetUniformLocation(progHandle, gl::unifNameTextureSky);
                if (unif != -1)
                {
                    glUniform1i(unif, gl::unifValueTextureSky);
                }

                // framebuffer depth texture
                unif = glGetUniformLocation(progHandle, gl::unifNameTextureBufferDepth);
                if (unif != -1)
                {
                    glUniform1i(unif, gl::unifValueTextureBufferDepth);
                }

                // framebuffer color0 texture
                unif = glGetUniformLocation(progHandle, gl::unifNameTextureBufferColor0);
                if (unif != -1)
                {
                    glUniform1i(unif, gl::unifValueTextureBufferColor0);
                }

                glUseProgram(0);
            }

            Shader *newShader = new Shader();
            newShader->m_programHandle = progHandle;
            return newShader;
        }

        Shader::~Shader()
        {
            glDeleteProgram(m_programHandle);
        }

        void Shader::useProgram(Lightsource *lightsource, yourgame::math::Camera *camera)
        {
            glUseProgram(m_programHandle);

            GLint unif;
            if (lightsource)
            {
                unif = getUniformLocation(gl::unifNameLightAmbient);
                if (unif != -1)
                {
                    glUniform3fv(unif, 1, &lightsource->ambient()[0]);
                }

                unif = getUniformLocation(gl::unifNameLightDiffuse);
                if (unif != -1)
                {
                    glUniform3fv(unif, 1, &lightsource->diffuse()[0]);
                }

                unif = getUniformLocation(gl::unifNameLightSpecular);
                if (unif != -1)
                {
                    glUniform3fv(unif, 1, &lightsource->specular()[0]);
                }

                unif = getUniformLocation(gl::unifNameLightPosition);
                if (unif != -1)
                {
                    glUniform3fv(unif, 1, &lightsource->position()[0]);
                }
            }

            if (camera)
            {
                unif = getUniformLocation(gl::unifNameVpMatrix);
                if (unif != -1)
                {
                    glUniformMatrix4fv(unif, 1, GL_FALSE, glm::value_ptr(camera->pMat() * camera->vMat()));
                }

                unif = getUniformLocation(gl::unifNameCameraTrafo);
                if (unif != -1)
                {
                    glUniformMatrix4fv(unif, 1, GL_FALSE, glm::value_ptr(camera->trafo()->mat()));
                }
            }
        }

        GLint Shader::getUniformLocation(const GLchar *name)
        {
            std::string unifName(name);

            auto unifFound = m_uniformLocations.find(unifName);
            if (unifFound != m_uniformLocations.end())
            {
                return unifFound->second;
            }
            else
            {
                GLint unifLoc = glGetUniformLocation(m_programHandle, name);
                m_uniformLocations[unifName] = unifLoc;
                return unifLoc;
            }
        }
    } // namespace gl
} // namespace yourgame
