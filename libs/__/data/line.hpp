/************************************************************************
 *
 * Copyright (C) 2009-2024 IRCAD France
 * Copyright (C) 2012-2021 IHU Strasbourg
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

#include "data/factory/new.hpp"
#include "data/object.hpp"
#include "data/point.hpp"

#include <utility>
#include <vector>

namespace sight::data
{

/**
 * @brief This class defines a line defined by two points.
 */
class SIGHT_DATA_CLASS_API line final : public object
{
public:

    SIGHT_DECLARE_CLASS(line, object);

    SIGHT_DATA_API ~line() noexcept override = default;

    SIGHT_DATA_API void set_value(const point::sptr& _position, const point::sptr& _direction);

    /** @{
     *  @brief get/set point position
     */
    point::sptr& get_position();
    point::csptr get_position() const;
    void set_position(const point::sptr& _position);
    /// @}

    /** @{
     *  @brief get/set point direction
     */
    point::sptr& get_direction();
    const point::sptr& get_direction() const;
    void set_direction(const point::sptr& _direction);
    /// @}

    /// Equality comparison operators
    /// @{
    SIGHT_DATA_API bool operator==(const line& _other) const noexcept;
    SIGHT_DATA_API bool operator!=(const line& _other) const noexcept;
    /// @}

    /// Defines shallow copy
    /// @throws data::exception if an errors occurs during copy
    /// @param[in] _source the source object to copy
    SIGHT_DATA_API void shallow_copy(const object::csptr& _source) override;

    /// Defines deep copy
    /// @throws data::exception if an errors occurs during copy
    /// @param _source source object to copy
    /// @param _cache cache used to deduplicate pointers
    SIGHT_DATA_API void deep_copy(
        const object::csptr& _source,
        const std::unique_ptr<deep_copy_cache_t>& _cache = std::make_unique<deep_copy_cache_t>()
    ) override;

protected:

    //! Points container
    point::sptr m_position;
    point::sptr m_direction;
}; // class line

//-----------------------------------------------------------------------------

inline point::sptr& line::get_position()
{
    return this->m_position;
}

//-----------------------------------------------------------------------------

inline point::csptr line::get_position() const
{
    return this->m_position;
}

//-----------------------------------------------------------------------------

inline void line::set_position(const point::sptr& _position)
{
    this->m_position = _position;
}

//-----------------------------------------------------------------------------

inline point::sptr& line::get_direction()
{
    return this->m_direction;
}

//-----------------------------------------------------------------------------

inline const point::sptr& line::get_direction() const
{
    return this->m_direction;
}

//-----------------------------------------------------------------------------

inline void line::set_direction(const point::sptr& _direction)
{
    this->m_direction = _direction;
}

//-----------------------------------------------------------------------------

} // namespace sight::data
