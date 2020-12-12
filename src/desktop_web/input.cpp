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
#include <map>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include "yourgame/input.h"

namespace yourgame_internal_desktop
{
    namespace
    {
        const std::map<int, yourgame::InputSource> keyApiMapping =
            {{GLFW_KEY_UNKNOWN, yourgame::InputSource::YOURGAME_KEY_UNKNOWN},
             {GLFW_KEY_APOSTROPHE, yourgame::InputSource::YOURGAME_KEY_APOSTROPHE},
             {GLFW_KEY_COMMA, yourgame::InputSource::YOURGAME_KEY_COMMA},
             {GLFW_KEY_MINUS, yourgame::InputSource::YOURGAME_KEY_MINUS},
             {GLFW_KEY_PERIOD, yourgame::InputSource::YOURGAME_KEY_PERIOD},
             {GLFW_KEY_SLASH, yourgame::InputSource::YOURGAME_KEY_SLASH},
             {GLFW_KEY_0, yourgame::InputSource::YOURGAME_KEY_0},
             {GLFW_KEY_1, yourgame::InputSource::YOURGAME_KEY_1},
             {GLFW_KEY_2, yourgame::InputSource::YOURGAME_KEY_2},
             {GLFW_KEY_3, yourgame::InputSource::YOURGAME_KEY_3},
             {GLFW_KEY_4, yourgame::InputSource::YOURGAME_KEY_4},
             {GLFW_KEY_5, yourgame::InputSource::YOURGAME_KEY_5},
             {GLFW_KEY_6, yourgame::InputSource::YOURGAME_KEY_6},
             {GLFW_KEY_7, yourgame::InputSource::YOURGAME_KEY_7},
             {GLFW_KEY_8, yourgame::InputSource::YOURGAME_KEY_8},
             {GLFW_KEY_9, yourgame::InputSource::YOURGAME_KEY_9},
             {GLFW_KEY_SEMICOLON, yourgame::InputSource::YOURGAME_KEY_SEMICOLON},
             {GLFW_KEY_EQUAL, yourgame::InputSource::YOURGAME_KEY_EQUAL},
             {GLFW_KEY_A, yourgame::InputSource::YOURGAME_KEY_A},
             {GLFW_KEY_B, yourgame::InputSource::YOURGAME_KEY_B},
             {GLFW_KEY_C, yourgame::InputSource::YOURGAME_KEY_C},
             {GLFW_KEY_D, yourgame::InputSource::YOURGAME_KEY_D},
             {GLFW_KEY_E, yourgame::InputSource::YOURGAME_KEY_E},
             {GLFW_KEY_F, yourgame::InputSource::YOURGAME_KEY_F},
             {GLFW_KEY_G, yourgame::InputSource::YOURGAME_KEY_G},
             {GLFW_KEY_H, yourgame::InputSource::YOURGAME_KEY_H},
             {GLFW_KEY_I, yourgame::InputSource::YOURGAME_KEY_I},
             {GLFW_KEY_J, yourgame::InputSource::YOURGAME_KEY_J},
             {GLFW_KEY_K, yourgame::InputSource::YOURGAME_KEY_K},
             {GLFW_KEY_L, yourgame::InputSource::YOURGAME_KEY_L},
             {GLFW_KEY_M, yourgame::InputSource::YOURGAME_KEY_M},
             {GLFW_KEY_N, yourgame::InputSource::YOURGAME_KEY_N},
             {GLFW_KEY_O, yourgame::InputSource::YOURGAME_KEY_O},
             {GLFW_KEY_P, yourgame::InputSource::YOURGAME_KEY_P},
             {GLFW_KEY_Q, yourgame::InputSource::YOURGAME_KEY_Q},
             {GLFW_KEY_R, yourgame::InputSource::YOURGAME_KEY_R},
             {GLFW_KEY_S, yourgame::InputSource::YOURGAME_KEY_S},
             {GLFW_KEY_T, yourgame::InputSource::YOURGAME_KEY_T},
             {GLFW_KEY_U, yourgame::InputSource::YOURGAME_KEY_U},
             {GLFW_KEY_V, yourgame::InputSource::YOURGAME_KEY_V},
             {GLFW_KEY_W, yourgame::InputSource::YOURGAME_KEY_W},
             {GLFW_KEY_X, yourgame::InputSource::YOURGAME_KEY_X},
             {GLFW_KEY_Y, yourgame::InputSource::YOURGAME_KEY_Y},
             {GLFW_KEY_Z, yourgame::InputSource::YOURGAME_KEY_Z},
             {GLFW_KEY_LEFT_BRACKET, yourgame::InputSource::YOURGAME_KEY_LEFT_BRACKET},
             {GLFW_KEY_BACKSLASH, yourgame::InputSource::YOURGAME_KEY_BACKSLASH},
             {GLFW_KEY_RIGHT_BRACKET, yourgame::InputSource::YOURGAME_KEY_RIGHT_BRACKET},
             {GLFW_KEY_GRAVE_ACCENT, yourgame::InputSource::YOURGAME_KEY_GRAVE_ACCENT},
             {GLFW_KEY_WORLD_1, yourgame::InputSource::YOURGAME_KEY_WORLD_1},
             {GLFW_KEY_WORLD_2, yourgame::InputSource::YOURGAME_KEY_WORLD_2},
             {GLFW_KEY_ESCAPE, yourgame::InputSource::YOURGAME_KEY_ESCAPE},
             {GLFW_KEY_ENTER, yourgame::InputSource::YOURGAME_KEY_ENTER},
             {GLFW_KEY_TAB, yourgame::InputSource::YOURGAME_KEY_TAB},
             {GLFW_KEY_BACKSPACE, yourgame::InputSource::YOURGAME_KEY_BACKSPACE},
             {GLFW_KEY_INSERT, yourgame::InputSource::YOURGAME_KEY_INSERT},
             {GLFW_KEY_DELETE, yourgame::InputSource::YOURGAME_KEY_DELETE},
             {GLFW_KEY_RIGHT, yourgame::InputSource::YOURGAME_KEY_RIGHT},
             {GLFW_KEY_LEFT, yourgame::InputSource::YOURGAME_KEY_LEFT},
             {GLFW_KEY_DOWN, yourgame::InputSource::YOURGAME_KEY_DOWN},
             {GLFW_KEY_UP, yourgame::InputSource::YOURGAME_KEY_UP},
             {GLFW_KEY_PAGE_UP, yourgame::InputSource::YOURGAME_KEY_PAGE_UP},
             {GLFW_KEY_PAGE_DOWN, yourgame::InputSource::YOURGAME_KEY_PAGE_DOWN},
             {GLFW_KEY_HOME, yourgame::InputSource::YOURGAME_KEY_HOME},
             {GLFW_KEY_END, yourgame::InputSource::YOURGAME_KEY_END},
             {GLFW_KEY_CAPS_LOCK, yourgame::InputSource::YOURGAME_KEY_CAPS_LOCK},
             {GLFW_KEY_SCROLL_LOCK, yourgame::InputSource::YOURGAME_KEY_SCROLL_LOCK},
             {GLFW_KEY_NUM_LOCK, yourgame::InputSource::YOURGAME_KEY_NUM_LOCK},
             {GLFW_KEY_PRINT_SCREEN, yourgame::InputSource::YOURGAME_KEY_PRINT_SCREEN},
             {GLFW_KEY_PAUSE, yourgame::InputSource::YOURGAME_KEY_PAUSE},
             {GLFW_KEY_F1, yourgame::InputSource::YOURGAME_KEY_F1},
             {GLFW_KEY_F2, yourgame::InputSource::YOURGAME_KEY_F2},
             {GLFW_KEY_F3, yourgame::InputSource::YOURGAME_KEY_F3},
             {GLFW_KEY_F4, yourgame::InputSource::YOURGAME_KEY_F4},
             {GLFW_KEY_F5, yourgame::InputSource::YOURGAME_KEY_F5},
             {GLFW_KEY_F6, yourgame::InputSource::YOURGAME_KEY_F6},
             {GLFW_KEY_F7, yourgame::InputSource::YOURGAME_KEY_F7},
             {GLFW_KEY_F8, yourgame::InputSource::YOURGAME_KEY_F8},
             {GLFW_KEY_F9, yourgame::InputSource::YOURGAME_KEY_F9},
             {GLFW_KEY_F10, yourgame::InputSource::YOURGAME_KEY_F10},
             {GLFW_KEY_F11, yourgame::InputSource::YOURGAME_KEY_F11},
             {GLFW_KEY_F12, yourgame::InputSource::YOURGAME_KEY_F12},
             {GLFW_KEY_F13, yourgame::InputSource::YOURGAME_KEY_F13},
             {GLFW_KEY_F14, yourgame::InputSource::YOURGAME_KEY_F14},
             {GLFW_KEY_F15, yourgame::InputSource::YOURGAME_KEY_F15},
             {GLFW_KEY_F16, yourgame::InputSource::YOURGAME_KEY_F16},
             {GLFW_KEY_F17, yourgame::InputSource::YOURGAME_KEY_F17},
             {GLFW_KEY_F18, yourgame::InputSource::YOURGAME_KEY_F18},
             {GLFW_KEY_F19, yourgame::InputSource::YOURGAME_KEY_F19},
             {GLFW_KEY_F20, yourgame::InputSource::YOURGAME_KEY_F20},
             {GLFW_KEY_F21, yourgame::InputSource::YOURGAME_KEY_F21},
             {GLFW_KEY_F22, yourgame::InputSource::YOURGAME_KEY_F22},
             {GLFW_KEY_F23, yourgame::InputSource::YOURGAME_KEY_F23},
             {GLFW_KEY_F24, yourgame::InputSource::YOURGAME_KEY_F24},
             {GLFW_KEY_F25, yourgame::InputSource::YOURGAME_KEY_F25},
             {GLFW_KEY_KP_0, yourgame::InputSource::YOURGAME_KEY_KP_0},
             {GLFW_KEY_KP_1, yourgame::InputSource::YOURGAME_KEY_KP_1},
             {GLFW_KEY_KP_2, yourgame::InputSource::YOURGAME_KEY_KP_2},
             {GLFW_KEY_KP_3, yourgame::InputSource::YOURGAME_KEY_KP_3},
             {GLFW_KEY_KP_4, yourgame::InputSource::YOURGAME_KEY_KP_4},
             {GLFW_KEY_KP_5, yourgame::InputSource::YOURGAME_KEY_KP_5},
             {GLFW_KEY_KP_6, yourgame::InputSource::YOURGAME_KEY_KP_6},
             {GLFW_KEY_KP_7, yourgame::InputSource::YOURGAME_KEY_KP_7},
             {GLFW_KEY_KP_8, yourgame::InputSource::YOURGAME_KEY_KP_8},
             {GLFW_KEY_KP_9, yourgame::InputSource::YOURGAME_KEY_KP_9},
             {GLFW_KEY_KP_DECIMAL, yourgame::InputSource::YOURGAME_KEY_KP_DECIMAL},
             {GLFW_KEY_KP_DIVIDE, yourgame::InputSource::YOURGAME_KEY_KP_DIVIDE},
             {GLFW_KEY_KP_MULTIPLY, yourgame::InputSource::YOURGAME_KEY_KP_MULTIPLY},
             {GLFW_KEY_KP_SUBTRACT, yourgame::InputSource::YOURGAME_KEY_KP_SUBTRACT},
             {GLFW_KEY_KP_ADD, yourgame::InputSource::YOURGAME_KEY_KP_ADD},
             {GLFW_KEY_KP_ENTER, yourgame::InputSource::YOURGAME_KEY_KP_ENTER},
             {GLFW_KEY_KP_EQUAL, yourgame::InputSource::YOURGAME_KEY_KP_EQUAL},
             {GLFW_KEY_LEFT_SHIFT, yourgame::InputSource::YOURGAME_KEY_LEFT_SHIFT},
             {GLFW_KEY_LEFT_CONTROL, yourgame::InputSource::YOURGAME_KEY_LEFT_CONTROL},
             {GLFW_KEY_LEFT_ALT, yourgame::InputSource::YOURGAME_KEY_LEFT_ALT},
             {GLFW_KEY_LEFT_SUPER, yourgame::InputSource::YOURGAME_KEY_LEFT_SUPER},
             {GLFW_KEY_RIGHT_SHIFT, yourgame::InputSource::YOURGAME_KEY_RIGHT_SHIFT},
             {GLFW_KEY_RIGHT_CONTROL, yourgame::InputSource::YOURGAME_KEY_RIGHT_CONTROL},
             {GLFW_KEY_RIGHT_ALT, yourgame::InputSource::YOURGAME_KEY_RIGHT_ALT},
             {GLFW_KEY_RIGHT_SUPER, yourgame::InputSource::YOURGAME_KEY_RIGHT_SUPER},
             {GLFW_KEY_MENU, yourgame::InputSource::YOURGAME_KEY_MENU}};

        const std::map<int, yourgame::InputSource> mouseButtonApiMapping =
            {{GLFW_MOUSE_BUTTON_1, yourgame::InputSource::YOURGAME_MOUSE_BUTTON_1},
             {GLFW_MOUSE_BUTTON_2, yourgame::InputSource::YOURGAME_MOUSE_BUTTON_2},
             {GLFW_MOUSE_BUTTON_3, yourgame::InputSource::YOURGAME_MOUSE_BUTTON_3},
             {GLFW_MOUSE_BUTTON_4, yourgame::InputSource::YOURGAME_MOUSE_BUTTON_4},
             {GLFW_MOUSE_BUTTON_5, yourgame::InputSource::YOURGAME_MOUSE_BUTTON_5},
             {GLFW_MOUSE_BUTTON_6, yourgame::InputSource::YOURGAME_MOUSE_BUTTON_6},
             {GLFW_MOUSE_BUTTON_7, yourgame::InputSource::YOURGAME_MOUSE_BUTTON_7},
             {GLFW_MOUSE_BUTTON_8, yourgame::InputSource::YOURGAME_MOUSE_BUTTON_8}};

        const std::map<int, yourgame::InputSource> gamepadConnectedApiMapping =
            {{GLFW_JOYSTICK_1, yourgame::InputSource::YOURGAME_GAMEPAD_0_CONNECTED},
             {GLFW_JOYSTICK_2, yourgame::InputSource::YOURGAME_GAMEPAD_1_CONNECTED},
             {GLFW_JOYSTICK_3, yourgame::InputSource::YOURGAME_GAMEPAD_2_CONNECTED},
             {GLFW_JOYSTICK_4, yourgame::InputSource::YOURGAME_GAMEPAD_3_CONNECTED}};

        const std::map<int, std::pair<yourgame::InputSource, yourgame::InputSource>> gamepadFirstButtonAxisIdx =
            {{GLFW_JOYSTICK_1, {yourgame::InputSource::YOURGAME_GAMEPAD_0_BUTTON_A, yourgame::InputSource::YOURGAME_GAMEPAD_0_AXIS_LEFT_X}},
             {GLFW_JOYSTICK_2, {yourgame::InputSource::YOURGAME_GAMEPAD_1_BUTTON_A, yourgame::InputSource::YOURGAME_GAMEPAD_1_AXIS_LEFT_X}},
             {GLFW_JOYSTICK_3, {yourgame::InputSource::YOURGAME_GAMEPAD_2_BUTTON_A, yourgame::InputSource::YOURGAME_GAMEPAD_2_AXIS_LEFT_X}},
             {GLFW_JOYSTICK_4, {yourgame::InputSource::YOURGAME_GAMEPAD_3_BUTTON_A, yourgame::InputSource::YOURGAME_GAMEPAD_3_AXIS_LEFT_X}}};

        struct InputValue
        {
            int isfloat; // 1=float, 0=int
            union
            {
                int vali;
                float valf;
            };
        };

        std::map<yourgame::InputSource, InputValue> _inputStates;

        void set(yourgame::InputSource source, float value)
        {
            InputValue newval{1};
            newval.valf = value;
            _inputStates[source] = newval;
        }

        void set(yourgame::InputSource source, int value)
        {
            InputValue newval{0};
            newval.vali = value;
            _inputStates[source] = newval;
        }

        void keyCallback(GLFWwindow *window, int key, int scancode, int action, int mods)
        {
            auto mapping = keyApiMapping.find(key);
            if (mapping != keyApiMapping.end())
            {
                if (action == GLFW_PRESS)
                    set(mapping->second, 1);
                else if (action == GLFW_RELEASE)
                    set(mapping->second, 0);
            }
        }

        static void cursorPositionCallback(GLFWwindow *window, double xpos, double ypos)
        {
            set(yourgame::InputSource::YOURGAME_MOUSE_X, (float)xpos);
            set(yourgame::InputSource::YOURGAME_MOUSE_Y, (float)ypos);
        }

        void mouseButtonCallback(GLFWwindow *window, int button, int action, int mods)
        {
            auto mapping = mouseButtonApiMapping.find(button);
            if (mapping != mouseButtonApiMapping.end())
            {
                if (action == GLFW_PRESS)
                    set(mapping->second, 1);
                else if (action == GLFW_RELEASE)
                    set(mapping->second, 0);
            }
        }

        void joystickCallback(int jid, int event)
        {
            auto mapping = gamepadConnectedApiMapping.find(jid);
            if (mapping != gamepadConnectedApiMapping.end())
            {
                if (event == GLFW_CONNECTED)
                    set(mapping->second, 1);
                else if (event == GLFW_DISCONNECTED)
                    set(mapping->second, 0);
            }
        }

    } // namespace

    void initInput(GLFWwindow *window)
    {
        glfwSetKeyCallback(window, keyCallback);
        glfwSetCursorPosCallback(window, cursorPositionCallback);
        glfwSetMouseButtonCallback(window, mouseButtonCallback);
        glfwSetJoystickCallback(joystickCallback);

// todo: mapped gamepad input available since glfw v3.3,
// emscripten implements glfw v3.2 API only (2020-12-12)
#ifndef __EMSCRIPTEN__
        // poll and set gamepads, that are already connected during init
        for (const auto &p : gamepadConnectedApiMapping)
        {
            set(p.second, glfwJoystickIsGamepad(p.first) == GLFW_TRUE ? 1 : 0);
        }
#endif
    }

    void tickInput()
    {
// todo: mapped gamepad input available since glfw v3.3,
// emscripten implements glfw v3.2 API only (2020-12-12)
#ifndef __EMSCRIPTEN__
        for (const auto &pad : gamepadFirstButtonAxisIdx)
        {
            GLFWgamepadstate state;
            if (glfwGetGamepadState(pad.first, &state))
            {
                int buttonWriteIdx = static_cast<int>(pad.second.first);
                set(static_cast<yourgame::InputSource>(buttonWriteIdx++), state.buttons[GLFW_GAMEPAD_BUTTON_A] == GLFW_PRESS ? 1 : 0);
                set(static_cast<yourgame::InputSource>(buttonWriteIdx++), state.buttons[GLFW_GAMEPAD_BUTTON_B] == GLFW_PRESS ? 1 : 0);
                set(static_cast<yourgame::InputSource>(buttonWriteIdx++), state.buttons[GLFW_GAMEPAD_BUTTON_X] == GLFW_PRESS ? 1 : 0);
                set(static_cast<yourgame::InputSource>(buttonWriteIdx++), state.buttons[GLFW_GAMEPAD_BUTTON_Y] == GLFW_PRESS ? 1 : 0);
                set(static_cast<yourgame::InputSource>(buttonWriteIdx++), state.buttons[GLFW_GAMEPAD_BUTTON_LEFT_BUMPER] == GLFW_PRESS ? 1 : 0);
                set(static_cast<yourgame::InputSource>(buttonWriteIdx++), state.buttons[GLFW_GAMEPAD_BUTTON_RIGHT_BUMPER] == GLFW_PRESS ? 1 : 0);
                set(static_cast<yourgame::InputSource>(buttonWriteIdx++), state.buttons[GLFW_GAMEPAD_BUTTON_BACK] == GLFW_PRESS ? 1 : 0);
                set(static_cast<yourgame::InputSource>(buttonWriteIdx++), state.buttons[GLFW_GAMEPAD_BUTTON_START] == GLFW_PRESS ? 1 : 0);
                set(static_cast<yourgame::InputSource>(buttonWriteIdx++), state.buttons[GLFW_GAMEPAD_BUTTON_GUIDE] == GLFW_PRESS ? 1 : 0);
                set(static_cast<yourgame::InputSource>(buttonWriteIdx++), state.buttons[GLFW_GAMEPAD_BUTTON_LEFT_THUMB] == GLFW_PRESS ? 1 : 0);
                set(static_cast<yourgame::InputSource>(buttonWriteIdx++), state.buttons[GLFW_GAMEPAD_BUTTON_RIGHT_THUMB] == GLFW_PRESS ? 1 : 0);
                set(static_cast<yourgame::InputSource>(buttonWriteIdx++), state.buttons[GLFW_GAMEPAD_BUTTON_DPAD_UP] == GLFW_PRESS ? 1 : 0);
                set(static_cast<yourgame::InputSource>(buttonWriteIdx++), state.buttons[GLFW_GAMEPAD_BUTTON_DPAD_RIGHT] == GLFW_PRESS ? 1 : 0);
                set(static_cast<yourgame::InputSource>(buttonWriteIdx++), state.buttons[GLFW_GAMEPAD_BUTTON_DPAD_DOWN] == GLFW_PRESS ? 1 : 0);
                set(static_cast<yourgame::InputSource>(buttonWriteIdx++), state.buttons[GLFW_GAMEPAD_BUTTON_DPAD_LEFT] == GLFW_PRESS ? 1 : 0);
                int axisWriteIdx = static_cast<int>(pad.second.second);
                set(static_cast<yourgame::InputSource>(axisWriteIdx++), state.axes[GLFW_GAMEPAD_AXIS_LEFT_X]);
                set(static_cast<yourgame::InputSource>(axisWriteIdx++), state.axes[GLFW_GAMEPAD_AXIS_LEFT_Y]);
                set(static_cast<yourgame::InputSource>(axisWriteIdx++), state.axes[GLFW_GAMEPAD_AXIS_RIGHT_X]);
                set(static_cast<yourgame::InputSource>(axisWriteIdx++), state.axes[GLFW_GAMEPAD_AXIS_RIGHT_Y]);
                set(static_cast<yourgame::InputSource>(axisWriteIdx++), state.axes[GLFW_GAMEPAD_AXIS_LEFT_TRIGGER]);
                set(static_cast<yourgame::InputSource>(axisWriteIdx++), state.axes[GLFW_GAMEPAD_AXIS_RIGHT_TRIGGER]);
            }
        }
#endif
    }
} // namespace yourgame_internal_desktop

namespace yourgame
{
    float getInputf(yourgame::InputSource source)
    {
        auto i = yourgame_internal_desktop::_inputStates.find(source);
        return (i == yourgame_internal_desktop::_inputStates.end()) ? 0.0f
                                                                    : (i->second.isfloat) ? (i->second).valf
                                                                                          : (float)((i->second).vali);
    }

    int getInputi(yourgame::InputSource source)
    {
        auto i = yourgame_internal_desktop::_inputStates.find(source);
        return (i == yourgame_internal_desktop::_inputStates.end()) ? 0
                                                                    : (i->second.isfloat) ? (int)((i->second).valf)
                                                                                          : (i->second).vali;
    }
} // namespace yourgame
