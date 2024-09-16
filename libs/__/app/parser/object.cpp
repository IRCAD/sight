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

#include "app/parser/object.hpp"

#include <app/helper/config.hpp>

#include <ranges>

namespace sight::app::parser
{

//------------------------------------------------------------------------------

void object::parse(const service::config_t& _cfg, core::object::sptr _obj, objects_t& _sub_objects)
{
    data::object::sptr associated_object = std::dynamic_pointer_cast<data::object>(_obj);
    SIGHT_ASSERT("associated_object not instanced", associated_object);

    for(const auto& elem : _cfg)
    {
        if(elem.first == "item")
        {
            const auto key = elem.second.get<std::string>("<xmlattr>.key");

            objects_t sub_objects;
            helper::config::parse_object(elem.second, sub_objects);
            SIGHT_ASSERT("Deferred sub-objects are not supported.", sub_objects.deferred.empty());
            _sub_objects.merge(sub_objects);
            associated_object->set_field(key, sub_objects.created.begin()->second);
        }
    }
}

//------------------------------------------------------------------------------

} // namespace sight::app::parser
