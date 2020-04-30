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

#include <memory>

namespace fwServices
{
namespace registry
{
class ObjectService;
}
class IService;
}

namespace fwActivities
{
class IActivityLauncher;
}

namespace fwData
{
namespace mt
{

template< class DATATYPE >
class locked_ptr;

/**
 * @brief This class holds a a non-owning ("weak") reference on a data.
 *
 * It must be converted to a locked_ptr in order to access the referenced object.
 */
template <class DATATYPE>
class weak_ptr
{

public:
    /// Constructor
    inline explicit weak_ptr(const std::weak_ptr<DATATYPE>& data) noexcept :
        m_data(data)
    {
    }

    /// Constructor
    inline explicit weak_ptr(const std::shared_ptr<DATATYPE>& data) noexcept :
        m_data(data)
    {
    }

    /// Constructor
    inline explicit weak_ptr(const locked_ptr<DATATYPE>& data) noexcept :
        m_data(data.getShared())
    {
    }

    /// Assignment operator
    inline weak_ptr& operator=(const std::weak_ptr<DATATYPE>& data) noexcept
    {
        m_data = data;
        return *this;
    }

    /// Assignment operator
    inline weak_ptr& operator=(const locked_ptr<DATATYPE>& data) noexcept
    {
        m_data = data.getShared();
        return *this;
    }

    /// Default constructors and assignment operator
    weak_ptr()                           = default;
    weak_ptr(const weak_ptr&)            = default;
    weak_ptr(weak_ptr&&)                 = default;
    weak_ptr& operator=(const weak_ptr&) = default;
    weak_ptr& operator=(weak_ptr&&)      = default;
    virtual ~weak_ptr()
    {
    }

    /// Returns the locked_ptr from the weak pointer
    inline locked_ptr<DATATYPE> lock() const noexcept
    {
        return locked_ptr<DATATYPE>(m_data.lock());
    }

    /// Returns true if the weak pointer have expired
    inline bool expired() const noexcept
    {
        return m_data.expired();
    }

    /// Convenience function that mimic std::dynamic_pointer_cast()
    template< class CASTED_DATATYPE >
    inline weak_ptr< CASTED_DATATYPE > dynamicPointerCast() const noexcept
    {
        return weak_ptr< CASTED_DATATYPE >(std::dynamic_pointer_cast< CASTED_DATATYPE >(m_data.lock()));
    }

protected:

    /// @todo remove me when IService and ObjectService will be ready to use lock()
    friend class ::fwServices::IService;
    friend class ::fwServices::registry::ObjectService;
    friend class ::fwActivities::IActivityLauncher;

    /// Convenience getter for weak_ptr
    /// @todo remove me when IService and ObjectService will be ready to use lock()
    inline std::weak_ptr< DATATYPE > getWeak() const noexcept
    {
        return m_data;
    }

    /// Convenience getter shared_ptr
    /// @todo remove me when IService and ObjectService will be ready to use lock()
    inline std::shared_ptr< DATATYPE > getShared() const noexcept
    {
        return m_data.lock();
    }

private:
    /// The data to guard
    std::weak_ptr< DATATYPE > m_data;
};

} // namespace mt
} // namespace fwData
