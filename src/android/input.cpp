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
            union
            {
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
            static const yourgame::InputSource srcsTouchDown[10] = {yourgame::InputSource::YOURGAME_TOUCH_0_DOWN,
                                                                    yourgame::InputSource::YOURGAME_TOUCH_1_DOWN,
                                                                    yourgame::InputSource::YOURGAME_TOUCH_2_DOWN,
                                                                    yourgame::InputSource::YOURGAME_TOUCH_3_DOWN,
                                                                    yourgame::InputSource::YOURGAME_TOUCH_4_DOWN,
                                                                    yourgame::InputSource::YOURGAME_TOUCH_5_DOWN,
                                                                    yourgame::InputSource::YOURGAME_TOUCH_6_DOWN,
                                                                    yourgame::InputSource::YOURGAME_TOUCH_7_DOWN,
                                                                    yourgame::InputSource::YOURGAME_TOUCH_8_DOWN,
                                                                    yourgame::InputSource::YOURGAME_TOUCH_9_DOWN};

            static const yourgame::InputSource srcsTouchX[10] = {yourgame::InputSource::YOURGAME_TOUCH_0_X,
                                                                 yourgame::InputSource::YOURGAME_TOUCH_1_X,
                                                                 yourgame::InputSource::YOURGAME_TOUCH_2_X,
                                                                 yourgame::InputSource::YOURGAME_TOUCH_3_X,
                                                                 yourgame::InputSource::YOURGAME_TOUCH_4_X,
                                                                 yourgame::InputSource::YOURGAME_TOUCH_5_X,
                                                                 yourgame::InputSource::YOURGAME_TOUCH_6_X,
                                                                 yourgame::InputSource::YOURGAME_TOUCH_7_X,
                                                                 yourgame::InputSource::YOURGAME_TOUCH_8_X,
                                                                 yourgame::InputSource::YOURGAME_TOUCH_9_X};

            static const yourgame::InputSource srcsTouchY[10] = {yourgame::InputSource::YOURGAME_TOUCH_0_Y,
                                                                 yourgame::InputSource::YOURGAME_TOUCH_1_Y,
                                                                 yourgame::InputSource::YOURGAME_TOUCH_2_Y,
                                                                 yourgame::InputSource::YOURGAME_TOUCH_3_Y,
                                                                 yourgame::InputSource::YOURGAME_TOUCH_4_Y,
                                                                 yourgame::InputSource::YOURGAME_TOUCH_5_Y,
                                                                 yourgame::InputSource::YOURGAME_TOUCH_6_Y,
                                                                 yourgame::InputSource::YOURGAME_TOUCH_7_Y,
                                                                 yourgame::InputSource::YOURGAME_TOUCH_8_Y,
                                                                 yourgame::InputSource::YOURGAME_TOUCH_9_Y};

            int32_t evType = AInputEvent_getType(inputEvent);
            switch (evType)
            {
            case AINPUT_EVENT_TYPE_MOTION:
            {
                int32_t evAction = AMotionEvent_getAction(inputEvent);
                int32_t evPointerIndex = (evAction & AMOTION_EVENT_ACTION_POINTER_INDEX_MASK) >> AMOTION_EVENT_ACTION_POINTER_INDEX_SHIFT;
                int32_t evPointerId = AMotionEvent_getPointerId(inputEvent, evPointerIndex);
                evAction &= AMOTION_EVENT_ACTION_MASK;
                switch (evAction)
                {
                /* assumption: the primary (first) touch pointer of a gesture triggers
                                 AMOTION_EVENT_ACTION_DOWN and AMOTION_EVENT_ACTION_UP.
                               additional non-primary pointers during this gesture trigger
                                 AMOTION_EVENT_ACTION_POINTER_DOWN and AMOTION_EVENT_ACTION_POINTER_UP.
                               here they are handled equally.
                   assumption: the pointer id of the primary pointer is always 0, but here it is
                               retrieved via AMotionEvent_getPointerId() (see above), which should
                               return 0 for primary pointers. */
                case AMOTION_EVENT_ACTION_DOWN:
                    // intended fallthrough...
                case AMOTION_EVENT_ACTION_UP:
                    // intended fallthrough...
                case AMOTION_EVENT_ACTION_POINTER_DOWN:
                    // intended fallthrough...
                case AMOTION_EVENT_ACTION_POINTER_UP:
                {
                    if (evPointerId >= 0 && evPointerId < sizeof(srcsTouchDown) / sizeof(srcsTouchDown[0]))
                    {
                        set(srcsTouchDown[evPointerId], ((evAction == AMOTION_EVENT_ACTION_DOWN ||
                                                          evAction == AMOTION_EVENT_ACTION_POINTER_DOWN)
                                                             ? 1
                                                             : 0));
                        // set initial location
                        set(srcsTouchX[evPointerId], (float)AMotionEvent_getX(inputEvent, evPointerIndex));
                        set(srcsTouchY[evPointerId], (float)AMotionEvent_getY(inputEvent, evPointerIndex));
                    }
                }
                break;
                case AMOTION_EVENT_ACTION_MOVE:
                {
                    auto evNumPtr = AMotionEvent_getPointerCount(inputEvent);
                    for (auto i = 0; i < evNumPtr; i++)
                    {
                        int32_t pointerId = AMotionEvent_getPointerId(inputEvent, i);
                        set(srcsTouchX[pointerId], (float)AMotionEvent_getX(inputEvent, i));
                        set(srcsTouchY[pointerId], (float)AMotionEvent_getY(inputEvent, i));
                    }
                }
                break;
                default:
                    break;
                }
            }
                return 1;
            case AINPUT_EVENT_TYPE_KEY:
                break;
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
