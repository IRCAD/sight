/************************************************************************
 *
 * Copyright (C) 2009-2022 IRCAD France
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

SIGHT_REGISTER_DATA(sight::data::PlaneList);

namespace sight::data
{

const core::com::Signals::SignalKeyType PlaneList::s_PLANE_ADDED_SIG         = "planeAdded";
const core::com::Signals::SignalKeyType PlaneList::s_PLANE_REMOVED_SIG       = "planeRemoved";
const core::com::Signals::SignalKeyType PlaneList::s_VISIBILITY_MODIFIED_SIG = "visibilityModified";

//------------------------------------------------------------------------------

PlaneList::PlaneList(data::Object::Key /*unused*/)
{
    newSignal<PlaneAddedSignalType>(s_PLANE_ADDED_SIG);
    newSignal<PlaneRemovedSignalType>(s_PLANE_REMOVED_SIG);
    newSignal<VisibilityModifiedSignalType>(s_VISIBILITY_MODIFIED_SIG);
}

//------------------------------------------------------------------------------

void PlaneList::shallowCopy(const Object::csptr& source)
{
    const auto& other = dynamicConstCast(source);

    SIGHT_THROW_EXCEPTION_IF(
        Exception(
            "Unable to copy " + (source ? source->getClassname() : std::string("<NULL>"))
            + " to " + getClassname()
        ),
        !other
    );

    this->m_vPlanes = other->m_vPlanes;

    BaseClass::shallowCopy(other);
}

//------------------------------------------------------------------------------

void PlaneList::deepCopy(const Object::csptr& source, const std::unique_ptr<DeepCopyCacheType>& cache)
{
    const auto& other = dynamicConstCast(source);

    SIGHT_THROW_EXCEPTION_IF(
        Exception(
            "Unable to copy " + (source ? source->getClassname() : std::string("<NULL>"))
            + " to " + getClassname()
        ),
        !other
    );

    this->m_vPlanes.clear();
    for(const auto& plane : other->m_vPlanes)
    {
        this->m_vPlanes.push_back(data::Object::copy(plane, cache));
    }

    BaseClass::deepCopy(other, cache);
}

//------------------------------------------------------------------------------

bool PlaneList::operator==(const PlaneList& other) const noexcept
{
    if(!core::tools::is_equal(m_vPlanes, other.m_vPlanes))
    {
        return false;
    }

    // Super class last
    return BaseClass::operator==(other);
}

//------------------------------------------------------------------------------

bool PlaneList::operator!=(const PlaneList& other) const noexcept
{
    return !(*this == other);
}

} // namespace sight::data
