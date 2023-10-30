/************************************************************************
 *
 * Copyright (C) 2023 IRCAD France
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

#include <activity/builder/data.hpp>

#include <data/factory/new.hpp>

#include <service/extension/factory.hpp>
#include <service/object_parser.hpp>

namespace sight::activity::detail::data
{

//------------------------------------------------------------------------------

sight::data::object::sptr create(const std::string& _type, const activity::extension::config_t& _config)
{
    auto object = sight::data::factory::make(_type);

    // Get the object parser associated with the object type
    const auto srv_factory = service::extension::factory::get();

    const std::string srv_impl = srv_factory->get_default_implementation_id_from_object_and_type(
        object->get_classname(),
        "sight::service::object_parser"
    );

    service::base::sptr srv = srv_factory->create(srv_impl);
    auto obj_parser         = std::dynamic_pointer_cast<service::object_parser>(srv);

    obj_parser->set_object_config(_config);
    obj_parser->create_config(object);
    obj_parser->start_config();
    obj_parser->update_config();
    obj_parser->stop_config();
    obj_parser->destroy_config();

    return object;
}

} // namespace sight::activity::detail::data
