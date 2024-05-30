/************************************************************************
 *
 * Copyright (C) 2024 IRCAD France
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

#include "long_job.hpp"

#include <core/com/signal.hpp>
#include <core/com/signal.hxx>
#include <core/jobs/aggregator.hpp>
#include <core/jobs/job.hpp>

#include <atomic>

namespace ex_progress_bar
{

static std::atomic_uint64_t job_id {0};

long_job::long_job() noexcept
{
    new_signal<signals::job_created_t>(signals::JOB_CREATED);
}

//------------------------------------------------------------------------------

void long_job::starting()
{
}

//------------------------------------------------------------------------------

void long_job::stopping()
{
}

//------------------------------------------------------------------------------

void long_job::configuring()
{
    m_cancelable = this->get_config().get<bool>("config.<xmlattr>.cancellable", m_cancelable);
}

//------------------------------------------------------------------------------

void long_job::updating()
{
    std::atomic_bool cancelled {false};

    const auto real_long_job = std::make_shared<sight::core::jobs::job>(
        "Long job " + std::to_string(job_id.fetch_add(1)),
        [&](sight::core::jobs::job& _running_job)
        {
            for(std::uint64_t i = 1 ; i <= 100 ; ++i)
            {
                if(_running_job.cancel_requested() || cancelled.load())
                {
                    return;
                }

                std::this_thread::sleep_for(std::chrono::milliseconds(50));

                _running_job.done_work(i);
            }

            _running_job.done();
        },
        this->worker()
    );

    real_long_job->set_cancelable(m_cancelable);

    // Manage the cancellation.
    if(m_cancelable)
    {
        real_long_job->add_cancel_hook(
            [&cancelled](sight::core::jobs::base&)
            {
                cancelled.store(true);
            });
    }

    // Emit signal to notify the job creation.
    this->signal<signals::job_created_t>(signals::JOB_CREATED)->emit(real_long_job);

    // Perform the job.
    real_long_job->run().get();
}

} // namespace ex_progress_bar.
