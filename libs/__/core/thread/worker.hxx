/************************************************************************
 *
 * Copyright (C) 2009-2023 IRCAD France
 * Copyright (C) 2012-2019 IHU Strasbourg
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

#pragma once

#include <core/thread/task_handler.hpp>

#include <future>

namespace sight::core::thread
{

//------------------------------------------------------------------------------

template<typename R, typename TASK>
std::shared_future<R> worker::post_task(TASK f)
{
    std::packaged_task<R()> task(f);
    std::future<R> future = task.get_future();

    std::function<void()> f_task = core::thread::move_task_into_function(task);

    if(core::thread::get_current_thread_id() == this->get_thread_id())
    {
        f_task();
    }
    else
    {
        this->post(f_task);
    }

    return future;
}

} //namespace sight::core::thread
