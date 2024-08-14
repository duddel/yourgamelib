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
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include "yourgame/input.h"
#include "yourgame_internal/input.h"

namespace
{
    const std::map<int, yourgame::input::Source> keyApiMapping =
        {{GLFW_KEY_UNKNOWN, yourgame::input::KEY_UNKNOWN},
         {GLFW_KEY_SPACE, yourgame::input::KEY_SPACE},
         {GLFW_KEY_APOSTROPHE, yourgame::input::KEY_APOSTROPHE},
         {GLFW_KEY_COMMA, yourgame::input::KEY_COMMA},
         {GLFW_KEY_MINUS, yourgame::input::KEY_MINUS},
         {GLFW_KEY_PERIOD, yourgame::input::KEY_PERIOD},
         {GLFW_KEY_SLASH, yourgame::input::KEY_SLASH},
         {GLFW_KEY_0, yourgame::input::KEY_0},
         {GLFW_KEY_1, yourgame::input::KEY_1},
         {GLFW_KEY_2, yourgame::input::KEY_2},
         {GLFW_KEY_3, yourgame::input::KEY_3},
         {GLFW_KEY_4, yourgame::input::KEY_4},
         {GLFW_KEY_5, yourgame::input::KEY_5},
         {GLFW_KEY_6, yourgame::input::KEY_6},
         {GLFW_KEY_7, yourgame::input::KEY_7},
         {GLFW_KEY_8, yourgame::input::KEY_8},
         {GLFW_KEY_9, yourgame::input::KEY_9},
         {GLFW_KEY_SEMICOLON, yourgame::input::KEY_SEMICOLON},
         {GLFW_KEY_EQUAL, yourgame::input::KEY_EQUAL},
         {GLFW_KEY_A, yourgame::input::KEY_A},
         {GLFW_KEY_B, yourgame::input::KEY_B},
         {GLFW_KEY_C, yourgame::input::KEY_C},
         {GLFW_KEY_D, yourgame::input::KEY_D},
         {GLFW_KEY_E, yourgame::input::KEY_E},
         {GLFW_KEY_F, yourgame::input::KEY_F},
         {GLFW_KEY_G, yourgame::input::KEY_G},
         {GLFW_KEY_H, yourgame::input::KEY_H},
         {GLFW_KEY_I, yourgame::input::KEY_I},
         {GLFW_KEY_J, yourgame::input::KEY_J},
         {GLFW_KEY_K, yourgame::input::KEY_K},
         {GLFW_KEY_L, yourgame::input::KEY_L},
         {GLFW_KEY_M, yourgame::input::KEY_M},
         {GLFW_KEY_N, yourgame::input::KEY_N},
         {GLFW_KEY_O, yourgame::input::KEY_O},
         {GLFW_KEY_P, yourgame::input::KEY_P},
         {GLFW_KEY_Q, yourgame::input::KEY_Q},
         {GLFW_KEY_R, yourgame::input::KEY_R},
         {GLFW_KEY_S, yourgame::input::KEY_S},
         {GLFW_KEY_T, yourgame::input::KEY_T},
         {GLFW_KEY_U, yourgame::input::KEY_U},
         {GLFW_KEY_V, yourgame::input::KEY_V},
         {GLFW_KEY_W, yourgame::input::KEY_W},
         {GLFW_KEY_X, yourgame::input::KEY_X},
         {GLFW_KEY_Y, yourgame::input::KEY_Y},
         {GLFW_KEY_Z, yourgame::input::KEY_Z},
         {GLFW_KEY_LEFT_BRACKET, yourgame::input::KEY_LEFT_BRACKET},
         {GLFW_KEY_BACKSLASH, yourgame::input::KEY_BACKSLASH},
         {GLFW_KEY_RIGHT_BRACKET, yourgame::input::KEY_RIGHT_BRACKET},
         {GLFW_KEY_GRAVE_ACCENT, yourgame::input::KEY_GRAVE_ACCENT},
         {GLFW_KEY_WORLD_1, yourgame::input::KEY_WORLD_1},
         {GLFW_KEY_WORLD_2, yourgame::input::KEY_WORLD_2},
         {GLFW_KEY_ESCAPE, yourgame::input::KEY_ESCAPE},
         {GLFW_KEY_ENTER, yourgame::input::KEY_ENTER},
         {GLFW_KEY_TAB, yourgame::input::KEY_TAB},
         {GLFW_KEY_BACKSPACE, yourgame::input::KEY_BACKSPACE},
         {GLFW_KEY_INSERT, yourgame::input::KEY_INSERT},
         {GLFW_KEY_DELETE, yourgame::input::KEY_DELETE},
         {GLFW_KEY_RIGHT, yourgame::input::KEY_RIGHT},
         {GLFW_KEY_LEFT, yourgame::input::KEY_LEFT},
         {GLFW_KEY_DOWN, yourgame::input::KEY_DOWN},
         {GLFW_KEY_UP, yourgame::input::KEY_UP},
         {GLFW_KEY_PAGE_UP, yourgame::input::KEY_PAGE_UP},
         {GLFW_KEY_PAGE_DOWN, yourgame::input::KEY_PAGE_DOWN},
         {GLFW_KEY_HOME, yourgame::input::KEY_HOME},
         {GLFW_KEY_END, yourgame::input::KEY_END},
         {GLFW_KEY_CAPS_LOCK, yourgame::input::KEY_CAPS_LOCK},
         {GLFW_KEY_SCROLL_LOCK, yourgame::input::KEY_SCROLL_LOCK},
         {GLFW_KEY_NUM_LOCK, yourgame::input::KEY_NUM_LOCK},
         {GLFW_KEY_PRINT_SCREEN, yourgame::input::KEY_PRINT_SCREEN},
         {GLFW_KEY_PAUSE, yourgame::input::KEY_PAUSE},
         {GLFW_KEY_F1, yourgame::input::KEY_F1},
         {GLFW_KEY_F2, yourgame::input::KEY_F2},
         {GLFW_KEY_F3, yourgame::input::KEY_F3},
         {GLFW_KEY_F4, yourgame::input::KEY_F4},
         {GLFW_KEY_F5, yourgame::input::KEY_F5},
         {GLFW_KEY_F6, yourgame::input::KEY_F6},
         {GLFW_KEY_F7, yourgame::input::KEY_F7},
         {GLFW_KEY_F8, yourgame::input::KEY_F8},
         {GLFW_KEY_F9, yourgame::input::KEY_F9},
         {GLFW_KEY_F10, yourgame::input::KEY_F10},
         {GLFW_KEY_F11, yourgame::input::KEY_F11},
         {GLFW_KEY_F12, yourgame::input::KEY_F12},
         {GLFW_KEY_F13, yourgame::input::KEY_F13},
         {GLFW_KEY_F14, yourgame::input::KEY_F14},
         {GLFW_KEY_F15, yourgame::input::KEY_F15},
         {GLFW_KEY_F16, yourgame::input::KEY_F16},
         {GLFW_KEY_F17, yourgame::input::KEY_F17},
         {GLFW_KEY_F18, yourgame::input::KEY_F18},
         {GLFW_KEY_F19, yourgame::input::KEY_F19},
         {GLFW_KEY_F20, yourgame::input::KEY_F20},
         {GLFW_KEY_F21, yourgame::input::KEY_F21},
         {GLFW_KEY_F22, yourgame::input::KEY_F22},
         {GLFW_KEY_F23, yourgame::input::KEY_F23},
         {GLFW_KEY_F24, yourgame::input::KEY_F24},
         {GLFW_KEY_F25, yourgame::input::KEY_F25},
         {GLFW_KEY_KP_0, yourgame::input::KEY_KP_0},
         {GLFW_KEY_KP_1, yourgame::input::KEY_KP_1},
         {GLFW_KEY_KP_2, yourgame::input::KEY_KP_2},
         {GLFW_KEY_KP_3, yourgame::input::KEY_KP_3},
         {GLFW_KEY_KP_4, yourgame::input::KEY_KP_4},
         {GLFW_KEY_KP_5, yourgame::input::KEY_KP_5},
         {GLFW_KEY_KP_6, yourgame::input::KEY_KP_6},
         {GLFW_KEY_KP_7, yourgame::input::KEY_KP_7},
         {GLFW_KEY_KP_8, yourgame::input::KEY_KP_8},
         {GLFW_KEY_KP_9, yourgame::input::KEY_KP_9},
         {GLFW_KEY_KP_DECIMAL, yourgame::input::KEY_KP_DECIMAL},
         {GLFW_KEY_KP_DIVIDE, yourgame::input::KEY_KP_DIVIDE},
         {GLFW_KEY_KP_MULTIPLY, yourgame::input::KEY_KP_MULTIPLY},
         {GLFW_KEY_KP_SUBTRACT, yourgame::input::KEY_KP_SUBTRACT},
         {GLFW_KEY_KP_ADD, yourgame::input::KEY_KP_ADD},
         {GLFW_KEY_KP_ENTER, yourgame::input::KEY_KP_ENTER},
         {GLFW_KEY_KP_EQUAL, yourgame::input::KEY_KP_EQUAL},
         {GLFW_KEY_LEFT_SHIFT, yourgame::input::KEY_LEFT_SHIFT},
         {GLFW_KEY_LEFT_CONTROL, yourgame::input::KEY_LEFT_CONTROL},
         {GLFW_KEY_LEFT_ALT, yourgame::input::KEY_LEFT_ALT},
         {GLFW_KEY_LEFT_SUPER, yourgame::input::KEY_LEFT_SUPER},
         {GLFW_KEY_RIGHT_SHIFT, yourgame::input::KEY_RIGHT_SHIFT},
         {GLFW_KEY_RIGHT_CONTROL, yourgame::input::KEY_RIGHT_CONTROL},
         {GLFW_KEY_RIGHT_ALT, yourgame::input::KEY_RIGHT_ALT},
         {GLFW_KEY_RIGHT_SUPER, yourgame::input::KEY_RIGHT_SUPER},
         {GLFW_KEY_MENU, yourgame::input::KEY_MENU}};

    const std::map<int, yourgame::input::Source> mouseButtonApiMapping =
        {{GLFW_MOUSE_BUTTON_1, yourgame::input::MOUSE_BUTTON_1},
         {GLFW_MOUSE_BUTTON_2, yourgame::input::MOUSE_BUTTON_2},
         {GLFW_MOUSE_BUTTON_3, yourgame::input::MOUSE_BUTTON_3},
         {GLFW_MOUSE_BUTTON_4, yourgame::input::MOUSE_BUTTON_4},
         {GLFW_MOUSE_BUTTON_5, yourgame::input::MOUSE_BUTTON_5},
         {GLFW_MOUSE_BUTTON_6, yourgame::input::MOUSE_BUTTON_6},
         {GLFW_MOUSE_BUTTON_7, yourgame::input::MOUSE_BUTTON_7},
         {GLFW_MOUSE_BUTTON_8, yourgame::input::MOUSE_BUTTON_8}};

    const std::map<int, yourgame::input::Source> gamepadConnectedApiMapping =
        {{GLFW_JOYSTICK_1, yourgame::input::GAMEPAD_0_CONNECTED},
         {GLFW_JOYSTICK_2, yourgame::input::GAMEPAD_1_CONNECTED},
         {GLFW_JOYSTICK_3, yourgame::input::GAMEPAD_2_CONNECTED},
         {GLFW_JOYSTICK_4, yourgame::input::GAMEPAD_3_CONNECTED}};

    const std::map<int, std::pair<yourgame::input::Source, yourgame::input::Source>> gamepadFirstButtonAxisIdx =
        {{GLFW_JOYSTICK_1, {yourgame::input::GAMEPAD_0_BUTTON_A, yourgame::input::GAMEPAD_0_AXIS_LEFT_X}},
         {GLFW_JOYSTICK_2, {yourgame::input::GAMEPAD_1_BUTTON_A, yourgame::input::GAMEPAD_1_AXIS_LEFT_X}},
         {GLFW_JOYSTICK_3, {yourgame::input::GAMEPAD_2_BUTTON_A, yourgame::input::GAMEPAD_2_AXIS_LEFT_X}},
         {GLFW_JOYSTICK_4, {yourgame::input::GAMEPAD_3_BUTTON_A, yourgame::input::GAMEPAD_3_AXIS_LEFT_X}}};

    void keyCallback(GLFWwindow *window, int key, int scancode, int action, int mods)
    {
        auto mapping = keyApiMapping.find(key);
        if (mapping != keyApiMapping.end())
        {
            if (action == GLFW_PRESS)
                yourgame_internal::input::setInput(mapping->second, 1.0f);
            else if (action == GLFW_RELEASE)
                yourgame_internal::input::setInput(mapping->second, 0.0f);
        }
    }

    static void cursorPositionCallback(GLFWwindow *window, double xpos, double ypos)
    {
        yourgame_internal::input::setInput2(yourgame::input::MOUSE_X, (float)xpos);
        yourgame_internal::input::setInput2(yourgame::input::MOUSE_Y, (float)ypos);
    }

    void mouseButtonCallback(GLFWwindow *window, int button, int action, int mods)
    {
        auto mapping = mouseButtonApiMapping.find(button);
        if (mapping != mouseButtonApiMapping.end())
        {
            if (action == GLFW_PRESS)
                yourgame_internal::input::setInput(mapping->second, 1.0f);
            else if (action == GLFW_RELEASE)
                yourgame_internal::input::setInput(mapping->second, 0.0f);
        }
    }

    void scrollCallback(GLFWwindow *window, double xoffset, double yoffset)
    {
        yourgame_internal::input::setInput(yourgame::input::SCROLL_OFFSET_X, (float)xoffset);
        yourgame_internal::input::setInput(yourgame::input::SCROLL_OFFSET_Y, (float)yoffset);
    }

    void joystickCallback(int jid, int event)
    {
        auto mapping = gamepadConnectedApiMapping.find(jid);
        if (mapping != gamepadConnectedApiMapping.end())
        {
            if (event == GLFW_CONNECTED)
                yourgame_internal::input::setInput(mapping->second, 1.0f);
            else if (event == GLFW_DISCONNECTED)
                yourgame_internal::input::setInput(mapping->second, 0.0f);
        }
    }
} // namespace

namespace yourgame_internal
{
    namespace input
    {
        void tickInput()
        {
            // prepare for new input: shift all "current" values to "last" values
            for (auto &i : yourgame_internal::input::inputStates)
            {
                i.second.valLast = i.second.val;
            }

            // reset certain inputs before new input:
            // these are "events" that might occur during a _single_ frame,
            // like an offset of a mouse scroll wheel
            yourgame_internal::input::setInput(yourgame::input::SCROLL_OFFSET_X, 0.0f);
            yourgame_internal::input::setInput(yourgame::input::SCROLL_OFFSET_Y, 0.0f);

// todo: mapped gamepad input available since glfw v3.3,
// emscripten implements glfw v3.2 API only (2020-12-12)
#ifndef __EMSCRIPTEN__
            for (const auto &pad : gamepadFirstButtonAxisIdx)
            {
                GLFWgamepadstate state;
                if (glfwGetGamepadState(pad.first, &state))
                {
                    int buttonWriteIdx = static_cast<int>(pad.second.first);
                    yourgame_internal::input::setInput(static_cast<yourgame::input::Source>(buttonWriteIdx++), state.buttons[GLFW_GAMEPAD_BUTTON_A] == GLFW_PRESS ? 1.0f : 0.0f);
                    yourgame_internal::input::setInput(static_cast<yourgame::input::Source>(buttonWriteIdx++), state.buttons[GLFW_GAMEPAD_BUTTON_B] == GLFW_PRESS ? 1.0f : 0.0f);
                    yourgame_internal::input::setInput(static_cast<yourgame::input::Source>(buttonWriteIdx++), state.buttons[GLFW_GAMEPAD_BUTTON_X] == GLFW_PRESS ? 1.0f : 0.0f);
                    yourgame_internal::input::setInput(static_cast<yourgame::input::Source>(buttonWriteIdx++), state.buttons[GLFW_GAMEPAD_BUTTON_Y] == GLFW_PRESS ? 1.0f : 0.0f);
                    yourgame_internal::input::setInput(static_cast<yourgame::input::Source>(buttonWriteIdx++), state.buttons[GLFW_GAMEPAD_BUTTON_LEFT_BUMPER] == GLFW_PRESS ? 1.0f : 0.0f);
                    yourgame_internal::input::setInput(static_cast<yourgame::input::Source>(buttonWriteIdx++), state.buttons[GLFW_GAMEPAD_BUTTON_RIGHT_BUMPER] == GLFW_PRESS ? 1.0f : 0.0f);
                    yourgame_internal::input::setInput(static_cast<yourgame::input::Source>(buttonWriteIdx++), state.buttons[GLFW_GAMEPAD_BUTTON_BACK] == GLFW_PRESS ? 1.0f : 0.0f);
                    yourgame_internal::input::setInput(static_cast<yourgame::input::Source>(buttonWriteIdx++), state.buttons[GLFW_GAMEPAD_BUTTON_START] == GLFW_PRESS ? 1.0f : 0.0f);
                    yourgame_internal::input::setInput(static_cast<yourgame::input::Source>(buttonWriteIdx++), state.buttons[GLFW_GAMEPAD_BUTTON_GUIDE] == GLFW_PRESS ? 1.0f : 0.0f);
                    yourgame_internal::input::setInput(static_cast<yourgame::input::Source>(buttonWriteIdx++), state.buttons[GLFW_GAMEPAD_BUTTON_LEFT_THUMB] == GLFW_PRESS ? 1.0f : 0.0f);
                    yourgame_internal::input::setInput(static_cast<yourgame::input::Source>(buttonWriteIdx++), state.buttons[GLFW_GAMEPAD_BUTTON_RIGHT_THUMB] == GLFW_PRESS ? 1.0f : 0.0f);
                    yourgame_internal::input::setInput(static_cast<yourgame::input::Source>(buttonWriteIdx++), state.buttons[GLFW_GAMEPAD_BUTTON_DPAD_UP] == GLFW_PRESS ? 1.0f : 0.0f);
                    yourgame_internal::input::setInput(static_cast<yourgame::input::Source>(buttonWriteIdx++), state.buttons[GLFW_GAMEPAD_BUTTON_DPAD_RIGHT] == GLFW_PRESS ? 1.0f : 0.0f);
                    yourgame_internal::input::setInput(static_cast<yourgame::input::Source>(buttonWriteIdx++), state.buttons[GLFW_GAMEPAD_BUTTON_DPAD_DOWN] == GLFW_PRESS ? 1.0f : 0.0f);
                    yourgame_internal::input::setInput(static_cast<yourgame::input::Source>(buttonWriteIdx++), state.buttons[GLFW_GAMEPAD_BUTTON_DPAD_LEFT] == GLFW_PRESS ? 1.0f : 0.0f);
                    int axisWriteIdx = static_cast<int>(pad.second.second);
                    yourgame_internal::input::setInput2(static_cast<yourgame::input::Source>(axisWriteIdx++), state.axes[GLFW_GAMEPAD_AXIS_LEFT_X]);
                    yourgame_internal::input::setInput2(static_cast<yourgame::input::Source>(axisWriteIdx++), state.axes[GLFW_GAMEPAD_AXIS_LEFT_Y]);
                    yourgame_internal::input::setInput2(static_cast<yourgame::input::Source>(axisWriteIdx++), state.axes[GLFW_GAMEPAD_AXIS_RIGHT_X]);
                    yourgame_internal::input::setInput2(static_cast<yourgame::input::Source>(axisWriteIdx++), state.axes[GLFW_GAMEPAD_AXIS_RIGHT_Y]);
                    yourgame_internal::input::setInput2(static_cast<yourgame::input::Source>(axisWriteIdx++), state.axes[GLFW_GAMEPAD_AXIS_LEFT_TRIGGER]);
                    yourgame_internal::input::setInput2(static_cast<yourgame::input::Source>(axisWriteIdx++), state.axes[GLFW_GAMEPAD_AXIS_RIGHT_TRIGGER]);
                }
            }
#endif
        }

        namespace desktop
        {
            void initInput(GLFWwindow *window)
            {
                glfwSetKeyCallback(window, keyCallback);
                glfwSetCursorPosCallback(window, cursorPositionCallback);
                glfwSetMouseButtonCallback(window, mouseButtonCallback);
                glfwSetScrollCallback(window, scrollCallback);
                glfwSetJoystickCallback(joystickCallback);

// todo: mapped gamepad input available since glfw v3.3,
// emscripten implements glfw v3.2 API only (2020-12-12)
#ifndef __EMSCRIPTEN__
                // poll and set gamepads, that are already connected during init
                for (const auto &p : gamepadConnectedApiMapping)
                {
                    yourgame_internal::input::setInput(p.second, glfwJoystickIsGamepad(p.first) == GLFW_TRUE ? 1.0f : 0.0f);
                }
#endif
            }
        } // namespace desktop
    } // namespace input
} // namespace yourgame_internal
