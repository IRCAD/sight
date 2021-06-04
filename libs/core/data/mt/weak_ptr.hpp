/************************************************************************
 *
 * Copyright (C) 2021 IRCAD France
 * Copyright (C) 2021 IHU Strasbourg
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

namespace sight::service
{

namespace registry
{

class ObjectService;

}
class IService;

}

namespace sight::activity
{

class IActivityLauncher;

}

namespace sight::data
{

namespace mt
{

template<class DATATYPE>
class locked_ptr;

/**
 * @brief This class holds a non-owning ("weak") reference on a data object.
 *
 * This class main purpose is to be used as class member to avoid continuous lookup in service tables.
 *
 * In a `IService`, the `weak_ptr` member could be initialized in the `starting()` and then 'locked'
 * and used in the `updating()` method. It will spare the lookup, and only lock the data in the `updating()`
 * which may have an impact when `updating()` is called very often.
 *
 * It must be converted to a locked_ptr in order to access the referenced object.
 */
template<class DATATYPE>
class weak_ptr final
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

    /// Default constructors, destructor and assignment operators
    weak_ptr()                           = default;
    weak_ptr(const weak_ptr&)            = default;
    weak_ptr(weak_ptr&&)                 = default;
    weak_ptr& operator=(const weak_ptr&) = default;
    weak_ptr& operator=(weak_ptr&&)      = default;
    ~weak_ptr()                          = default;

    [[nodiscard]] locked_ptr<DATATYPE> lock() const noexcept;

    /// Returns true if the weak pointer have expired
    inline bool expired() const noexcept
    {
        return m_data.expired();
    }

    /// Convenience function that mimic std::dynamic_pointer_cast()
    template<class CASTED_DATATYPE>
    inline weak_ptr<CASTED_DATATYPE> dynamicPointerCast() const noexcept
    {
        return weak_ptr<CASTED_DATATYPE>(std::dynamic_pointer_cast<CASTED_DATATYPE>(m_data.lock()));
    }

private:

    /// @todo remove me when IService and ObjectService will be ready to use lock()
    friend class service::IService;
    friend class service::registry::ObjectService;
    friend class activity::IActivityLauncher;

    /// Convenience getter for weak_ptr
    /// @todo remove me when IService and ObjectService will be ready to use lock()
    inline std::weak_ptr<DATATYPE> getWeak() const noexcept
    {
        return m_data;
    }

    /// Convenience getter shared_ptr
    /// @todo remove me when IService and ObjectService will be ready to use lock()
    inline std::shared_ptr<DATATYPE> getShared() const noexcept
    {
        return m_data.lock();
    }

    /// The data to guard
    std::weak_ptr<DATATYPE> m_data;
};

//-----------------------------------------------------------------------------

template<class DATATYPE>
inline locked_ptr<DATATYPE> weak_ptr<DATATYPE>::lock() const noexcept
{
    return locked_ptr<DATATYPE>(m_data.lock());
}

//-----------------------------------------------------------------------------

} // namespace mt

} // namespace sight::data
