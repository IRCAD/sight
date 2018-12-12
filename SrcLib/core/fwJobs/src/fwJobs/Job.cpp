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

#define BOOST_SCOPE_EXIT_CONFIG_USE_LAMBDAS

#include "fwJobs/Job.hpp"

#include "fwJobs/exception/Waiting.hpp"

#include <fwThread/Worker.hpp>
#include <fwThread/Worker.hxx>

#include <boost/scope_exit.hpp>

#include <memory>

namespace fwJobs
{

//------------------------------------------------------------------------------

Job::sptr Job::New(const std::string& name, Job::Task task, const ::fwThread::Worker::sptr& worker)
{
    return std::make_shared<Job>( name, task, worker );
}

//------------------------------------------------------------------------------

Job::Job(const std::string& name, Job::Task task, const ::fwThread::Worker::sptr& worker) :
    IJob(name),
    m_task(task),
    m_worker(worker)
{
    m_totalWorkUnits = 100;
}

//------------------------------------------------------------------------------

IJob::SharedFuture Job::runImpl()
{
    // No need to lock : m_task & m_worker only writable in constructor
    if(m_task)
    {
        // workaround because of vs2010 issue : http://goo.gl/WHEkQ5
        // see IJob.hpp
        auto fCallback = this->finishCallback();

        auto jobTask = [ = ]()
                       {
                           BOOST_SCOPE_EXIT_ALL( = )
                           {
                               fCallback();
                               ::fwCore::mt::WriteLock lock(m_mutex);
                               m_task = nullptr;
                           };

                           m_task(*this);
                       };
        if(m_worker)
        {
            return m_worker->postTask< void >( jobTask );
        }
        else
        {
            jobTask();
        }
    }
    else
    {
        this->finish();
    }

    return ::std::async( []() {} );
}

//------------------------------------------------------------------------------

IJob::SharedFuture Job::cancel()
{
    auto future = this->IJob::cancel();
    ::fwCore::mt::WriteLock lock(m_mutex);
    if (m_task)
    {
        m_task = nullptr;
    }
    return future;
}

//------------------------------------------------------------------------------

Job::ProgressCallback Job::progressCallback()
{
    return [this](std::uint64_t doneWork)
           {
               this->doneWork(doneWork);
           };
}

//------------------------------------------------------------------------------

::fwThread::Worker::sptr Job::getWorker()
{
    // No need to lock : m_worker only writable in contructor
    return m_worker;
}

} //namespace fwJobs
