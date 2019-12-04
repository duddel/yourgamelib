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
#ifndef YOURGAME_INPUT_H
#define YOURGAME_INPUT_H

namespace yourgame
{

enum class InputSource
{
    YOURGAME_KEY_UNKNOWN,
    YOURGAME_KEY_APOSTROPHE,
    YOURGAME_KEY_COMMA,
    YOURGAME_KEY_MINUS,
    YOURGAME_KEY_PERIOD,
    YOURGAME_KEY_SLASH,
    YOURGAME_KEY_0,
    YOURGAME_KEY_1,
    YOURGAME_KEY_2,
    YOURGAME_KEY_3,
    YOURGAME_KEY_4,
    YOURGAME_KEY_5,
    YOURGAME_KEY_6,
    YOURGAME_KEY_7,
    YOURGAME_KEY_8,
    YOURGAME_KEY_9,
    YOURGAME_KEY_SEMICOLON,
    YOURGAME_KEY_EQUAL,
    YOURGAME_KEY_A,
    YOURGAME_KEY_B,
    YOURGAME_KEY_C,
    YOURGAME_KEY_D,
    YOURGAME_KEY_E,
    YOURGAME_KEY_F,
    YOURGAME_KEY_G,
    YOURGAME_KEY_H,
    YOURGAME_KEY_I,
    YOURGAME_KEY_J,
    YOURGAME_KEY_K,
    YOURGAME_KEY_L,
    YOURGAME_KEY_M,
    YOURGAME_KEY_N,
    YOURGAME_KEY_O,
    YOURGAME_KEY_P,
    YOURGAME_KEY_Q,
    YOURGAME_KEY_R,
    YOURGAME_KEY_S,
    YOURGAME_KEY_T,
    YOURGAME_KEY_U,
    YOURGAME_KEY_V,
    YOURGAME_KEY_W,
    YOURGAME_KEY_X,
    YOURGAME_KEY_Y,
    YOURGAME_KEY_Z,
    YOURGAME_KEY_LEFT_BRACKET,
    YOURGAME_KEY_BACKSLASH,
    YOURGAME_KEY_RIGHT_BRACKET,
    YOURGAME_KEY_GRAVE_ACCENT,
    YOURGAME_KEY_WORLD_1,
    YOURGAME_KEY_WORLD_2,
    YOURGAME_KEY_ESCAPE,
    YOURGAME_KEY_ENTER,
    YOURGAME_KEY_TAB,
    YOURGAME_KEY_BACKSPACE,
    YOURGAME_KEY_INSERT,
    YOURGAME_KEY_DELETE,
    YOURGAME_KEY_RIGHT,
    YOURGAME_KEY_LEFT,
    YOURGAME_KEY_DOWN,
    YOURGAME_KEY_UP,
    YOURGAME_KEY_PAGE_UP,
    YOURGAME_KEY_PAGE_DOWN,
    YOURGAME_KEY_HOME,
    YOURGAME_KEY_END,
    YOURGAME_KEY_CAPS_LOCK,
    YOURGAME_KEY_SCROLL_LOCK,
    YOURGAME_KEY_NUM_LOCK,
    YOURGAME_KEY_PRINT_SCREEN,
    YOURGAME_KEY_PAUSE,
    YOURGAME_KEY_F1,
    YOURGAME_KEY_F2,
    YOURGAME_KEY_F3,
    YOURGAME_KEY_F4,
    YOURGAME_KEY_F5,
    YOURGAME_KEY_F6,
    YOURGAME_KEY_F7,
    YOURGAME_KEY_F8,
    YOURGAME_KEY_F9,
    YOURGAME_KEY_F10,
    YOURGAME_KEY_F11,
    YOURGAME_KEY_F12,
    YOURGAME_KEY_F13,
    YOURGAME_KEY_F14,
    YOURGAME_KEY_F15,
    YOURGAME_KEY_F16,
    YOURGAME_KEY_F17,
    YOURGAME_KEY_F18,
    YOURGAME_KEY_F19,
    YOURGAME_KEY_F20,
    YOURGAME_KEY_F21,
    YOURGAME_KEY_F22,
    YOURGAME_KEY_F23,
    YOURGAME_KEY_F24,
    YOURGAME_KEY_F25,
    YOURGAME_KEY_KP_0,
    YOURGAME_KEY_KP_1,
    YOURGAME_KEY_KP_2,
    YOURGAME_KEY_KP_3,
    YOURGAME_KEY_KP_4,
    YOURGAME_KEY_KP_5,
    YOURGAME_KEY_KP_6,
    YOURGAME_KEY_KP_7,
    YOURGAME_KEY_KP_8,
    YOURGAME_KEY_KP_9,
    YOURGAME_KEY_KP_DECIMAL,
    YOURGAME_KEY_KP_DIVIDE,
    YOURGAME_KEY_KP_MULTIPLY,
    YOURGAME_KEY_KP_SUBTRACT,
    YOURGAME_KEY_KP_ADD,
    YOURGAME_KEY_KP_ENTER,
    YOURGAME_KEY_KP_EQUAL,
    YOURGAME_KEY_LEFT_SHIFT,
    YOURGAME_KEY_LEFT_CONTROL,
    YOURGAME_KEY_LEFT_ALT,
    YOURGAME_KEY_LEFT_SUPER,
    YOURGAME_KEY_RIGHT_SHIFT,
    YOURGAME_KEY_RIGHT_CONTROL,
    YOURGAME_KEY_RIGHT_ALT,
    YOURGAME_KEY_RIGHT_SUPER,
    YOURGAME_KEY_MENU,
    YOURGAME_MOUSE_X,
    YOURGAME_MOUSE_Y,
    YOURGAME_MOUSE_BUTTON_1, // left
    YOURGAME_MOUSE_BUTTON_2, // right
    YOURGAME_MOUSE_BUTTON_3, // middle
    YOURGAME_MOUSE_BUTTON_4,
    YOURGAME_MOUSE_BUTTON_5,
    YOURGAME_MOUSE_BUTTON_6,
    YOURGAME_MOUSE_BUTTON_7,
    YOURGAME_MOUSE_BUTTON_8,
    YOURGAME_TOUCH_0_DOWN,
    YOURGAME_TOUCH_0_X,
    YOURGAME_TOUCH_0_Y
};

float getInputf(InputSource source);
int getInputi(InputSource source);

} // namespace yourgame

#endif
