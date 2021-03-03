/************************************************************************
 *
 * Copyright (C) 2009-2021 IRCAD France
 * Copyright (C) 2012-2020 IHU Strasbourg
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

#include "data/PlaneList.hpp"

#include "data/Exception.hpp"
#include "data/registry/macros.hpp"

#include <core/base.hpp>
#include <core/com/Signal.hpp>
#include <core/com/Signal.hxx>

SIGHT_REGISTER_DATA( sight::data::PlaneList );

namespace sight::data
{

const core::com::Signals::SignalKeyType PlaneList::s_PLANE_ADDED_SIG         = "planeAdded";
const core::com::Signals::SignalKeyType PlaneList::s_PLANE_REMOVED_SIG       = "planeRemoved";
const core::com::Signals::SignalKeyType PlaneList::s_VISIBILITY_MODIFIED_SIG = "visibilityModified";

//------------------------------------------------------------------------------

PlaneList::PlaneList(data::Object::Key)
{
    newSignal< PlaneAddedSignalType >(s_PLANE_ADDED_SIG);
    newSignal< PlaneRemovedSignalType >(s_PLANE_REMOVED_SIG);
    newSignal< VisibilityModifiedSignalType >(s_VISIBILITY_MODIFIED_SIG);
}

//------------------------------------------------------------------------------

PlaneList::~PlaneList()
{
}

//------------------------------------------------------------------------------

void PlaneList::shallowCopy(const Object::csptr& _source )
{
    PlaneList::csptr other = PlaneList::dynamicConstCast(_source);
    SIGHT_THROW_EXCEPTION_IF( data::Exception(
                                  "Unable to copy" + (_source ? _source->getClassname() : std::string("<NULL>"))
                                  + " to " + this->getClassname()), !other );
    this->fieldShallowCopy( _source );

    this->m_vPlanes = other->m_vPlanes;
}

//------------------------------------------------------------------------------

void PlaneList::cachedDeepCopy(const Object::csptr& _source, DeepCopyCacheType& cache)
{
    PlaneList::csptr other = PlaneList::dynamicConstCast(_source);
    SIGHT_THROW_EXCEPTION_IF( data::Exception(
                                  "Unable to copy" + (_source ? _source->getClassname() : std::string("<NULL>"))
                                  + " to " + this->getClassname()), !other );
    this->fieldDeepCopy( _source, cache );

    this->m_vPlanes.clear();
    for(const data::Plane::sptr& plane : other->m_vPlanes)
    {
        Plane::sptr newPlane = data::Object::copy(plane, cache);
        this->m_vPlanes.push_back( newPlane );
    }
}

} // namespace sight::data
