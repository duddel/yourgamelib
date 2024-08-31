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
#ifdef EMSCRIPTEN_USE_PORT_CONTRIB_GLFW3
#include <GLFW/emscripten_glfw3.h>
#endif
#include <GLFW/glfw3.h>
#ifdef YOURGAME_EXTPROJ_imgui
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#endif
#include "yourgame/yourgame.h"
#include "yourgame_internal/mygame_external.h"
#include "yourgame_internal/file_desktop.h"
#include "yourgame_internal/input.h"
#include "yourgame_internal/input_desktop.h"
#include "yourgame_internal/log.h"
#include "yourgame_internal/time.h"
#ifdef YOURGAME_TOOLBOX
#include "yourgame_internal/gl/init.h"
#include "yourgame_internal/util/assets.h"
#include "yourgame_internal/util/postproc.h"
#endif

namespace
{
    bool _wantToExit = false;
    GLFWwindow *_window = NULL;

    void framebufferSizeCallback(GLFWwindow *window, int width, int height)
    {
        yourgame::log::debug("GLFW framebuffer resized: %v,%v", width, height);

        yourgame_internal::input::setInput2(yourgame::input::WINDOW_WIDTH, static_cast<float>(width));
        yourgame_internal::input::setInput2(yourgame::input::WINDOW_HEIGHT, static_cast<float>(height));
        yourgame_internal::input::setInput2(yourgame::input::WINDOW_WIDTH_INVERSE,
                                            width < 1 ? 1.0f : 1.0f / static_cast<float>(width));
        yourgame_internal::input::setInput2(yourgame::input::WINDOW_HEIGHT_INVERSE,
                                            height < 1 ? 1.0f : 1.0f / static_cast<float>(height));
        yourgame_internal::input::setInput2(yourgame::input::WINDOW_ASPECT_RATIO,
                                            (width < 1 || height < 1) ? 1.0f : static_cast<float>(width) / static_cast<float>(height));
        yourgame_internal::input::setInput2(yourgame::input::WINDOW_ASPECT_RATIO_INVERSE,
                                            (width < 1 || height < 1) ? 1.0f : static_cast<float>(height) / static_cast<float>(width));
    }

    void errorCallback(int errorCode, char const *errorMessage)
    {
        yourgame::log::error("GLFW error %v: %v", errorCode, errorMessage);
    }

#ifdef __EMSCRIPTEN__
    bool getHtmlCanvasSize(int *width, int *height)
    {
        if (width == nullptr || height == nullptr)
        {
            return false;
        }

        double widthD, heightD;
        if (emscripten_get_element_css_size("#canvas", &widthD, &heightD) !=
            EMSCRIPTEN_RESULT_SUCCESS)
        {
            return false;
        }

        *width = static_cast<int>(widthD);
        *height = static_cast<int>(heightD);

        return true;
    }
#endif
} // namespace

namespace yourgame_internal
{
    void tick()
    {
        yourgame_internal::time::tick();

#ifndef YOURGAME_CLIMODE
#if defined(__EMSCRIPTEN__) && !defined(EMSCRIPTEN_USE_PORT_CONTRIB_GLFW3)
        {
            int width, height;
            if (getHtmlCanvasSize(&width, &height))
            {
                if (yourgame::input::geti(yourgame::input::WINDOW_WIDTH) != width ||
                    yourgame::input::geti(yourgame::input::WINDOW_HEIGHT) != height)
                {
                    yourgame::log::debug("Canvas resized. Calling glfwSetWindowSize(%v, %v, %v)", _window, width, height);
                    glfwSetWindowSize(_window, width, height);
                    // Assuming framebuffer is resized and the FramebufferSizeCallback
                    // is invoked after glfwSetWindowSize()
                }
            }
        }
#endif

#ifdef YOURGAME_EXTPROJ_imgui
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
#endif

#ifdef YOURGAME_TOOLBOX
        yourgame_internal::util::postproc::activate();
#endif
#endif // #ifndef YOURGAME_CLIMODE

        mygame::tick();

#ifndef YOURGAME_CLIMODE
#ifdef YOURGAME_TOOLBOX
        yourgame_internal::util::postproc::deactivateAndDraw();
#endif

#ifdef YOURGAME_EXTPROJ_imgui
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
#endif

        glfwSwapBuffers(_window);

        yourgame_internal::input::tickInput();

        // ToDo: glfwPollEvents() seems to be doing nothing with emscripten GLFW
        // stock implementation (-sUSE_GLFW=3), but likely does not harm.
        glfwPollEvents();
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

    namespace desktop
    {
        bool wantToExit()
        {
            return _wantToExit;
        }

        int init(int argc, char *argv[])
        {
            yourgame_internal::log::init(argc, argv);

            yourgame_internal::file::desktop::initFile();

// Log YourGameLib GL version info
#if defined(YOURGAME_GL_API_GL)
            yourgame::log::info("YOURGAME_GL_API_GL");
#elif defined(YOURGAME_GL_API_GLES)
            yourgame::log::info("YOURGAME_GL_API_GLES");
#endif
            yourgame::log::info("YOURGAME_GLSL_VERSION_STRING: %v", YOURGAME_GLSL_VERSION_STRING);
            yourgame::log::info("YOURGAME_GL_MAJOR: %v", YOURGAME_GL_MAJOR);
            yourgame::log::info("YOURGAME_GL_MINOR: %v", YOURGAME_GL_MINOR);

#ifndef YOURGAME_CLIMODE
            glfwSetErrorCallback(errorCallback);

            yourgame::log::info("GLFW %v, glfwInit()...", glfwGetVersionString());
            if (!glfwInit())
            {
                yourgame::log::error("glfwInit() failed");
                return -1;
            }

// Set GL version hints for GLFW
#if defined(YOURGAME_GL_API_GLES)
            glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_ES_API);
#elif defined(YOURGAME_GL_API_GL)
            glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_API);
#endif
            glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, YOURGAME_GL_MAJOR);
            glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, YOURGAME_GL_MINOR);

// Window creation
#ifdef __EMSCRIPTEN__
#ifdef EMSCRIPTEN_USE_PORT_CONTRIB_GLFW3
            emscripten_glfw_set_next_window_canvas_selector("#canvas");
            _window = glfwCreateWindow(960, 540, "", NULL, NULL);
            if (_window)
            {
                emscripten_glfw_make_canvas_resizable(_window, "window", nullptr);
            }
#else
            {
                // Create GLFW window matching canvas size
                int width, height;
                if (getHtmlCanvasSize(&width, &height))
                {
                    _window = glfwCreateWindow(width, height, "", NULL, NULL);
                }
                else
                {
                    _window = glfwCreateWindow(960, 540, "", NULL, NULL);
                }
            }
#endif
#else
            // create window with the current primary monitor mode ("desktop")
            const GLFWvidmode *mode = glfwGetVideoMode(glfwGetPrimaryMonitor());
            glfwWindowHint(GLFW_RED_BITS, mode->redBits);
            glfwWindowHint(GLFW_GREEN_BITS, mode->greenBits);
            glfwWindowHint(GLFW_BLUE_BITS, mode->blueBits);
            glfwWindowHint(GLFW_REFRESH_RATE, mode->refreshRate);
            if (yourgame::input::geti(yourgame::input::WINDOW_FULLSCREEN))
            {
                // Fullscreen
                _window = glfwCreateWindow(mode->width, mode->height, "", glfwGetPrimaryMonitor(), NULL);
            }
            else
            {
                // Window, centered, 80% of desktop resolution
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

            // Set the framebufferSizeCallback and call it once, to announce the initial size
            glfwSetFramebufferSizeCallback(_window, framebufferSizeCallback);
            {
                int width, height;
                glfwGetFramebufferSize(_window, &width, &height);
                framebufferSizeCallback(_window, width, height);
            }

#if defined(YOURGAME_GL_EXT_LOADER_GLAD)
            yourgame::log::info("gladLoadGL()...");
            if (!gladLoadGL())
            {
                yourgame::log::error("gladLoadGL() failed");
                return -1;
            }
#endif

            // Log GL info
            yourgame::log::info("GL_VERSION: %v", glGetString(GL_VERSION));
            yourgame::log::info("GL_VENDOR: %v", glGetString(GL_VENDOR));
            yourgame::log::info("GL_RENDERER: %v", glGetString(GL_RENDERER));
            yourgame::log::info("GL_SHADING_LANGUAGE_VERSION: %v", glGetString(GL_SHADING_LANGUAGE_VERSION));

            glfwSwapInterval(yourgame::input::geti(yourgame::input::VSYNC_ON) ? 1 : 0);

            if (glfwRawMouseMotionSupported())
            {
                glfwSetInputMode(_window, GLFW_RAW_MOUSE_MOTION, GLFW_TRUE);
            }

            yourgame_internal::input::desktop::initInput(_window);

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

#ifdef YOURGAME_TOOLBOX
            yourgame_internal::gl::initState();

            if (yourgame_internal::util::assets::loadStockAssets())
            {
                yourgame::log::info("Loading stock assets: success");
            }
            else
            {
                yourgame::log::error("Loading stock assets: FAILED");
            }
#endif
#endif // #ifndef YOURGAME_CLIMODE

            mygame::init(argc, argv);

            // Initialize timing
            // First cycle will see a small time delta
            yourgame_internal::time::init();
            yourgame::log::info("steady_clock precision: %vs (%vns)", yourgame::time::getClockPeriod(), yourgame::time::getClockPeriod() * 1.0e+9);

            return 0;
        }
    } // namespace desktop
} // namespace yourgame_internal

namespace yourgame
{
    namespace control
    {
        void exit()
        {
            _wantToExit = true;
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
                glfwSetWindowMonitor(_window, glfwGetPrimaryMonitor(), 0, 0, mode->width, mode->height, mode->refreshRate);
            }
            else
            {
                // Window, centered, 80% of desktop resolution
                glfwSetWindowMonitor(_window, NULL, 0, 0, (mode->width * 8) / 10, (mode->height * 8) / 10, mode->refreshRate);
                glfwSetWindowPos(_window, mode->width / 10, mode->height / 10);
            }
            yourgame_internal::input::setInput2(yourgame::input::WINDOW_FULLSCREEN, enable ? 1.0f : 0.0f);
#endif
        }

        void enableVSync(bool enable)
        {
            glfwSwapInterval(enable ? 1 : 0);
            yourgame_internal::input::setInput2(yourgame::input::VSYNC_ON, enable ? 1.0f : 0.0f);
        }

        void catchMouse(bool enable)
        {
            glfwSetInputMode(_window, GLFW_CURSOR, enable ? GLFW_CURSOR_DISABLED : GLFW_CURSOR_NORMAL);
            yourgame_internal::input::setInput2(yourgame::input::MOUSE_CATCHED, enable ? 1.0f : 0.0f);
        }
    } // namespace control
} // namespace yourgame
