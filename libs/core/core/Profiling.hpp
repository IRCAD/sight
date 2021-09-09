/************************************************************************
 *
 * Copyright (C) 2004-2021 IRCAD France
 * Copyright (C) 2012-2015 IHU Strasbourg
 *
 * This file is part of Sight.
 *
 * Sight is free software: you can redistribute it and/or modify it under
 * the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Sight is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with Sight. If not, see <https://www.gnu.org/licenses/>.
 *
 ***********************************************************************/

#pragma once

#include "core/HiResTimer.hpp"
#include "core/spyLog.hpp"

// Define FW_PROFILING_DISABLED before including this header if you need to disable profiling output

namespace sight::core
{

/**
 * @brief This class holds a timer. It displays elapsed time at destruction.
 */
class fwProfileScope
{
public:

    fwProfileScope(const char* label) :
        m_label(label)
    {
        m_timer.start();
    }

    ~fwProfileScope()
    {
        m_timer.stop();

        std::stringstream log;
        log << "TIMER : " << m_label << " = " << m_timer.getElapsedTimeInMilliSec() << " ms.";
        core::log::SpyLogger::getSpyLogger().info(log.str(), __FILE__, __LINE__);
    }

    /// Timer
    core::HiResTimer m_timer;
    /// Timer label
    const char* m_label;
};

/**
 * @brief This class holds a timer. It allows to compute the elapsed time between two profiling scopes.
 */
class fwProfileFrameTimer
{
public:

    fwProfileFrameTimer(double interval) :
        m_interval(interval),
        m_average(0),
        m_count(0)
    {
        m_timer.start();
    }

    //------------------------------------------------------------------------------

    bool tick()
    {
        m_average = m_timer.getElapsedTimeInMilliSec() / ++m_count;
        return m_timer.getElapsedTimeInMilliSec() >= m_interval * 1000;
    }

    //------------------------------------------------------------------------------

    bool tick(core::HiResClock::HiResClockType time)
    {
        m_average = (m_average * m_count + time) / (m_count + 1);
        ++m_count;
        return m_timer.getElapsedTimeInMilliSec() >= m_interval * 1000;
    }

    //------------------------------------------------------------------------------

    void reset()
    {
        m_count   = 0;
        m_average = 0;
        m_timer.reset();
    }

    /// Timer
    core::HiResTimer m_timer;
    /// Timer label
    double m_interval;
    /// Actual elapsed time average
    double m_average;
    /// Number of calls, used to compute the average
    unsigned int m_count;
};

/**
 * @brief This class holds a timer. It displays elapsed time at destruction every N seconds.
 */
class fwProfileScopeAvg
{
public:

    fwProfileScopeAvg(const char* label, fwProfileFrameTimer& frameTimer) :
        m_label(label),
        m_frameTimer(frameTimer)
    {
        m_timer.start();
    }

    ~fwProfileScopeAvg()
    {
        m_timer.stop();

        if(m_frameTimer.tick(m_timer.getElapsedTimeInMilliSec()))
        {
            std::stringstream log;
            log << "TIMER (avg over " << m_frameTimer.m_interval << "s) : " << m_label
            << " = " << m_frameTimer.m_average << " ms.";
            core::log::SpyLogger::getSpyLogger().info(log.str(), __FILE__, __LINE__);
            m_frameTimer.reset();
        }
    }

    /// Timer
    core::HiResTimer m_timer;
    /// Timer label
    const char* m_label;
    /// Timer used to get the elapsed time between two profiling scopes
    fwProfileFrameTimer& m_frameTimer;
};

/**
 * @brief This class is used to compute the elapsed time between two profiling scopes.
 */
class fwProfileFrame
{
public:

    fwProfileFrame(const char* label, fwProfileFrameTimer& frameTimer) :
        m_label(label),
        m_frameTimer(frameTimer)
    {
        std::stringstream log;
        log << "FRAME : " << m_label << " = " << m_frameTimer.m_timer.getElapsedTimeInMilliSec() << " ms.";
        core::log::SpyLogger::getSpyLogger().info(log.str(), __FILE__, __LINE__);
        m_frameTimer.reset();
    }

    ~fwProfileFrame()
    {
    }

    /// Timer label
    const char* m_label;
    /// Timer used to get the elapsed time between two profiling scopes
    fwProfileFrameTimer& m_frameTimer;
};

/**
 * @brief This class is used to compute the elapsed time between two profiling scopes every N seconds.
 */
class fwProfileFrameAvg
{
public:

    fwProfileFrameAvg(const char* label, fwProfileFrameTimer& frameTimer) :
        m_label(label),
        m_frameTimer(frameTimer)
    {
        if(m_frameTimer.tick())
        {
            std::stringstream log;
            log << "FRAME (avg over " << m_frameTimer.m_interval << "s) : " << m_label
            << " = " << m_frameTimer.m_average << " ms.";
            core::log::SpyLogger::getSpyLogger().info(log.str(), __FILE__, __LINE__);
            m_frameTimer.reset();
        }
    }

    ~fwProfileFrameAvg()
    {
    }

    /// Timer label
    const char* m_label;
    /// Timer used to get the elapsed time between two profiling scopes
    fwProfileFrameTimer& m_frameTimer;
};

#ifndef FW_PROFILING_DISABLED
/// Display the elapsed time inside a code block
#define FW_PROFILE(_label) \
    sight::core::fwProfileScope BOOST_PP_CAT(profiler, __LINE__)(_label);

/// Display the elapsed time inside a code block, every N seconds
#define FW_PROFILE_AVG(_label, interval) \
    static sight::core::fwProfileFrameTimer BOOST_PP_CAT(frameTimer, __LINE__)(interval); \
    sight::core::fwProfileScopeAvg BOOST_PP_CAT(profiler, __LINE__)(_label, BOOST_PP_CAT(frameTimer, __LINE__));

/// Display the elapsed time between two calls of a code block
#define FW_PROFILE_FRAME(_label) \
    static sight::core::fwProfileFrameTimer BOOST_PP_CAT(frameTimer, __LINE__)(0); \
    sight::core::fwProfileFrame BOOST_PP_CAT(profiler, __LINE__)(_label, BOOST_PP_CAT(frameTimer, __LINE__));

/// Display the elapsed time between two calls of a code block, every N seconds
#define FW_PROFILE_FRAME_AVG(_label, interval) \
    static sight::core::fwProfileFrameTimer BOOST_PP_CAT(frameTimer, __LINE__)(interval); \
    sight::core::fwProfileFrameAvg BOOST_PP_CAT(profiler, __LINE__)(_label, BOOST_PP_CAT(frameTimer, __LINE__));
#else // FW_PROFILING_DISABLED
#define FW_PROFILE(_label)
#define FW_PROFILE_AVG(_label, interval)
#define FW_PROFILE_FRAME(_label)
#define FW_PROFILE_FRAME_AVG(_label, interval)
#endif // FW_PROFILING_DISABLED

} //namespace sight::core
