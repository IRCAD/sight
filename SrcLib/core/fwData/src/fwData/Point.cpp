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

#include "fwData/Point.hpp"

#include "fwData/Exception.hpp"
#include "fwData/registry/macros.hpp"

#include <fwCore/base.hpp>

fwDataRegisterMacro( ::fwData::Point );

namespace fwData
{

//------------------------------------------------------------------------------

Point::Point(::fwData::Object::Key)
{
    m_vCoord[0] = 0.0;
    m_vCoord[1] = 0.0;
    m_vCoord[2] = 0.0;
}

//------------------------------------------------------------------------------

Point::sptr Point::New(float x, float y, float z)
{
    Point::sptr point = ::fwData::Point::New();
    point->m_vCoord[0] = x;
    point->m_vCoord[1] = y;
    point->m_vCoord[2] = z;
    return point;
}

//------------------------------------------------------------------------------

Point::sptr Point::New(double x, double y, double z)
{
    Point::sptr point = ::fwData::Point::New();
    point->m_vCoord[0] = x;
    point->m_vCoord[1] = y;
    point->m_vCoord[2] = z;
    return point;
}

//------------------------------------------------------------------------------

Point::sptr Point::New(const PointCoordArrayType& coord)
{
    Point::sptr point = ::fwData::Point::New();
    point->m_vCoord = coord;
    return point;
}

//------------------------------------------------------------------------------

Point::sptr Point::New(const Point::sptr& p )
{
    Point::sptr point = ::fwData::Point::New();
    point->m_vCoord[0] = p->m_vCoord[0];
    point->m_vCoord[1] = p->m_vCoord[1];
    point->m_vCoord[2] = p->m_vCoord[2];
    return point;
}

//------------------------------------------------------------------------------

Point::~Point ()
{
}

//------------------------------------------------------------------------------

void Point::shallowCopy(const Object::csptr& _source )
{
    Point::csptr other = Point::dynamicConstCast(_source);
    FW_RAISE_EXCEPTION_IF( ::fwData::Exception(
                               "Unable to copy" + (_source ? _source->getClassname() : std::string("<NULL>"))
                               + " to " + this->getClassname()), !bool(other) );
    this->fieldShallowCopy( _source );
    m_vCoord = other->m_vCoord;
}

//------------------------------------------------------------------------------

void Point::cachedDeepCopy(const Object::csptr& _source, DeepCopyCacheType& cache)
{
    Point::csptr other = Point::dynamicConstCast(_source);
    FW_RAISE_EXCEPTION_IF( ::fwData::Exception(
                               "Unable to copy" + (_source ? _source->getClassname() : std::string("<NULL>"))
                               + " to " + this->getClassname()), !bool(other) );
    this->fieldDeepCopy( _source, cache );
    m_vCoord = other->m_vCoord;
}

//------------------------------------------------------------------------------

} // namespace fwData
