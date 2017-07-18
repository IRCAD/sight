/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "fwJobs/Observer.hpp"

namespace fwJobs
{

//------------------------------------------------------------------------------

Observer::sptr Observer::New(const std::string& name, std::uint64_t workUnits)
{
    return std::make_shared<Observer>( name, workUnits );
}

//------------------------------------------------------------------------------

Observer::Observer(const std::string& name, std::uint64_t workUnits) :
    IJob(name)
{
    m_finishTask = PackagedTask([this]()
        {
            ::fwCore::mt::WriteLock lock(m_mutex);
            this->finishNoLock();
        });
    m_totalWorkUnits = workUnits;

    this->addSimpleCancelHook( [this]()
        {
            this->finish();
        } );
    this->run();
}

//------------------------------------------------------------------------------

Observer::ProgressCallback Observer::progressCallback()
{
    return [this](std::uint64_t doneWork)
           {
               this->doneWork(doneWork);
           };
}

//------------------------------------------------------------------------------

void Observer::finish()
{
    ::fwCore::mt::ReadLock lock(m_mutex);
    if( m_state == RUNNING || m_state == CANCELING )
    {
        lock.unlock();
        m_finishTask();
    }
}

//------------------------------------------------------------------------------

IJob::SharedFuture Observer::runImpl()
{
    ::fwCore::mt::ReadLock lock(m_mutex);
    return std::move(m_finishTask.get_future());
}

} //namespace fwJobs
