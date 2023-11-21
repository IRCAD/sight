/************************************************************************
 *
 * Copyright (C) 2009-2023 IRCAD France
 * Copyright (C) 2012-2018 IHU Strasbourg
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

#include "core/config.hpp"

namespace sight::core
{

/**
 * @brief This namespace provides several function that returns a system EPOCH time.
 *
 * @deprecated This class is deprecated, use std::chrono instead.
 */
namespace clock
{

/**
 * @brief Type returned by clock Functions
 */
using type = double;

/**
 * @return EPOCH time in microseconds
 */
CORE_API type get_time_in_micro_sec();

/**
 * @return EPOCH time in milliseconds
 */
CORE_API type get_time_in_milli_sec();

/**
 * @return EPOCH time in seconds
 */
CORE_API type get_time_in_sec();

} //namespace clock

} //namespace sight::core
