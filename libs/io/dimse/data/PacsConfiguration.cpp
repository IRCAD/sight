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

#include "io/dimse/data/PacsConfiguration.hpp"

#include <data/Exception.hpp>
#include <data/registry/macros.hpp>

SIGHT_REGISTER_DATA(sight::io::dimse::data::PacsConfiguration);

namespace sight::io::dimse::data
{

//------------------------------------------------------------------------------

void PacsConfiguration::shallow_copy(const sight::data::Object::csptr& source)
{
    const auto& other = std::dynamic_pointer_cast<const PacsConfiguration>(source);

    SIGHT_THROW_EXCEPTION_IF(
        sight::data::Exception(
            "Unable to copy " + (source ? source->get_classname() : std::string("<NULL>"))
            + " to " + get_classname()
        ),
        !bool(other)
    );

    m_localApplicationTitle = other->m_localApplicationTitle;
    m_pacsHostName          = other->m_pacsHostName;
    m_pacsApplicationTitle  = other->m_pacsApplicationTitle;
    m_pacsApplicationPort   = other->m_pacsApplicationPort;
    m_moveApplicationTitle  = other->m_moveApplicationTitle;
    m_moveApplicationPort   = other->m_moveApplicationPort;
    m_retrieveMethod        = other->m_retrieveMethod;

    base_class::shallow_copy(other);
}

//------------------------------------------------------------------------------

} // namespace sight::io::dimse::data
