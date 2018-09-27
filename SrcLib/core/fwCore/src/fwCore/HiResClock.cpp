/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2018.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "fwCore/HiResClock.hpp"

#include <chrono>
#include <cstdlib>

namespace fwCore
{

namespace HiResClock
{

//------------------------------------------------------------------------------

HiResClockType getTimeInMicroSec()
{
    const auto now = std::chrono::system_clock::now();
    const auto res = std::chrono::duration_cast< std::chrono::microseconds >(now.time_since_epoch()).count();
    return static_cast<HiResClockType>(res);
}

//------------------------------------------------------------------------------

HiResClockType getTimeInMilliSec()
{
    return getTimeInMicroSec() * 0.001;
}

//------------------------------------------------------------------------------

HiResClockType getTimeInSec()
{
    return getTimeInMicroSec() * 0.000001;
}

} //namespace HiResClock

} //namespace fwCore
