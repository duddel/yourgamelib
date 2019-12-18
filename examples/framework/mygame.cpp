/*
Copyright (c) 2019 Alexander Scholz

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
#include <cmath>
#include <vector>
#include "yourgame/assetfile.h"
#include "yourgame/yourgame.h"
#include "yourgame/gl_include.h"
#include "yourgame/input.h"

namespace mygame
{

namespace
{
double colorFadingT = 0.0;
GLuint vaoHandle;
GLuint progHandle;
GLint unifLocLight;

/*
sets up a gl test scene with some basics:
vbo, ibo, vao, vertex and fragment shaders.
shaded triangle with varying brightness (via uniform).
shaders are loaded from assets. limited error detection.
*/
initGlTest()
{
    // attribute location of position vertex data
    const GLuint attrLocPosition = 3;
    // triangle vertex positions
    const GLfloat vertPos[] = {-0.5f, -0.5f, 0.0f, 0.5f, -0.5f, 0.0f, 0.0f, 0.75f, 0.0f};
    // vertex indices
    const GLuint vertIdx[] = {0, 1, 2};

    // VBO
    auto vertPosSize = sizeof(vertPos);
    GLuint vboHandle;
    glGenBuffers(1, &vboHandle);
    glBindBuffer(GL_ARRAY_BUFFER, vboHandle);
    glBufferData(GL_ARRAY_BUFFER, vertPosSize, vertPos, GL_STATIC_DRAW);
    GLint checkSize = -1;
    glGetBufferParameteriv(GL_ARRAY_BUFFER, GL_BUFFER_SIZE, &checkSize);
    if (vertPosSize != checkSize)
    {
        yourgame::loge("VBO creation failed: size of data: %v, size in buffer: %v", vertPosSize, checkSize);
    }

    // IBO
    auto vertIdxSize = sizeof(vertIdx);
    GLuint iboHandle;
    glGenBuffers(1, &iboHandle);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, iboHandle);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, vertIdxSize, vertIdx, GL_STATIC_DRAW);
    checkSize = -1;
    glGetBufferParameteriv(GL_ELEMENT_ARRAY_BUFFER, GL_BUFFER_SIZE, &checkSize);
    if (vertIdxSize != checkSize)
    {
        yourgame::loge("IBO creation failed: size of data: %v, size in buffer: %v", vertIdxSize, checkSize);
    }

    // VAO
    glGenVertexArrays(1, &vaoHandle);
    glBindVertexArray(vaoHandle);
    glEnableVertexAttribArray(attrLocPosition);
    glVertexAttribPointer(attrLocPosition, 3, GL_FLOAT, GL_FALSE, 0, (void *)0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, iboHandle);
    glBindVertexArray(0);

    // shaders
    yourgame::logi("attempting to read glsl shaders from assets:");
#ifdef YOURGAME_GL_API_GLES
    auto vertCode = yourgame::readAssetFile("simple.es.vert");
    auto fragCode = yourgame::readAssetFile("simple.es.frag");
#else
    auto vertCode = yourgame::readAssetFile("simple.vert");
    auto fragCode = yourgame::readAssetFile("simple.frag");
#endif

    std::string vertShader(vertCode.begin(), vertCode.end());
    std::string fragShader(fragCode.begin(), fragCode.end());

    yourgame::logi("vertex shader:\n%v", std::string(vertShader.begin(), vertShader.end()));
    yourgame::logi("fragment shader:\n%v", std::string(fragShader.begin(), fragShader.end()));

    // vertex shader
    const GLchar *glsrc = vertShader.c_str();
    GLuint vertHandle = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertHandle, 1, &glsrc, NULL);
    glCompileShader(vertHandle);

    // fragment shader
    glsrc = fragShader.c_str();
    GLuint fragHandle = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragHandle, 1, &glsrc, NULL);
    glCompileShader(fragHandle);

    // shader program
    progHandle = glCreateProgram();
    glAttachShader(progHandle, vertHandle);
    glAttachShader(progHandle, fragHandle);
    glBindAttribLocation(progHandle, attrLocPosition, "posi");
#ifndef YOURGAME_GL_API_GLES
    glBindFragDataLocation(progHandle, 0, "color");
#endif
    glLinkProgram(progHandle);

    GLint linkStatus;
    glGetProgramiv(progHandle, GL_LINK_STATUS, &linkStatus);
    if (linkStatus == GL_FALSE)
    {
        GLint infoLen;
        glGetShaderiv(progHandle, GL_INFO_LOG_LENGTH, &infoLen);
        GLchar *infoLog = new GLchar[infoLen + 1];
        glGetProgramInfoLog(progHandle, infoLen, NULL, infoLog);
        yourgame::loge("shader program linking failed: %v", infoLog);
        delete[] infoLog;
    }

    unifLocLight = glGetUniformLocation(progHandle, "light");

    glDetachShader(progHandle, vertHandle);
    glDetachShader(progHandle, fragHandle);
    glDeleteShader(vertHandle);
    glDeleteShader(fragHandle);
}

} // namespace

void init(const yourgame::context &ctx)
{
    yourgame::logi("setting up gl test scene:");
    initGlTest();
}

void update(const yourgame::context &ctx)
{
    colorFadingT += (1.0 * ctx.deltaTimeS);

    if (yourgame::getInputi(yourgame::InputSource::YOURGAME_KEY_ESCAPE))
    {
        yourgame::notifyShutdown();
    }

    if (yourgame::getInputi(yourgame::InputSource::YOURGAME_MOUSE_BUTTON_1))
        yourgame::logi("mouse: %v/%v", yourgame::getInputf(yourgame::InputSource::YOURGAME_MOUSE_X),
                       yourgame::getInputf(yourgame::InputSource::YOURGAME_MOUSE_Y));

    if (yourgame::getInputi(yourgame::InputSource::YOURGAME_TOUCH_0_DOWN))
        yourgame::logi("touch 0: %v/%v", yourgame::getInputf(yourgame::InputSource::YOURGAME_TOUCH_0_X),
                       yourgame::getInputf(yourgame::InputSource::YOURGAME_TOUCH_0_Y));
}

void draw(const yourgame::context &ctx)
{
    float colFade = (float)((sin(colorFadingT) * 0.5) + 0.5);
    glClearColor(colFade, 1.0f - colFade, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // draw the gl test scene
    glUseProgram(progHandle);
    glUniform1f(unifLocLight, colFade);
    glBindVertexArray(vaoHandle);
    glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}

void shutdown(const yourgame::context &ctx)
{
}

} // namespace mygame
