/************************************************************************
 *
 * Copyright (C) 2009-2026 IRCAD France
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

#include "core/com/slot_run.hpp"

#include "core/com/exception/no_worker.hpp"
#include "core/com/util/weak_call.hpp"

namespace sight::core::thread
{

class worker;

} // namespace sight::core::thread

namespace sight::core::com
{

template<typename F>
struct slot_call;

/**
 * @brief Slot call interface (call with return value).
 *
 * @tparam R Slot's return type.
 * @tparam A Slot's arguments types.
 */
template<typename R, typename ... A>
struct SIGHT_CORE_CLASS_API slot_call<R(A ...)>: slot_run<void(A ...)>
{
    /**
     * @name Typedefs
     * @{ */
    using signature_type     = R(A ...);
    using self_t             = slot_call<signature_type>;
    using sptr               = std::shared_ptr<self_t>;
    using wptr               = std::weak_ptr<self_t>;
    using shared_future_type = std::shared_future<R>;
    /**  @} */

    slot_call() :
        slot_run<void(A ...)>()
    {
    }

    ~slot_call() override
    = default;

    /**
     * @brief Call the Slot with the given parameters.
     */
    virtual R call(A ...) const = 0;

    /**
     * @brief Call the Slot with the given parameters asynchronously.
     *
     * @param _worker Worker that will run the Slot.
     * @param _args run arguments.
     *
     * @return a shared_future object associated with Slot's call result.
     *
     * @throws NoWorker if given worker is not valid.
     */
    virtual shared_future_type async_call(const sight::sptr<core::thread::worker>& _worker, A ... _args) const;

    /**
     * @brief Call the Slot with the given parameters asynchronously.
     * The execution of this slot will occur on it's own worker.
     * @pre Slot's worker must be set.
     *
     * @return a shared_future object associated with Slot's call result.
     * @throws NoWorker if slot has no worker set.
     */
    virtual shared_future_type async_call(A ... _args) const;

    protected:

        template<typename WEAKCALL>
        static std::shared_future<R> post_weak_call(const sight::sptr<core::thread::worker>& _worker, WEAKCALL _f);

        /**
         * @brief Binds the given parameters to the call method within a R() function.
         *
         * @return a R() function.
         */
        virtual std::function<R()> bind_call(A ... _args) const;
};

//------------------------------------------------------------------------------

template<typename R, typename ... A>
std::function<R()> slot_call<R(A ...)>::bind_call(A ... _args) const
{
    return [this, ... args = _args](){return call(args ...);};
}

//-----------------------------------------------------------------------------

template<typename R, typename ... A>
slot_call<R(A ...)>::shared_future_type slot_call<R(A ...)>::async_call(
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
slot_call<R(A ...)>::shared_future_type slot_call<R(A ...)>::async_call(A ... _args) const
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

    return post_weak_call(
        this->m_worker,
        core::com::util::weakcall(
            std::dynamic_pointer_cast<const slot_base>(this->shared_from_this()),
            this->bind_call(_args ...),
            this->m_worker
        )
    );
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
