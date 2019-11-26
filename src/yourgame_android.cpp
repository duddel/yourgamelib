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
#include <android/native_window.h>
#include <android/log.h>
#include <EGL/egl.h>
#include "yourgame/gl_include.h"
#include "easylogging++.h"
#include "yourgame/timer.h"
#include "yourgame/yourgame.h"

INITIALIZE_EASYLOGGINGPP

namespace mygame
{
extern void init(const yourgame::context &ctx);
extern void update(const yourgame::context &ctx);
extern void draw(const yourgame::context &ctx);
extern void shutdown(const yourgame::context &ctx);
} // namespace mygame

namespace yourgame
{

class elAndroidDispatcher : public el::LogDispatchCallback
{
protected:
    void handle(const el::LogDispatchData *data) noexcept override
    {
        auto logMsg = data->logMessage();
        switch (logMsg->level())
        {
        case el::Level::Debug:
            __android_log_print(ANDROID_LOG_DEBUG, "yourgame_app", "%s", logMsg->message().c_str());
            break;
        case el::Level::Info:
            __android_log_print(ANDROID_LOG_INFO, "yourgame_app", "%s", logMsg->message().c_str());
            break;
        case el::Level::Warning:
            __android_log_print(ANDROID_LOG_WARN, "yourgame_app", "%s", logMsg->message().c_str());
            break;
        case el::Level::Error:
            __android_log_print(ANDROID_LOG_ERROR, "yourgame_app", "%s", logMsg->message().c_str());
            break;
        }
    }
};

namespace
{
yourgame::context _context;
yourgame::Timer _timer(0U);

EGLDisplay _display = EGL_NO_DISPLAY;
EGLSurface _surface = EGL_NO_SURFACE;
EGLContext _eglContext = EGL_NO_CONTEXT;
ANativeWindow *_win = NULL;
} // namespace

const yourgame::context &getCtx()
{
    return _context;
}

void init(ANativeWindow *win)
{
    _win = win;

    // initialize logging
    _context.logger = el::Loggers::getLogger("default");
    el::Helpers::installLogDispatchCallback<elAndroidDispatcher>("AndroidDispatcher");
    el::Helpers::uninstallLogDispatchCallback<el::base::DefaultLogDispatchCallback>("DefaultLogDispatchCallback");
    elAndroidDispatcher *dispatcher = el::Helpers::logDispatchCallback<elAndroidDispatcher>("AndroidDispatcher");
    dispatcher->setEnabled(true);

    // check the timer clock
    yourgame::logi("timer clock is%v monotonic", _timer.isMonotonic() ? "" : " NOT");

    // initialize EGL
    _display = eglGetDisplay(EGL_DEFAULT_DISPLAY);
    if (_display == EGL_NO_DISPLAY)
    {
        yourgame::loge("eglGetDisplay(EGL_DEFAULT_DISPLAY) returned EGL_NO_DISPLAY");
    }

    if (eglInitialize(_display, 0, 0) != EGL_TRUE)
    {
        yourgame::loge("eglInitialize(..) returned with an error");
    }

    // use these attributes to choose a matching EGL frame buffer config
    const EGLint eglAttribs[] = {
        EGL_BLUE_SIZE, 8,
        EGL_GREEN_SIZE, 8,
        EGL_RED_SIZE, 8,
        EGL_DEPTH_SIZE, 24,
        EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
        EGL_NONE};

    EGLint numConfigs = 0;
    if (eglChooseConfig(_display, eglAttribs, nullptr, 0, &numConfigs) != EGL_TRUE)
    {
        yourgame::loge("eglChooseConfig(..) returned with an error");
    }

    if (numConfigs == 0)
    {
        yourgame::loge("eglChooseConfig(..) returned 0 matching configs");
    }

    /* the first returned config matches the desired attributes "best",
    but might not be the ideal choice. we take in anyway.
    todo: we could check attributes of the matching configs and choose
    a suitable config by some criteria (with eglGetConfigAttrib(..)) */
    yourgame::logi("choosing the first egl config (of %v total configs)", numConfigs);

    // get the actual (first) matching config
    EGLConfig config;
    eglChooseConfig(_display, eglAttribs, &config, 1, &numConfigs);

    /* the attribute EGL_NATIVE_VISUAL_ID is an element of the enum AHardwareBuffer_Format,
    that matches the chosen egl config. configure the buffers of the native window
    appropriately (via ANativeWindow_setBuffersGeometry(..)) */
    EGLint format;
    eglGetConfigAttrib(_display, config, EGL_NATIVE_VISUAL_ID, &format);
    yourgame::logi("calling ANativeWindow_setBuffersGeometry(..) with format %v", format);
    ANativeWindow_setBuffersGeometry(_win, 0, 0, format);

    // create context, surface and make the context current
    const EGLint contextAttribs[] = {
        EGL_CONTEXT_CLIENT_VERSION, YOURGAME_GL_MAJOR,
        EGL_NONE};
    _eglContext = eglCreateContext(_display, config, EGL_NO_CONTEXT, contextAttribs);
    if (_eglContext == EGL_NO_CONTEXT)
    {
        yourgame::loge("eglCreateContext(..) returned EGL_NO_CONTEXT");
    }
    _surface = eglCreateWindowSurface(_display, config, _win, NULL);
    eglMakeCurrent(_display, _surface, _surface, _eglContext);

    // get width and height of surface
    // todo: communicate surface width and height to the framework
    EGLint width;
    EGLint height;
    eglQuerySurface(_display, _surface, EGL_WIDTH, &width);
    eglQuerySurface(_display, _surface, EGL_HEIGHT, &height);

    yourgame::logi("GL_VERSION: %v", glGetString(GL_VERSION));
    yourgame::logi("GL_VENDOR: %v", glGetString(GL_VENDOR));
    yourgame::logi("GL_RENDERER: %v", glGetString(GL_RENDERER));
    yourgame::logi("GL_EXTENSIONS: %v", glGetString(GL_EXTENSIONS));

    mygame::init(_context);
}

void tick()
{
    // update context
    _context.deltaTimeUs = _timer.tick();
    _context.deltaTimeS = ((double)_context.deltaTimeUs) * 1.0e-6;

    mygame::update(_context);

    mygame::draw(_context);

    eglSwapBuffers(_display, _surface);
}

int shutdown()
{
    if (_display != EGL_NO_DISPLAY)
    {
        eglMakeCurrent(_display, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);

        if (_eglContext != EGL_NO_CONTEXT)
        {
            eglDestroyContext(_display, _eglContext);
        }

        if (_surface != EGL_NO_SURFACE)
        {
            eglDestroySurface(_display, _surface);
        }

        eglTerminate(_display);
    }

    _display = EGL_NO_DISPLAY;
    _eglContext = EGL_NO_CONTEXT;
    _surface = EGL_NO_SURFACE;

    mygame::shutdown(_context);

    return 0;
}

} // namespace yourgame
