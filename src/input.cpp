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

#include "yourgame/input.h"
#include "yourgame_internal/input.h"

namespace yourgame_internal
{
    std::map<yourgame::INPUT, InputValue> inputStates;

    void setInput(yourgame::INPUT source, float value)
    {
        inputStates[source].val = value;
    }

    void setInput2(yourgame::INPUT source, float value)
    {
        auto emret = inputStates.emplace(source, value);
        emret.first->second.val = value;
    }
} // namespace yourgame_internal

namespace yourgame
{
    float input(yourgame::INPUT source)
    {
        auto i = yourgame_internal::inputStates.find(source);
        return (i == yourgame_internal::inputStates.end()) ? 0.0f : (i->second).val;
    }

    float inputDelta(yourgame::INPUT source)
    {
        auto i = yourgame_internal::inputStates.find(source);
        return (i == yourgame_internal::inputStates.end()) ? 0.0f : (i->second).val - (i->second).valLast;
    }
} // namespace yourgame
