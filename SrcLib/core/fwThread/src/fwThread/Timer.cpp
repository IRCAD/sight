/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2012.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "fwThread/Timer.hpp"

namespace fwThread
{

Timer::Timer(Worker::sptr worker, bool oneShot) :
    m_timer(worker->m_ioService),
    m_duration(::boost::posix_time::seconds(1)),
    m_worker(worker),
    m_oneShot(oneShot),
    m_running(false)
{
}

Timer::~Timer()
{
    this->stop();
}

void Timer::setDuration(TimeDurationType duration)
{
    ::fwCore::mt::ScopedLock lock(m_mutex);
    m_duration = duration;
}

void Timer::start()
{
    ::fwCore::mt::ScopedLock lock(m_mutex);
    this->rearmNoLock(m_duration);
    m_running=true;
}

void Timer::stop()
{
    ::fwCore::mt::ScopedLock lock(m_mutex);
    if (m_running )
    {
        m_running=false;
        this->cancelNoLock();
    }
}

void Timer::rearmNoLock(TimeDurationType duration)
{
    this->cancelNoLock();
    m_timer.expires_from_now(duration);
    m_timer.async_wait( ::boost::bind(&Timer::call, this, _1));
}

void Timer::call(const ::boost::system::error_code & error)
{
    if(!error)
    {
        ::fwCore::mt::ScopedLock lock(m_mutex);
        if (!m_oneShot)
        {
            this->rearmNoLock(m_duration);
            m_function();
        }
        else
        {
            m_function();
            m_running = false;
        }
    }
}

void Timer::cancelNoLock()
{
    m_timer.cancel();
}






} //namespace fwThread
