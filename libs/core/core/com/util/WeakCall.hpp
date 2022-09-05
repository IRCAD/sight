/************************************************************************
 *
 * Copyright (C) 2009-2022 IRCAD France
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

#include "core/com/exception/WorkerChanged.hpp"

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
struct WeakCall
{
    WeakCall(const std::shared_ptr<T const>& ptr, std::function<R()> f) :
        m_weakPtr(ptr),
        m_func(std::move(f))
    {
    }

    WeakCall(
        const std::shared_ptr<T const>& ptr,
        std::function<R()> f,
        const std::shared_ptr<core::thread::Worker>& m
    ) :
        m_weakPtr(ptr),
        m_func(std::move(f)),
        m_worker(m)
    {
    }

    ~WeakCall()
    = default;

    //------------------------------------------------------------------------------

    R operator()() const
    {
        std::shared_ptr<T const> ptr(this->m_weakPtr.lock());

        if(!ptr)
        {
            m_worker.reset();
            // will throw an exception because m_weakPtr is expired
            std::shared_ptr<T const> errorPtr(this->m_weakPtr);
            SIGHT_NOT_USED(errorPtr);
        }

        core::mt::ReadLock lock(ptr->m_workerMutex);

        std::shared_ptr<core::thread::Worker> worker = m_worker.lock();

        if(worker && ptr->m_worker != worker)
        {
            //Worker changed since WeakCall creation
            SIGHT_THROW_EXCEPTION(core::com::exception::WorkerChanged("Worker changed since WeakCall creation"));
        }

        this->m_weakPtr.reset();

        return this->m_func();
    }

    protected:

        mutable std::weak_ptr<T const> m_weakPtr;
        std::function<R()> m_func;
        mutable std::weak_ptr<core::thread::Worker> m_worker;
};

/// Returns weak call from given object and function.
template<typename T, typename R>
WeakCall<T, R> weakcall(const std::shared_ptr<T const>& ptr, std::function<R()> f)
{
    return WeakCall<T, R>(ptr, f);
}

/// Returns weak call from given object, function and mutex.
template<typename T, typename R>
WeakCall<T, R> weakcall(
    const std::shared_ptr<T const>& ptr,
    std::function<R()> f,
    const std::shared_ptr<core::thread::Worker>& m
)
{
    return WeakCall<T, R>(ptr, f, m);
}

} // namespace sight::core::com::util
