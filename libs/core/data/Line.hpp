/************************************************************************
 *
 * Copyright (C) 2009-2022 IRCAD France
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
#include "data/Object.hpp"
#include "data/Point.hpp"

#include <utility>
#include <vector>

namespace sight::data
{

/**
 * @brief This class defines a Line defined by two points.
 */
class DATA_CLASS_API Line : public Object
{
public:

    SIGHT_DECLARE_CLASS(Line, Object, factory::New<Line>);

    /**
     * @brief Constructor
     * @param key Private construction key
     */
    DATA_API Line(Object::Key key);

    /// Destructor
    DATA_API virtual ~Line();

    /// Defines shallow copy
    DATA_API void shallowCopy(const Object::csptr& _source) override;

    DATA_API void setValue(const Point::sptr& _position, const Point::sptr& _direction);

    /** @{
     *  @brief get/set point position
     */
    Point::sptr& getPosition();
    Point::csptr getPosition() const;
    void setPosition(const Point::sptr& _position);
    /// @}

    /** @{
     *  @brief get/set point direction
     */
    Point::sptr& getDirection();
    const Point::sptr& getDirection() const;
    void setDirection(const Point::sptr& _direction);
    /// @}

    /// Equality comparison operators
    /// @{
    DATA_API bool operator==(const Line& other) const noexcept;
    DATA_API bool operator!=(const Line& other) const noexcept;
    /// @}

protected:

    /// Defines deep copy
    DATA_API void cachedDeepCopy(const Object::csptr& _source, DeepCopyCacheType& cache) override;

    //! Points container
    Point::sptr m_position;
    Point::sptr m_direction;
}; // class Line

//-----------------------------------------------------------------------------

inline Point::sptr& Line::getPosition()
{
    return this->m_position;
}

//-----------------------------------------------------------------------------

inline Point::csptr Line::getPosition() const
{
    return this->m_position;
}

//-----------------------------------------------------------------------------

inline void Line::setPosition(const Point::sptr& _position)
{
    this->m_position = _position;
}

//-----------------------------------------------------------------------------

inline Point::sptr& Line::getDirection()
{
    return this->m_direction;
}

//-----------------------------------------------------------------------------

inline const Point::sptr& Line::getDirection() const
{
    return this->m_direction;
}

//-----------------------------------------------------------------------------

inline void Line::setDirection(const Point::sptr& _direction)
{
    this->m_direction = _direction;
}

//-----------------------------------------------------------------------------

} // namespace sight::data
