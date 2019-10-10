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
#include <algorithm> // std::replace()
// todo: where define GLFW_INCLUDE_NONE?
#ifndef GLFW_INCLUDE_NONE
#define GLFW_INCLUDE_NONE
#endif
#include <GLFW/glfw3.h>
#include "easylogging++.h"
#ifdef YOURGAME_USE_WHEREAMI
#include "whereami.h"
#endif
#include "yourgame/gl_include.h"
#include "yourgame/timer.h"
#include "yourgame/yourgame.h"

INITIALIZE_EASYLOGGINGPP

namespace yourgame
{

namespace
{
yourgame::context _context;
yourgame::Timer _timer(0U);
GLFWwindow *_window = NULL;
void (*_cbInit)(const yourgame::context &ctx) = NULL;
void (*_cbUpdate)(const yourgame::context &ctx) = NULL;
void (*_cbDraw)(const yourgame::context &ctx) = NULL;
void (*_cbShutdown)(const yourgame::context &ctx) = NULL;
} // namespace

void registerCbInit(void (*func)(const yourgame::context &ctx)) { _cbInit = func; }
void registerCbUpdate(void (*func)(const yourgame::context &ctx)) { _cbUpdate = func; }
void registerCbDraw(void (*func)(const yourgame::context &ctx)) { _cbDraw = func; }
void registerCbShutdown(void (*func)(const yourgame::context &ctx)) { _cbShutdown = func; }

const yourgame::context &getCtx()
{
    return _context;
}

int init(int argc, char *argv[])
{
    // initialize logging
    START_EASYLOGGINGPP(argc, argv);
    _context.logger = el::Loggers::getLogger("default");

    // check the timer clock
    yourgame::logi("timer clock is%v monotonic", _timer.isMonotonic() ? "" : " NOT");

// get absolute path to assets/ along the executable
// and make it available via the context
#ifdef YOURGAME_USE_WHEREAMI
    int exeBasePathLength;
    int exePathLength = wai_getExecutablePath(NULL, 0, NULL);
    char *path = (char *)malloc(exePathLength + 1);
    wai_getExecutablePath(path, exePathLength, &exeBasePathLength);
    path[exeBasePathLength + 1] = '\0';
    _context.assetPath = path;
    free(path);
    std::replace(_context.assetPath.begin(), _context.assetPath.end(), '\\', '/');
    _context.assetPath += "assets/";
#else
    _context.assetPath = "/assets/";
#endif

    // initialize glfw, gl
    yourgame::logi("glfwInit()...");
    if (!glfwInit())
    {
        yourgame::loge("glfwInit() failed");
        return -1;
    }

#if defined(YOURGAME_GL_API_GLES)
    glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_ES_API);
#elif defined(YOURGAME_GL_API_GL)
    glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_API);
#endif
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, YOURGAME_GL_MAJOR);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, YOURGAME_GL_MINOR);

    _window = glfwCreateWindow(800, 512, "", NULL, NULL);
    if (_window == NULL)
    {
        yourgame::loge("glfwCreateWindow() failed");
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(_window);

#if defined(YOURGAME_GL_EXT_LOADER_GLAD)
    yourgame::logi("gladLoadGL()...");
    if (!gladLoadGL())
    {
        yourgame::loge("gladLoadGL() failed");
        return -1;
    }
#endif
    yourgame::logi("GL_VERSION: %v", glGetString(GL_VERSION));
    yourgame::logi("GL_VENDOR: %v", glGetString(GL_VENDOR));
    yourgame::logi("GL_RENDERER: %v", glGetString(GL_RENDERER));
    yourgame::logi("GL_SHADING_LANGUAGE_VERSION: %v", glGetString(GL_SHADING_LANGUAGE_VERSION));

    glfwSwapInterval(1);

    if (_cbInit != NULL)
    {
        _cbInit(_context);
    }

    return 0;
}

void tick()
{
    glfwPollEvents();

    // update context
    _context.deltaTimeUs = _timer.tick();
    _context.deltaTimeS = ((double)_context.deltaTimeUs) * 1.0e-6;

    if (_cbUpdate != NULL)
    {
        _cbUpdate(_context);
    }

    if (_cbDraw != NULL)
    {
        _cbDraw(_context);
    }

    glfwSwapBuffers(_window);
}

int shutdown()
{
    if (_window != NULL)
    {
        glfwDestroyWindow(_window);
    }
    glfwTerminate();

    if (_cbShutdown != NULL)
    {
        _cbShutdown(_context);
    }

    return 0;
}

} // namespace yourgame
