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

template <class DATATYPE>
class locked_ptr;

/**
 * @brief This class holds a a non-owning ("weak") reference on a data.
 *
 * It must be converted to a locked_ptr in order to access the referenced object.
 */
template <class DATATYPE>
class shared_ptr
{

public:
    /// Constructor
    inline explicit shared_ptr(const std::weak_ptr<DATATYPE>& data) noexcept :
        m_data(data.lock())
    {
    }

    /// Constructor
    inline explicit shared_ptr(const std::shared_ptr<DATATYPE>& data) noexcept :
        m_data(data)
    {
    }

    /// Constructor
    inline explicit shared_ptr(const locked_ptr<DATATYPE>& data) noexcept :
        m_data(data.getShared())
    {
    }

    /// Assignment operator
    inline shared_ptr& operator=(const std::weak_ptr<DATATYPE>& data) noexcept
    {
        m_data = data.lock();
        return *this;
    }

    /// Assignment operator
    inline shared_ptr& operator=(const locked_ptr<DATATYPE>& data) noexcept
    {
        m_data = data.getShared();
        return *this;
    }

    /// Default constructors and assignment operator
    shared_ptr()                             = default;
    shared_ptr(const shared_ptr&)            = default;
    shared_ptr(shared_ptr&&)                 = default;
    shared_ptr& operator=(const shared_ptr&) = default;
    shared_ptr& operator=(shared_ptr&&)      = default;
    virtual ~shared_ptr()
    {
    }

    /// Returns the locked_ptr from the weak pointer
    inline locked_ptr<DATATYPE> lock() const noexcept
    {
        return locked_ptr<DATATYPE>(m_data);
    }

    /// Convenience function that mimic std::dynamic_pointer_cast()
    template< class CASTED_DATATYPE >
    inline shared_ptr< CASTED_DATATYPE > dynamicPointerCast() const noexcept
    {
        return shared_ptr< CASTED_DATATYPE >(std::dynamic_pointer_cast< CASTED_DATATYPE >(m_data));
    }

protected:

    /// @todo remove me when IService and ObjectService will be ready to use lock()
    friend class ::fwServices::IService;
    friend class ::fwServices::registry::ObjectService;

    /// Convenience getter shared_ptr
    /// @todo remove me when IService and ObjectService will be ready to use lock()
    inline std::shared_ptr<DATATYPE> getShared() const noexcept
    {
        return m_data;
    }

private:
    /// The data to guard
    std::shared_ptr<DATATYPE> m_data;
};

} // namespace mt
} // namespace fwData
