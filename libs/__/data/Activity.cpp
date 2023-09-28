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

#include "data/Activity.hpp"

#include "data/Exception.hpp"
#include "data/registry/macros.hpp"

SIGHT_REGISTER_DATA(sight::data::Activity)

namespace sight::data
{

//------------------------------------------------------------------------------

void Activity::shallow_copy(const Object::csptr& source)
{
    const auto& other = std::dynamic_pointer_cast<const Activity>(source);

    SIGHT_THROW_EXCEPTION_IF(
        Exception(
            "Unable to copy " + (source ? source->get_classname() : std::string("<NULL>"))
            + " to " + get_classname()
        ),
        !bool(other)
    );

    m_activityConfigId = other->m_activityConfigId;

    data::Composite::shallow_copy(other);
}

//------------------------------------------------------------------------------

void Activity::deep_copy(const Object::csptr& source, const std::unique_ptr<DeepCopyCacheType>& cache)
{
    const auto& other = std::dynamic_pointer_cast<const Activity>(source);

    SIGHT_THROW_EXCEPTION_IF(
        data::Exception(
            "Unable to copy " + (source ? source->get_classname() : std::string("<NULL>"))
            + " to " + get_classname()
        ),
        !bool(other)
    );

    m_activityConfigId = other->m_activityConfigId;

    data::Composite::deep_copy(other, cache);
}

//------------------------------------------------------------------------------

bool Activity::operator==(const Activity& other) const noexcept
{
    if(m_activityConfigId != other.m_activityConfigId)
    {
        return false;
    }

    // Super class last
    return data::Composite::operator==(other);
}

//------------------------------------------------------------------------------

bool Activity::operator!=(const Activity& other) const noexcept
{
    return !(*this == other);
}

} // namespace sight::data
