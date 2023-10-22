/************************************************************************
 *
 * Copyright (C) 2020-2023 IRCAD France
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

#include <core/memory/buffer_object.hpp>
#include <core/memory/buffered.hpp>
#include <core/mt/types.hpp>

#include <type_traits>

namespace sight::data::mt
{

template<class DATATYPE>
class weak_ptr;

/**
 * @brief This class holds a shared pointer on a data object, and acts as a mutex and a dump lock if the data own a
 * data::array
 */
template<class DATATYPE>
class locked_ptr final
{
using Lock = std::conditional_t<std::is_const_v<DATATYPE>, core::mt::read_lock, core::mt::write_lock>;

public:

    /// Constructor
    explicit constexpr locked_ptr(const std::shared_ptr<DATATYPE>& _data) noexcept :
        m_data(_data)
    {
        if(m_data)
        {
            m_locker = Lock(m_data->get_mutex());

            if constexpr(std::is_base_of_v<core::memory::buffered, DATATYPE>)
            {
                m_dump_locks = m_data->dump_lock();
            }
            else if(const auto& buffered = std::dynamic_pointer_cast<const core::memory::buffered>(m_data); buffered)
            {
                m_dump_locks = buffered->dump_lock();
            }

            if constexpr(!std::is_const<DATATYPE>::value)
            {
                m_data->setModified();
            }
        }
    }

    /// Constructor
    explicit constexpr locked_ptr(const std::weak_ptr<DATATYPE>& _data) noexcept :
        locked_ptr(_data.lock())
    {
    }

    /// Constructor
    explicit constexpr locked_ptr(const weak_ptr<DATATYPE>& _data) noexcept :
        locked_ptr(_data.lock())
    {
    }

    /// Assignment operator
    constexpr locked_ptr& operator=(const std::shared_ptr<DATATYPE>& _data) noexcept
    {
        if(_data != m_data)
        {
            if(_data)
            {
                if constexpr(std::is_base_of_v<core::memory::buffered, DATATYPE>)
                {
                    m_dump_locks = _data->dump_lock();
                }
                else if(const auto& buffered = std::dynamic_pointer_cast<const core::memory::buffered>(_data); buffered)
                {
                    m_dump_locks = buffered->dump_lock();
                }
                else
                {
                    m_dump_locks.clear();
                }

                m_locker = Lock(_data->get_mutex());
            }
            else
            {
                m_dump_locks.clear();
                m_locker.reset();
            }

            m_data = _data;
        }

        return *this;
    }

    /// Assignment operator
    constexpr locked_ptr& operator=(const std::weak_ptr<DATATYPE>& _data) noexcept
    {
        this->operator=(_data.lock());
        return *this;
    }

    /// Assignment operator
    constexpr locked_ptr& operator=(const weak_ptr<DATATYPE>& _data) noexcept
    {
        this->operator=(_data.lock());
        return *this;
    }

    /// Default constructors, destructor and assignment operators
    constexpr locked_ptr()                             = default;
    constexpr locked_ptr(const locked_ptr&)            = default;
    constexpr locked_ptr(locked_ptr&&)                 = default;
    constexpr locked_ptr& operator=(const locked_ptr&) = default;
    constexpr locked_ptr& operator=(locked_ptr&&)      = default;
    inline ~locked_ptr()                               = default;

    /// Returns the internal shared pointer
    [[nodiscard]] constexpr std::shared_ptr<DATATYPE> get_shared() const noexcept
    {
        return m_data;
    }

    /// Returns a pointer to the hold data
    [[nodiscard]] constexpr DATATYPE* get() const noexcept
    {
        return m_data.get();
    }

    /// Returns a reference to the hold data
    constexpr DATATYPE& operator*() const noexcept
    {
        return *(get());
    }

    /// Returns a pointer to the hold data
    constexpr DATATYPE* operator->() const noexcept
    {
        return get();
    }

    /// Checks if *this stores a non-null pointer, i.e. whether get() != nullptr
    constexpr explicit operator bool() const noexcept
    {
        return m_data.operator bool();
    }

    /// Allows to compare pointer address
    constexpr bool operator==(const locked_ptr<DATATYPE>& _data) const noexcept
    {
        return get() == _data.get();
    }

    /// Allows to compare pointer address
    constexpr bool operator!=(const locked_ptr<DATATYPE>& _data) const noexcept
    {
        return get() != _data.get();
    }

    /// Allows to compare pointer address
    constexpr bool operator==(const std::shared_ptr<DATATYPE>& _data) const noexcept
    {
        return get() == _data.get();
    }

    /// Allows to compare pointer address
    constexpr bool operator!=(const std::shared_ptr<DATATYPE>& _data) const noexcept
    {
        return get() != _data.get();
    }

    /// Allows to compare pointer address
    constexpr bool operator==(const DATATYPE* _data) const noexcept
    {
        return get() == _data;
    }

    /// Allows to compare pointer address
    constexpr bool operator!=(const DATATYPE* _data) const noexcept
    {
        return get() != _data;
    }

private:

    /// Data to guard
    std::shared_ptr<DATATYPE> m_data;

    /// Read lock if the data is const, write lock otherwise
    Lock m_locker;

    std::vector<core::memory::buffer_object::lock_t> m_dump_locks;
};

} // namespace sight::data::mt
