/************************************************************************
 *
 * Copyright (C) 2009-2021 IRCAD France
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

#include "core/jobs/Aggregator.hpp"

#include "core/jobs/exception/Waiting.hpp"
#include "core/jobs/Job.hpp"
#include <core/spyLog.hpp>
#include <core/thread/Worker.hpp>
#include <core/thread/Worker.hxx>

#include <memory>

namespace sight::core::jobs
{

//------------------------------------------------------------------------------

Aggregator::sptr Aggregator::New(const std::string& name)
{
    return std::make_shared<Aggregator>(name);
}

//------------------------------------------------------------------------------

Aggregator::Aggregator()
{
}

//------------------------------------------------------------------------------

Aggregator::Aggregator(const std::string& name) :
    IJob(name)
{
}

//------------------------------------------------------------------------------

IJob::SharedFuture Aggregator::runImpl()
{
    decltype(m_jobs) jobs;

    {
        core::mt::ReadLock lock(m_mutex);
        jobs = m_jobs;
    }

    std::vector<SharedFuture> futures;
    for(const core::jobs::IJob::sptr& iJob : jobs)
    {
        futures.push_back(iJob->run());
    }

    auto future = std::async(
        [ = ]() mutable
        {
            std::for_each(futures.begin(), futures.end(), std::mem_fn(&::std::shared_future<void>::wait));

            this->finish();

            // forwards exceptions that might have been thrown
            for(SharedFuture f : futures)
            {
                f.get();
            }
        });

    return std::move(future);
}

//------------------------------------------------------------------------------

void Aggregator::add(const core::jobs::IJob::sptr& iJob, double weight)
{
    SIGHT_ASSERT("iJob shall not be null", iJob);

    SIGHT_ASSERT("iJob shall not be added to itself", this != iJob.get());

    if(!iJob)
    {
        return;
    }

    core::mt::ReadToWriteLock lock(m_mutex);

    SIGHT_ASSERT("Jobs can't be added when Aggregator is running", m_state == WAITING || m_state == RUNNING);

    const auto normValue = std::uint64_t(weight * 100);

    if(m_state == WAITING || m_state == RUNNING)
    {
        m_jobInfo[iJob.get()] = JobInfo(*iJob);
        auto& jobInfo = m_jobInfo[iJob.get()];

        this->setTotalWorkUnitsUpgradeLock(m_totalWorkUnits + (jobInfo.totalWork ? normValue : 0), lock);
        lock.lock();
        // doneWork call after setTotalWorkUnitsUpgradeLock, because
        // doneWork value can be thresholded by setTotalWorkUnitsUpgradeLock
        // call
        jobInfo.lastValue = std::uint64_t(jobInfo.progress() * normValue);
        {
            core::mt::UpgradeToWriteLock writeLock(lock);
            m_jobs.push_back(iJob);
        }
        // take care : doneWork unlocks 'lock'
        this->doneWork(jobInfo.lastValue, lock);

        // TODO : add a way to disconnect on aggregator destruction
        iJob->addDoneWorkHook(
            [ =, &jobInfo](IJob& subJob, std::uint64_t oldDoneWork)
            {
                core::mt::ReadToWriteLock sublock(m_mutex);

                auto oldInfo = jobInfo;
                jobInfo      = Aggregator::JobInfo(subJob);

                jobInfo.lastValue = std::uint64_t(normValue * jobInfo.progress());

                auto doneWork = m_doneWorkUnits + jobInfo.lastValue;
                // minimize numerical uncertainty by substracting in a second time :
                doneWork -= oldInfo.lastValue;

                this->doneWork(static_cast<std::uint64_t>(doneWork), sublock);
            });

        iJob->addTotalWorkUnitsHook(
            [ = ](IJob& subJob, std::uint64_t oldTotalWorkUnits)
            {
                core::mt::ReadToWriteLock sublock(m_mutex);

                auto workUnits         = m_totalWorkUnits;
                auto newTotalWorkUnits = subJob.getTotalWorkUnits();

                if(oldTotalWorkUnits != newTotalWorkUnits)
                {
                    if(oldTotalWorkUnits && 0 == newTotalWorkUnits)
                    {
                        workUnits -= normValue;
                    }
                    else if(0 == oldTotalWorkUnits && newTotalWorkUnits)
                    {
                        workUnits += normValue;
                    }
                }

                this->setTotalWorkUnitsUpgradeLock(workUnits, sublock);
            });

        this->addCancelHookNoLock(
            [iJob](IJob& /* cancelingJob */)
            {
                iJob->cancel();
            });

        auto iJobName = iJob->getName();
        iJobName = iJobName.empty() ? "" : "[" + iJobName + "] ";
        iJob->addLogHook(
            [ = ](IJob& /* job */, const std::string& message)
            {
                this->log(iJobName + message);
            });

        auto iJobLogs = iJob->getLogs();
        std::for_each(
            iJobLogs.begin(),
            iJobLogs.end(),
            [&](const Logs::value_type& message)
            {
                this->logNoLock(iJobName + message);
            });
    }
}

//------------------------------------------------------------------------------

Aggregator::IJobSeq Aggregator::getSubJobs()
{
    core::mt::ReadLock lock(m_mutex);
    return m_jobs;
}

} //namespace sight::core::jobs
