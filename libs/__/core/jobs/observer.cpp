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

#include "core/jobs/observer.hpp"

namespace sight::core::jobs
{

//------------------------------------------------------------------------------

observer::observer(const std::string& _name, std::uint64_t _work_units) :
    base(_name)
{
    m_finish_task = packaged_task(
        [this]()
        {
            core::mt::write_lock lock(m_mutex);
            this->finish_no_lock();
        });
    m_total_work_units = _work_units;

    this->add_simple_cancel_hook(
        [this]()
        {
            this->finish();
        });
    this->run();
}

//------------------------------------------------------------------------------

observer::progress_callback_t observer::progress_callback()
{
    return [this](std::uint64_t _done_work)
           {
               this->done_work(_done_work);
           };
}

//------------------------------------------------------------------------------

void observer::finish()
{
    core::mt::read_lock lock(m_mutex);
    if(m_state == RUNNING || m_state == CANCELING)
    {
        lock.unlock();
        m_finish_task();
    }
}

//------------------------------------------------------------------------------

base::shared_future observer::run_impl()
{
    core::mt::read_lock lock(m_mutex);
    return m_finish_task.get_future();
}

} //namespace sight::core::jobs
