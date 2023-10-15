/************************************************************************
 *
 * Copyright (C) 2009-2023 IRCAD France
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

#include "data/config.hpp"
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
class DATA_CLASS_API line final : public object
{
public:

    SIGHT_DECLARE_CLASS(line, object);

    DATA_API ~line() noexcept override = default;

    DATA_API void setValue(const point::sptr& _position, const point::sptr& _direction);

    /** @{
     *  @brief get/set point position
     */
    point::sptr& getPosition();
    point::csptr getPosition() const;
    void setPosition(const point::sptr& _position);
    /// @}

    /** @{
     *  @brief get/set point direction
     */
    point::sptr& getDirection();
    const point::sptr& getDirection() const;
    void setDirection(const point::sptr& _direction);
    /// @}

    /// Equality comparison operators
    /// @{
    DATA_API bool operator==(const line& other) const noexcept;
    DATA_API bool operator!=(const line& other) const noexcept;
    /// @}

    /// Defines shallow copy
    /// @throws data::exception if an errors occurs during copy
    /// @param[in] source the source object to copy
    DATA_API void shallow_copy(const object::csptr& source) override;

    /// Defines deep copy
    /// @throws data::exception if an errors occurs during copy
    /// @param source source object to copy
    /// @param cache cache used to deduplicate pointers
    DATA_API void deep_copy(
        const object::csptr& source,
        const std::unique_ptr<deep_copy_cache_t>& cache = std::make_unique<deep_copy_cache_t>()
    ) override;

protected:

    //! Points container
    point::sptr m_position;
    point::sptr m_direction;
}; // class line

//-----------------------------------------------------------------------------

inline point::sptr& line::getPosition()
{
    return this->m_position;
}

//-----------------------------------------------------------------------------

inline point::csptr line::getPosition() const
{
    return this->m_position;
}

//-----------------------------------------------------------------------------

inline void line::setPosition(const point::sptr& _position)
{
    this->m_position = _position;
}

//-----------------------------------------------------------------------------

inline point::sptr& line::getDirection()
{
    return this->m_direction;
}

//-----------------------------------------------------------------------------

inline const point::sptr& line::getDirection() const
{
    return this->m_direction;
}

//-----------------------------------------------------------------------------

inline void line::setDirection(const point::sptr& _direction)
{
    this->m_direction = _direction;
}

//-----------------------------------------------------------------------------

} // namespace sight::data
