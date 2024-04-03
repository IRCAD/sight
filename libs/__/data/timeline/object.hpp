/************************************************************************
 *
 * Copyright (C) 2009-2024 IRCAD France
 * Copyright (C) 2012-2016 IHU Strasbourg
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

#include <sight/data/config.hpp>

#include <core/clock.hpp>
#include <core/mt/types.hpp>

#include <data/object.hpp>

namespace sight::data::timeline
{

/**
 * @brief   This class defines a TimeLine object. Such an object is intended to be managed by a Timeline implementation.
 *          Use Timeline::createObject() to allocate a new object.
 */
class SIGHT_DATA_CLASS_API object
{
public:

    /// Constructor
    SIGHT_DATA_API object(core::clock::type _timestamp = 0);

    /// Destructor
    SIGHT_DATA_API virtual ~object();

    /// Define deep copy
    SIGHT_DATA_API virtual void deep_copy(const object& _source);

    /// Return timestamp
    [[nodiscard]] core::clock::type get_timestamp() const
    {
        return m_timestamp;
    }

    /// Equality comparison operators
    /// @{
    SIGHT_DATA_API bool operator==(const object& _other) const noexcept;
    SIGHT_DATA_API bool operator!=(const object& _other) const noexcept;
    /// @}

protected:

    /// Timestamp
    core::clock::type m_timestamp;
};

} // namespace sight::data::timeline
