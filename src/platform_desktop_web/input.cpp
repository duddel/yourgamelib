/*
Copyright (c) 2019-2021 Alexander Scholz

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

namespace yourgame_internal_desktop
{
    namespace
    {
        const std::map<int, yourgame::INPUT> keyApiMapping =
            {{GLFW_KEY_UNKNOWN, yourgame::INPUT::KEY_UNKNOWN},
             {GLFW_KEY_SPACE, yourgame::INPUT::KEY_SPACE},
             {GLFW_KEY_APOSTROPHE, yourgame::INPUT::KEY_APOSTROPHE},
             {GLFW_KEY_COMMA, yourgame::INPUT::KEY_COMMA},
             {GLFW_KEY_MINUS, yourgame::INPUT::KEY_MINUS},
             {GLFW_KEY_PERIOD, yourgame::INPUT::KEY_PERIOD},
             {GLFW_KEY_SLASH, yourgame::INPUT::KEY_SLASH},
             {GLFW_KEY_0, yourgame::INPUT::KEY_0},
             {GLFW_KEY_1, yourgame::INPUT::KEY_1},
             {GLFW_KEY_2, yourgame::INPUT::KEY_2},
             {GLFW_KEY_3, yourgame::INPUT::KEY_3},
             {GLFW_KEY_4, yourgame::INPUT::KEY_4},
             {GLFW_KEY_5, yourgame::INPUT::KEY_5},
             {GLFW_KEY_6, yourgame::INPUT::KEY_6},
             {GLFW_KEY_7, yourgame::INPUT::KEY_7},
             {GLFW_KEY_8, yourgame::INPUT::KEY_8},
             {GLFW_KEY_9, yourgame::INPUT::KEY_9},
             {GLFW_KEY_SEMICOLON, yourgame::INPUT::KEY_SEMICOLON},
             {GLFW_KEY_EQUAL, yourgame::INPUT::KEY_EQUAL},
             {GLFW_KEY_A, yourgame::INPUT::KEY_A},
             {GLFW_KEY_B, yourgame::INPUT::KEY_B},
             {GLFW_KEY_C, yourgame::INPUT::KEY_C},
             {GLFW_KEY_D, yourgame::INPUT::KEY_D},
             {GLFW_KEY_E, yourgame::INPUT::KEY_E},
             {GLFW_KEY_F, yourgame::INPUT::KEY_F},
             {GLFW_KEY_G, yourgame::INPUT::KEY_G},
             {GLFW_KEY_H, yourgame::INPUT::KEY_H},
             {GLFW_KEY_I, yourgame::INPUT::KEY_I},
             {GLFW_KEY_J, yourgame::INPUT::KEY_J},
             {GLFW_KEY_K, yourgame::INPUT::KEY_K},
             {GLFW_KEY_L, yourgame::INPUT::KEY_L},
             {GLFW_KEY_M, yourgame::INPUT::KEY_M},
             {GLFW_KEY_N, yourgame::INPUT::KEY_N},
             {GLFW_KEY_O, yourgame::INPUT::KEY_O},
             {GLFW_KEY_P, yourgame::INPUT::KEY_P},
             {GLFW_KEY_Q, yourgame::INPUT::KEY_Q},
             {GLFW_KEY_R, yourgame::INPUT::KEY_R},
             {GLFW_KEY_S, yourgame::INPUT::KEY_S},
             {GLFW_KEY_T, yourgame::INPUT::KEY_T},
             {GLFW_KEY_U, yourgame::INPUT::KEY_U},
             {GLFW_KEY_V, yourgame::INPUT::KEY_V},
             {GLFW_KEY_W, yourgame::INPUT::KEY_W},
             {GLFW_KEY_X, yourgame::INPUT::KEY_X},
             {GLFW_KEY_Y, yourgame::INPUT::KEY_Y},
             {GLFW_KEY_Z, yourgame::INPUT::KEY_Z},
             {GLFW_KEY_LEFT_BRACKET, yourgame::INPUT::KEY_LEFT_BRACKET},
             {GLFW_KEY_BACKSLASH, yourgame::INPUT::KEY_BACKSLASH},
             {GLFW_KEY_RIGHT_BRACKET, yourgame::INPUT::KEY_RIGHT_BRACKET},
             {GLFW_KEY_GRAVE_ACCENT, yourgame::INPUT::KEY_GRAVE_ACCENT},
             {GLFW_KEY_WORLD_1, yourgame::INPUT::KEY_WORLD_1},
             {GLFW_KEY_WORLD_2, yourgame::INPUT::KEY_WORLD_2},
             {GLFW_KEY_ESCAPE, yourgame::INPUT::KEY_ESCAPE},
             {GLFW_KEY_ENTER, yourgame::INPUT::KEY_ENTER},
             {GLFW_KEY_TAB, yourgame::INPUT::KEY_TAB},
             {GLFW_KEY_BACKSPACE, yourgame::INPUT::KEY_BACKSPACE},
             {GLFW_KEY_INSERT, yourgame::INPUT::KEY_INSERT},
             {GLFW_KEY_DELETE, yourgame::INPUT::KEY_DELETE},
             {GLFW_KEY_RIGHT, yourgame::INPUT::KEY_RIGHT},
             {GLFW_KEY_LEFT, yourgame::INPUT::KEY_LEFT},
             {GLFW_KEY_DOWN, yourgame::INPUT::KEY_DOWN},
             {GLFW_KEY_UP, yourgame::INPUT::KEY_UP},
             {GLFW_KEY_PAGE_UP, yourgame::INPUT::KEY_PAGE_UP},
             {GLFW_KEY_PAGE_DOWN, yourgame::INPUT::KEY_PAGE_DOWN},
             {GLFW_KEY_HOME, yourgame::INPUT::KEY_HOME},
             {GLFW_KEY_END, yourgame::INPUT::KEY_END},
             {GLFW_KEY_CAPS_LOCK, yourgame::INPUT::KEY_CAPS_LOCK},
             {GLFW_KEY_SCROLL_LOCK, yourgame::INPUT::KEY_SCROLL_LOCK},
             {GLFW_KEY_NUM_LOCK, yourgame::INPUT::KEY_NUM_LOCK},
             {GLFW_KEY_PRINT_SCREEN, yourgame::INPUT::KEY_PRINT_SCREEN},
             {GLFW_KEY_PAUSE, yourgame::INPUT::KEY_PAUSE},
             {GLFW_KEY_F1, yourgame::INPUT::KEY_F1},
             {GLFW_KEY_F2, yourgame::INPUT::KEY_F2},
             {GLFW_KEY_F3, yourgame::INPUT::KEY_F3},
             {GLFW_KEY_F4, yourgame::INPUT::KEY_F4},
             {GLFW_KEY_F5, yourgame::INPUT::KEY_F5},
             {GLFW_KEY_F6, yourgame::INPUT::KEY_F6},
             {GLFW_KEY_F7, yourgame::INPUT::KEY_F7},
             {GLFW_KEY_F8, yourgame::INPUT::KEY_F8},
             {GLFW_KEY_F9, yourgame::INPUT::KEY_F9},
             {GLFW_KEY_F10, yourgame::INPUT::KEY_F10},
             {GLFW_KEY_F11, yourgame::INPUT::KEY_F11},
             {GLFW_KEY_F12, yourgame::INPUT::KEY_F12},
             {GLFW_KEY_F13, yourgame::INPUT::KEY_F13},
             {GLFW_KEY_F14, yourgame::INPUT::KEY_F14},
             {GLFW_KEY_F15, yourgame::INPUT::KEY_F15},
             {GLFW_KEY_F16, yourgame::INPUT::KEY_F16},
             {GLFW_KEY_F17, yourgame::INPUT::KEY_F17},
             {GLFW_KEY_F18, yourgame::INPUT::KEY_F18},
             {GLFW_KEY_F19, yourgame::INPUT::KEY_F19},
             {GLFW_KEY_F20, yourgame::INPUT::KEY_F20},
             {GLFW_KEY_F21, yourgame::INPUT::KEY_F21},
             {GLFW_KEY_F22, yourgame::INPUT::KEY_F22},
             {GLFW_KEY_F23, yourgame::INPUT::KEY_F23},
             {GLFW_KEY_F24, yourgame::INPUT::KEY_F24},
             {GLFW_KEY_F25, yourgame::INPUT::KEY_F25},
             {GLFW_KEY_KP_0, yourgame::INPUT::KEY_KP_0},
             {GLFW_KEY_KP_1, yourgame::INPUT::KEY_KP_1},
             {GLFW_KEY_KP_2, yourgame::INPUT::KEY_KP_2},
             {GLFW_KEY_KP_3, yourgame::INPUT::KEY_KP_3},
             {GLFW_KEY_KP_4, yourgame::INPUT::KEY_KP_4},
             {GLFW_KEY_KP_5, yourgame::INPUT::KEY_KP_5},
             {GLFW_KEY_KP_6, yourgame::INPUT::KEY_KP_6},
             {GLFW_KEY_KP_7, yourgame::INPUT::KEY_KP_7},
             {GLFW_KEY_KP_8, yourgame::INPUT::KEY_KP_8},
             {GLFW_KEY_KP_9, yourgame::INPUT::KEY_KP_9},
             {GLFW_KEY_KP_DECIMAL, yourgame::INPUT::KEY_KP_DECIMAL},
             {GLFW_KEY_KP_DIVIDE, yourgame::INPUT::KEY_KP_DIVIDE},
             {GLFW_KEY_KP_MULTIPLY, yourgame::INPUT::KEY_KP_MULTIPLY},
             {GLFW_KEY_KP_SUBTRACT, yourgame::INPUT::KEY_KP_SUBTRACT},
             {GLFW_KEY_KP_ADD, yourgame::INPUT::KEY_KP_ADD},
             {GLFW_KEY_KP_ENTER, yourgame::INPUT::KEY_KP_ENTER},
             {GLFW_KEY_KP_EQUAL, yourgame::INPUT::KEY_KP_EQUAL},
             {GLFW_KEY_LEFT_SHIFT, yourgame::INPUT::KEY_LEFT_SHIFT},
             {GLFW_KEY_LEFT_CONTROL, yourgame::INPUT::KEY_LEFT_CONTROL},
             {GLFW_KEY_LEFT_ALT, yourgame::INPUT::KEY_LEFT_ALT},
             {GLFW_KEY_LEFT_SUPER, yourgame::INPUT::KEY_LEFT_SUPER},
             {GLFW_KEY_RIGHT_SHIFT, yourgame::INPUT::KEY_RIGHT_SHIFT},
             {GLFW_KEY_RIGHT_CONTROL, yourgame::INPUT::KEY_RIGHT_CONTROL},
             {GLFW_KEY_RIGHT_ALT, yourgame::INPUT::KEY_RIGHT_ALT},
             {GLFW_KEY_RIGHT_SUPER, yourgame::INPUT::KEY_RIGHT_SUPER},
             {GLFW_KEY_MENU, yourgame::INPUT::KEY_MENU}};

        const std::map<int, yourgame::INPUT> mouseButtonApiMapping =
            {{GLFW_MOUSE_BUTTON_1, yourgame::INPUT::MOUSE_BUTTON_1},
             {GLFW_MOUSE_BUTTON_2, yourgame::INPUT::MOUSE_BUTTON_2},
             {GLFW_MOUSE_BUTTON_3, yourgame::INPUT::MOUSE_BUTTON_3},
             {GLFW_MOUSE_BUTTON_4, yourgame::INPUT::MOUSE_BUTTON_4},
             {GLFW_MOUSE_BUTTON_5, yourgame::INPUT::MOUSE_BUTTON_5},
             {GLFW_MOUSE_BUTTON_6, yourgame::INPUT::MOUSE_BUTTON_6},
             {GLFW_MOUSE_BUTTON_7, yourgame::INPUT::MOUSE_BUTTON_7},
             {GLFW_MOUSE_BUTTON_8, yourgame::INPUT::MOUSE_BUTTON_8}};

        const std::map<int, yourgame::INPUT> gamepadConnectedApiMapping =
            {{GLFW_JOYSTICK_1, yourgame::INPUT::GAMEPAD_0_CONNECTED},
             {GLFW_JOYSTICK_2, yourgame::INPUT::GAMEPAD_1_CONNECTED},
             {GLFW_JOYSTICK_3, yourgame::INPUT::GAMEPAD_2_CONNECTED},
             {GLFW_JOYSTICK_4, yourgame::INPUT::GAMEPAD_3_CONNECTED}};

        const std::map<int, std::pair<yourgame::INPUT, yourgame::INPUT>> gamepadFirstButtonAxisIdx =
            {{GLFW_JOYSTICK_1, {yourgame::INPUT::GAMEPAD_0_BUTTON_A, yourgame::INPUT::GAMEPAD_0_AXIS_LEFT_X}},
             {GLFW_JOYSTICK_2, {yourgame::INPUT::GAMEPAD_1_BUTTON_A, yourgame::INPUT::GAMEPAD_1_AXIS_LEFT_X}},
             {GLFW_JOYSTICK_3, {yourgame::INPUT::GAMEPAD_2_BUTTON_A, yourgame::INPUT::GAMEPAD_2_AXIS_LEFT_X}},
             {GLFW_JOYSTICK_4, {yourgame::INPUT::GAMEPAD_3_BUTTON_A, yourgame::INPUT::GAMEPAD_3_AXIS_LEFT_X}}};

        void keyCallback(GLFWwindow *window, int key, int scancode, int action, int mods)
        {
            auto mapping = keyApiMapping.find(key);
            if (mapping != keyApiMapping.end())
            {
                if (action == GLFW_PRESS)
                    yourgame_internal::setInput(mapping->second, 1.0f);
                else if (action == GLFW_RELEASE)
                    yourgame_internal::setInput(mapping->second, 0.0f);
            }
        }

        static void cursorPositionCallback(GLFWwindow *window, double xpos, double ypos)
        {
            yourgame_internal::setInput2(yourgame::INPUT::MOUSE_X, (float)xpos);
            yourgame_internal::setInput2(yourgame::INPUT::MOUSE_Y, (float)ypos);
        }

        void mouseButtonCallback(GLFWwindow *window, int button, int action, int mods)
        {
            auto mapping = mouseButtonApiMapping.find(button);
            if (mapping != mouseButtonApiMapping.end())
            {
                if (action == GLFW_PRESS)
                    yourgame_internal::setInput(mapping->second, 1.0f);
                else if (action == GLFW_RELEASE)
                    yourgame_internal::setInput(mapping->second, 0.0f);
            }
        }

        void scrollCallback(GLFWwindow *window, double xoffset, double yoffset)
        {
            yourgame_internal::setInput(yourgame::INPUT::SCROLL_OFFSET_X, (float)xoffset);
            yourgame_internal::setInput(yourgame::INPUT::SCROLL_OFFSET_Y, (float)yoffset);
        }

        void joystickCallback(int jid, int event)
        {
            auto mapping = gamepadConnectedApiMapping.find(jid);
            if (mapping != gamepadConnectedApiMapping.end())
            {
                if (event == GLFW_CONNECTED)
                    yourgame_internal::setInput(mapping->second, 1.0f);
                else if (event == GLFW_DISCONNECTED)
                    yourgame_internal::setInput(mapping->second, 0.0f);
            }
        }
    } // namespace

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
            yourgame_internal::setInput(p.second, glfwJoystickIsGamepad(p.first) == GLFW_TRUE ? 1.0f : 0.0f);
        }
#endif
    }

    void tickInput()
    {
        // prepare for new input: shift all "current" values to "last" values
        for (auto &i : yourgame_internal::inputStates)
        {
            i.second.valLast = i.second.val;
        }

        // reset certain inputs before new input:
        // these are "events" that might occur during a _single_ frame,
        // like an offset of a mouse scroll wheel
        yourgame_internal::setInput(yourgame::INPUT::SCROLL_OFFSET_X, 0.0f);
        yourgame_internal::setInput(yourgame::INPUT::SCROLL_OFFSET_Y, 0.0f);

// todo: mapped gamepad input available since glfw v3.3,
// emscripten implements glfw v3.2 API only (2020-12-12)
#ifndef __EMSCRIPTEN__
        for (const auto &pad : gamepadFirstButtonAxisIdx)
        {
            GLFWgamepadstate state;
            if (glfwGetGamepadState(pad.first, &state))
            {
                int buttonWriteIdx = static_cast<int>(pad.second.first);
                yourgame_internal::setInput(static_cast<yourgame::INPUT>(buttonWriteIdx++), state.buttons[GLFW_GAMEPAD_BUTTON_A] == GLFW_PRESS ? 1.0f : 0.0f);
                yourgame_internal::setInput(static_cast<yourgame::INPUT>(buttonWriteIdx++), state.buttons[GLFW_GAMEPAD_BUTTON_B] == GLFW_PRESS ? 1.0f : 0.0f);
                yourgame_internal::setInput(static_cast<yourgame::INPUT>(buttonWriteIdx++), state.buttons[GLFW_GAMEPAD_BUTTON_X] == GLFW_PRESS ? 1.0f : 0.0f);
                yourgame_internal::setInput(static_cast<yourgame::INPUT>(buttonWriteIdx++), state.buttons[GLFW_GAMEPAD_BUTTON_Y] == GLFW_PRESS ? 1.0f : 0.0f);
                yourgame_internal::setInput(static_cast<yourgame::INPUT>(buttonWriteIdx++), state.buttons[GLFW_GAMEPAD_BUTTON_LEFT_BUMPER] == GLFW_PRESS ? 1.0f : 0.0f);
                yourgame_internal::setInput(static_cast<yourgame::INPUT>(buttonWriteIdx++), state.buttons[GLFW_GAMEPAD_BUTTON_RIGHT_BUMPER] == GLFW_PRESS ? 1.0f : 0.0f);
                yourgame_internal::setInput(static_cast<yourgame::INPUT>(buttonWriteIdx++), state.buttons[GLFW_GAMEPAD_BUTTON_BACK] == GLFW_PRESS ? 1.0f : 0.0f);
                yourgame_internal::setInput(static_cast<yourgame::INPUT>(buttonWriteIdx++), state.buttons[GLFW_GAMEPAD_BUTTON_START] == GLFW_PRESS ? 1.0f : 0.0f);
                yourgame_internal::setInput(static_cast<yourgame::INPUT>(buttonWriteIdx++), state.buttons[GLFW_GAMEPAD_BUTTON_GUIDE] == GLFW_PRESS ? 1.0f : 0.0f);
                yourgame_internal::setInput(static_cast<yourgame::INPUT>(buttonWriteIdx++), state.buttons[GLFW_GAMEPAD_BUTTON_LEFT_THUMB] == GLFW_PRESS ? 1.0f : 0.0f);
                yourgame_internal::setInput(static_cast<yourgame::INPUT>(buttonWriteIdx++), state.buttons[GLFW_GAMEPAD_BUTTON_RIGHT_THUMB] == GLFW_PRESS ? 1.0f : 0.0f);
                yourgame_internal::setInput(static_cast<yourgame::INPUT>(buttonWriteIdx++), state.buttons[GLFW_GAMEPAD_BUTTON_DPAD_UP] == GLFW_PRESS ? 1.0f : 0.0f);
                yourgame_internal::setInput(static_cast<yourgame::INPUT>(buttonWriteIdx++), state.buttons[GLFW_GAMEPAD_BUTTON_DPAD_RIGHT] == GLFW_PRESS ? 1.0f : 0.0f);
                yourgame_internal::setInput(static_cast<yourgame::INPUT>(buttonWriteIdx++), state.buttons[GLFW_GAMEPAD_BUTTON_DPAD_DOWN] == GLFW_PRESS ? 1.0f : 0.0f);
                yourgame_internal::setInput(static_cast<yourgame::INPUT>(buttonWriteIdx++), state.buttons[GLFW_GAMEPAD_BUTTON_DPAD_LEFT] == GLFW_PRESS ? 1.0f : 0.0f);
                int axisWriteIdx = static_cast<int>(pad.second.second);
                yourgame_internal::setInput2(static_cast<yourgame::INPUT>(axisWriteIdx++), state.axes[GLFW_GAMEPAD_AXIS_LEFT_X]);
                yourgame_internal::setInput2(static_cast<yourgame::INPUT>(axisWriteIdx++), state.axes[GLFW_GAMEPAD_AXIS_LEFT_Y]);
                yourgame_internal::setInput2(static_cast<yourgame::INPUT>(axisWriteIdx++), state.axes[GLFW_GAMEPAD_AXIS_RIGHT_X]);
                yourgame_internal::setInput2(static_cast<yourgame::INPUT>(axisWriteIdx++), state.axes[GLFW_GAMEPAD_AXIS_RIGHT_Y]);
                yourgame_internal::setInput2(static_cast<yourgame::INPUT>(axisWriteIdx++), state.axes[GLFW_GAMEPAD_AXIS_LEFT_TRIGGER]);
                yourgame_internal::setInput2(static_cast<yourgame::INPUT>(axisWriteIdx++), state.axes[GLFW_GAMEPAD_AXIS_RIGHT_TRIGGER]);
            }
        }
#endif
    }
} // namespace yourgame_internal_desktop
