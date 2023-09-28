/************************************************************************
 *
 * Copyright (C) 2009-2023 IRCAD France
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
#include <core/com/signal.hpp>
#include <core/com/signal.hxx>

SIGHT_REGISTER_DATA(sight::data::PlaneList);

namespace sight::data
{

const core::com::signals::key_t PlaneList::PLANE_ADDED_SIG         = "planeAdded";
const core::com::signals::key_t PlaneList::PLANE_REMOVED_SIG       = "planeRemoved";
const core::com::signals::key_t PlaneList::VISIBILITY_MODIFIED_SIG = "visibilityModified";

//------------------------------------------------------------------------------

PlaneList::PlaneList()
{
    new_signal<PlaneAddedSignalType>(PLANE_ADDED_SIG);
    new_signal<PlaneRemovedSignalType>(PLANE_REMOVED_SIG);
    new_signal<VisibilityModifiedSignalType>(VISIBILITY_MODIFIED_SIG);
}

//------------------------------------------------------------------------------

void PlaneList::shallow_copy(const Object::csptr& source)
{
    const auto& other = std::dynamic_pointer_cast<const PlaneList>(source);

    SIGHT_THROW_EXCEPTION_IF(
        Exception(
            "Unable to copy " + (source ? source->get_classname() : std::string("<NULL>"))
            + " to " + get_classname()
        ),
        !other
    );

    this->m_vPlanes = other->m_vPlanes;

    base_class::shallow_copy(other);
}

//------------------------------------------------------------------------------

void PlaneList::deep_copy(const Object::csptr& source, const std::unique_ptr<DeepCopyCacheType>& cache)
{
    const auto& other = std::dynamic_pointer_cast<const PlaneList>(source);

    SIGHT_THROW_EXCEPTION_IF(
        Exception(
            "Unable to copy " + (source ? source->get_classname() : std::string("<NULL>"))
            + " to " + get_classname()
        ),
        !other
    );

    this->m_vPlanes.clear();
    for(const auto& plane : other->m_vPlanes)
    {
        this->m_vPlanes.push_back(data::Object::copy(plane, cache));
    }

    base_class::deep_copy(other, cache);
}

//------------------------------------------------------------------------------

bool PlaneList::operator==(const PlaneList& other) const noexcept
{
    if(!core::tools::is_equal(m_vPlanes, other.m_vPlanes))
    {
        return false;
    }

    // Super class last
    return base_class::operator==(other);
}

//------------------------------------------------------------------------------

bool PlaneList::operator!=(const PlaneList& other) const noexcept
{
    return !(*this == other);
}

} // namespace sight::data
