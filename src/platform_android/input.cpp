/*
Copyright (c) 2019-2022 Alexander Scholz

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
#include <android_native_app_glue.h>
#include <android/input.h>
#include <android/keycodes.h>
#include "yourgame/input.h"
#include "yourgame_internal/input.h"

namespace yourgame_internal_android
{
    int32_t handleInputEvent(AInputEvent *inputEvent)
    {
        static const yourgame::input::Source srcsTouchDown[10] = {yourgame::input::TOUCH_0_DOWN,
                                                                  yourgame::input::TOUCH_1_DOWN,
                                                                  yourgame::input::TOUCH_2_DOWN,
                                                                  yourgame::input::TOUCH_3_DOWN,
                                                                  yourgame::input::TOUCH_4_DOWN,
                                                                  yourgame::input::TOUCH_5_DOWN,
                                                                  yourgame::input::TOUCH_6_DOWN,
                                                                  yourgame::input::TOUCH_7_DOWN,
                                                                  yourgame::input::TOUCH_8_DOWN,
                                                                  yourgame::input::TOUCH_9_DOWN};

        static const yourgame::input::Source srcsTouchX[10] = {yourgame::input::TOUCH_0_X,
                                                               yourgame::input::TOUCH_1_X,
                                                               yourgame::input::TOUCH_2_X,
                                                               yourgame::input::TOUCH_3_X,
                                                               yourgame::input::TOUCH_4_X,
                                                               yourgame::input::TOUCH_5_X,
                                                               yourgame::input::TOUCH_6_X,
                                                               yourgame::input::TOUCH_7_X,
                                                               yourgame::input::TOUCH_8_X,
                                                               yourgame::input::TOUCH_9_X};

        static const yourgame::input::Source srcsTouchY[10] = {yourgame::input::TOUCH_0_Y,
                                                               yourgame::input::TOUCH_1_Y,
                                                               yourgame::input::TOUCH_2_Y,
                                                               yourgame::input::TOUCH_3_Y,
                                                               yourgame::input::TOUCH_4_Y,
                                                               yourgame::input::TOUCH_5_Y,
                                                               yourgame::input::TOUCH_6_Y,
                                                               yourgame::input::TOUCH_7_Y,
                                                               yourgame::input::TOUCH_8_Y,
                                                               yourgame::input::TOUCH_9_Y};

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
                    yourgame_internal::setInput(srcsTouchDown[evPointerId], ((evAction == AMOTION_EVENT_ACTION_DOWN ||
                                                                              evAction == AMOTION_EVENT_ACTION_POINTER_DOWN)
                                                                                 ? 1.0f
                                                                                 : 0.0f));
                    // set initial location
                    yourgame_internal::setInput2(srcsTouchX[evPointerId], (float)AMotionEvent_getX(inputEvent, evPointerIndex));
                    yourgame_internal::setInput2(srcsTouchY[evPointerId], (float)AMotionEvent_getY(inputEvent, evPointerIndex));
                }
            }
            break;
            case AMOTION_EVENT_ACTION_MOVE:
            {
                auto evNumPtr = AMotionEvent_getPointerCount(inputEvent);
                for (auto i = 0; i < evNumPtr; i++)
                {
                    int32_t pointerId = AMotionEvent_getPointerId(inputEvent, i);
                    yourgame_internal::setInput2(srcsTouchX[pointerId], (float)AMotionEvent_getX(inputEvent, i));
                    yourgame_internal::setInput2(srcsTouchY[pointerId], (float)AMotionEvent_getY(inputEvent, i));
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
        for (auto &i : yourgame_internal::inputStates)
        {
            i.second.valLast = i.second.val;
        }
    }
} // namespace yourgame_internal_android
