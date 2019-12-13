/*
Copyright (c) 2019 Alexander Scholz

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
#include <GLFW/glfw3.h>
#include "yourgame/input.h"

namespace yourgame
{
namespace
{
const std::map<int, InputSource> keyApiMapping =
    {{GLFW_KEY_UNKNOWN, InputSource::YOURGAME_KEY_UNKNOWN},
     {GLFW_KEY_APOSTROPHE, InputSource::YOURGAME_KEY_APOSTROPHE},
     {GLFW_KEY_COMMA, InputSource::YOURGAME_KEY_COMMA},
     {GLFW_KEY_MINUS, InputSource::YOURGAME_KEY_MINUS},
     {GLFW_KEY_PERIOD, InputSource::YOURGAME_KEY_PERIOD},
     {GLFW_KEY_SLASH, InputSource::YOURGAME_KEY_SLASH},
     {GLFW_KEY_0, InputSource::YOURGAME_KEY_0},
     {GLFW_KEY_1, InputSource::YOURGAME_KEY_1},
     {GLFW_KEY_2, InputSource::YOURGAME_KEY_2},
     {GLFW_KEY_3, InputSource::YOURGAME_KEY_3},
     {GLFW_KEY_4, InputSource::YOURGAME_KEY_4},
     {GLFW_KEY_5, InputSource::YOURGAME_KEY_5},
     {GLFW_KEY_6, InputSource::YOURGAME_KEY_6},
     {GLFW_KEY_7, InputSource::YOURGAME_KEY_7},
     {GLFW_KEY_8, InputSource::YOURGAME_KEY_8},
     {GLFW_KEY_9, InputSource::YOURGAME_KEY_9},
     {GLFW_KEY_SEMICOLON, InputSource::YOURGAME_KEY_SEMICOLON},
     {GLFW_KEY_EQUAL, InputSource::YOURGAME_KEY_EQUAL},
     {GLFW_KEY_A, InputSource::YOURGAME_KEY_A},
     {GLFW_KEY_B, InputSource::YOURGAME_KEY_B},
     {GLFW_KEY_C, InputSource::YOURGAME_KEY_C},
     {GLFW_KEY_D, InputSource::YOURGAME_KEY_D},
     {GLFW_KEY_E, InputSource::YOURGAME_KEY_E},
     {GLFW_KEY_F, InputSource::YOURGAME_KEY_F},
     {GLFW_KEY_G, InputSource::YOURGAME_KEY_G},
     {GLFW_KEY_H, InputSource::YOURGAME_KEY_H},
     {GLFW_KEY_I, InputSource::YOURGAME_KEY_I},
     {GLFW_KEY_J, InputSource::YOURGAME_KEY_J},
     {GLFW_KEY_K, InputSource::YOURGAME_KEY_K},
     {GLFW_KEY_L, InputSource::YOURGAME_KEY_L},
     {GLFW_KEY_M, InputSource::YOURGAME_KEY_M},
     {GLFW_KEY_N, InputSource::YOURGAME_KEY_N},
     {GLFW_KEY_O, InputSource::YOURGAME_KEY_O},
     {GLFW_KEY_P, InputSource::YOURGAME_KEY_P},
     {GLFW_KEY_Q, InputSource::YOURGAME_KEY_Q},
     {GLFW_KEY_R, InputSource::YOURGAME_KEY_R},
     {GLFW_KEY_S, InputSource::YOURGAME_KEY_S},
     {GLFW_KEY_T, InputSource::YOURGAME_KEY_T},
     {GLFW_KEY_U, InputSource::YOURGAME_KEY_U},
     {GLFW_KEY_V, InputSource::YOURGAME_KEY_V},
     {GLFW_KEY_W, InputSource::YOURGAME_KEY_W},
     {GLFW_KEY_X, InputSource::YOURGAME_KEY_X},
     {GLFW_KEY_Y, InputSource::YOURGAME_KEY_Y},
     {GLFW_KEY_Z, InputSource::YOURGAME_KEY_Z},
     {GLFW_KEY_LEFT_BRACKET, InputSource::YOURGAME_KEY_LEFT_BRACKET},
     {GLFW_KEY_BACKSLASH, InputSource::YOURGAME_KEY_BACKSLASH},
     {GLFW_KEY_RIGHT_BRACKET, InputSource::YOURGAME_KEY_RIGHT_BRACKET},
     {GLFW_KEY_GRAVE_ACCENT, InputSource::YOURGAME_KEY_GRAVE_ACCENT},
     {GLFW_KEY_WORLD_1, InputSource::YOURGAME_KEY_WORLD_1},
     {GLFW_KEY_WORLD_2, InputSource::YOURGAME_KEY_WORLD_2},
     {GLFW_KEY_ESCAPE, InputSource::YOURGAME_KEY_ESCAPE},
     {GLFW_KEY_ENTER, InputSource::YOURGAME_KEY_ENTER},
     {GLFW_KEY_TAB, InputSource::YOURGAME_KEY_TAB},
     {GLFW_KEY_BACKSPACE, InputSource::YOURGAME_KEY_BACKSPACE},
     {GLFW_KEY_INSERT, InputSource::YOURGAME_KEY_INSERT},
     {GLFW_KEY_DELETE, InputSource::YOURGAME_KEY_DELETE},
     {GLFW_KEY_RIGHT, InputSource::YOURGAME_KEY_RIGHT},
     {GLFW_KEY_LEFT, InputSource::YOURGAME_KEY_LEFT},
     {GLFW_KEY_DOWN, InputSource::YOURGAME_KEY_DOWN},
     {GLFW_KEY_UP, InputSource::YOURGAME_KEY_UP},
     {GLFW_KEY_PAGE_UP, InputSource::YOURGAME_KEY_PAGE_UP},
     {GLFW_KEY_PAGE_DOWN, InputSource::YOURGAME_KEY_PAGE_DOWN},
     {GLFW_KEY_HOME, InputSource::YOURGAME_KEY_HOME},
     {GLFW_KEY_END, InputSource::YOURGAME_KEY_END},
     {GLFW_KEY_CAPS_LOCK, InputSource::YOURGAME_KEY_CAPS_LOCK},
     {GLFW_KEY_SCROLL_LOCK, InputSource::YOURGAME_KEY_SCROLL_LOCK},
     {GLFW_KEY_NUM_LOCK, InputSource::YOURGAME_KEY_NUM_LOCK},
     {GLFW_KEY_PRINT_SCREEN, InputSource::YOURGAME_KEY_PRINT_SCREEN},
     {GLFW_KEY_PAUSE, InputSource::YOURGAME_KEY_PAUSE},
     {GLFW_KEY_F1, InputSource::YOURGAME_KEY_F1},
     {GLFW_KEY_F2, InputSource::YOURGAME_KEY_F2},
     {GLFW_KEY_F3, InputSource::YOURGAME_KEY_F3},
     {GLFW_KEY_F4, InputSource::YOURGAME_KEY_F4},
     {GLFW_KEY_F5, InputSource::YOURGAME_KEY_F5},
     {GLFW_KEY_F6, InputSource::YOURGAME_KEY_F6},
     {GLFW_KEY_F7, InputSource::YOURGAME_KEY_F7},
     {GLFW_KEY_F8, InputSource::YOURGAME_KEY_F8},
     {GLFW_KEY_F9, InputSource::YOURGAME_KEY_F9},
     {GLFW_KEY_F10, InputSource::YOURGAME_KEY_F10},
     {GLFW_KEY_F11, InputSource::YOURGAME_KEY_F11},
     {GLFW_KEY_F12, InputSource::YOURGAME_KEY_F12},
     {GLFW_KEY_F13, InputSource::YOURGAME_KEY_F13},
     {GLFW_KEY_F14, InputSource::YOURGAME_KEY_F14},
     {GLFW_KEY_F15, InputSource::YOURGAME_KEY_F15},
     {GLFW_KEY_F16, InputSource::YOURGAME_KEY_F16},
     {GLFW_KEY_F17, InputSource::YOURGAME_KEY_F17},
     {GLFW_KEY_F18, InputSource::YOURGAME_KEY_F18},
     {GLFW_KEY_F19, InputSource::YOURGAME_KEY_F19},
     {GLFW_KEY_F20, InputSource::YOURGAME_KEY_F20},
     {GLFW_KEY_F21, InputSource::YOURGAME_KEY_F21},
     {GLFW_KEY_F22, InputSource::YOURGAME_KEY_F22},
     {GLFW_KEY_F23, InputSource::YOURGAME_KEY_F23},
     {GLFW_KEY_F24, InputSource::YOURGAME_KEY_F24},
     {GLFW_KEY_F25, InputSource::YOURGAME_KEY_F25},
     {GLFW_KEY_KP_0, InputSource::YOURGAME_KEY_KP_0},
     {GLFW_KEY_KP_1, InputSource::YOURGAME_KEY_KP_1},
     {GLFW_KEY_KP_2, InputSource::YOURGAME_KEY_KP_2},
     {GLFW_KEY_KP_3, InputSource::YOURGAME_KEY_KP_3},
     {GLFW_KEY_KP_4, InputSource::YOURGAME_KEY_KP_4},
     {GLFW_KEY_KP_5, InputSource::YOURGAME_KEY_KP_5},
     {GLFW_KEY_KP_6, InputSource::YOURGAME_KEY_KP_6},
     {GLFW_KEY_KP_7, InputSource::YOURGAME_KEY_KP_7},
     {GLFW_KEY_KP_8, InputSource::YOURGAME_KEY_KP_8},
     {GLFW_KEY_KP_9, InputSource::YOURGAME_KEY_KP_9},
     {GLFW_KEY_KP_DECIMAL, InputSource::YOURGAME_KEY_KP_DECIMAL},
     {GLFW_KEY_KP_DIVIDE, InputSource::YOURGAME_KEY_KP_DIVIDE},
     {GLFW_KEY_KP_MULTIPLY, InputSource::YOURGAME_KEY_KP_MULTIPLY},
     {GLFW_KEY_KP_SUBTRACT, InputSource::YOURGAME_KEY_KP_SUBTRACT},
     {GLFW_KEY_KP_ADD, InputSource::YOURGAME_KEY_KP_ADD},
     {GLFW_KEY_KP_ENTER, InputSource::YOURGAME_KEY_KP_ENTER},
     {GLFW_KEY_KP_EQUAL, InputSource::YOURGAME_KEY_KP_EQUAL},
     {GLFW_KEY_LEFT_SHIFT, InputSource::YOURGAME_KEY_LEFT_SHIFT},
     {GLFW_KEY_LEFT_CONTROL, InputSource::YOURGAME_KEY_LEFT_CONTROL},
     {GLFW_KEY_LEFT_ALT, InputSource::YOURGAME_KEY_LEFT_ALT},
     {GLFW_KEY_LEFT_SUPER, InputSource::YOURGAME_KEY_LEFT_SUPER},
     {GLFW_KEY_RIGHT_SHIFT, InputSource::YOURGAME_KEY_RIGHT_SHIFT},
     {GLFW_KEY_RIGHT_CONTROL, InputSource::YOURGAME_KEY_RIGHT_CONTROL},
     {GLFW_KEY_RIGHT_ALT, InputSource::YOURGAME_KEY_RIGHT_ALT},
     {GLFW_KEY_RIGHT_SUPER, InputSource::YOURGAME_KEY_RIGHT_SUPER},
     {GLFW_KEY_MENU, InputSource::YOURGAME_KEY_MENU}};

const std::map<int, InputSource> mouseButtonApiMapping =
    {{GLFW_MOUSE_BUTTON_1, InputSource::YOURGAME_MOUSE_BUTTON_1},
     {GLFW_MOUSE_BUTTON_2, InputSource::YOURGAME_MOUSE_BUTTON_2},
     {GLFW_MOUSE_BUTTON_3, InputSource::YOURGAME_MOUSE_BUTTON_3},
     {GLFW_MOUSE_BUTTON_4, InputSource::YOURGAME_MOUSE_BUTTON_4},
     {GLFW_MOUSE_BUTTON_5, InputSource::YOURGAME_MOUSE_BUTTON_5},
     {GLFW_MOUSE_BUTTON_6, InputSource::YOURGAME_MOUSE_BUTTON_6},
     {GLFW_MOUSE_BUTTON_7, InputSource::YOURGAME_MOUSE_BUTTON_7},
     {GLFW_MOUSE_BUTTON_8, InputSource::YOURGAME_MOUSE_BUTTON_8}};

struct InputValue
{
    int isfloat; // 1=float, 0=int
    union {
        int vali;
        float valf;
    };
};

std::map<InputSource, InputValue> _inputStates;

void set(InputSource source, float value)
{
    InputValue newval{1};
    newval.valf = value;
    _inputStates[source] = newval;
}

void set(InputSource source, int value)
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

} // namespace

void initInput(GLFWwindow *window)
{
    glfwSetKeyCallback(window, keyCallback);
    glfwSetCursorPosCallback(window, cursorPositionCallback);
    glfwSetMouseButtonCallback(window, mouseButtonCallback);
}

float getInputf(InputSource source)
{
    auto i = _inputStates.find(source);
    return (i == _inputStates.end()) ? 0.0f
                                     : (i->second.isfloat) ? (i->second).valf
                                                           : (float)((i->second).vali);
}

int getInputi(InputSource source)
{
    auto i = _inputStates.find(source);
    return (i == _inputStates.end()) ? 0
                                     : (i->second.isfloat) ? (int)((i->second).valf)
                                                           : (i->second).vali;
}

} // namespace yourgame
