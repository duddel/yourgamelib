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
#include <chrono>
#include "yourgame/gl_include.h"
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#ifdef YOURGAME_EXTPROJ_imgui
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#endif
#ifdef YOURGAME_EXTPROJ_stb
#include "stb_image.h"
#endif
#include "yourgame/yourgame.h"
#include "yourgame_internal/yourgame_internal_desktop.h"
#include "yourgame_internal/mygame_external.h"

INITIALIZE_EASYLOGGINGPP

namespace yourgame_internal_desktop
{
    namespace
    {
        el::Logger *logger = nullptr;
        yourgame::context _context;
        std::chrono::steady_clock::time_point lastNowTime;
        bool _pendingShutdown = false;

        GLFWwindow *_window = NULL;
        void framebufferSizeCallback(GLFWwindow *window, int width, int height)
        {
            _context.winWidth = width;
            _context.winHeight = height;
            _context.winAspectRatio = (float)width / (float)height;
        }
    } // namespace

    bool pendingShutdown()
    {
        return _pendingShutdown;
    }

    int init(int argc, char *argv[])
    {
        // initialize logging
        START_EASYLOGGINGPP(argc, argv);
        logger = el::Loggers::getLogger("default");

        // initialize timing
        lastNowTime = std::chrono::steady_clock::now();
        double clockPeriod = (double)std::chrono::steady_clock::period::num /
                             (double)std::chrono::steady_clock::period::den;
        yourgame::logi("steady_clock precision: %vs (%vns)", clockPeriod, clockPeriod * 1.0e+9);

        // initialize asset loading
        yourgame_internal_desktop::initFileIO();

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

        // todo: determine reasonable initial window size and position
        _window = glfwCreateWindow(1536, 864, "", NULL, NULL);
        if (_window == NULL)
        {
            yourgame::loge("glfwCreateWindow() failed");
            glfwTerminate();
            return -1;
        }
        glfwSetWindowPos(_window, 100, 100);

        glfwMakeContextCurrent(_window);

        glfwSetFramebufferSizeCallback(_window, framebufferSizeCallback);
        // call it manually to set initial size
        int width, height;
        glfwGetFramebufferSize(_window, &width, &height);
        framebufferSizeCallback(_window, width, height);

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

#ifdef YOURGAME_EXTPROJ_stb
        stbi_set_flip_vertically_on_load(true);
#endif

        mygame::init(_context);
        return 0;
    }

    void tick()
    {
        glfwPollEvents();
        yourgame_internal_desktop::tickInput();

        // timing
        auto now = std::chrono::steady_clock::now();
        std::chrono::duration<double> duration = now - lastNowTime;
        lastNowTime = now;
        _context.deltaTimeS = (duration.count());

#ifdef YOURGAME_EXTPROJ_imgui
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
#endif

        mygame::tick(_context);

#ifdef YOURGAME_EXTPROJ_imgui
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
#endif

        glfwSwapBuffers(_window);
    }

    int shutdown()
    {
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

        mygame::shutdown(_context);

        return 0;
    }
} // namespace yourgame_internal_desktop

namespace yourgame
{
    const yourgame::context &getCtx()
    {
        return yourgame_internal_desktop::_context;
    }

    el::Logger *getLogr()
    {
        return yourgame_internal_desktop::logger;
    }

    void notifyShutdown()
    {
        yourgame_internal_desktop::_pendingShutdown = true;
    }

    int sendCmdToEnv(int cmdId, int data0, int data1, int data2)
    {
        return -1;
    }
} // namespace yourgame
