/************************************************************************
 *
 * Copyright (C) 2020 IRCAD France
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

#include <fwCore/mt/types.hpp>

#include <fwMemory/BufferObject.hpp>
#include <fwMemory/IBuffered.hpp>

#include <type_traits>

namespace fwData
{
namespace mt
{

template <class DATATYPE>
class weak_ptr;

/**
 * @brief This class holds shared pointer on a data, and act as a mutex lock and a dump lock if relevant
 */
template< class DATATYPE >
class locked_ptr
{
public:

    /// Constructor
    explicit inline locked_ptr(const std::shared_ptr< DATATYPE >& data ) noexcept :
        m_data(data),
        m_locker(data->getMutex()),
        m_BufferLocks(data)
    {
    }

    /// Constructor
    explicit inline locked_ptr(const std::weak_ptr< DATATYPE >& data ) noexcept :
        locked_ptr(data.lock())
    {
    }

    /// Constructor
    explicit inline locked_ptr(const weak_ptr< DATATYPE >& data ) noexcept :
        locked_ptr(data.lock())
    {
    }

    /// Assignment operator
    inline locked_ptr& operator=(const std::shared_ptr< DATATYPE >& data) noexcept
    {
        if( data != m_data )
        {
            m_data = data;
            m_locker.unlock();
            m_BufferLocks = BufferLocks(data);
        }

        return *this;
    }

    /// Assignment operator
    inline locked_ptr& operator=(const std::weak_ptr< DATATYPE >& data) noexcept
    {
        return this->operator=(data.lock());
    }

    /// Assignment operator
    inline locked_ptr& operator=(const weak_ptr< DATATYPE >& data) noexcept
    {
        return this->operator=(data.lock());
    }

    /// Default constructors and assignment operator
    locked_ptr()                             = default;
    locked_ptr(const locked_ptr&)            = default;
    locked_ptr(locked_ptr&&)                 = default;
    locked_ptr& operator=(const locked_ptr&) = default;
    locked_ptr& operator=(locked_ptr&&)      = default;

    /// Default destructor
    virtual ~locked_ptr()
    {
    }

    /// Returns the internal shared pointer
    inline std::shared_ptr< DATATYPE > getShared() const noexcept
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
    std::shared_ptr< DATATYPE > m_data;

    /// Read lock if the data is const, write lock otherwise
    typename std::conditional_t< std::is_const< DATATYPE >::value, ::fwCore::mt::ReadLock,
                                 ::fwCore::mt::WriteLock > m_locker;

    template< class C, class = void >
    class BufferLocks
    {
    };

    template<class C>
    class BufferLocks<C, typename std::enable_if_t< std::is_base_of_v< ::fwMemory::IBuffered, C > > >
    {
    friend locked_ptr;

    inline explicit BufferLocks( const std::shared_ptr< C >& data )
    {
        if(data)
        {
            data->lockBuffer(m_Locks);
        }
    }

    std::vector< ::fwMemory::BufferObject::Lock > m_Locks;
    };

    template<class C>
    class BufferLocks<C, typename std::enable_if_t< !std::is_base_of_v< ::fwMemory::IBuffered, C > > >
    {
    friend locked_ptr;

    inline explicit BufferLocks( const std::shared_ptr< C >& )
    {
    }
    };

    BufferLocks<DATATYPE> m_BufferLocks;
};

} // namespace mt
} // namespace fwData
