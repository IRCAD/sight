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

#include "app/parser/map.hpp"

#include <app/helper/config.hpp>

#include <data/map.hpp>

#include <ranges>

namespace sight::app::parser
{

//------------------------------------------------------------------------------

void map::parse(const config_t& _cfg, core::object::sptr _obj, objects_t& _sub_objects)
{
    data::map::sptr data_map = std::dynamic_pointer_cast<data::map>(_obj);
    SIGHT_ASSERT("The passed object must be a data::map", data_map);

    for(const auto& elem : _cfg)
    {
        if(elem.first == "item")
        {
            const auto key = elem.second.get<std::string>("<xmlattr>.key");

            objects_t sub_objects;
            helper::config::parse_object(elem.second, sub_objects);
            SIGHT_ASSERT("Deferred sub-objects are not supported.", sub_objects.deferred.empty());
            _sub_objects.merge(sub_objects);
            (*data_map)[key] = sub_objects.created.begin()->second;
        }
    }
}

//------------------------------------------------------------------------------

} // namespace sight::app::parser
