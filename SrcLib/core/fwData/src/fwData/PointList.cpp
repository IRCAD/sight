/************************************************************************
 *
 * Copyright (C) 2009-2018 IRCAD France
 * Copyright (C) 2012-2018 IHU Strasbourg
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
