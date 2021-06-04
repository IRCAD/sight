/************************************************************************
 *
 * Copyright (C) 2020-2021 IRCAD France
 * Copyright (C) 2020 IHU Strasbourg
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

#include "weak_ptr.hpp"

#include <core/memory/BufferObject.hpp>
#include <core/memory/IBuffered.hpp>
#include <core/mt/types.hpp>

#include <type_traits>

namespace sight::data
{

namespace mt
{

template<class DATATYPE>
class weak_ptr;

/**
 * @brief This class holds a shared pointer on a data object, and acts as a mutex and a dump lock if the data own a
 * data::Array
 */
template<class DATATYPE>
class locked_ptr final
{
public:

    /// Constructor
    explicit inline locked_ptr(const std::shared_ptr<DATATYPE>& data) noexcept :
        m_data(data),
        m_buffer_locks(data)
    {
        if(m_data)
        {
            m_locker = std::conditional_t<std::is_const<DATATYPE>::value, core::mt::ReadLock,
                                          core::mt::WriteLock>(data->getMutex());
        }
    }

    /// Constructor
    explicit inline locked_ptr(const std::weak_ptr<DATATYPE>& data) noexcept :
        locked_ptr(data.lock())
    {
    }

    /// Constructor
    explicit inline locked_ptr(const weak_ptr<DATATYPE>& data) noexcept :
        locked_ptr(data.lock())
    {
    }

    /// Assignment operator
    inline locked_ptr& operator=(const std::shared_ptr<DATATYPE>& data) noexcept
    {
        if(data != m_data)
        {
            if(m_data)
            {
                m_locker.unlock();
            }

            m_data = data;
            if(m_data)
            {
                m_locker = std::conditional_t<std::is_const<DATATYPE>::value, core::mt::ReadLock,
                                              core::mt::WriteLock>(data->getMutex());
            }

            m_buffer_locks = BufferLocks(data);
        }

        return *this;
    }

    /// Assignment operator
    inline locked_ptr& operator=(const std::weak_ptr<DATATYPE>& data) noexcept
    {
        return this->operator=(data.lock());
    }

    /// Assignment operator
    inline locked_ptr& operator=(const weak_ptr<DATATYPE>& data) noexcept
    {
        return this->operator=(data.lock());
    }

    /// Default constructors, destructor and assignment operators
    locked_ptr()                             = default;
    locked_ptr(const locked_ptr&)            = default;
    locked_ptr(locked_ptr&&)                 = default;
    locked_ptr& operator=(const locked_ptr&) = default;
    locked_ptr& operator=(locked_ptr&&)      = default;
    ~locked_ptr()                            = default;

    /// Returns the internal shared pointer
    inline std::shared_ptr<DATATYPE> get_shared() const noexcept
    {
        return m_data;
    }

    /// Returns a pointer to the hold data
    inline DATATYPE* get() const noexcept
    {
        return m_data.get();
    }

    /// Returns a reference to the hold data
    inline DATATYPE& operator*() const noexcept
    {
        return *(this->get());
    }

    /// Returns a pointer to the hold data
    inline DATATYPE* operator->() const noexcept
    {
        return this->get();
    }

    /// Checks if *this stores a non-null pointer, i.e. whether get() != nullptr
    inline explicit operator bool() const noexcept
    {
        return m_data.operator bool();
    }

private:

    /// Data to guard
    std::shared_ptr<DATATYPE> m_data;

    /// Read lock if the data is const, write lock otherwise
    typename std::conditional_t<std::is_const<DATATYPE>::value, core::mt::ReadLock,
                                core::mt::WriteLock> m_locker;

    template<class C, class = void>
    class buffer_locks final
    {
    };

    template<class C>
    class buffer_locks<C, typename std::enable_if_t<std::is_base_of_v<core::memory::IBuffered, C> > > final
    {
    friend locked_ptr;

    inline explicit buffer_locks(const std::shared_ptr<C>& data)
    {
        if(data)
        {
            data->lockBuffer(m_Locks);
        }
    }

    std::vector<core::memory::BufferObject::Lock> m_Locks;
    };

    template<class C>
    class buffer_locks<C, typename std::enable_if_t<!std::is_base_of_v<core::memory::IBuffered, C> > >
    {
    friend locked_ptr;

    inline explicit buffer_locks(const std::shared_ptr<C>&)
    {
    }
    };

    buffer_locks<DATATYPE> m_buffer_locks;
};

} // namespace mt

} // namespace sight::data
