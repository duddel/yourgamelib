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
#include <android_native_app_glue.h>
#include <android/input.h>
#include <android/keycodes.h>
#include "yourgame/input.h"

namespace yourgame
{
namespace
{
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

int32_t handleInputEvent(struct android_app *app, AInputEvent *inputEvent)
{
    int32_t evType = AInputEvent_getType(inputEvent);
    switch (evType)
    {
    case AINPUT_EVENT_TYPE_KEY:
        break;
    case AINPUT_EVENT_TYPE_MOTION:
    {
        // todo: handle multi-touch
        int32_t evAction = AMotionEvent_getAction(inputEvent);
        int32_t evPointerIndex = (evAction & AMOTION_EVENT_ACTION_POINTER_INDEX_MASK) >> AMOTION_EVENT_ACTION_POINTER_INDEX_SHIFT;
        int32_t evPointerId = AMotionEvent_getPointerId(inputEvent, evPointerIndex);
        evAction &= AMOTION_EVENT_ACTION_MASK;
        switch (evAction)
        {
        case AMOTION_EVENT_ACTION_DOWN:
        case AMOTION_EVENT_ACTION_UP:
            set(yourgame::InputSource::YOURGAME_TOUCH_0_DOWN, (evAction == AMOTION_EVENT_ACTION_DOWN));
            // intended fallthrough...
        case AMOTION_EVENT_ACTION_MOVE:
            set(yourgame::InputSource::YOURGAME_TOUCH_0_X, AMotionEvent_getX(inputEvent, evPointerIndex));
            set(yourgame::InputSource::YOURGAME_TOUCH_0_Y, AMotionEvent_getY(inputEvent, evPointerIndex));
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

} // namespace

void initInput(struct android_app *app)
{
    app->onInputEvent = handleInputEvent;
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
