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

#include "service/value_parameters.hpp"

#include <core/runtime/runtime.hpp>

#include <data/factory/new.hpp>
#include <data/string_serializable.hpp>

namespace sight::service
{

//------------------------------------------------------------------------------

data::object::sptr make_object_from_value(const std::string& _type, const std::string& _value)
{
    SIGHT_THROW_IF("The type of the object to build from the value '" << _value << "' is unknown.", _type.empty());

    if(auto ext = core::runtime::find_extension(_type); ext)
    {
        SIGHT_ASSERT("Extension and classname are different.", ext->point() == core::get_classname<data::object>());
        ext->get_module()->start();
    }

    const auto object = data::factory::make(_type);
    SIGHT_THROW_IF("Factory failed to build an object of type '" << _type << "'.", !object);

    const auto serializable = std::dynamic_pointer_cast<data::string_serializable>(object);
    SIGHT_THROW_IF("Type '" << _type << "' does not support literal values.", !serializable);

    serializable->from_string(_value);
    serializable->set_default_value();

    return object;
}

//------------------------------------------------------------------------------

std::vector<data::object::sptr> materialize_value_parameters(
    const value_parameters_t& _value_parameters,
    replace_map_t& _replacement_map,
    const type_resolver_t& _type_resolver,
    const uid_generator_t& _uid_generator,
    const std::string& _context_id
)
{
    std::vector<data::object::sptr> objects;

    for(const auto& [key, value] : _value_parameters)
    {
        const auto type = _type_resolver(key);

        // A value can be passed for a parameter that is actually not used by the target configuration. This is not an
        // error, the parameter is simply ignored.
        if(type.empty())
        {
            SIGHT_DEBUG(
                "[" << _context_id << "] key '" << key << "' is passed with 'value' but no object parameter exists."
            );
            continue;
        }

        const auto object = make_object_from_value(type, value);

        if(!object->has_id())
        {
            const auto uid = _uid_generator(key);
            SIGHT_THROW_IF("[" << _context_id << "] cannot generate UID for key '" << key << "'.", uid.empty());
            object->set_id(uid);
        }

        _replacement_map[key] = object->get_id();
        objects.push_back(object);
    }

    return objects;
}

//------------------------------------------------------------------------------

} // namespace sight::service
