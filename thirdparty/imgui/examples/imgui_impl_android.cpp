// dear imgui: Platform Binding for Android native app
// This needs to be used along with the OpenGL 3 Renderer (imgui_impl_opengl3)

// Implemented features:
//  [x] Basic mouse input via touch
//  [x] Open soft keyboard if io.WantTextInput and perform proper keyboard input
//  [ ] Handle Unicode characters
//  [ ] Handle physical mouse input

// You can copy and use unmodified imgui_impl_* files in your project. See main.cpp for an example of using this.
// If you are new to dear imgui, read examples/README.txt and read the documentation at the top of imgui.cpp.
// https://github.com/ocornut/imgui

// CHANGELOG
// (minor and older changes stripped away, please see git history for details)
//  2020-03-02: basic draft, touch input
//  2020-08-31: On-screen and physical keyboard input (ASCII characters only)

#include "imgui.h"
#include "imgui_impl_android.h"
#include <ctime>
#include <map>
#include <queue>

// Android
#include <android_native_app_glue.h>
#include <android/input.h>
#include <android/keycodes.h>
#include <android/log.h>

static double                                   g_Time = 0.0;
static struct android_app*                      g_App;
static char                                     g_logTag[] = "ImguiExample";
static std::map<int32_t, std::queue<int32_t>>   g_keyEventQueues;
static int32_t (*g_UserOnInputEvent)(struct android_app *app, AInputEvent *event) = NULL;

// Unfortunately, there is no way to show the on-screen input from native code.
// Therefore, we call showSoftInput() of the main activity implemented in MainActivity.kt via JNI.
int ImGui_ImplAndroid_showSoftInput()
{
    JavaVM *jVM = g_App->activity->vm;
    JNIEnv *jEnv = NULL;

    jint jniRet = jVM->GetEnv((void **)&jEnv, JNI_VERSION_1_6);
    if (jniRet == JNI_ERR)
        return -1;

    jniRet = jVM->AttachCurrentThread(&jEnv, NULL);
    if (jniRet != JNI_OK)
        return -2;

    jclass natActClazz = jEnv->GetObjectClass(g_App->activity->clazz);
    if (natActClazz == NULL)
        return -3;

    jmethodID methodID = jEnv->GetMethodID(natActClazz, "showSoftInput", "()V");
    if (methodID == NULL)
        return -4;

    jEnv->CallVoidMethod(g_App->activity->clazz, methodID);

    jniRet = jVM->DetachCurrentThread();
    if (jniRet != JNI_OK)
        return -5;

    return 0;
}

// Unfortunately, the native KeyEvent implementation has no getUnicodeChar() function.
// Therefore, we construct a non-native KeyEvent instance and call getUnicodeChar() here via JNI.
// todo: This whole procedure is done on every key press and deserves optimization
int ImGui_ImplAndroid_getCharacter(int event_type, int key_code, int meta_state)
{
    JavaVM *jVM = g_App->activity->vm;
    JNIEnv *jEnv = NULL;

    jint jniRet = jVM->GetEnv((void **)&jEnv, JNI_VERSION_1_6);
    if (jniRet == JNI_ERR)
        return -1;

    jniRet = jVM->AttachCurrentThread(&jEnv, NULL);
    if (jniRet != JNI_OK)
        return -2;

    jclass keyEventCls = jEnv->FindClass("android/view/KeyEvent");
    if (keyEventCls == NULL)
        return -3;

    jmethodID keyEventCtor = jEnv->GetMethodID(keyEventCls, "<init>", "(II)V");
    if (keyEventCtor == NULL)
        return -4;

    jmethodID keyEventGetUnicodeChar = jEnv->GetMethodID(keyEventCls, "getUnicodeChar", "(I)I");
    if (keyEventGetUnicodeChar == NULL)
        return -5;

    jobject jKeyEvent = jEnv->NewObject(keyEventCls, keyEventCtor, event_type, key_code);
    jint keyEventChar = jEnv->CallIntMethod(jKeyEvent, keyEventGetUnicodeChar, meta_state);

    jVM->DetachCurrentThread();
    if (jniRet != JNI_OK)
        return -6;

    return keyEventChar;
}

int32_t ImGui_ImplAndroid_handleInputEvent(struct android_app *app, AInputEvent *inputEvent)
{
    if (g_UserOnInputEvent != NULL)
        g_UserOnInputEvent(app, inputEvent);

    ImGuiIO &io = ImGui::GetIO();
    int32_t evType = AInputEvent_getType(inputEvent);
    switch (evType)
    {
    case AINPUT_EVENT_TYPE_KEY:
    {
        int32_t evKeyCode = AKeyEvent_getKeyCode(inputEvent);
        int32_t evAction = AKeyEvent_getAction(inputEvent);
        int32_t evMetaState = AKeyEvent_getMetaState(inputEvent);

        io.KeyCtrl = ((evMetaState & AMETA_CTRL_ON) != 0);
        io.KeyShift = ((evMetaState & AMETA_SHIFT_ON) != 0);
        io.KeyAlt = ((evMetaState & AMETA_ALT_ON) != 0);

        switch (evAction)
        {
        // todo: AKEY_EVENT_ACTION_DOWN and AKEY_EVENT_ACTION_UP occur at once
        // as soon as a touch pointer goes up from a key. We use a simple key event queue
        // and process one event per key per ImGui frame in ImGui_ImplAndroid_NewFrame().
        // ...or consider ImGui IO queue, if suitable: https://github.com/ocornut/imgui/issues/2787
        case AKEY_EVENT_ACTION_DOWN:
        {
            // todo: The procedure in ImGui_ImplAndroid_getCharacter() returns ASCII characters
            // only. To get unicode characters, we may have to consider AKEY_EVENT_ACTION_MULTIPLE
            // and feed the result into ImGui via io.AddInputCharactersUTF8()
            int character = ImGui_ImplAndroid_getCharacter(evAction, evKeyCode, evMetaState);
            // make sure the character really is ASCII only
            if (character > 127)
                character = 63; // questionmark
            io.AddInputCharacter(character);
        }
        // intended fallthrough...
        case AKEY_EVENT_ACTION_UP:
            g_keyEventQueues[evKeyCode].push(evAction);
            break;
        case AKEY_EVENT_ACTION_MULTIPLE:
            break;
        default:
            break;
        }
        break;
    }
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

    // Keyboard mapping. ImGui will use those indices to peek into the io.KeysDown[] array.
    io.KeyMap[ImGuiKey_Tab] = AKEYCODE_TAB;
    io.KeyMap[ImGuiKey_LeftArrow] = AKEYCODE_DPAD_LEFT;   // also covers physical keyboard arrow key
    io.KeyMap[ImGuiKey_RightArrow] = AKEYCODE_DPAD_RIGHT; // also covers physical keyboard arrow key
    io.KeyMap[ImGuiKey_UpArrow] = AKEYCODE_DPAD_UP;       // also covers physical keyboard arrow key
    io.KeyMap[ImGuiKey_DownArrow] = AKEYCODE_DPAD_DOWN;   // also covers physical keyboard arrow key
    io.KeyMap[ImGuiKey_PageUp] = AKEYCODE_PAGE_UP;
    io.KeyMap[ImGuiKey_PageDown] = AKEYCODE_PAGE_DOWN;
    io.KeyMap[ImGuiKey_Home] = AKEYCODE_MOVE_HOME;
    io.KeyMap[ImGuiKey_End] = AKEYCODE_MOVE_END;
    io.KeyMap[ImGuiKey_Insert] = AKEYCODE_INSERT;
    io.KeyMap[ImGuiKey_Delete] = AKEYCODE_FORWARD_DEL;
    io.KeyMap[ImGuiKey_Backspace] = AKEYCODE_DEL;
    io.KeyMap[ImGuiKey_Space] = AKEYCODE_SPACE;
    io.KeyMap[ImGuiKey_Enter] = AKEYCODE_ENTER;
    io.KeyMap[ImGuiKey_Escape] = AKEYCODE_ESCAPE;
    io.KeyMap[ImGuiKey_KeyPadEnter] = AKEYCODE_NUMPAD_ENTER;
    io.KeyMap[ImGuiKey_A] = AKEYCODE_A;
    io.KeyMap[ImGuiKey_C] = AKEYCODE_C;
    io.KeyMap[ImGuiKey_V] = AKEYCODE_V;
    io.KeyMap[ImGuiKey_X] = AKEYCODE_X;
    io.KeyMap[ImGuiKey_Y] = AKEYCODE_Y;
    io.KeyMap[ImGuiKey_Z] = AKEYCODE_Z;

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

    // Process queued key events
    for (auto &keyQueue : g_keyEventQueues)
    {
        if (keyQueue.second.size() == 0)
            continue;
        io.KeysDown[keyQueue.first] = (keyQueue.second.front() == AKEY_EVENT_ACTION_DOWN);
        keyQueue.second.pop();
    }

    static bool WantTextInputLast = false;
    if (io.WantTextInput && !WantTextInputLast)
        ImGui_ImplAndroid_showSoftInput();
    WantTextInputLast = io.WantTextInput;

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
