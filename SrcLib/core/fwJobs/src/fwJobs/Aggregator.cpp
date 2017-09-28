/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "fwJobs/Aggregator.hpp"

#include "fwJobs/exception/Waiting.hpp"
#include "fwJobs/Job.hpp"

#include <fwCore/spyLog.hpp>

#include <fwThread/Worker.hpp>
#include <fwThread/Worker.hxx>

#include <memory>

namespace fwJobs
{

//------------------------------------------------------------------------------

Aggregator::sptr Aggregator::New(const std::string& name)
{
    return std::make_shared<Aggregator>( name );
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
    decltype(m_jobs)jobs;

    {
        ::fwCore::mt::ReadLock lock(m_mutex);
        jobs = m_jobs;
    }

    std::vector< SharedFuture > futures;
    for(const ::fwJobs::IJob::sptr& iJob : jobs)
    {
        futures.push_back(iJob->run());
    }

    auto future = std::async(
        [ = ]() mutable
        {
            std::for_each(futures.begin(), futures.end(), std::mem_fn(&::std::shared_future<void>::wait));

            this->finish();

            // forwards exceptions that might have been thrown
            for( SharedFuture f : futures )
            {
                f.get();
            }
        }
        );

    return std::move(future);
}

//------------------------------------------------------------------------------

void Aggregator::add(const ::fwJobs::IJob::sptr& iJob, double weight)
{
    SLM_ASSERT("iJob shall not be null", iJob);

    SLM_ASSERT("iJob shall not be added to itself", this != iJob.get());

    if(!iJob)
    {
        return;
    }

    ::fwCore::mt::ReadToWriteLock lock(m_mutex);

    SLM_ASSERT("Jobs can't be added when Aggregator is running", m_state == WAITING || m_state == RUNNING);

    const auto normValue = std::uint64_t(weight*100);

    if( m_state == WAITING || m_state == RUNNING )
    {
        m_jobInfo[iJob.get()] = JobInfo( *iJob );
        auto& jobInfo = m_jobInfo[iJob.get()];

        this->setTotalWorkUnitsUpgradeLock( m_totalWorkUnits + (jobInfo.totalWork ? normValue : 0), lock);
        lock.lock();
        // doneWork call after setTotalWorkUnitsUpgradeLock, because
        // doneWork value can be thresholded by setTotalWorkUnitsUpgradeLock
        // call
        jobInfo.lastValue = std::uint64_t(jobInfo.progress() * normValue);
        {
            ::fwCore::mt::UpgradeToWriteLock writeLock(lock);
            m_jobs.push_back(iJob);
        }
        // take care : doneWork unlocks 'lock'
        this->doneWork(jobInfo.lastValue, lock);

        // TODO : add a way to disconnect on aggregator destruction
        iJob->addDoneWorkHook(
            [ =, &jobInfo](IJob& subJob, std::uint64_t oldDoneWork)
            {
                ::fwCore::mt::ReadToWriteLock sublock(m_mutex);

                auto oldInfo = jobInfo;
                jobInfo = Aggregator::JobInfo( subJob );

                jobInfo.lastValue = std::uint64_t(normValue * jobInfo.progress());

                auto doneWork = m_doneWorkUnits + jobInfo.lastValue;
                // minimize numerical uncertainty by substracting in a second time :
                doneWork -= oldInfo.lastValue;

                this->doneWork( static_cast<std::uint64_t>(doneWork), sublock );
            }
            );

        iJob->addTotalWorkUnitsHook(
            [ = ](IJob& subJob, std::uint64_t oldTotalWorkUnits)
            {
                ::fwCore::mt::ReadToWriteLock sublock(m_mutex);

                auto workUnits = m_totalWorkUnits;
                auto newTotalWorkUnits = subJob.getTotalWorkUnits();

                if( oldTotalWorkUnits != newTotalWorkUnits)
                {
                    if( oldTotalWorkUnits && 0 == newTotalWorkUnits)
                    {
                        workUnits -= normValue;
                    }
                    else if( 0 == oldTotalWorkUnits && newTotalWorkUnits)
                    {
                        workUnits += normValue;
                    }
                }

                this->setTotalWorkUnitsUpgradeLock( workUnits, sublock );
            }
            );

        this->addCancelHookNoLock( [iJob]( IJob& /* cancelingJob */ )
            {
                iJob->cancel();
            } );

        auto iJobName = iJob->getName();
        iJobName = iJobName.empty() ? "" : "[" + iJobName + "] ";
        iJob->addLogHook( [ = ]( IJob& /* job */, const std::string& message)
            {
                this->log( iJobName + message);
            } );

        auto iJobLogs = iJob->getLogs();
        std::for_each(iJobLogs.begin(), iJobLogs.end(),
                      [&]( const Logs::value_type& message )
            {
                this->logNoLock(iJobName + message);
            }
                      );
    }

}

//------------------------------------------------------------------------------

Aggregator::IJobSeq Aggregator::getSubJobs()
{
    ::fwCore::mt::ReadLock lock(m_mutex);
    return m_jobs;
}

} //namespace fwJobs
