/************************************************************************
 *
 * Copyright (C) 2009-2018 IRCAD France
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

#include "fwCore/config.hpp"

namespace fwCore
{

/**
 * @brief This namespace provides several function that returns a hi
 * resolution EPOCH time.
 *
 * @deprecated This class is deprecated, use std::chrono instead.
 */
namespace HiResClock
{

/**
 * @brief Type returned by HiResClock Functions
 */
typedef double HiResClockType;

/**
 * @return EPOCH time in microseconds
 */
FWCORE_API HiResClockType getTimeInMicroSec();

/**
 * @return EPOCH time in milliseconds
 */
FWCORE_API HiResClockType getTimeInMilliSec();

/**
 * @return EPOCH time in seconds
 */
FWCORE_API HiResClockType getTimeInSec();

} //namespace HiResClock

} //namespace fwCore
