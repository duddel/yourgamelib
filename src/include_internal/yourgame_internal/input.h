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
#ifndef YOURGAME_INTERNAL_INPUT_H
#define YOURGAME_INTERNAL_INPUT_H

#include <map>
#include "yourgame/input.h"

namespace yourgame_internal
{
    struct InputValue
    {
        InputValue(float v = 0.0f) : val(v), valLast(v) {} // default ctor
        float val;
        float valLast;
    };

    extern std::map<yourgame::input::Source, InputValue> inputStates;

    /** \brief set input value. used for buttons/keys
    if first input of source occurs, the last (previous) value is set to 0.0,
    resulting in positive value delta, if first input of source is "key down"
    */
    void setInput(yourgame::input::Source source, float value);

    /** \brief set input value. used for positions/axes or other "continuous" signals
    if first input of source occurs, the last (previous) value is set to value,
    resulting in zero value delta after first input.
    */
    void setInput2(yourgame::input::Source source, float value);
} // namespace yourgame_internal

#endif
