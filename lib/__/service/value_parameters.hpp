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

#pragma once

#include <sight/service/config.hpp>

#include <data/object.hpp>

#include <functional>
#include <map>
#include <string>
#include <vector>

namespace sight::service
{

/// Literal values indexed by the key they are declared with.
using value_parameters_t = std::map<std::string, std::string>;

/// Substitution map of a configuration, associating a parameter name to the identifier replacing it.
using replace_map_t = std::map<std::string, std::string>;

/// Returns the class name of the object to build for a given key, or an empty string when it is unknown.
using type_resolver_t = std::function<std::string(const std::string&)>;

/// Returns the identifier to assign to the object built for a given key.
using uid_generator_t = std::function<std::string(const std::string&)>;

/**
 * @brief Builds an object of the given type, initialized from a literal string representation.
 *
 * @param _type class name of the data to instantiate, which must be a sight::data::string_serializable
 * @param _value literal representation of the value, as understood by data::string_serializable::from_string()
 * @return the new object, never nullptr
 * @throw core::exception if the type is empty, unknown, or does not support literal values
 */
SIGHT_SERVICE_API data::object::sptr make_object_from_value(const std::string& _type, const std::string& _value);

/**
 * @brief Builds the objects declared with a literal value and registers their identifier in the replacement map.
 *
 * This is meant for services launching a sub-configuration, which forward their inputs as substitution parameters.
 * Since the objects are not declared by the application configuration, the caller owns them and must keep the
 * returned objects alive as long as the sub-configuration runs.
 *
 * @param _value_parameters literal values to materialize
 * @param _replacement_map substitution map, updated with the identifier of each new object
 * @param _type_resolver resolves the type expected by the sub-configuration for a given key
 * @param _uid_generator generates a unique identifier for a given key
 * @param _context_id human-readable context used in error messages
 * @return the new objects, in the order of the keys
 */
SIGHT_SERVICE_API std::vector<data::object::sptr> materialize_value_parameters(
    const value_parameters_t& _value_parameters,
    replace_map_t& _replacement_map,
    const type_resolver_t& _type_resolver,
    const uid_generator_t& _uid_generator,
    const std::string& _context_id
);

} // namespace sight::service
