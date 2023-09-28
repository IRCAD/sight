/************************************************************************
 *
 * Copyright (C) 2009-2023 IRCAD France
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

#include "data/config.hpp"

#include <core/hires_clock.hpp>
#include <core/mt/types.hpp>

#include <data/Object.hpp>

namespace sight::data::timeline
{

/**
 * @brief   This class defines a TimeLine object. Such an object is intended to be managed by a Timeline implementation.
 *          Use Timeline::createObject() to allocate a new Object.
 */
class DATA_CLASS_API Object
{
public:

    /// Constructor
    DATA_API Object(core::hires_clock::type timestamp = 0);

    /// Destructor
    DATA_API virtual ~Object();

    /// Define deep copy
    DATA_API virtual void deep_copy(const Object& _source);

    /// Return timestamp
    [[nodiscard]] core::hires_clock::type getTimestamp() const
    {
        return m_timestamp;
    }

    /// Equality comparison operators
    /// @{
    DATA_API bool operator==(const Object& other) const noexcept;
    DATA_API bool operator!=(const Object& other) const noexcept;
    /// @}

protected:

    /// Timestamp
    core::hires_clock::type m_timestamp;
};

} // namespace sight::data::timeline
