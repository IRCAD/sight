/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as 
 * published by the Free Software Foundation.  
 * ****** END LICENSE BLOCK ****** */

#include "fwCore/HiResClock.hpp"


#ifdef WIN32
#include <windows.h>
#else
#include <sys/time.h>
#endif

#include <cstdlib>


namespace fwCore
{

namespace HiResClock
{

HiResClockType getTimeInMicroSec()
{
    HiResClockType TimeInMicroSec = 0;

#ifdef WIN32
    LARGE_INTEGER count;
    LARGE_INTEGER frequency;
    QueryPerformanceFrequency(&frequency);
    QueryPerformanceCounter(&count);

    TimeInMicroSec = count.QuadPart * (1000000.0 / frequency.QuadPart);
#else
    timeval count;
    gettimeofday(&count, NULL);

    TimeInMicroSec = (count.tv_sec * 1000000.0) + count.tv_usec;
#endif

    return TimeInMicroSec;
}

HiResClockType getTimeInMilliSec()
{
    return getTimeInMicroSec() * 0.001;
}


HiResClockType getTimeInSec()
{
    return getTimeInMicroSec() * 0.000001;
}

} //namespace HiResClock

} //namespace fwCore
