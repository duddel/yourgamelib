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
#include <string>
#include "yourgame/gl_include.h"
#include "yourgame/glshader.h"

namespace yourgame
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
                GLint infoLen;
                glGetShaderiv(*handle, GL_INFO_LOG_LENGTH, &infoLen);
                // todo: check infoLen before creating infoLog array
                GLchar *infoLog = new GLchar[infoLen + 1];
                glGetShaderInfoLog(*handle, infoLen, NULL, infoLog);
                errorLog += infoLog;
                delete[] infoLog;
                glDeleteShader(*handle);
                return -1;
            }

            return 0;
        }

    } // namespace

    GLShader *GLShader::make(std::vector<std::pair<GLenum, std::string>> shaderCodes,
                             std::vector<std::pair<GLuint, std::string>> attrLocs,
                             std::vector<std::pair<GLuint, std::string>> fragDataLocs,
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
            GLint infoLen;
            glGetShaderiv(progHandle, GL_INFO_LOG_LENGTH, &infoLen);
            // todo: check infoLen before creating infoLog array
            GLchar *infoLog = new GLchar[infoLen + 1];
            glGetProgramInfoLog(progHandle, infoLen, NULL, infoLog);
            errorLog += infoLog;
            delete[] infoLog;
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

        GLShader *newShader = new GLShader();
        newShader->m_programHandle = progHandle;
        return newShader;
    }

    GLShader::~GLShader()
    {
        glDeleteProgram(m_programHandle);
    }

    void GLShader::useProgram()
    {
        glUseProgram(m_programHandle);
    }

    GLint GLShader::getUniformLocation(const GLchar *name)
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
} // namespace yourgame
