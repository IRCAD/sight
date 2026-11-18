/************************************************************************
 *
 * Copyright (C) 2009-2026 IRCAD France
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

#include "core/time_stamp.hpp"

#include <chrono>

namespace sight::core
{

//------------------------------------------------------------------------------

/// Milliseconds on a monotonic clock: a period measured on the wall clock follows its corrections (NTP).
static time_stamp::time_stamp_type now_in_milli_sec()
{
    return std::chrono::duration<time_stamp::time_stamp_type, std::milli>(
        std::chrono::steady_clock::now().time_since_epoch()
    ).count();
}

//------------------------------------------------------------------------------

void time_stamp::modified()
{
    m_modified_time = now_in_milli_sec();
}

//------------------------------------------------------------------------------

bool time_stamp::period_expired() const
{
    return (now_in_milli_sec() - this->m_modified_time) > m_life_period;
}

} //namespace sight::core
