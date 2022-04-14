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
#include "doctest.h"
#include "yourgame/yourgame.h"

namespace yg = yourgame; // convenience

TEST_CASE("Motion")
{
    SUBCASE("basics")
    {
        yg::util::Motion mot(yg::util::Motion::FlowType::ONCE);

        mot.addRamp(1.5f, 0.0f, 5.0f, yg::util::Motion::SegmentEase::SQUARE)
            .addRamp(1.5f, 0.0f, 5.0f, yg::util::Motion::SegmentEase::SQUAREDOWN)
            .addRamp(1.5f, 0.0f, 5.0f, yg::util::Motion::SegmentEase::SMOOTH)
            .addRamp(1.5f, 0.0f, 5.0f, yg::util::Motion::SegmentEase::SMOOTH2);

        for (int i = 0; i < 100; i++)
        {
            mot.tick(0.1);
            yg::log::info("value: %v", mot.val());
        }
    }
}
