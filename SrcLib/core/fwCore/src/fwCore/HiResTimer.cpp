/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <stdlib.h>

#include "fwCore/HiResTimer.hpp"

namespace fwCore
{

HiResTimer::HiResTimer()
{
    m_stopped = true;
    this->reset();
}

HiResTimer::~HiResTimer()
{
}

void HiResTimer::start()
{
    m_stopped = false;

    m_cumulTimeInMicroSec = m_endTimeInMicroSec - m_startTimeInMicroSec;
    m_startTimeInMicroSec = ::fwCore::HiResClock::getTimeInMicroSec();
}

void HiResTimer::stop()
{
    m_stopped = true;

    m_endTimeInMicroSec = ::fwCore::HiResClock::getTimeInMicroSec();
}

void HiResTimer::reset(::fwCore::HiResClock::HiResClockType initial_value){
    bool wasStopped = m_stopped;
    if (!wasStopped)
    {
        this->stop();
    }
    m_startTimeInMicroSec = 0.;
    m_endTimeInMicroSec   = 0.;
    m_cumulTimeInMicroSec = initial_value;
    if (!wasStopped)
    {
        this->start();
    }
}

::fwCore::HiResClock::HiResClockType HiResTimer::getElapsedTimeInMicroSec()
{
    if(!m_stopped)
        m_endTimeInMicroSec = ::fwCore::HiResClock::getTimeInMicroSec();

    return m_cumulTimeInMicroSec + (m_endTimeInMicroSec - m_startTimeInMicroSec);
}


::fwCore::HiResClock::HiResClockType HiResTimer::getElapsedTimeInMilliSec()
{
    return this->getElapsedTimeInMicroSec() * 0.001;
}


::fwCore::HiResClock::HiResClockType HiResTimer::getElapsedTimeInSec()
{
    return this->getElapsedTimeInMicroSec() * 0.000001;
}


} //namespace fwCore

