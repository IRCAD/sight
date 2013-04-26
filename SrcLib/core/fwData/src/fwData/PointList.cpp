/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2012.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <boost/foreach.hpp>

#include <fwCore/base.hpp>

#include "fwData/registry/macros.hpp"
#include "fwData/Exception.hpp"
#include "fwData/PointList.hpp"

fwDataRegisterMacro( ::fwData::PointList );

namespace fwData
{

//------------------------------------------------------------------------------

PointList::PointList(::fwData::Object::Key key)
{}

//------------------------------------------------------------------------------

PointList::~PointList()
{}

//------------------------------------------------------------------------------

void PointList::shallowCopy(const Object::csptr &_source )
{
    PointList::csptr other = PointList::dynamicConstCast(_source);
    FW_RAISE_EXCEPTION_IF( ::fwData::Exception(
            "Unable to copy" + (_source?_source->getClassname():std::string("<NULL>"))
            + " to " + this->getClassname()), !bool(other) );
    this->fieldShallowCopy( _source );

    m_vPoints = other->m_vPoints;
}

//------------------------------------------------------------------------------

void PointList::cachedDeepCopy(const Object::csptr &_source, DeepCopyCacheType &cache)
{
    PointList::csptr other = PointList::dynamicConstCast(_source);
    FW_RAISE_EXCEPTION_IF( ::fwData::Exception(
            "Unable to copy" + (_source?_source->getClassname():std::string("<NULL>"))
            + " to " + this->getClassname()), !bool(other) );
    this->fieldDeepCopy( _source, cache );

    m_vPoints.clear();
    BOOST_FOREACH(PointListContainer::value_type point, other->m_vPoints )
    {
        Point::sptr newPoint = Point::New();
        newPoint = ::fwData::Object::copy(point, cache);
        m_vPoints.push_back( newPoint );
    }
}

//------------------------------------------------------------------------------

} // namespace fwData


