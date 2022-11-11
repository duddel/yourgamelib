/*
Copyright (c) 2019-2023 Alexander Scholz

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
#ifndef YOURGAME_MOTION_H
#define YOURGAME_MOTION_H

#include <functional>
#include <vector>

namespace yourgame
{
    namespace util
    {
        class Motion
        {
        public:
            enum class FlowType
            {
                ONCE,
                REPEAT,
                PINGPONG
            };

            enum class SegmentEase
            {
                NONE,
                SQUARE,
                SQUAREDOWN,
                CUBE,
                CUBEDOWN,
                SINE,
                SINEDOWN,
                SMOOTH,
                SMOOTH2
            };

            Motion(FlowType flowType);
            Motion &addIdle(float dt, float val);
            Motion &addRamp(float dt, float val0, float val1, SegmentEase ease);

            void tick(double dt);
            float val();

            void pause(bool pause);
            void restart();
            void setDirection(bool forward);

            bool isFinished();
            bool isPaused();
            bool getDirection();

        private:
            struct Segment
            {
                Segment(float dt,
                        std::function<float(float, const Segment &)> calcFn,
                        std::function<float(float, float)> easeFn,
                        float val0,
                        float val1,
                        float val2) : dt(dt), calcFn(calcFn), easeFn(easeFn), val0(val0), val1(val1), val2(val2) {}

                float dt;
                std::function<float(float, const Segment &)> calcFn;
                std::function<float(float, float)> easeFn;
                float val0;
                float val1;
                float val2;
            };

            std::vector<Segment> m_segments;
            FlowType m_flowType;
            double m_t;
            double m_direction;
            double m_duration;
            float m_currentValue;
            bool m_finished;
            bool m_paused;

            static float calcIdle(float t, const Segment &segment);
            static float calcRamp(float t, const Segment &segment);
            static std::function<float(float, float)> getEaseFn(SegmentEase ease);
            static float calcEaseNone(float t, float dt);
            static float calcEaseSquare(float t, float dt);
            static float calcEaseSquareDown(float t, float dt);
            static float calcEaseCube(float t, float dt);
            static float calcEaseCubeDown(float t, float dt);
            static float calcEaseSine(float t, float dt);
            static float calcEaseSineDown(float t, float dt);
            static float calcEaseSmooth(float t, float dt);
            static float calcEaseSmooth2(float t, float dt);
        };
    } // namespace yourgame
}

#endif
