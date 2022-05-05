/************************************************************************
 *
 * Copyright (C) 2022 IRCAD France
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

#include "ActivitySet.hpp"

#include "data/registry/macros.hpp"

#include <core/com/Signal.hxx>

SIGHT_REGISTER_DATA(sight::data::ActivitySet);

namespace sight::data
{

ActivitySet::ActivitySet(Object::Key key) :
    IContainer<ActivitySet::container_type>(key)
{
}

//------------------------------------------------------------------------------

void ActivitySet::shallowCopy(const Object::csptr& source)
{
    const auto& other = ActivitySet::dynamicCast(source);

    SIGHT_THROW_EXCEPTION_IF(
        Exception(
            "Unable to copy" + (source ? source->getClassname() : std::string("<NULL>")) + " to " + getClassname()
        ),
        !other
    );

    IContainer<ActivitySet::container_type>::shallowCopy(other);
}

//------------------------------------------------------------------------------

bool ActivitySet::operator==(const ActivitySet& other) const noexcept
{
    return IContainer<ActivitySet::container_type>::operator==(other);
}

//------------------------------------------------------------------------------

bool ActivitySet::operator!=(const ActivitySet& other) const noexcept
{
    return IContainer<ActivitySet::container_type>::operator!=(other);
}

//------------------------------------------------------------------------------

void ActivitySet::cachedDeepCopy(const Object::csptr& source, DeepCopyCacheType& cache)
{
    const auto& other = ActivitySet::dynamicCast(source);

    SIGHT_THROW_EXCEPTION_IF(
        Exception(
            "Unable to copy" + (source ? source->getClassname() : std::string("<NULL>")) + " to " + getClassname()
        ),
        !other
    );

    IContainer<ActivitySet::container_type>::cachedDeepCopy(other, cache);
}

} // namespace sight::data
