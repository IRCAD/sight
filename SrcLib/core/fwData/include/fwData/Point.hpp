/************************************************************************
 *
 * Copyright (C) 2009-2019 IRCAD France
 * Copyright (C) 2012-2019 IHU Strasbourg
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

#include "fwData/config.hpp"
#include "fwData/factory/new.hpp"
#include "fwData/Object.hpp"

#include <array>

fwCampAutoDeclareDataMacro((fwData)(Point), FWDATA_API);

namespace fwData
{
/**
 * @brief   This class define a 3D point.
 */
class FWDATA_CLASS_API Point : public Object
{

public:

    typedef double PointCoordType;
    typedef std::array<double, 3> PointCoordArrayType;

    fwCoreClassMacro(Point, ::fwData::Object, ::fwData::factory::New< Point >);

    /// Point factory
    FWDATA_API static Point::sptr New(float x, float y = 0.f, float z = 0.f);
    FWDATA_API static Point::sptr New(double x, double y = 0., double z = 0.);
    FWDATA_API static Point::sptr New(const PointCoordArrayType& coord);
    FWDATA_API static Point::sptr New(const Point::sptr& p);

    fwCampMakeFriendDataMacro((fwData)(Point));

    /**
     * @brief Constructor
     * @param key Private construction key
     */
    FWDATA_API Point(::fwData::Object::Key key);

    /// Destructor
    FWDATA_API virtual ~Point();

    /// Defines shallow copy
    FWDATA_API void shallowCopy( const Object::csptr& _source ) override;

    /// Defines deep copy
    FWDATA_API void cachedDeepCopy(const Object::csptr& _source, DeepCopyCacheType& cache) override;

    /// @brief get/set point coordinates
    /// @{
    PointCoordArrayType& getCoord ();
    const PointCoordArrayType& getCoord() const;
    void setCoord(const PointCoordArrayType& _vCoord);
    /// @}

protected:

    /// point coordinates
    PointCoordArrayType m_vCoord;

}; // end class Point

//-----------------------------------------------------------------------------

inline Point::PointCoordArrayType& Point::getCoord ()
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

} // end namespace fwData
