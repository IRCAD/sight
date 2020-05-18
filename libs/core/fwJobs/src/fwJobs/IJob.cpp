/************************************************************************
 *
 * Copyright (C) 2009-2019 IRCAD France
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

#include "fwJobs/IJob.hpp"

#include "fwJobs/exception/Waiting.hpp"

#include <fwCom/Signal.hpp>
#include <fwCom/Signal.hxx>
#include <fwCom/Signals.hpp>

#include <fwThread/Worker.hpp>
#include <fwThread/Worker.hxx>

#include <algorithm>

namespace fwJobs
{

IJob::IJob(const std::string& name) :
    m_sigCancelRequested(CancelRequestedSignal::New()),
    m_sigCanceled(StateSignal::New()),
    m_sigStarted(StateSignal::New()),
    m_sigFinished(StateSignal::New()),
    m_sigDoneWork(DoneWorkSignal::New()),
    m_sigLogged(LogSignal::New()),
    m_name(name),
    m_doneWorkUnits(0),
    m_totalWorkUnits(0),
    m_cancelRequested(false),
    m_cancelable(true),
    m_state(WAITING)
{
}

//------------------------------------------------------------------------------

IJob::~IJob()
{
    std::for_each(m_stateHooks.begin(), m_stateHooks.end(),
                  [&]( const StateHookSeq::value_type& f )
        {
            f(m_state);
        } );
}

//------------------------------------------------------------------------------

const bool& IJob::cancelRequested() const
{
    // ::fwCore::mt::ReadLock lock(m_mutex);
    return m_cancelRequested;
}

//------------------------------------------------------------------------------

IJob::CancelRequestCallback IJob::cancelRequestedCallback() const
{
    return [this]() -> bool
           {
               ::fwCore::mt::ReadLock lock(m_mutex);
               return m_cancelRequested;
           };
}

//------------------------------------------------------------------------------

std::uint64_t IJob::getDoneWorkUnits() const
{
    ::fwCore::mt::ReadLock lock(m_mutex);
    return m_doneWorkUnits;
}

//------------------------------------------------------------------------------

std::uint64_t IJob::getTotalWorkUnits() const
{
    ::fwCore::mt::ReadLock lock(m_mutex);
    return m_totalWorkUnits;
}

//------------------------------------------------------------------------------

IJob::SharedFuture IJob::cancel()
{
    ::fwCore::mt::ReadToWriteLock lock(m_mutex);
    if( m_cancelable && (m_state == WAITING || m_state == RUNNING) )
    {
        State nextState = (m_state == WAITING) ? CANCELED : CANCELING;

        {
            ::fwCore::mt::UpgradeToWriteLock writeLock(lock);
            m_cancelRequested = true;
            this->setStateNoLock(CANCELING);
        }

        // unlock mutex for cancel callbacks sanity
        lock.unlock();
        // m_cancelHooks can not be changed when m_state != WAITING or
        // RUNNING, no need to lock m_mutex
        for(auto cancel : m_cancelHooks)
        {
            (cancel)(*this);
        }
        lock.lock();

        ::fwCore::mt::UpgradeToWriteLock writeLock(lock);

        SLM_ASSERT("State shall be only CANCELING or CANCELED, not " << m_state,
                   m_state == CANCELED || m_state == CANCELING);

        if(m_state == CANCELING)
        {
            this->setStateNoLock(nextState);

            if(nextState == CANCELED)
            {
                // If we use the default constructor, the future is not valid and we will not be able to wait for it
                // Thus we build a dummy future to get a valid one
                m_runFuture = std::async( []() {} );
            }
        }
    }
    // else if (m_state == CANCELING)
    // NOP
    return m_runFuture;
}

//------------------------------------------------------------------------------

void IJob::addCancelHook(JobCancelHook callback)
{
    ::fwCore::mt::WriteLock lock(m_mutex);
    this->addCancelHookNoLock(callback);
}

//------------------------------------------------------------------------------

void IJob::addSimpleCancelHook(CancelHook callback)
{
    this->addCancelHook(JobCancelHook([ = ](IJob& /*job*/)
        {
            callback();
        }));
}

//------------------------------------------------------------------------------

void IJob::addDoneWorkHook(DoneWorkHook callback)
{
    ::fwCore::mt::WriteLock lock(m_mutex);
    this->addDoneWorkHookNoLock(callback);
}

//------------------------------------------------------------------------------

void IJob::addTotalWorkUnitsHook(TotalWorkUnitsHook callback)
{
    ::fwCore::mt::WriteLock lock(m_mutex);
    this->addTotalWorkUnitsHookNoLock(callback);
}

//------------------------------------------------------------------------------

void IJob::addLogHook(LogHook callback)
{
    ::fwCore::mt::WriteLock lock(m_mutex);
    this->addLogHookNoLock(callback);
}

//------------------------------------------------------------------------------

void IJob::addStateHook(StateHook callback)
{
    ::fwCore::mt::WriteLock lock(m_mutex);
    this->addStateHookNoLock(callback);
}

//------------------------------------------------------------------------------

IJob::State IJob::getState() const
{
    ::fwCore::mt::ReadLock lock(m_mutex);
    return m_state;
}

//------------------------------------------------------------------------------

const std::string& IJob::getName() const
{
    return m_name; //Does not need to lock mutex, the name can't be changed
}

//------------------------------------------------------------------------------

IJob::SharedFuture IJob::run()
{
    ::fwCore::mt::ReadToWriteLock lock(m_mutex);

    if( m_state == WAITING )
    {
        {
            ::fwCore::mt::UpgradeToWriteLock writeLock(lock);
            this->setStateNoLock(RUNNING);
        }

        lock.unlock();
        auto future = this->runImpl();
        lock.lock();

        ::fwCore::mt::UpgradeToWriteLock writeLock(lock);
        m_runFuture = future;
    }

    return m_runFuture;
}

//------------------------------------------------------------------------------

void IJob::setState(IJob::State state)
{
    ::fwCore::mt::WriteLock lock(m_mutex);
    this->setStateNoLock(state);
}

//------------------------------------------------------------------------------

void IJob::setStateNoLock(IJob::State state)
{
    m_state = state;
    switch(state)
    {
        case WAITING:
            break;
        case RUNNING:
            m_sigStarted->asyncEmit();
            break;
        case CANCELING:
            m_sigCancelRequested->asyncEmit();
            break;
        case CANCELED:
            m_sigCanceled->asyncEmit();
            break;
        case FINISHED:
            m_sigFinished->asyncEmit();
            break;
        default:
            SLM_ASSERT("You shall not pass !", 0);
    }

    std::for_each(m_stateHooks.begin(), m_stateHooks.end(),
                  [&]( const StateHookSeq::value_type& f )
        {
            f(m_state);
        } );
}

//------------------------------------------------------------------------------

void IJob::finish()
{
    ::fwCore::mt::WriteLock lock(m_mutex);
    this->finishNoLock();
}

//------------------------------------------------------------------------------

void IJob::finishNoLock()
{
    this->setStateNoLock( ( m_state == CANCELING ) ? CANCELED : FINISHED );
}

//------------------------------------------------------------------------------

std::function< void() > IJob::finishCallback()
{
    return [ = ] { this->finish(); };
}

//------------------------------------------------------------------------------

void IJob::wait()
{
    decltype(m_runFuture)runFuture;

    {
        ::fwCore::mt::ReadLock lock(m_mutex);
        runFuture = m_runFuture;
    }

    // JobTest::APIAndStateTest() crash on macOS if we don't test the validity of the future before calling wait()
    // No std::future_error are raised before the segfault
    if(!runFuture.valid())
    {
        FW_RAISE_EXCEPTION( ::fwJobs::exception::Waiting("Job has not been started") );
    }

    try
    {
        runFuture.wait();
    }
    catch( std::future_error& )
    {
        FW_RAISE_EXCEPTION( ::fwJobs::exception::Waiting("Job has not been started") );
    }
}

//------------------------------------------------------------------------------

void IJob::log(const std::string& message)
{
    ::fwCore::mt::WriteLock lock(m_mutex);
    this->logNoLock(message);
}

//------------------------------------------------------------------------------

void IJob::logNoLock(const std::string& message)
{
    m_logs.push_back(message);

    m_sigLogged->asyncEmit(message);

    std::for_each(m_logHooks.begin(), m_logHooks.end(),
                  [&]( const LogHookSeq::value_type& f )
        {
            f(*this, message);
        } );
}

//------------------------------------------------------------------------------

IJob::State IJob::getStateNoLock() const
{
    return m_state;
}

//------------------------------------------------------------------------------

void IJob::addCancelHookNoLock(JobCancelHook callback)
{
    if(m_state == WAITING || m_state == RUNNING)
    {
        m_cancelHooks.push_back(callback);
    }
}

//------------------------------------------------------------------------------

void IJob::addDoneWorkHookNoLock(DoneWorkHook callback)
{
    if(m_state == WAITING || m_state == RUNNING)
    {
        m_doneWorkHooks.push_back(callback);
    }
}

//------------------------------------------------------------------------------

void IJob::addTotalWorkUnitsHookNoLock(TotalWorkUnitsHook callback)
{
    if(m_state == WAITING || m_state == RUNNING)
    {
        m_totalWorkUnitsHooks.push_back(callback);
    }
}

//------------------------------------------------------------------------------

void IJob::addLogHookNoLock(LogHook callback)
{
    m_logHooks.push_back(callback);
}

//------------------------------------------------------------------------------

void IJob::addStateHookNoLock(StateHook callback)
{
    m_stateHooks.push_back(callback);
}

//------------------------------------------------------------------------------

void IJob::doneWork( std::uint64_t units )
{
    ::fwCore::mt::ReadToWriteLock lock(m_mutex);
    this->doneWork( units, lock );
}

//------------------------------------------------------------------------------

void IJob::doneWork( std::uint64_t units, ::fwCore::mt::ReadToWriteLock& lock )
{
    auto oldDoneWork = m_doneWorkUnits;
    decltype(m_doneWorkHooks)doneWorkHooks;

    if(m_doneWorkUnits == units)
    {
        return;
    }

    doneWorkHooks = m_doneWorkHooks;

    {
        ::fwCore::mt::UpgradeToWriteLock writeLock(lock);
        m_doneWorkUnits = units;
    }

    m_sigDoneWork->asyncEmit(m_doneWorkUnits, m_totalWorkUnits);

    lock.unlock();

    std::for_each(doneWorkHooks.begin(), doneWorkHooks.end(),
                  [&]( const DoneWorkHookSeq::value_type& f )
        {
            f(*this, oldDoneWork);
        } );
}

//------------------------------------------------------------------------------

void IJob::setTotalWorkUnits(std::uint64_t units)
{
    ::fwCore::mt::ReadToWriteLock lock(m_mutex);
    this->setTotalWorkUnitsUpgradeLock( units, lock );
}

//------------------------------------------------------------------------------

void IJob::setTotalWorkUnitsUpgradeLock( std::uint64_t units, ::fwCore::mt::ReadToWriteLock& lock )
{
    auto oldTotalWorkUnits = m_totalWorkUnits;
    decltype(m_totalWorkUnitsHooks)totalWorkUnitsHook;

    totalWorkUnitsHook = m_totalWorkUnitsHooks;

    if(m_doneWorkUnits > units)
    {
        this->doneWork( units, lock );
    }

    {
        ::fwCore::mt::UpgradeToWriteLock writeLock(lock);
        m_totalWorkUnits = units;
    }

    lock.unlock();
    std::for_each(totalWorkUnitsHook.begin(), totalWorkUnitsHook.end(),
                  [&]( const TotalWorkUnitsHookSeq::value_type& f )
        {
            f(*this, oldTotalWorkUnits);
        } );
}

//------------------------------------------------------------------------------

IJob::Logs IJob::getLogs() const
{
    ::fwCore::mt::ReadLock lock(m_mutex);
    return m_logs;
}

//------------------------------------------------------------------------------

void IJob::setCancelable(bool cancel)
{
    ::fwCore::mt::WriteLock lock(m_mutex);
    m_cancelable = cancel;
}

//------------------------------------------------------------------------------

bool IJob::isCancelable() const
{
    ::fwCore::mt::ReadLock lock(m_mutex);
    return m_cancelable;
}

//------------------------------------------------------------------------------

void IJob::done()
{
    this->doneWork(m_totalWorkUnits);
}

//------------------------------------------------------------------------------

} //namespace fwJobs
