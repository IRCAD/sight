/************************************************************************
 *
 * Copyright (C) 2021-2023 IRCAD France
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

class base;

} // namespace sight::service

namespace sight::data::mt
{

template<class DATATYPE>
class locked_ptr;

/**
 * @brief This class holds a owning ("shared") reference on a data object.
 *
 * Basically, it is a regular `std::shared_ptr`, that requires to lock the data, before using them.
 * Because the reference is not 'weak', the held data will not be deleted as long as this class exists,
 * and, of course, no other reference exists.
 *
 * This is the class to use to hold data that belongs to the current service.
 *
 * It must be converted to a locked_ptr in order to access the referenced object.
 */
template<class DATATYPE>
class shared_ptr
{
public:

    /// Constructor
    inline explicit shared_ptr(const std::weak_ptr<DATATYPE>& _data) noexcept :
        m_data(_data.lock())
    {
    }

    /// Constructor
    inline explicit shared_ptr(const std::shared_ptr<DATATYPE>& _data) noexcept :
        m_data(_data)
    {
    }

    /// Constructor
    inline explicit shared_ptr(const locked_ptr<DATATYPE>& _data) noexcept :
        m_data(_data.get_shared())
    {
    }

    /// Assignment operator
    inline shared_ptr& operator=(const std::weak_ptr<DATATYPE>& _data) noexcept
    {
        m_data = _data.lock();
        return *this;
    }

    /// Assignment operator
    inline shared_ptr& operator=(const locked_ptr<DATATYPE>& _data) noexcept
    {
        m_data = _data.get_shared();
        return *this;
    }

    /// Default constructors, destructor and assignment operators
    shared_ptr()                  = default;
    shared_ptr(const shared_ptr&) = default;
    shared_ptr(shared_ptr&&) noexcept = default;
    shared_ptr& operator=(const shared_ptr&) = default;
    shared_ptr& operator=(shared_ptr&&) noexcept = default;
    ~shared_ptr() = default;

    /// Returns the locked_ptr from the shared pointer
    [[nodiscard]] locked_ptr<DATATYPE> lock() const noexcept;

    /// Resets the pointer to null
    inline void reset() noexcept
    {
        m_data.reset();
    }

    /// Convenience function that mimics std::dynamic_pointer_cast()
    template<class CASTED_DATATYPE>
    inline shared_ptr<CASTED_DATATYPE> dynamicPointerCast() const noexcept
    {
        return shared_ptr<CASTED_DATATYPE>(std::dynamic_pointer_cast<CASTED_DATATYPE>(m_data));
    }

protected:

    /// @todo remove me when base will be ready to use lock()
    friend class service::base;

    /// Convenience getter shared_ptr
    /// @todo remove me when base will be ready to use lock()
    [[nodiscard]] inline std::shared_ptr<DATATYPE> get_shared() const noexcept
    {
        return m_data;
    }

private:

    /// The data to store
    std::shared_ptr<DATATYPE> m_data;
};

//-----------------------------------------------------------------------------

template<class DATATYPE>
inline locked_ptr<DATATYPE> shared_ptr<DATATYPE>::lock() const noexcept
{
    return locked_ptr<DATATYPE>(m_data);
}

} // namespace sight::data::mt
