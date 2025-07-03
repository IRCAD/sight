/************************************************************************
 *
 * Copyright (C) 2009-2025 IRCAD France
 * Copyright (C) 2012-2019 IHU Strasbourg
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

#include <sight/core/config.hpp>

#include "core/base_object.hpp"
#include "core/clock.hpp"

namespace sight::core
{

/**
 * @brief This class provide a timer (stopwatch).
 * Timer is able to measure the elapsed time with a few micro-seconds.
 * accuracy, on Linux, Mac OS and Windows
 */
class SIGHT_CORE_CLASS_API timer
{
public:

    using sptr = std::shared_ptr<timer>;

    /**
     * @name Constructor/Destructor
     * @{ */
    SIGHT_CORE_API timer();

    /**  @} */

    /**
     * @brief Start the timer.
     */
    SIGHT_CORE_API void start();

    /**
     * @brief Stop the timer.
     * stop() will not reset the timer.
     */
    SIGHT_CORE_API void stop();

    /**
     * @brief Reset the timer and leave it in the same state it was (started or stopped).
     *
     * @param _initial_value Initial value in microseconds from which the timer
     * will start to count.
     */
    SIGHT_CORE_API void reset(clock::type _initial_value = 0.);

    /**
     * @name Elapsed time
     * @brief These methods return the cumulated elapsed running time of the timer
     * the timer is running when start() has been invoked and stop running on
     * stop() or reset() calls.
     * @{ */

    /**
     * @return Elapsed time in seconds
     */
    SIGHT_CORE_API core::clock::type get_elapsed_time_in_sec() const;

    /**
     * @return Elapsed time in milliseconds
     */
    SIGHT_CORE_API core::clock::type get_elapsed_time_in_milli_sec() const;

    /**
     * @return Elapsed time in microseconds
     */
    SIGHT_CORE_API core::clock::type get_elapsed_time_in_micro_sec() const;

/**  @} */

private:

    /**
     * @brief Last start time.
     */
    core::clock::type m_start_time_in_micro_sec {};
    /**
     * @brief Last "break" time.
     */
    core::clock::type m_end_time_in_micro_sec {};
    /**
     * @brief accumulated running time.
     */
    core::clock::type m_cumul_time_in_micro_sec {};
    /**
     * @brief timer status.
     */
    bool m_stopped {true};
};

} //namespace sight::core
