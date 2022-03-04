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

#include <array>

namespace sight::data
{

/**
 * @brief   This class define a 3D point.
 */
class DATA_CLASS_API Point : public Object
{
public:

    typedef double PointCoordType;
    typedef std::array<double, 3> PointCoordArrayType;

    SIGHT_DECLARE_CLASS(Point, Object, factory::New<Point>);

    /// Point factory
    DATA_API static Point::sptr New(float x, float y   = 0.f, float z = 0.f);
    DATA_API static Point::sptr New(double x, double y = 0., double z = 0.);
    DATA_API static Point::sptr New(const PointCoordArrayType& coord);
    DATA_API static Point::sptr New(const Point::sptr& p);

    /**
     * @brief Constructor
     * @param key Private construction key
     */
    DATA_API Point(Object::Key key);

    /// Destructor
    DATA_API virtual ~Point();

    /// Defines shallow copy
    DATA_API void shallowCopy(const Object::csptr& _source) override;

    /// @brief get/set point coordinates
    /// @{
    PointCoordArrayType& getCoord();
    const PointCoordArrayType& getCoord() const;
    void setCoord(const PointCoordArrayType& _vCoord);
    /// @}

    DATA_API std::string getLabel() const;

    DATA_API void setLabel(const std::string& _label);

    /// Equality comparison operators
    /// @{
    DATA_API bool operator==(const Point& other) const noexcept;
    DATA_API bool operator!=(const Point& other) const noexcept;
    /// @}

protected:

    /// Defines deep copy
    DATA_API void cachedDeepCopy(const Object::csptr& _source, DeepCopyCacheType& cache) override;

    /// point coordinates
    PointCoordArrayType m_vCoord;
}; // end class Point

//-----------------------------------------------------------------------------

inline Point::PointCoordArrayType& Point::getCoord()
{
    return this->m_vCoord;
}

//-----------------------------------------------------------------------------

inline const Point::PointCoordArrayType& Point::getCoord() const
{
    return this->m_vCoord;
}

//-----------------------------------------------------------------------------

inline void Point::setCoord(const PointCoordArrayType& _vCoord)
{
    this->m_vCoord = _vCoord;
}

//-----------------------------------------------------------------------------

} // end namespace sight::data
