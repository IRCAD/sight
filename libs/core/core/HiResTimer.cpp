/************************************************************************
 *
 * Copyright (C) 2009-2022 IRCAD France
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

#include "core/HiResTimer.hpp"

#include <cstdlib>

namespace sight::core
{

HiResTimer::HiResTimer()
{
    this->reset();
}

HiResTimer::~HiResTimer()
= default;

//------------------------------------------------------------------------------

void HiResTimer::start()
{
    m_stopped = false;

    m_cumulTimeInMicroSec = m_endTimeInMicroSec - m_startTimeInMicroSec;
    m_startTimeInMicroSec = core::HiResClock::getTimeInMicroSec();
}

//------------------------------------------------------------------------------

void HiResTimer::stop()
{
    m_stopped = true;

    m_endTimeInMicroSec = core::HiResClock::getTimeInMicroSec();
}

//------------------------------------------------------------------------------

void HiResTimer::reset(core::HiResClock::HiResClockType initial_value)
{
    bool wasStopped = m_stopped;
    if(!wasStopped)
    {
        this->stop();
    }

    m_startTimeInMicroSec = 0.;
    m_endTimeInMicroSec   = 0.;
    m_cumulTimeInMicroSec = initial_value;
    if(!wasStopped)
    {
        this->start();
    }
}

//------------------------------------------------------------------------------

core::HiResClock::HiResClockType HiResTimer::getElapsedTimeInMicroSec()
{
    if(!m_stopped)
    {
        m_endTimeInMicroSec = core::HiResClock::getTimeInMicroSec();
    }

    return m_cumulTimeInMicroSec + (m_endTimeInMicroSec - m_startTimeInMicroSec);
}

//------------------------------------------------------------------------------

core::HiResClock::HiResClockType HiResTimer::getElapsedTimeInMilliSec()
{
    return this->getElapsedTimeInMicroSec() * 0.001;
}

//------------------------------------------------------------------------------

core::HiResClock::HiResClockType HiResTimer::getElapsedTimeInSec()
{
    return this->getElapsedTimeInMicroSec() * 0.000001;
}

} //namespace sight::core
