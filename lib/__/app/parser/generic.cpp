/************************************************************************
 *
 * Copyright (C) 2024 IRCAD France
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

#include "app/parser/generic.hpp"

#include <core/runtime/helper.hpp>

#include <data/string_serializable.hpp>

namespace sight::app::parser
{

//------------------------------------------------------------------------------

void generic::parse(const service::config_t& _cfg, core::object::sptr _obj, objects_t& /*_sub_objects*/)
{
    auto input = _cfg.get_optional<std::string>("<xmlattr>.value");
    if(not input.has_value())
    {
        input = _cfg.get_optional<std::string>("value");
    }

    if(input.has_value())
    {
        const auto generic_obj = std::dynamic_pointer_cast<data::string_serializable>(_obj);
        SIGHT_ASSERT("generic not instanced", generic_obj);
        generic_obj->from_string(input.value());
        generic_obj->set_default_value();
    }
}

//------------------------------------------------------------------------------

} // namespace sight::app::parser
