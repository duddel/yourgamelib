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
#include <android/native_window.h>
#include <android_native_app_glue.h>
#include <android/log.h>
#include <EGL/egl.h>
#include "yourgame/yourgame.h"
#include "yourgame/gl_include.h"
#include "yourgame_internal/yourgame_port.h"
#include "yourgame_internal/timer.h"
#include "yourgame_internal/mygame_external.h"

#ifdef YOURGAME_EXTPROJ_imgui
#include "imgui.h"
#include "imgui_impl_android.h"
#include "imgui_impl_opengl3.h"
#endif

INITIALIZE_EASYLOGGINGPP

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
yourgame_internal::Timer _timer(0U);
bool _initialized = false;

EGLDisplay _display = EGL_NO_DISPLAY;
EGLSurface _surface = EGL_NO_SURFACE;
EGLContext _eglContext = EGL_NO_CONTEXT;
ANativeWindow *_win = NULL;

/*
todo: this is called every frame, but actually only required
if the screen size changed. however, the app command
APP_CMD_WINDOW_RESIZED is not invoked reliable.
*/
void updateContextWinSize()
{
    EGLint width, height;
    eglQuerySurface(_display, _surface, EGL_WIDTH, &width);
    eglQuerySurface(_display, _surface, EGL_HEIGHT, &height);
    _context.winWidth = width;
    _context.winHeight = height;
    _context.winAspectRatio = (float)width / (float)height;
}
} // namespace

// API
const yourgame::context &getCtx()
{
    return _context;
}

// do nothing. we do not exit the app manually
void notifyShutdown() {}

// INTERNAL API
bool isInitialized()
{
    return _initialized;
}

void init(struct android_app *app)
{
    if (_initialized)
    {
        return;
    }

    _win = app->window;

    initInput(app);
    initFileIO(app);

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

    // update window size in yourgame context
    updateContextWinSize();

    yourgame::logi("GL_VERSION: %v", glGetString(GL_VERSION));
    yourgame::logi("GL_VENDOR: %v", glGetString(GL_VENDOR));
    yourgame::logi("GL_RENDERER: %v", glGetString(GL_RENDERER));
    yourgame::logi("GL_EXTENSIONS: %v", glGetString(GL_EXTENSIONS));

#ifdef YOURGAME_EXTPROJ_imgui
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO &imgio = ImGui::GetIO();
    //imgio.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    //imgio.IniFilename = NULL;

    ImGui::StyleColorsDark();
    ImGui_ImplAndroid_Init(app);
    ImGui_ImplOpenGL3_Init(YOURGAME_GLSL_VERSION_STRING);
#endif

    mygame::init(_context);

    _initialized = true;
}

void tick()
{
    // update time delta in yourgame context
    _context.deltaTimeUs = _timer.tick();
    _context.deltaTimeS = ((double)_context.deltaTimeUs) * 1.0e-6;

    if (_display != EGL_NO_DISPLAY)
    {
        // update window size in yourgame context
        updateContextWinSize();

#ifdef YOURGAME_EXTPROJ_imgui
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplAndroid_NewFrame();
        ImGui::NewFrame();
#endif
        mygame::update(_context);
        mygame::draw(_context);
#ifdef YOURGAME_EXTPROJ_imgui
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
#endif
        eglSwapBuffers(_display, _surface);
    }
}

void shutdown()
{
    if (!_initialized)
    {
        return;
    }

#ifdef YOURGAME_EXTPROJ_imgui
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplAndroid_Shutdown();
    ImGui::DestroyContext();
#endif

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

    _initialized = false;
}

} // namespace yourgame
