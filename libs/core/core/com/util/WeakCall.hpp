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
        const std::shared_ptr<T const>& weakPtr,
        std::function<R()> f,
        const std::shared_ptr<core::thread::Worker>& m,
        const std::shared_ptr<T const>& sharedPtr
    ) :
        m_weakPtr(weakPtr),
        m_sharedPtr(sharedPtr),
        m_func(std::move(f)),
        m_worker(m)
    {
    }

    ~WeakCall()
    = default;

    //------------------------------------------------------------------------------

    R operator()() const
    {
        auto ptr = this->m_weakPtr.lock();

        if(!ptr)
        {
            m_worker.reset();
            // will throw an exception because m_weakPtr is expired
            throw std::bad_weak_ptr();
        }

        core::mt::ReadLock lock(ptr->m_workerMutex);

        auto sharedPtr                               = this->m_sharedPtr;
        std::shared_ptr<core::thread::Worker> worker = m_worker.lock();

        if(worker && ptr->m_worker != worker)
        {
            //Worker changed since WeakCall creation
            SIGHT_THROW_EXCEPTION(core::com::exception::WorkerChanged("Worker changed since WeakCall creation"));
        }

        this->m_weakPtr.reset();
        this->m_sharedPtr.reset();

        return this->m_func();
    }

    protected:

        // Maintain a pointer to the callee. Allows to not proceed if the callee has been destroyed in between.
        mutable std::weak_ptr<T const> m_weakPtr;
        // This is used to extend the life of a wrapped slot, which is needed if the signal is disconnected before the
        // call. Indeed, otherwise, a wrapped slot is not owned by anyone else the connected signal.
        mutable std::shared_ptr<T const> m_sharedPtr;
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
    const std::shared_ptr<T const>& weakPtr,
    std::function<R()> f,
    const std::shared_ptr<core::thread::Worker>& m,
    const std::shared_ptr<T const>& sharedPtr = nullptr
)
{
    return WeakCall<T, R>(weakPtr, f, m, sharedPtr);
}

} // namespace sight::core::com::util
