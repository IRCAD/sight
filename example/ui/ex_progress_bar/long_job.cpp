/************************************************************************
 *
 * Copyright (C) 2024-2026 IRCAD France
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

#include <atomic>

namespace ex_progress_bar
{

static std::atomic_uint64_t job_id {0};

long_job::long_job() noexcept :
    has_monitors(has_signals::signals())
{
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

    const auto real_long_job = this->observe(
        "Long job "
        + std::to_string(job_id.fetch_add(1)),
        m_cancelable
    );

    // Manage the cancellation.
    if(m_cancelable)
    {
        real_long_job->add_cancel_hook(
            [&cancelled]()
            {
                cancelled.store(true);
            });
    }

    for(std::uint64_t i = 1 ; i <= 100 ; ++i)
    {
        if(real_long_job->cancel_requested() || cancelled.load())
        {
            return;
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(50));

        real_long_job->done_work(i);
    }

    real_long_job->done();
}

} // namespace ex_progress_bar.
