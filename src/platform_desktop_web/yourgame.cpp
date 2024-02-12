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
#ifdef __EMSCRIPTEN__
#include <emscripten/html5.h>
#endif
#include "yourgame/gl_include.h"
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#ifdef YOURGAME_EXTPROJ_imgui
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#endif
#include "yourgame/yourgame.h"
#include "yourgame_internal/yourgame_internal_desktop.h"
#include "yourgame_internal/mygame_external.h"
#include "yourgame_internal/input.h"
#include "yourgame_internal/log.h"
#include "yourgame_internal/time.h"

namespace yourgame_internal_desktop
{
    namespace
    {
        bool _wantToExit = false;

        GLFWwindow *_window = NULL;
        void framebufferSizeCallback(GLFWwindow *window, int width, int height)
        {
            yourgame_internal::setInput2(yourgame::input::WINDOW_WIDTH, static_cast<float>(width));
            yourgame_internal::setInput2(yourgame::input::WINDOW_HEIGHT, static_cast<float>(height));
            yourgame_internal::setInput2(yourgame::input::WINDOW_WIDTH_INVERSE,
                                         width < 1 ? 1.0f : 1.0f / static_cast<float>(width));
            yourgame_internal::setInput2(yourgame::input::WINDOW_HEIGHT_INVERSE,
                                         height < 1 ? 1.0f : 1.0f / static_cast<float>(height));
            yourgame_internal::setInput2(
                yourgame::input::WINDOW_ASPECT_RATIO,
                (width < 1 || height < 1) ? 1.0f : static_cast<float>(width) / static_cast<float>(height));
        }
    } // namespace

    bool wantToExit()
    {
        return _wantToExit;
    }

    int init(int argc, char *argv[])
    {
        // initialize logging
        yourgame_internal::log::init(argc, argv);

        // initialize file io
        yourgame_internal_desktop::initFile();

#ifndef YOURGAME_CLIMODE
        // initialize glfw, gl
        yourgame::log::info("glfwInit()...");
        if (!glfwInit())
        {
            yourgame::log::error("glfwInit() failed");
            return -1;
        }

#if defined(YOURGAME_GL_API_GLES)
        glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_ES_API);
#elif defined(YOURGAME_GL_API_GL)
        glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_API);
#endif
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, YOURGAME_GL_MAJOR);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, YOURGAME_GL_MINOR);

        // create the window
#ifdef __EMSCRIPTEN__
        {
            // the desired size of the glfw "window" is the size of the canvas
            // element in the .html file.
            double widthD, heightD;
            auto ret = emscripten_get_element_css_size("#canvas", &widthD, &heightD);
            if (ret == EMSCRIPTEN_RESULT_SUCCESS)
            {
                int width = static_cast<int>(widthD);
                int height = static_cast<int>(heightD);
                _window = glfwCreateWindow(width, height, "", NULL, NULL);
            }
            else
            {
                _window = glfwCreateWindow(960, 540, "", NULL, NULL);
            }
        }
#else
        // create window with the current primary monitor mode ("desktop")
        const GLFWvidmode *mode = glfwGetVideoMode(glfwGetPrimaryMonitor());
        glfwWindowHint(GLFW_RED_BITS, mode->redBits);
        glfwWindowHint(GLFW_GREEN_BITS, mode->greenBits);
        glfwWindowHint(GLFW_BLUE_BITS, mode->blueBits);
        glfwWindowHint(GLFW_REFRESH_RATE, mode->refreshRate);
        if (yourgame::input::geti(yourgame::input::WINDOW_FULLSCREEN))
        {
            // create fullscreen window
            _window = glfwCreateWindow(mode->width, mode->height, "", glfwGetPrimaryMonitor(), NULL);
        }
        else
        {
            // create windowed window, roughly centered, 0.8 times desktop resolution
            _window = glfwCreateWindow((mode->width * 8) / 10, (mode->height * 8) / 10, "", NULL, NULL);
            if (_window)
            {
                glfwSetWindowPos(_window, mode->width / 10, mode->height / 10);
            }
        }
#endif

        if (!_window)
        {
            yourgame::log::error("glfwCreateWindow() failed");
            glfwTerminate();
            return -1;
        }

        glfwMakeContextCurrent(_window);

        glfwSetFramebufferSizeCallback(_window, framebufferSizeCallback);
        // call it manually to set initial size
        int width, height;
        glfwGetFramebufferSize(_window, &width, &height);
        framebufferSizeCallback(_window, width, height);

#if defined(YOURGAME_GL_EXT_LOADER_GLAD)
        yourgame::log::info("gladLoadGL()...");
        if (!gladLoadGL())
        {
            yourgame::log::error("gladLoadGL() failed");
            return -1;
        }
#endif
        yourgame::log::info("GL_VERSION: %v", glGetString(GL_VERSION));
        yourgame::log::info("GL_VENDOR: %v", glGetString(GL_VENDOR));
        yourgame::log::info("GL_RENDERER: %v", glGetString(GL_RENDERER));
        yourgame::log::info("GL_SHADING_LANGUAGE_VERSION: %v", glGetString(GL_SHADING_LANGUAGE_VERSION));

        glfwSwapInterval(yourgame::input::geti(yourgame::input::VSYNC_ON) ? 1 : 0);

        // enable raw mouse input if supported. affects catched mouse mode, see catchMouse()
// todo: raw mouse motion available since glfw v3.3,
// emscripten implements glfw v3.2 API only (2021-01-19)
#ifndef __EMSCRIPTEN__
        if (glfwRawMouseMotionSupported())
        {
            glfwSetInputMode(_window, GLFW_RAW_MOUSE_MOTION, GLFW_TRUE);
        }
#endif

        yourgame_internal_desktop::initInput(_window);

#ifdef YOURGAME_EXTPROJ_imgui
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGuiIO &imgio = ImGui::GetIO();
        imgio.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
        imgio.IniFilename = NULL;

        ImGui::StyleColorsDark();
        ImGui_ImplGlfw_InitForOpenGL(_window, true);
        ImGui_ImplOpenGL3_Init(YOURGAME_GLSL_VERSION_STRING);
#endif
#endif // #ifndef YOURGAME_CLIMODE

        mygame::init(argc, argv);

        // initialize timing:
        // get the initial time point just before tick() starts, which will
        // result in a small time delta in the first tick() cycle.
        // todo: is it desirable to have time delta == 0.0 in first tick() cycle?
        yourgame_internal::time::init();
        yourgame::log::info("steady_clock precision: %vs (%vns)", yourgame::time::getClockPeriod(), yourgame::time::getClockPeriod() * 1.0e+9);

        return 0;
    }

    void tick()
    {
        // timing
        yourgame_internal::time::tick();

#ifndef YOURGAME_CLIMODE
#ifdef __EMSCRIPTEN__
        {
            // the desired size of the glfw "window" is the size of the canvas
            // element in the .html file, which can change if the browser
            // window is resized or set to fullscreen.
            // glfwSetWindowSize() seems to take care of resizing the framebuffer.
            // todo: do this via a "canvas resized" callback, if available
            double widthD, heightD;
            auto ret = emscripten_get_element_css_size("#canvas", &widthD, &heightD);
            int width = static_cast<int>(widthD);
            int height = static_cast<int>(heightD);
            if (ret == EMSCRIPTEN_RESULT_SUCCESS &&
                (yourgame::input::geti(yourgame::input::WINDOW_WIDTH) != width ||
                 yourgame::input::geti(yourgame::input::WINDOW_HEIGHT) != height))
            {
                glfwSetWindowSize(_window, width, height);
            }
        }
#endif

#ifdef YOURGAME_EXTPROJ_imgui
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
#endif
#endif // #ifndef YOURGAME_CLIMODE

        mygame::tick();

#ifndef YOURGAME_CLIMODE
#ifdef YOURGAME_EXTPROJ_imgui
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
#endif

        glfwSwapBuffers(_window);

        yourgame_internal_desktop::tickInput();
// todo: glfw callbacks seem to be invoked by emscripten automatically
// sometime after glfwSwapBuffers()
#ifndef __EMSCRIPTEN__
        glfwPollEvents();
#endif
#endif // #ifndef YOURGAME_CLIMODE
    }

    int shutdown()
    {
        int ret = mygame::shutdown();

#ifndef YOURGAME_CLIMODE
#ifdef YOURGAME_EXTPROJ_imgui
        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplGlfw_Shutdown();
        ImGui::DestroyContext();
#endif

        if (_window != NULL)
        {
            glfwDestroyWindow(_window);
        }
        glfwTerminate();
#endif // #ifndef YOURGAME_CLIMODE

        return ret;
    }
} // namespace yourgame_internal_desktop

namespace yourgame
{
    namespace control
    {
        void exit()
        {
            yourgame_internal_desktop::_wantToExit = true;
        }

        int sendCmdToEnv(int cmdId, int data0, int data1, int data2)
        {
            return -1;
        }

        void enableFullscreen(bool enable)
        {
#ifdef __EMSCRIPTEN__
            // todo
#else
            auto mode = glfwGetVideoMode(glfwGetPrimaryMonitor());
            if (enable)
            {
                // set fullscreen
                glfwSetWindowMonitor(yourgame_internal_desktop::_window, glfwGetPrimaryMonitor(), 0, 0, mode->width, mode->height, mode->refreshRate);
            }
            else
            {
                // set windowed, roughly centered, 0.8 times desktop resolution
                glfwSetWindowMonitor(yourgame_internal_desktop::_window, NULL, 0, 0, (mode->width * 8) / 10, (mode->height * 8) / 10, mode->refreshRate);
                glfwSetWindowPos(yourgame_internal_desktop::_window, mode->width / 10, mode->height / 10);
            }
            yourgame_internal::setInput2(yourgame::input::WINDOW_FULLSCREEN, enable ? 1.0f : 0.0f);
            // todo: glfwSwapInterval() is required to be called again after
            // window mode changed: https://github.com/glfw/glfw/issues/1072
            glfwSwapInterval(yourgame::input::geti(yourgame::input::VSYNC_ON) ? 1 : 0);
#endif
        }

        void enableVSync(bool enable)
        {
            glfwSwapInterval(enable ? 1 : 0);
            yourgame_internal::setInput2(yourgame::input::VSYNC_ON, enable ? 1.0f : 0.0f);
        }

        void catchMouse(bool enable)
        {
            glfwSetInputMode(yourgame_internal_desktop::_window, GLFW_CURSOR, enable ? GLFW_CURSOR_DISABLED : GLFW_CURSOR_NORMAL);
            yourgame_internal::setInput2(yourgame::input::MOUSE_CATCHED, enable ? 1.0f : 0.0f);
        }
    } // namespace control
} // namespace yourgame
