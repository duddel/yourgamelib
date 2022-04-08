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
#include <android/native_window.h>
#include <android_native_app_glue.h>
#include <jni.h>
#include <EGL/egl.h>
#ifdef YOURGAME_EXTPROJ_imgui
#include "imgui.h"
#include "imgui_impl_android.h"
#include "imgui_impl_opengl3.h"
#endif
#ifdef YOURGAME_EXTPROJ_stb
#include "stb_image.h"
#include "stb_image_write.h"
#endif
#include "yourgame/yourgame.h"
#include "yourgame/gl_include.h"
#include "yourgame_internal/yourgame_internal_android.h"
#include "yourgame_internal/mygame_external.h"
#include "yourgame_internal/input.h"
#include "yourgame_internal/log.h"
#include "yourgame_internal/time.h"

namespace yourgame_internal_android
{
    namespace
    {
        bool _initialized = false;

        EGLDisplay _display = EGL_NO_DISPLAY;
        EGLSurface _surface = EGL_NO_SURFACE;
        EGLContext _eglContext = EGL_NO_CONTEXT;
        ANativeWindow *_win = NULL;
        struct android_app *_app = NULL;

        /* todo: this is called every frame, but actually only required
        if the screen size changed. however, the app command
        APP_CMD_WINDOW_RESIZED is not invoked reliable.*/
        void updateWinSize()
        {
            EGLint width, height;
            eglQuerySurface(_display, _surface, EGL_WIDTH, &width);
            eglQuerySurface(_display, _surface, EGL_HEIGHT, &height);
            yourgame_internal::setInput2(yourgame::input::WINDOW_WIDTH, static_cast<float>(width));
            yourgame_internal::setInput2(yourgame::input::WINDOW_HEIGHT, static_cast<float>(height));
            yourgame_internal::setInput2(yourgame::input::WINDOW_ASPECT_RATIO,
                                         static_cast<float>(width) / static_cast<float>(height));
        }
    } // namespace

    bool isInitialized()
    {
        return _initialized;
    }

    static int32_t onInputEvent(struct android_app *app, AInputEvent *inputEvent)
    {
        yourgame_internal_android::handleInputEvent(inputEvent);
#ifdef YOURGAME_EXTPROJ_imgui
        ImGui_ImplAndroid_HandleInputEvent(inputEvent);
#endif
        return 1;
    }

    void init(struct android_app *app)
    {
        if (_initialized)
        {
            return;
        }

        _win = app->window;
        _app = app;

        _app->onInputEvent = onInputEvent;
        yourgame_internal_android::initFile(_app);

        // initialize logging
        char *initLogArgv = nullptr;
        yourgame_internal::log::init(0, &initLogArgv);

        // initialize EGL
        _display = eglGetDisplay(EGL_DEFAULT_DISPLAY);
        if (_display == EGL_NO_DISPLAY)
        {
            yourgame::log::error("eglGetDisplay(EGL_DEFAULT_DISPLAY) returned EGL_NO_DISPLAY");
        }

        if (eglInitialize(_display, 0, 0) != EGL_TRUE)
        {
            yourgame::log::error("eglInitialize(..) returned with an error");
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
            yourgame::log::error("eglChooseConfig(..) returned with an error");
        }

        if (numConfigs == 0)
        {
            yourgame::log::error("eglChooseConfig(..) returned 0 matching configs");
        }

        /* the first returned config matches the desired attributes "best",
        but might not be the ideal choice. we take in anyway.
        todo: we could check attributes of the matching configs and choose
        a suitable config by some criteria (with eglGetConfigAttrib(..)) */
        yourgame::log::info("choosing the first egl config (of %v total configs)", numConfigs);

        // get the actual (first) matching config
        EGLConfig config;
        eglChooseConfig(_display, eglAttribs, &config, 1, &numConfigs);

        /* the attribute EGL_NATIVE_VISUAL_ID is an element of the enum AHardwareBuffer_Format,
        that matches the chosen egl config. configure the buffers of the native window
        appropriately (via ANativeWindow_setBuffersGeometry(..)) */
        EGLint format;
        eglGetConfigAttrib(_display, config, EGL_NATIVE_VISUAL_ID, &format);
        yourgame::log::info("calling ANativeWindow_setBuffersGeometry(..) with format %v", format);
        ANativeWindow_setBuffersGeometry(_win, 0, 0, format);

        // create context, surface and make the context current
        const EGLint contextAttribs[] = {
            EGL_CONTEXT_CLIENT_VERSION, YOURGAME_GL_MAJOR,
            EGL_NONE};
        _eglContext = eglCreateContext(_display, config, EGL_NO_CONTEXT, contextAttribs);
        if (_eglContext == EGL_NO_CONTEXT)
        {
            yourgame::log::error("eglCreateContext(..) returned EGL_NO_CONTEXT");
        }
        _surface = eglCreateWindowSurface(_display, config, _win, NULL);
        eglMakeCurrent(_display, _surface, _surface, _eglContext);

        // update window size in yourgame
        updateWinSize();

        yourgame::log::info("GL_VERSION: %v", glGetString(GL_VERSION));
        yourgame::log::info("GL_VENDOR: %v", glGetString(GL_VENDOR));
        yourgame::log::info("GL_RENDERER: %v", glGetString(GL_RENDERER));
        yourgame::log::info("GL_EXTENSIONS: %v", glGetString(GL_EXTENSIONS));

#ifdef YOURGAME_EXTPROJ_imgui
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGuiIO &imgio = ImGui::GetIO();
        // imgio.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
        // imgio.IniFilename = NULL;

        ImGui::StyleColorsDark();
        ImGui_ImplAndroid_Init(_win);
        ImGui_ImplOpenGL3_Init(YOURGAME_GLSL_VERSION_STRING);
#endif

#ifdef YOURGAME_EXTPROJ_stb
        stbi_set_flip_vertically_on_load(true);
        stbi_flip_vertically_on_write(true);
#endif

        char *initArgv = nullptr;
        mygame::init(0, &initArgv);

        _initialized = true;

        // initialize timing:
        // get the initial time point just before tick() starts, which will
        // result in a small time delta in the first tick() cycle.
        // todo: is it desirable to have time delta == 0.0 in first tick() cycle?
        yourgame_internal::time::init();
        yourgame::log::info("steady_clock precision: %vs (%vns)", yourgame::time::getClockPeriod(), yourgame::time::getClockPeriod() * 1.0e+9);
    }

    void tick()
    {
        // timing
        yourgame_internal::time::tick();

        if (_display != EGL_NO_DISPLAY)
        {
            // update window size in yourgame
            updateWinSize();

#ifdef YOURGAME_EXTPROJ_imgui
            ImGui_ImplOpenGL3_NewFrame();
            ImGui_ImplAndroid_NewFrame();
            ImGui::NewFrame();
#endif
            mygame::tick();
#ifdef YOURGAME_EXTPROJ_imgui
            ImGui::Render();
            ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
#endif
            eglSwapBuffers(_display, _surface);
        }

        yourgame_internal_android::tickInput();
    }

    int shutdown()
    {
        if (!_initialized)
        {
            return;
        }

        int ret = mygame::shutdown();

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

        _initialized = false;

        return ret;
    }
} // namespace yourgame_internal_android

namespace yourgame
{
    namespace control
    {
        void exit()
        {
            ANativeActivity_finish(yourgame_internal_android::_app->activity);
        }

        int sendCmdToEnv(int cmdId, int data0, int data1, int data2)
        {
            // the whole procedure (get environment, etc.) is done every time to
            // not risk inconsistencies with global references to JNI objects.

            JavaVM *jVM = yourgame_internal_android::_app->activity->vm;
            JNIEnv *jEnv = NULL;

            // todo: clearify JNI version to request
            jint jniRet = jVM->GetEnv((void **)&jEnv, JNI_VERSION_1_6);
            if (jniRet == JNI_ERR)
            {
                return -1;
            }

            jniRet = jVM->AttachCurrentThread(&jEnv, NULL);
            if (jniRet != JNI_OK)
            {
                return -2;
            }

            jclass natActClazz = jEnv->GetObjectClass(yourgame_internal_android::_app->activity->clazz);
            if (natActClazz == NULL)
            {
                return -3;
            }

            // get the "method ID"
            jmethodID methodID = jEnv->GetMethodID(natActClazz, "receiveCmd", "(IIII)I");
            if (methodID == NULL)
            {
                return -4;
            }

            // call the actual method
            jint methodRet = jEnv->CallIntMethod(yourgame_internal_android::_app->activity->clazz, methodID, cmdId, data0, data1, data2);
            if (methodRet != 0)
            {
                return -10;
            }

            jniRet = jVM->DetachCurrentThread();
            if (jniRet != JNI_OK)
            {
                return -5;
            }

            return 0;
        }

        void enableFullscreen(bool enable) {}
        void enableVSync(bool enable) {}
        void catchMouse(bool enable) {}
    }
} // namespace yourgame
