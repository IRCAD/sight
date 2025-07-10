/************************************************************************
 *
 * Copyright (C) 2009-2024 IRCAD France
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

#include "core/thread/timer.hpp"

#ifdef _WIN32
// for timeBeginPeriod and timeEndPeriod
#include <Windows.h>
#endif

namespace sight::core::thread
{

#ifdef _WIN32
/**
 * @brief Set the Windows timer precision.
 *
 * This is absolutely needed to have precise std::this_thread::sleep_for (indeed, all "timed" functions), otherwise the
 * windows scheduler resolution (1/64 seconds or ~15.62 ms) will be used. This would implicitly limit timers frequency
 * to 64 Hz, which can be too low.
 *
 * @note This is a process wide settings
 *
 * @see https://learn.microsoft.com/en-us/windows/win32/api/timeapi/nf-timeapi-timebeginperiod
 */
const static struct timer_precision
{
    timer_precision()
    {
        // Set the timer precision to 1 ms on windows for the current process
        timeBeginPeriod(1);
    }

    ~timer_precision()
    {
        // Restore the timer precision. An application must match each call to timeBeginPeriod and timeEndPeriod
        // The parameter must match the one used for timeBeginPeriod
        timeEndPeriod(1);
    }
} timer_precision;
#endif

} //namespace sight::core::thread
