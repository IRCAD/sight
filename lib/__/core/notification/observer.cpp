/************************************************************************
 *
 * Copyright (C) 2009-2026 IRCAD France
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

#include "observer.hpp"

namespace sight::core::notification
{

//------------------------------------------------------------------------------

observer::observer(std::string _name, bool _cancelable, cancel_hook _cancel_hook, std::uint64_t _work_units) :
    monitor(std::move(_name), _cancelable, std::move(_cancel_hook))
{
    m_finish_task = packaged_task(
        [this]()
        {
            core::mt::write_lock lock(mutex());
            if(const auto new_state = finish_no_lock(); new_state)
            {
                lock.unlock();
                this->notify_state(*new_state);
            }
        });
    set_total_work_units(_work_units);

    this->add_cancel_hook(
        [this]()
        {
            this->finish();
        });
    this->run();
}

//------------------------------------------------------------------------------

observer::~observer()
{
    this->finish();
}

//------------------------------------------------------------------------------

void observer::finish()
{
    core::mt::read_lock lock(mutex());
    const auto state = get_state_no_lock();
    if(state == running || state == canceling)
    {
        lock.unlock();
        m_finish_task();
    }
}

//------------------------------------------------------------------------------

} //namespace sight::core::notification
