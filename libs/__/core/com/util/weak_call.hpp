/************************************************************************
 *
 * Copyright (C) 2009-2023 IRCAD France
 * Copyright (C) 2012-2020 IHU Strasbourg
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

#include "core/com/exception/worker_changed.hpp"

#include <core/exceptionmacros.hpp>
#include <core/mt/types.hpp>

#include <functional>
#include <utility>

namespace sight::core::com::util
{

/**
 * @brief Weak functor class.
 * Store a zero-arg-function and a weak pointer on an object. The WeakCall is
 * callable only once.
 * A bad_weak_ptr exception is thrown if the weak_ptr is expired when operator()
 * is called
 * A core::com::exception::WorkerChanged exception is thrown if the target slot
 * worker changed since weakcall request
 * @tparam T Stored object type.
 * @tparam R Stored function return type.
 */
template<typename T, typename R>
struct weak_call
{
    weak_call(const std::shared_ptr<T const>& ptr, std::function<R()> f) :
        m_weak_ptr(ptr),
        m_func(std::move(f))
    {
    }

    weak_call(
        const std::shared_ptr<T const>& weak_ptr,
        std::function<R()> f,
        const std::shared_ptr<core::thread::worker>& m,
        const std::shared_ptr<T const>& shared_ptr
    ) :
        m_weak_ptr(weak_ptr),
        m_shared_ptr(shared_ptr),
        m_func(std::move(f)),
        m_worker(m)
    {
    }

    ~weak_call()
    = default;

    //------------------------------------------------------------------------------

    R operator()() const
    {
        auto ptr = this->m_weak_ptr.lock();

        if(!ptr)
        {
            m_worker.reset();
            // will throw an exception because m_weakPtr is expired
            throw std::bad_weak_ptr();
        }

        core::mt::read_lock lock(ptr->m_worker_mutex);

        auto shared_ptr                              = this->m_shared_ptr;
        std::shared_ptr<core::thread::worker> worker = m_worker.lock();

        if(worker && ptr->m_worker != worker)
        {
            //Worker changed since WeakCall creation
            SIGHT_THROW_EXCEPTION(core::com::exception::worker_changed("Worker changed since WeakCall creation"));
        }

        this->m_weak_ptr.reset();
        this->m_shared_ptr.reset();

        return this->m_func();
    }

    protected:

        // Maintain a pointer to the callee. Allows to not proceed if the callee has been destroyed in between.
        mutable std::weak_ptr<T const> m_weak_ptr;
        // This is used to extend the life of a wrapped slot, which is needed if the signal is disconnected before the
        // call. Indeed, otherwise, a wrapped slot is not owned by anyone else the connected signal.
        mutable std::shared_ptr<T const> m_shared_ptr;
        std::function<R()> m_func;
        mutable std::weak_ptr<core::thread::worker> m_worker;
};

/// Returns weak call from given object and function.
template<typename T, typename R>
weak_call<T, R> weakcall(const std::shared_ptr<T const>& ptr, std::function<R()> f)
{
    return weak_call<T, R>(ptr, f);
}

/// Returns weak call from given object, function and mutex.
template<typename T, typename R>
weak_call<T, R> weakcall(
    const std::shared_ptr<T const>& weak_ptr,
    std::function<R()> f,
    const std::shared_ptr<core::thread::worker>& m,
    const std::shared_ptr<T const>& shared_ptr = nullptr
)
{
    return weak_call<T, R>(weak_ptr, f, m, shared_ptr);
}

} // namespace sight::core::com::util
