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

#if !defined(FWCOM_SLOTCALL_HPP)
#error core/com/slot_call.hpp not included
#endif

#include "core/com/exception/no_worker.hpp"
#include "core/com/slot_run.hxx"
#include "core/com/util/weak_call.hpp"

#include <core/mt/types.hpp>
#include <core/thread/task_handler.hpp>
#include <core/thread/worker.hpp>

#include <future>

namespace sight::core::com
{

//------------------------------------------------------------------------------

template<typename R, typename ... A>
std::function<R()> slot_call<R(A ...)>::bind_call(A ... _args) const
{
    return [ =, this]{return call(_args ...);};
}

//-----------------------------------------------------------------------------

template<typename R, typename ... A>
typename slot_call<R(A ...)>::shared_future_type slot_call<R(A ...)>::async_call(
    const core::thread::worker::sptr& _worker,
    A ... _args
) const
{
    if(!_worker)
    {
        SIGHT_THROW_EXCEPTION(core::com::exception::no_worker("No valid worker."));
    }

    return post_weak_call(
        _worker,
        core::com::util::weakcall(
            std::dynamic_pointer_cast<const slot_base>(this->shared_from_this()),
            this->bind_call(_args ...)
        )
    );
}

//-----------------------------------------------------------------------------

template<typename R, typename ... A>
typename slot_call<R(A ...)>::shared_future_type slot_call<R(A ...)>::async_call(A ... _args) const
{
    core::mt::read_lock lock(this->m_worker_mutex);

    if(!this->m_worker)
    {
        SIGHT_THROW_EXCEPTION(core::com::exception::no_worker("Slot has no worker set."));
    }

    if(const auto& source_slot = this->m_source_slot.lock(); source_slot)
    {
        return post_weak_call(
            this->m_worker,
            core::com::util::weakcall(
                std::dynamic_pointer_cast<const slot_base>(source_slot),
                this->bind_call(_args ...),
                this->m_worker,
                std::dynamic_pointer_cast<const slot_base>(this->shared_from_this())
            )
        );
    }
    else
    {
        return post_weak_call(
            this->m_worker,
            core::com::util::weakcall(
                std::dynamic_pointer_cast<const slot_base>(this->shared_from_this()),
                this->bind_call(_args ...),
                this->m_worker
            )
        );
    }
}

//-----------------------------------------------------------------------------

template<typename R, typename ... A>
template<typename WEAKCALL>
std::shared_future<R> slot_call<R(A ...)>::post_weak_call(const core::thread::worker::sptr& _worker, WEAKCALL _f)
{
    std::packaged_task<R()> task(_f);
    std::future<R> future = task.get_future();

    std::function<void()> f_task = core::thread::move_task_into_function(task);

    _worker->post(f_task);

    return future;
}

} // namespace sight::core::com
