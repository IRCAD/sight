/************************************************************************
 *
 * Copyright (C) 2009-2023 IRCAD France
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

#include "core/jobs/aggregator.hpp"

#include "core/jobs/exception/waiting.hpp"
#include "core/jobs/job.hpp"

#include <core/spy_log.hpp>
#include <core/thread/worker.hpp>
#include <core/thread/worker.hxx>

#include <memory>

namespace sight::core::jobs
{

//------------------------------------------------------------------------------

aggregator::aggregator()
= default;

//------------------------------------------------------------------------------

aggregator::aggregator(const std::string& name) :
    base(name)
{
}

//------------------------------------------------------------------------------

base::shared_future aggregator::run_impl()
{
    decltype(m_jobs) jobs;

    {
        core::mt::read_lock lock(m_mutex);
        jobs = m_jobs;
    }

    std::vector<shared_future> futures;
    for(const core::jobs::base::sptr& i_job : jobs)
    {
        futures.push_back(i_job->run());
    }

    auto future = std::async(
        [futures, this]() mutable
        {
            std::for_each(futures.begin(), futures.end(), std::mem_fn(&std::shared_future<void>::wait));

            this->finish();

            // forwards exceptions that might have been thrown
            for(const shared_future& f : futures)
            {
                f.get();
            }
        });

    return future;
}

//------------------------------------------------------------------------------

void aggregator::add(const core::jobs::base::sptr& i_job, double weight)
{
    SIGHT_ASSERT("job shall not be null", i_job);

    SIGHT_ASSERT("job shall not be added to itself", this != i_job.get());

    if(!i_job)
    {
        return;
    }

    core::mt::read_to_write_lock lock(m_mutex);

    SIGHT_ASSERT("Jobs can't be added when aggregator is running", m_state == WAITING || m_state == RUNNING);

    const auto norm_value = std::uint64_t(weight * 100);

    if(m_state == WAITING || m_state == RUNNING)
    {
        m_job_info[i_job.get()] = job_info(*i_job);
        auto& job_info = m_job_info[i_job.get()];

        this->set_total_work_units_upgrade_lock(
            m_total_work_units + (job_info.total_work != 0U ? norm_value : 0),
            lock
        );
        lock.lock();
        // done_work call after set_total_work_unitsUpgradeLock, because
        // done_work value can be thresholded by set_total_work_unitsUpgradeLock
        // call
        job_info.last_value = std::uint64_t(job_info.progress() * double(norm_value));
        {
            core::mt::upgrade_to_write_lock write_lock(lock);
            m_jobs.push_back(i_job);
        }
        // take care : done_work unlocks 'lock'
        this->done_work(job_info.last_value, lock);

        // TODO : add a way to disconnect on aggregator destruction
        i_job->add_done_work_hook(
            [norm_value, &job_info, this](base& sub_job, std::uint64_t)
            {
                core::mt::read_to_write_lock sublock(m_mutex);

                auto old_info = job_info;
                job_info      = aggregator::job_info(sub_job);

                job_info.last_value = std::uint64_t(job_info.progress() * double(norm_value));

                auto done_work = m_done_work_units + job_info.last_value;
                // minimize numerical uncertainty by substracting in a second time :
                done_work -= old_info.last_value;

                this->done_work(static_cast<std::uint64_t>(done_work), sublock);
            });

        i_job->add_total_work_units_hook(
            [norm_value, this](base& sub_job, std::uint64_t old_total_work_units)
            {
                core::mt::read_to_write_lock sublock(m_mutex);

                auto work_units           = m_total_work_units;
                auto new_total_work_units = sub_job.get_total_work_units();

                if(old_total_work_units != new_total_work_units)
                {
                    if((old_total_work_units != 0U) && 0 == new_total_work_units)
                    {
                        work_units -= norm_value;
                    }
                    else if(0 == old_total_work_units && (new_total_work_units != 0U))
                    {
                        work_units += norm_value;
                    }
                }

                this->set_total_work_units_upgrade_lock(work_units, sublock);
            });

        this->add_cancel_hook_no_lock(
            [i_job](base& /* cancelingJob */)
            {
                i_job->cancel();
            });

        auto i_job_name = i_job->name();
        i_job_name = i_job_name.empty() ? "" : std::string("[") + i_job_name + "] ";
        i_job->add_log_hook(
            [i_job_name, this](base& /* job */, const std::string& message)
            {
                this->log(i_job_name + message);
            });

        auto i_job_logs = i_job->get_logs();
        std::for_each(
            i_job_logs.begin(),
            i_job_logs.end(),
            [&](const logs::value_type& message)
            {
                this->log_no_lock(i_job_name + message);
            });
    }
}

//------------------------------------------------------------------------------

aggregator::job_seq aggregator::get_sub_jobs()
{
    core::mt::read_lock lock(m_mutex);
    return m_jobs;
}

} //namespace sight::core::jobs
