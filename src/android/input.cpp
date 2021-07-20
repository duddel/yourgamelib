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
#include <map>
#include <android_native_app_glue.h>
#include <android/input.h>
#include <android/keycodes.h>
#include "yourgame/input.h"

namespace yourgame_internal_android
{
    namespace
    {
        struct InputValue
        {
            InputValue(float v = 0.0f) : val(v), valLast(v) {} // default ctor
            float val;
            float valLast;
        };

        std::map<yourgame::INPUT, InputValue> _inputStates;

        // used for buttons/keys. if first input of source occurs, valLast
        // is defaulted to 0.0f (via default ctor), resulting in positive
        // value delta, if first input of source is "key down".
        void set(yourgame::INPUT source, float value)
        {
            _inputStates[source].val = value;
        }

        // used for positions/axes or other "continuous" signals.
        // if first input of source occurs, valLast is set = value,
        // resulting in zero value delta after first input.
        void set2(yourgame::INPUT source, float value)
        {
            auto emret = _inputStates.emplace(source, value);
            emret.first->second.val = value;
        }
    } // namespace

    int32_t handleInputEvent(AInputEvent *inputEvent)
    {
        static const yourgame::INPUT srcsTouchDown[10] = {yourgame::INPUT::TOUCH_0_DOWN,
                                                          yourgame::INPUT::TOUCH_1_DOWN,
                                                          yourgame::INPUT::TOUCH_2_DOWN,
                                                          yourgame::INPUT::TOUCH_3_DOWN,
                                                          yourgame::INPUT::TOUCH_4_DOWN,
                                                          yourgame::INPUT::TOUCH_5_DOWN,
                                                          yourgame::INPUT::TOUCH_6_DOWN,
                                                          yourgame::INPUT::TOUCH_7_DOWN,
                                                          yourgame::INPUT::TOUCH_8_DOWN,
                                                          yourgame::INPUT::TOUCH_9_DOWN};

        static const yourgame::INPUT srcsTouchX[10] = {yourgame::INPUT::TOUCH_0_X,
                                                       yourgame::INPUT::TOUCH_1_X,
                                                       yourgame::INPUT::TOUCH_2_X,
                                                       yourgame::INPUT::TOUCH_3_X,
                                                       yourgame::INPUT::TOUCH_4_X,
                                                       yourgame::INPUT::TOUCH_5_X,
                                                       yourgame::INPUT::TOUCH_6_X,
                                                       yourgame::INPUT::TOUCH_7_X,
                                                       yourgame::INPUT::TOUCH_8_X,
                                                       yourgame::INPUT::TOUCH_9_X};

        static const yourgame::INPUT srcsTouchY[10] = {yourgame::INPUT::TOUCH_0_Y,
                                                       yourgame::INPUT::TOUCH_1_Y,
                                                       yourgame::INPUT::TOUCH_2_Y,
                                                       yourgame::INPUT::TOUCH_3_Y,
                                                       yourgame::INPUT::TOUCH_4_Y,
                                                       yourgame::INPUT::TOUCH_5_Y,
                                                       yourgame::INPUT::TOUCH_6_Y,
                                                       yourgame::INPUT::TOUCH_7_Y,
                                                       yourgame::INPUT::TOUCH_8_Y,
                                                       yourgame::INPUT::TOUCH_9_Y};

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
                                                         ? 1.0f
                                                         : 0.0f));
                    // set initial location
                    set2(srcsTouchX[evPointerId], (float)AMotionEvent_getX(inputEvent, evPointerIndex));
                    set2(srcsTouchY[evPointerId], (float)AMotionEvent_getY(inputEvent, evPointerIndex));
                }
            }
            break;
            case AMOTION_EVENT_ACTION_MOVE:
            {
                auto evNumPtr = AMotionEvent_getPointerCount(inputEvent);
                for (auto i = 0; i < evNumPtr; i++)
                {
                    int32_t pointerId = AMotionEvent_getPointerId(inputEvent, i);
                    set2(srcsTouchX[pointerId], (float)AMotionEvent_getX(inputEvent, i));
                    set2(srcsTouchY[pointerId], (float)AMotionEvent_getY(inputEvent, i));
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

    void tickInput()
    {
        // prepare for new input: shift all "current" values to "last" values
        for (auto &i : _inputStates)
        {
            i.second.valLast = i.second.val;
        }
    }
} // namespace yourgame_internal_android

namespace yourgame
{
    float input(yourgame::INPUT source)
    {
        auto i = yourgame_internal_android::_inputStates.find(source);
        return (i == yourgame_internal_android::_inputStates.end()) ? 0.0f : (i->second).val;
    }

    float inputDelta(yourgame::INPUT source)
    {
        auto i = yourgame_internal_android::_inputStates.find(source);
        return (i == yourgame_internal_android::_inputStates.end()) ? 0.0f : (i->second).val - (i->second).valLast;
    }
} // namespace yourgame
