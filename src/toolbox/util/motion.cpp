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
#include <cmath>
#include "yourgame/util/motion.h"

namespace yourgame
{
    namespace util
    {
        Motion::Motion(FlowType flowType) : m_flowType(flowType),
                                            m_t(0.0),
                                            m_direction(1.0),
                                            m_duration(0.0),
                                            m_currentValue(0.0f),
                                            m_finished(false),
                                            m_paused(false) {}

        Motion &Motion::addIdle(float dt, float val)
        {
            m_duration += dt;
            m_segments.emplace_back(dt, Motion::calcIdle, getEaseFn(SegmentEase::NONE), val, 0.0f, 0.0f);
            return *this;
        }

        Motion &Motion::addRamp(float dt, float val0, float val1, SegmentEase ease)
        {
            m_duration += dt;
            m_segments.emplace_back(dt, Motion::calcRamp, getEaseFn(ease), val0, val1, 0.0f);
            return *this;
        }

        void Motion::tick(double dt)
        {
            if (m_paused || m_finished)
            {
                return;
            }

            m_t += (m_direction * dt);

            if (m_t >= m_duration)
            {
                switch (m_flowType)
                {
                case FlowType::ONCE:
                    m_t = m_duration;
                    m_finished = true;
                    break;
                case FlowType::REPEAT:
                    m_t -= m_duration;
                    break;
                case FlowType::PINGPONG:
                    m_t = m_duration - (m_t - m_duration);
                    m_direction = -1.0;
                    break;
                }
            }

            if (m_t < 0.0)
            {
                switch (m_flowType)
                {
                case FlowType::ONCE:
                    m_t = 0.0;
                    m_finished = true;
                    break;
                case FlowType::REPEAT:
                    m_t = m_duration + m_t; // assuming direction = -1 -> restart at top
                    break;
                case FlowType::PINGPONG:
                    m_t = -m_t;
                    m_direction = 1.0;
                    break;
                }
            }

            int nSeg = 0;
            float tLocal = 0.0f;
            float tFind = 0.0f;
            for (const auto &seg : m_segments)
            {
                tLocal = m_t - tFind;
                tFind += seg.dt;
                if (m_t < tFind || nSeg >= m_segments.size() - 1)
                {
                    break;
                }
                nSeg++;
            }

            auto &seg = m_segments.at(nSeg);

            tLocal = seg.easeFn(tLocal, seg.dt);

            m_currentValue = seg.calcFn(tLocal, seg);
        }

        float Motion::val()
        {
            return m_currentValue;
        }

        void Motion::pause(bool pause)
        {
            m_paused = pause;
        }

        void Motion::restart()
        {
            m_t = 0.0;
            m_paused = false;
            m_finished = false;
            m_direction = 1.0;
        }

        void Motion::setDirection(bool forward)
        {
            m_direction = (forward ? 1.0 : -1.0);
        }

        bool Motion::isFinished()
        {
            return m_finished;
        }

        bool Motion::isPaused()
        {
            return m_paused;
        }

        bool Motion::getDirection()
        {
            return m_direction > 0.0;
        }

        float Motion::calcIdle(float t, const Segment &segment)
        {
            return segment.val0;
        }

        float Motion::calcRamp(float t, const Segment &segment)
        {
            return segment.val0 + (segment.val1 - segment.val0) * (t / segment.dt);
        }

        std::function<float(float, float)> Motion::getEaseFn(SegmentEase ease)
        {
            switch (ease)
            {
            case SegmentEase::NONE:
                return Motion::calcEaseNone;
            case SegmentEase::SQUARE:
                return Motion::calcEaseSquare;
            case SegmentEase::SQUAREDOWN:
                return Motion::calcEaseSquareDown;
            case SegmentEase::CUBE:
                return Motion::calcEaseCube;
            case SegmentEase::CUBEDOWN:
                return Motion::calcEaseCubeDown;
            case SegmentEase::SINE:
                return Motion::calcEaseSine;
            case SegmentEase::SINEDOWN:
                return Motion::calcEaseSineDown;
            case SegmentEase::SMOOTH:
                return Motion::calcEaseSmooth;
            case SegmentEase::SMOOTH2:
                return Motion::calcEaseSmooth2;
            default:
                return Motion::calcEaseNone;
            }
        }

        float Motion::calcEaseNone(float t, float dt)
        {
            return t;
        }

        float Motion::calcEaseSquare(float t, float dt)
        {
            return t * t / dt;
        }

        float Motion::calcEaseSquareDown(float t, float dt)
        {
            float t01 = t / dt;
            return (2.0f * t01 - (t01 * t01)) * dt;
        }

        float Motion::calcEaseCube(float t, float dt)
        {
            return t * t * t / (dt * dt);
        }

        float Motion::calcEaseCubeDown(float t, float dt)
        {
            float t01 = 1.0f - t / dt;
            return (1.0f - (t01 * t01 * t01)) * dt;
        }

        float Motion::calcEaseSine(float t, float dt)
        {
            return std::sin(t / dt * 1.57079632679f) * dt;
        }

        float Motion::calcEaseSineDown(float t, float dt)
        {
            float t01 = 1.0f - t / dt;
            return (1.0f - std::sin(t01 * 1.57079632679f)) * dt;
        }

        float Motion::calcEaseSmooth(float t, float dt)
        {
            float t01 = t / dt;
            return (3.0f * t01 * t01 - 2.0f * t01 * t01 * t01) * dt;
        }

        float Motion::calcEaseSmooth2(float t, float dt)
        {
            float t01 = t / dt;
            float t01s = (3.0f * t01 * t01 - 2.0f * t01 * t01 * t01);
            return (3.0f * t01s * t01s - 2.0f * t01s * t01s * t01s) * dt;
        }
    }
} // namespace yourgame
