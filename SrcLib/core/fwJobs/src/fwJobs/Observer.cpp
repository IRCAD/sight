/************************************************************************
 *
 * Copyright (C) 2009-2017 IRCAD France
 * Copyright (C) 2012-2017 IHU Strasbourg
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
    return m_finishTask.get_future();
}

} //namespace fwJobs
