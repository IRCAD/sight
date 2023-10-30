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

#include "app/parser/generic_field.hpp"

#include "service/macros.hpp"

#include <data/generic_field_base.hpp>

namespace sight::app::parser
{

//------------------------------------------------------------------------------

void generic_field::updating()
{
    SIGHT_FATAL("This method is deprecated, and thus shouldn't be used.");
}

//------------------------------------------------------------------------------

void generic_field::create_config(core::tools::object::sptr _obj)
{
    const auto input = m_cfg.get_optional<std::string>("value");

    if(input.has_value())
    {
        const auto field = std::dynamic_pointer_cast<data::generic_field_base>(_obj);
        SIGHT_ASSERT("generic_field not instanced", field);
        field->from_string(input.value());
    }
}

//------------------------------------------------------------------------------

} // namespace sight::app::parser
