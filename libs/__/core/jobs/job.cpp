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

#define BOOST_SCOPE_EXIT_CONFIG_USE_LAMBDAS

#include "core/jobs/job.hpp"

#include "core/jobs/exception/waiting.hpp"

#include <core/thread/worker.hpp>
#include <core/thread/worker.hxx>

#include <boost/scope_exit.hpp>

#include <memory>

namespace sight::core::jobs
{

//------------------------------------------------------------------------------

job::job(const std::string& name, job::task task, core::thread::worker::sptr worker) :
    base(name),
    m_task(std::move(task)),
    m_worker(std::move(worker))
{
    m_total_work_units = 100;
}

//------------------------------------------------------------------------------

base::shared_future job::run_impl()
{
    task task_copy;
    {
        core::mt::read_to_write_lock lock(m_mutex);

        if(m_task == nullptr)
        {
            core::mt::upgrade_to_write_lock write_lock(lock);
            this->finish_no_lock();
            return std::async([](){});
        }

        task_copy = m_task;
    }

    const auto job_task =
        [task_copy, this]
        {
            task_copy(*this);

            core::mt::write_lock lock(m_mutex);
            this->finish_no_lock();
            m_task = nullptr;
        };

    if(m_worker)
    {
        return m_worker->post_task<void>(job_task);
    }

    job_task();

    return std::async([](){});
}

//------------------------------------------------------------------------------

base::shared_future job::cancel()
{
    auto future = this->base::cancel();

    core::mt::write_lock lock(m_mutex);
    if(m_task)
    {
        m_task = nullptr;
    }

    return future;
}

//------------------------------------------------------------------------------

job::progress_callback_t job::progress_callback()
{
    return [this](std::uint64_t done_work)
           {
               this->done_work(done_work);
           };
}

//------------------------------------------------------------------------------

core::thread::worker::sptr job::get_worker()
{
    // No need to lock : m_worker only writable in constructor
    return m_worker;
}

} //namespace sight::core::jobs
