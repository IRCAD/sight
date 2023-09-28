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

#if !defined(__FWCOM_SLOTRUN_HPP__)
#error core/com/slot_run.hpp not included
#endif

#include "core/com/exception/no_worker.hpp"
#include "core/com/slot_base.hxx"
#include "core/com/util/weak_call.hpp"

#include <core/mt/types.hpp>
#include <core/thread/task_handler.hpp>
#include <core/thread/worker.hpp>

#include <future>

namespace sight::core::com
{

//------------------------------------------------------------------------------

template<typename ... A>
inline std::function<void()> slot_run<void(A ...)>::bind_run(A ... args) const
{
    return [ =, this]{run(args ...);};
}

//-----------------------------------------------------------------------------

template<typename ... A>
inline slot_base::void_shared_future_type slot_run<void(A ...)>::async_run(
    const core::thread::worker::sptr& worker,
    A ... args
) const
{
    if(!worker)
    {
        SIGHT_THROW_EXCEPTION(core::com::exception::no_worker("No valid worker."));
    }

    return post_weak_call<void>(
        worker,
        core::com::util::weakcall(
            std::dynamic_pointer_cast<const slot_base>(this->shared_from_this()),
            this->bind_run(args ...)
        )
    );
}

//-----------------------------------------------------------------------------

template<typename ... A>
inline slot_base::void_shared_future_type slot_run<void(A ...)>::async_run(A ... args) const
{
    core::mt::read_lock lock(this->m_worker_mutex);

    if(!this->m_worker)
    {
        SIGHT_THROW_EXCEPTION(core::com::exception::no_worker("Slot has no worker set."));
    }

    if(const auto& source_slot = this->m_source_slot.lock(); source_slot)
    {
        return post_weak_call<void>(
            m_worker,
            core::com::util::weakcall(
                std::dynamic_pointer_cast<const slot_base>(source_slot),
                this->bind_run(args ...),
                this->m_worker,
                std::dynamic_pointer_cast<const slot_base>(this->shared_from_this())
            )
        );
    }
    else
    {
        return post_weak_call<void>(
            m_worker,
            core::com::util::weakcall(
                std::dynamic_pointer_cast<const slot_base>(this->shared_from_this()),
                this->bind_run(args ...),
                this->m_worker
            )
        );
    }
}

//-----------------------------------------------------------------------------

// Copied from core::thread::worker because of issues with gcc 4.2 and template
// keyword
template<typename ... A>
template<typename R, typename WEAKCALL>
std::shared_future<R> slot_run<void(A ...)>::post_weak_call(const core::thread::worker::sptr& worker, WEAKCALL f)
{
    std::packaged_task<R()> task(f);
    std::future<R> future = task.get_future();

    std::function<void()> f_task = core::thread::move_task_into_function(task);

    worker->post(f_task);

    return future;
}

//-----------------------------------------------------------------------------

} // namespace sight::core::com
