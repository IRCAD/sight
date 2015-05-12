/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <fwCore/base.hpp>

#include "fwData/registry/macros.hpp"
#include "fwData/Exception.hpp"
#include "fwData/Point.hpp"

fwDataRegisterMacro( ::fwData::Point );

namespace fwData
{

//------------------------------------------------------------------------------

Point::Point(::fwData::Object::Key key)
{
    m_vCoord[0] = 0.0;
    m_vCoord[1] = 0.0;
    m_vCoord[2] = 0.0;
}

//------------------------------------------------------------------------------

Point::sptr Point::PointFactory(float x, float y, float z)
{
    Point::sptr point = ::fwData::Point::New();
    point->m_vCoord[0] = x;
    point->m_vCoord[1] = y;
    point->m_vCoord[2] = z;
    return point;
}

//------------------------------------------------------------------------------

Point::sptr Point::PointFactory(double x, double y, double z)
{
    Point::sptr point = ::fwData::Point::New();
    point->m_vCoord[0] = x;
    point->m_vCoord[1] = y;
    point->m_vCoord[2] = z;
    return point;
}

//------------------------------------------------------------------------------

Point::sptr Point::PointFactory(const PointCoordArrayType& coord)
{
    Point::sptr point = ::fwData::Point::New();
    point->m_vCoord = coord;
    return point;
}

//------------------------------------------------------------------------------

Point::sptr Point::PointFactory( Point::sptr p )
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

void Point::shallowCopy(const Object::csptr &_source )
{
    Point::csptr other = Point::dynamicConstCast(_source);
    FW_RAISE_EXCEPTION_IF( ::fwData::Exception(
                               "Unable to copy" + (_source ? _source->getClassname() : std::string("<NULL>"))
                               + " to " + this->getClassname()), !bool(other) );
    this->fieldShallowCopy( _source );
    m_vCoord = other->m_vCoord;
}

//------------------------------------------------------------------------------

void Point::cachedDeepCopy(const Object::csptr &_source, DeepCopyCacheType &cache)
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


