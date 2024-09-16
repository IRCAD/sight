/************************************************************************
 *
 * Copyright (C) 2009-2024 IRCAD France
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

#include "data/activity.hpp"

#include "data/exception.hpp"
#include "data/registry/macros.hpp"

SIGHT_REGISTER_DATA(sight::data::activity)

namespace sight::data
{

//------------------------------------------------------------------------------

void activity::shallow_copy(const object::csptr& _source)
{
    const auto& other = std::dynamic_pointer_cast<const activity>(_source);

    SIGHT_THROW_EXCEPTION_IF(
        exception(
            "Unable to copy " + (_source ? _source->get_classname() : std::string("<NULL>"))
            + " to " + get_classname()
        ),
        !bool(other)
    );

    m_activity_config_id = other->m_activity_config_id;

    data::map::shallow_copy(other);
}

//------------------------------------------------------------------------------

void activity::deep_copy(const object::csptr& _source, const std::unique_ptr<deep_copy_cache_t>& _cache)
{
    const auto& other = std::dynamic_pointer_cast<const activity>(_source);

    SIGHT_THROW_EXCEPTION_IF(
        data::exception(
            "Unable to copy " + (_source ? _source->get_classname() : std::string("<NULL>"))
            + " to " + get_classname()
        ),
        !bool(other)
    );

    m_activity_config_id = other->m_activity_config_id;

    data::map::deep_copy(other, _cache);
}

//------------------------------------------------------------------------------

bool activity::operator==(const activity& _other) const noexcept
{
    if(m_activity_config_id != _other.m_activity_config_id)
    {
        return false;
    }

    // Super class last
    return data::map::operator==(_other);
}

//------------------------------------------------------------------------------

bool activity::operator!=(const activity& _other) const noexcept
{
    return !(*this == _other);
}

} // namespace sight::data
