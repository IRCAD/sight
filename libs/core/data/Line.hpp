/************************************************************************
 *
 * Copyright (C) 2009-2021 IRCAD France
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

SIGHT_DECLARE_DATA_REFLECTION((sight) (data) (Line));

namespace sight::data
{

/**
 * @brief This class defines a Line defined by two points.
 */
class DATA_CLASS_API Line : public Object
{
public:

    SIGHT_DECLARE_CLASS(Line, data::Object, data::factory::New<Line>);

    /**
     * @brief Constructor
     * @param key Private construction key
     */
    DATA_API Line(data::Object::Key key);

    /// Destructor
    DATA_API virtual ~Line();

    SIGHT_MAKE_FRIEND_REFLECTION((sight) (data) (Line));

    /// Defines shallow copy
    DATA_API void shallowCopy(const Object::csptr& _source) override;

    /// Defines deep copy
    DATA_API void cachedDeepCopy(const Object::csptr& _source, DeepCopyCacheType& cache) override;

    DATA_API void setValue(const data::Point::sptr& _position, const data::Point::sptr& _direction);

    /** @{
     *  @brief get/set point position
     */
    data::Point::sptr& getPosition();
    const data::Point::sptr& getPosition() const;
    void setPosition(const data::Point::sptr& _position);
    /// @}

    /** @{
     *  @brief get/set point direction
     */
    data::Point::sptr& getDirection();
    const data::Point::sptr& getDirection() const;
    void setDirection(const data::Point::sptr& _direction);
    /// @}

protected:

    //! Points container
    data::Point::sptr m_position;
    data::Point::sptr m_direction;
}; // class Line

//-----------------------------------------------------------------------------

inline data::Point::sptr& Line::getPosition()
{
    return this->m_position;
}

//-----------------------------------------------------------------------------

inline const data::Point::sptr& Line::getPosition() const
{
    return this->m_position;
}

//-----------------------------------------------------------------------------

inline void Line::setPosition(const data::Point::sptr& _position)
{
    this->m_position = _position;
}

//-----------------------------------------------------------------------------

inline data::Point::sptr& Line::getDirection()
{
    return this->m_direction;
}

//-----------------------------------------------------------------------------

inline const data::Point::sptr& Line::getDirection() const
{
    return this->m_direction;
}

//-----------------------------------------------------------------------------

inline void Line::setDirection(const data::Point::sptr& _direction)
{
    this->m_direction = _direction;
}

//-----------------------------------------------------------------------------

} // namespace sight::data
