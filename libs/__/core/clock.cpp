/************************************************************************
 *
 * Copyright (C) 2009-2023 IRCAD France
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

#include "core/clock.hpp"

#include <core/spy_log.hpp>

#include <chrono>
#include <cstdlib>

namespace sight::core::clock
{

//------------------------------------------------------------------------------

type get_time_in_micro_sec()
{
    const auto now = std::chrono::system_clock::now();
    const auto res = std::chrono::duration_cast<std::chrono::microseconds>(now.time_since_epoch()).count();
    return static_cast<type>(res);
}

//------------------------------------------------------------------------------

type get_time_in_milli_sec()
{
    const auto now = std::chrono::system_clock::now();
    const auto res = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()).count();
    return static_cast<type>(res);
}

//------------------------------------------------------------------------------

type get_time_in_sec()
{
    const auto now = std::chrono::system_clock::now();
    const auto res = std::chrono::duration_cast<std::chrono::seconds>(now.time_since_epoch()).count();
    return static_cast<type>(res);
}

} // namespace sight::core::clock
