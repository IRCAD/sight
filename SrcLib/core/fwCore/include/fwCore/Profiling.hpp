/************************************************************************
 *
 * Copyright (C) 2004-2015 IRCAD France
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

#ifndef __FWCORE_PROFILING_HPP__
#define __FWCORE_PROFILING_HPP__

#include "fwCore/HiResTimer.hpp"
#include "fwCore/spyLog.hpp"

// Define FW_PROFILING_DISABLED before including this header if you need to disable profiling output

namespace fwCore
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
        ::fwCore::log::SpyLogger::getSpyLogger().info(log.str(), __FILE__, __LINE__);
    }

    /// Timer
    ::fwCore::HiResTimer m_timer;
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

    bool tick()
    {
        m_average = m_timer.getElapsedTimeInMilliSec() / ++m_count;
        return m_timer.getElapsedTimeInMilliSec() >= m_interval * 1000;
    }

    bool tick(::fwCore::HiResClock::HiResClockType time)
    {
        m_average = (m_average * m_count + time) / (m_count + 1);
        ++m_count;
        return m_timer.getElapsedTimeInMilliSec() >= m_interval * 1000;
    }

    void reset()
    {
        m_count   = 0;
        m_average = 0;
        m_timer.reset();
    }

    /// Timer
    ::fwCore::HiResTimer m_timer;
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

        if( m_frameTimer.tick(m_timer.getElapsedTimeInMilliSec()) )
        {
            std::stringstream log;
            log << "TIMER (avg over " << m_frameTimer.m_interval << "s) : " << m_label <<
                " = " << m_frameTimer.m_average << " ms.";
            ::fwCore::log::SpyLogger::getSpyLogger().info(log.str(), __FILE__, __LINE__);
            m_frameTimer.reset();
        }
    }

    /// Timer
    ::fwCore::HiResTimer m_timer;
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
        ::fwCore::log::SpyLogger::getSpyLogger().info(log.str(), __FILE__, __LINE__);
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
        if( m_frameTimer.tick() )
        {
            std::stringstream log;
            log << "FRAME (avg over " << m_frameTimer.m_interval << "s) : " << m_label <<
                " = " << m_frameTimer.m_average << " ms.";
            ::fwCore::log::SpyLogger::getSpyLogger().info(log.str(), __FILE__, __LINE__);
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
    ::fwCore::fwProfileScope BOOST_PP_CAT( profiler, __LINE__ ) (_label);

/// Display the elapsed time inside a code block, every N seconds
#define FW_PROFILE_AVG(_label, interval) \
    static ::fwCore::fwProfileFrameTimer BOOST_PP_CAT( frameTimer, __LINE__ ) (interval); \
    ::fwCore::fwProfileScopeAvg BOOST_PP_CAT( profiler, __LINE__ ) (_label, BOOST_PP_CAT( frameTimer, __LINE__ ));

/// Display the elapsed time between two calls of a code block
#define FW_PROFILE_FRAME(_label) \
    static ::fwCore::fwProfileFrameTimer BOOST_PP_CAT( frameTimer, __LINE__ ) (0); \
    ::fwCore::fwProfileFrame BOOST_PP_CAT( profiler, __LINE__ ) (_label, BOOST_PP_CAT( frameTimer, __LINE__ ));

/// Display the elapsed time between two calls of a code block, every N seconds
#define FW_PROFILE_FRAME_AVG(_label, interval) \
    static ::fwCore::fwProfileFrameTimer BOOST_PP_CAT( frameTimer, __LINE__ ) (interval); \
    ::fwCore::fwProfileFrameAvg BOOST_PP_CAT( profiler, __LINE__ ) (_label, BOOST_PP_CAT( frameTimer, __LINE__ ));

#else // FW_PROFILING_DISABLED

#define FW_PROFILE(_label)
#define FW_PROFILE_AVG(_label, interval)
#define FW_PROFILE_FRAME(_label)
#define FW_PROFILE_FRAME_AVG(_label, interval)

#endif // FW_PROFILING_DISABLED

} //namespace fwCore


#endif // __FWCORE_PROFILING_HPP__
