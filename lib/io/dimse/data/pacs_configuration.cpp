/************************************************************************
 *
 * Copyright (C) 2009-2023 IRCAD France
 * Copyright (C) 2012-2016 IHU Strasbourg
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

#include "io/dimse/data/pacs_configuration.hpp"

#include <data/exception.hpp>
#include <data/registry/macros.hpp>

SIGHT_REGISTER_DATA(sight::io::dimse::data::pacs_configuration);

namespace sight::io::dimse::data
{

//------------------------------------------------------------------------------

void pacs_configuration::shallow_copy(const sight::data::object::csptr& _source)
{
    const auto& other = std::dynamic_pointer_cast<const pacs_configuration>(_source);

    SIGHT_THROW_EXCEPTION_IF(
        sight::data::exception(
            "Unable to copy " + (_source ? _source->get_classname() : std::string("<NULL>"))
            + " to " + get_classname()
        ),
        !bool(other)
    );

    m_local_application_title = other->m_local_application_title;
    m_pacs_host_name          = other->m_pacs_host_name;
    m_pacs_application_title  = other->m_pacs_application_title;
    m_pacs_application_port   = other->m_pacs_application_port;
    m_move_application_title  = other->m_move_application_title;
    m_move_application_port   = other->m_move_application_port;
    m_retrieve_method         = other->m_retrieve_method;

    base_class_t::shallow_copy(other);
}

//------------------------------------------------------------------------------

} // namespace sight::io::dimse::data
