/************************************************************************
 *
 * Copyright (C) 2004-2026 IRCAD France
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

#include "core/spy_log.hpp"
#include "core/timer.hpp"

// Define SIGHT_PROFILING_DISABLED before including this header if you need to disable profiling output

namespace sight::core
{

/**
 * @brief This class holds a timer. It displays elapsed time at destruction.
 */
class sight_profile_scope
{
public:

    explicit sight_profile_scope(const char* _label) :
        m_label(_label)
    {
        m_timer.start();
    }

    ~sight_profile_scope()
    {
        m_timer.stop();

        SIGHT_INFO("TIMER : " << m_label << " = " << m_timer.get_elapsed_time_in_milli_sec() << " ms.");
    }

    /// Timer
    core::timer m_timer;
    /// Timer label
    const char* m_label;
};

/**
 * @brief This class holds a timer. It allows to compute the elapsed time between two profiling scopes.
 */
class sight_profile_frame_timer
{
public:

    explicit sight_profile_frame_timer(double _interval) :
        m_interval(_interval)
    {
        m_timer.start();
    }

    //------------------------------------------------------------------------------

    bool tick()
    {
        m_average = m_timer.get_elapsed_time_in_milli_sec() / ++m_count;
        return m_timer.get_elapsed_time_in_milli_sec() >= m_interval * 1000;
    }

    //------------------------------------------------------------------------------

    bool tick(core::clock::type _time)
    {
        m_average = (m_average * m_count + _time) / (m_count + 1);
        ++m_count;
        return m_timer.get_elapsed_time_in_milli_sec() >= m_interval * 1000;
    }

    //------------------------------------------------------------------------------

    void reset()
    {
        m_count   = 0;
        m_average = 0;
        m_timer.reset();
    }

    /// Timer
    core::timer m_timer;
    /// Timer label
    double m_interval;
    /// Actual elapsed time average
    double m_average {0};
    /// Number of calls, used to compute the average
    unsigned int m_count {0};
};

/**
 * @brief This class holds a timer. It displays elapsed time at destruction every N seconds.
 */
class sight_profile_scope_avg
{
public:

    sight_profile_scope_avg(const char* _label, sight_profile_frame_timer& _frame_timer) :
        m_label(_label),
        m_frame_timer(_frame_timer)
    {
        m_timer.start();
    }

    ~sight_profile_scope_avg()
    {
        m_timer.stop();

        if(m_frame_timer.tick(m_timer.get_elapsed_time_in_milli_sec()))
        {
            SIGHT_INFO(
                "TIMER (avg over " << m_frame_timer.m_interval << "s) : "
                << m_label << " = " << m_frame_timer.m_average << " ms."
            )
            m_frame_timer.reset();
        }
    }

    /// Timer
    core::timer m_timer;
    /// Timer label
    const char* m_label;
    /// Timer used to get the elapsed time between two profiling scopes
    sight_profile_frame_timer& m_frame_timer;
};

/**
 * @brief This class is used to compute the elapsed time between two profiling scopes.
 */
class sight_profile_frame
{
public:

    sight_profile_frame(const char* _label, sight_profile_frame_timer& _frame_timer) :
        m_label(_label),
        m_frame_timer(_frame_timer)
    {
        SIGHT_INFO("FRAME : " << m_label << " = " << m_frame_timer.m_timer.get_elapsed_time_in_milli_sec() << " ms.");
        m_frame_timer.reset();
    }

    ~sight_profile_frame()
    = default;

    /// Timer label
    const char* m_label;
    /// Timer used to get the elapsed time between two profiling scopes
    sight_profile_frame_timer& m_frame_timer;
};

/**
 * @brief This class is used to compute the elapsed time between two profiling scopes every N seconds.
 */
class sight_profile_frame_avg
{
public:

    sight_profile_frame_avg(const char* _label, sight_profile_frame_timer& _frame_timer) :
        m_label(_label),
        m_frame_timer(_frame_timer)
    {
        if(m_frame_timer.tick())
        {
            SIGHT_INFO(
                "FRAME (avg over " << m_frame_timer.m_interval << "s) : "
                << m_label << " = " << m_frame_timer.m_average << " ms."
            );
            m_frame_timer.reset();
        }
    }

    ~sight_profile_frame_avg()
    = default;

    /// Timer label
    const char* m_label;
    /// Timer used to get the elapsed time between two profiling scopes
    sight_profile_frame_timer& m_frame_timer;
};

#ifndef SIGHT_PROFILING_DISABLED
// NOLINTBEGIN(cppcoreguidelines-macro-usage)

/// Display the elapsed time inside a code block
#define SIGHT_PROFILE(_label) \
        sight::core::sight_profile_scope BOOST_PP_CAT(profiler, __LINE__)(_label);

/// Display the elapsed time inside a code block, every N seconds
#define SIGHT_PROFILE_AVG(_label, interval) \
        static sight::core::sight_profile_frame_timer BOOST_PP_CAT(frame_timer, __LINE__)(interval); \
        sight::core::sight_profile_scope_avg BOOST_PP_CAT( \
            profiler, \
            __LINE__ \
        )(_label, BOOST_PP_CAT(frame_timer, __LINE__));

/// Display the elapsed time between two calls of a code block
#define SIGHT_PROFILE_FRAME(_label) \
        static sight::core::sight_profile_frame_timer BOOST_PP_CAT(frame_timer, __LINE__)(0); \
        sight::core::sight_profile_frame BOOST_PP_CAT(profiler, __LINE__)(_label, BOOST_PP_CAT(frame_timer, __LINE__));

/// Display the elapsed time between two calls of a code block, every N seconds
#define SIGHT_PROFILE_FRAME_AVG(_label, interval) \
        static sight::core::sight_profile_frame_timer BOOST_PP_CAT(frame_timer, __LINE__)(interval); \
        sight::core::sight_profile_frame_avg BOOST_PP_CAT( \
            profiler, \
            __LINE__ \
        )(_label, BOOST_PP_CAT(frame_timer, __LINE__));
#else // SIGHT_PROFILING_DISABLED
#define SIGHT_PROFILE(_label)
#define SIGHT_PROFILE_AVG(_label, interval)
#define SIGHT_PROFILE_FRAME(_label)
#define SIGHT_PROFILE_FRAME_AVG(_label, interval)
#endif // SIGHT_PROFILING_DISABLED

// NOLINTEND(cppcoreguidelines-macro-usage)

} //namespace sight::core
