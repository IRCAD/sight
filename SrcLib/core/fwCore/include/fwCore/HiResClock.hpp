/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2018.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

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
