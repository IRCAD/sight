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

#include "fwData/PlaneList.hpp"

#include "fwData/Exception.hpp"
#include "fwData/registry/macros.hpp"

#include <fwCom/Signal.hpp>
#include <fwCom/Signal.hxx>

#include <fwCore/base.hpp>

fwDataRegisterMacro( ::fwData::PlaneList );

namespace fwData
{

const ::fwCom::Signals::SignalKeyType PlaneList::s_PLANE_ADDED_SIG         = "planeAdded";
const ::fwCom::Signals::SignalKeyType PlaneList::s_PLANE_REMOVED_SIG       = "planeRemoved";
const ::fwCom::Signals::SignalKeyType PlaneList::s_VISIBILITY_MODIFIED_SIG = "visibilityModified";

//------------------------------------------------------------------------------

PlaneList::PlaneList(::fwData::Object::Key)
{
    newSignal< PlaneAddedSignalType >(s_PLANE_ADDED_SIG);
    newSignal< PlaneRemovedSignalType >(s_PLANE_REMOVED_SIG);
    newSignal< VisibilityModifiedSignalType >(s_VISIBILITY_MODIFIED_SIG);
}

//------------------------------------------------------------------------------

PlaneList::~PlaneList()
{
    SLM_TRACE_FUNC();
}

//------------------------------------------------------------------------------

void PlaneList::shallowCopy(const Object::csptr& _source )
{
    PlaneList::csptr other = PlaneList::dynamicConstCast(_source);
    FW_RAISE_EXCEPTION_IF( ::fwData::Exception(
                               "Unable to copy" + (_source ? _source->getClassname() : std::string("<NULL>"))
                               + " to " + this->getClassname()), !other );
    this->fieldShallowCopy( _source );

    this->m_vPlanes = other->m_vPlanes;
}

//------------------------------------------------------------------------------

void PlaneList::cachedDeepCopy(const Object::csptr& _source, DeepCopyCacheType& cache)
{
    PlaneList::csptr other = PlaneList::dynamicConstCast(_source);
    FW_RAISE_EXCEPTION_IF( ::fwData::Exception(
                               "Unable to copy" + (_source ? _source->getClassname() : std::string("<NULL>"))
                               + " to " + this->getClassname()), !other );
    this->fieldDeepCopy( _source, cache );

    this->m_vPlanes.clear();
    for(const ::fwData::Plane::sptr& plane : other->m_vPlanes)
    {
        Plane::sptr newPlane = ::fwData::Object::copy(plane, cache);
        this->m_vPlanes.push_back( newPlane );
    }
}

} // namespace fwData
