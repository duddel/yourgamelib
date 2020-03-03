// dear imgui: Platform Binding for Android native app
// This needs to be used along with the OpenGL 3 Renderer (imgui_impl_opengl3)

// Implemented features:
//  [x] Basic mouse input via touch
//  [ ] Open soft keyboard if io.WantTextInput and perform proper keyboard input

// You can copy and use unmodified imgui_impl_* files in your project. See main.cpp for an example of using this.
// If you are new to dear imgui, read examples/README.txt and read the documentation at the top of imgui.cpp.
// https://github.com/ocornut/imgui

// CHANGELOG
// (minor and older changes stripped away, please see git history for details)
//  2020-03-02: basic draft

#include "imgui.h"
#include "imgui_impl_android.h"
#include <ctime>

// Android
#include <android_native_app_glue.h>
#include <android/input.h>
#include <android/keycodes.h>
#include <android/log.h>

static double               g_Time = 0.0;
static struct android_app*  g_App;
static char                 g_logTag[] = "ImguiExample";
static int32_t (*g_UserOnInputEvent)(struct android_app *app, AInputEvent *event) = NULL;

int32_t ImGui_ImplAndroid_handleInputEvent(struct android_app *app, AInputEvent *inputEvent)
{
    if (g_UserOnInputEvent != NULL)
        g_UserOnInputEvent(app, inputEvent);

    ImGuiIO &io = ImGui::GetIO();
    int32_t evType = AInputEvent_getType(inputEvent);
    switch (evType)
    {
    case AINPUT_EVENT_TYPE_KEY:
        break;
    case AINPUT_EVENT_TYPE_MOTION:
    {
        int32_t evAction = AMotionEvent_getAction(inputEvent);
        int32_t evPointerIndex = (evAction & AMOTION_EVENT_ACTION_POINTER_INDEX_MASK) >> AMOTION_EVENT_ACTION_POINTER_INDEX_SHIFT;
        int32_t evPointerId = AMotionEvent_getPointerId(inputEvent, evPointerIndex);
        evAction &= AMOTION_EVENT_ACTION_MASK;
        switch (evAction)
        {
        case AMOTION_EVENT_ACTION_DOWN:
        case AMOTION_EVENT_ACTION_UP:
            io.MouseDown[0] = (evAction == AMOTION_EVENT_ACTION_DOWN) ? true : false;
            // intended fallthrough...
        case AMOTION_EVENT_ACTION_MOVE:
            io.MousePos = ImVec2(
                AMotionEvent_getX(inputEvent, evPointerIndex),
                AMotionEvent_getY(inputEvent, evPointerIndex));
            break;
        default:
            break;
        }
    }
        return 1;
    default:
        break;
    }

    return 0;
}

bool ImGui_ImplAndroid_Init(struct android_app *app, int32_t (*userOnInputEvent)(struct android_app *app, AInputEvent *event))
{
    g_App = app;
    g_UserOnInputEvent = userOnInputEvent;
    g_Time = 0.0;
    g_App->onInputEvent = ImGui_ImplAndroid_handleInputEvent;

    // Setup back-end capabilities flags
    ImGuiIO &io = ImGui::GetIO();
    // todo: any reasonable io.BackendFlags?
    io.BackendPlatformName = "imgui_impl_android";

    return true;
}

void ImGui_ImplAndroid_Shutdown()
{
    g_UserOnInputEvent = NULL;
}

void ImGui_ImplAndroid_NewFrame()
{
    ImGuiIO &io = ImGui::GetIO();
    IM_ASSERT(io.Fonts->IsBuilt() && "Font atlas not built! It is generally built by the renderer back-end. Missing call to renderer _NewFrame() function? e.g. ImGui_ImplOpenGL3_NewFrame().");

    if (io.WantTextInput)
    {
        // todo: Show soft keyboard
        // Something like: ANativeActivity_showSoftInput(g_App->activity, ANATIVEACTIVITY_SHOW_SOFT_INPUT_FORCED);
    }

    // Setup display size (every frame to accommodate for window resizing)
    // todo: utilize the actual framebuffer size
    int32_t w = ANativeWindow_getWidth(g_App->window);
    int32_t h = ANativeWindow_getHeight(g_App->window);
    int display_w = w;
    int display_h = h;

    io.DisplaySize = ImVec2((float)w, (float)h);
    if (w > 0 && h > 0)
        io.DisplayFramebufferScale = ImVec2((float)display_w / w, (float)display_h / h);

    // Setup time step
    struct timespec current_timespec;
    clock_gettime(CLOCK_MONOTONIC, &current_timespec);
    double current_time = (double)(current_timespec.tv_sec) + (current_timespec.tv_nsec / 1000000000.0);
    io.DeltaTime = g_Time > 0.0 ? (float)(current_time - g_Time) : (float)(1.0f / 60.0f);
    g_Time = current_time;
}
