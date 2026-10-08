/************************************************************************
 *
 * Copyright (C) 2026 IRCAD France
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

#include "copy_field.hpp"

#include <data/object.hpp>

namespace sight::module::data
{

//------------------------------------------------------------------------------

service::base::connections_t copy_field::auto_connections() const
{
    base::connections_t connections = {
        {m_source, sight::data::signals::CHANGED_FIELDS, slots::UPDATE},
        {m_source, sight::data::signals::ADDED_FIELDS, slots::UPDATE},
        {m_source, sight::data::signals::REMOVED_FIELDS, slots::UPDATE}
    };
    return connections + service::base::auto_connections();
}

//-----------------------------------------------------------------------------

void copy_field::starting()
{
    // Nothing to do on start
}

//-----------------------------------------------------------------------------

void copy_field::updating()
{
    // Extract the source object.
    const auto source = m_source.lock();
    SIGHT_ASSERT("Source object is null", source);

    // Copy the fields to the inout.
    const auto target = m_target.lock();

    const auto old_fields = target->get_fields();
    const auto new_fields = source->get_fields();

    sight::data::fields_container_t old_fields_container;
    std::ranges::copy(old_fields, std::inserter(old_fields_container, old_fields_container.end()));
    sight::data::fields_container_t new_fields_container;
    std::ranges::copy(new_fields, std::inserter(new_fields_container, new_fields_container.end()));

    target->set_fields(sight::data::object::field_map_t());
    for(const auto& [key, value] : new_fields)
    {
        target->set_field(key, sight::data::object::copy(value));
    }

    target->async_emit(this, sight::data::signals::CHANGED_FIELDS, old_fields_container, new_fields_container);
}

//-----------------------------------------------------------------------------

void copy_field::stopping()
{
}

//-----------------------------------------------------------------------------

} // namespace sight::module::data
