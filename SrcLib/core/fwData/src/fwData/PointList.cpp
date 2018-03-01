/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2018.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "fwData/PointList.hpp"

#include "fwData/Exception.hpp"
#include "fwData/registry/macros.hpp"

#include <fwCom/Signal.hpp>
#include <fwCom/Signal.hxx>

#include <fwCore/base.hpp>

fwDataRegisterMacro( ::fwData::PointList );

namespace fwData
{

const ::fwCom::Signals::SignalKeyType PointList::s_POINT_ADDED_SIG   = "pointAdded";
const ::fwCom::Signals::SignalKeyType PointList::s_POINT_REMOVED_SIG = "pointRemoved";

//------------------------------------------------------------------------------

PointList::PointList(::fwData::Object::Key _key)
{
    newSignal<PointAddedSignalType>(s_POINT_ADDED_SIG);
    newSignal<PointRemovedSignalType>(s_POINT_REMOVED_SIG);
}

//------------------------------------------------------------------------------

PointList::~PointList()
{
}

//------------------------------------------------------------------------------

void PointList::shallowCopy(const Object::csptr& _source )
{
    PointList::csptr other = PointList::dynamicConstCast(_source);
    FW_RAISE_EXCEPTION_IF( ::fwData::Exception(
                               "Unable to copy" + (_source ? _source->getClassname() : std::string("<NULL>"))
                               + " to " + this->getClassname()), !bool(other) );
    this->fieldShallowCopy( _source );

    m_vPoints = other->m_vPoints;
}

//------------------------------------------------------------------------------

void PointList::cachedDeepCopy(const Object::csptr& _source, DeepCopyCacheType& _cache)
{
    PointList::csptr other = PointList::dynamicConstCast(_source);
    FW_RAISE_EXCEPTION_IF( ::fwData::Exception(
                               "Unable to copy" + (_source ? _source->getClassname() : std::string("<NULL>"))
                               + " to " + this->getClassname()), !bool(other) );
    this->fieldDeepCopy( _source, _cache );

    m_vPoints.clear();
    for(const PointListContainer::value_type& point : other->m_vPoints )
    {
        Point::sptr newPoint = Point::New();
        newPoint = ::fwData::Object::copy(point, _cache);
        m_vPoints.push_back( newPoint );
    }
}

//------------------------------------------------------------------------------

} // namespace fwData
