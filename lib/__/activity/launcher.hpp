/************************************************************************
 *
 * Copyright (C) 2019-2026 IRCAD France
 * Copyright (C) 2019-2020 IHU Strasbourg
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

#include <sight/activity/config.hpp>

#include "activity/extension/activity.hpp"

#include <data/activity.hpp>
#include <data/object.hpp>

#include <boost/property_tree/ptree.hpp>

#include <functional>
#include <map>
#include <vector>

namespace sight::activity
{

/**
 * @brief Base class for services that launches Activity instances.
 */
class SIGHT_ACTIVITY_CLASS_API launcher
{
public:

    using configuration_t    = boost::property_tree::ptree;
    using parameter_t        = activity::extension::activity_config_param;
    using parameters_t       = activity::extension::activity_config_params_type;
    using replace_map_t      = std::map<std::string, std::string>;
    using in_out_map_t       = std::vector<std::string>;
    using value_parameters_t = std::map<std::string, std::string>;

    using type_resolver_t = std::function<std::string(const std::string&)>;
    using uid_generator_t = std::function<std::string(const std::string&)>;

    /// Constructor. Do nothing.
    SIGHT_ACTIVITY_API launcher() = default;

    /// Destructor. Do nothing.
    SIGHT_ACTIVITY_API virtual ~launcher() = default;

protected:

    /// Parses the configuration
    SIGHT_ACTIVITY_API virtual void parse_configuration(
        const configuration_t& _config,
        const in_out_map_t& _inouts = in_out_map_t()
    );

    /**
     * @brief Create typed objects from literal value parameters and update replacement map with their UIDs.
     * @param _value_parameters key/value parameters parsed from <inout><key value="..."/>
     * @param _replacement_map replacement map updated with generated object UIDs
     * @param _type_resolver callback returning the expected object type for a key
     * @param _uid_generator callback used to generate a unique UID for a key
     * @param _context_id human-readable context used in error messages
     * @return created objects that must be kept alive while the launched config is running
     */
    static SIGHT_ACTIVITY_API std::vector<data::object::sptr> materialize_value_parameters(
        const value_parameters_t& _value_parameters,
        replace_map_t& _replacement_map,
        const type_resolver_t& _type_resolver,
        const uid_generator_t& _uid_generator,
        const std::string& _context_id
    );

    /// Create the activity given in 'mainActivity' configuration
    [[nodiscard]] SIGHT_ACTIVITY_API virtual data::activity::sptr create_main_activity() const;

    /**
     * @brief Check if the activity is valid by calling the activity validator.
     * @return Return true if the given activity is valid
     */
    static SIGHT_ACTIVITY_API std::pair<bool, std::string> validate_activity(
        const data::activity::csptr& _activity
    );

    // NOLINTBEGIN(cppcoreguidelines-non-private-member-variables-in-classes)
    std::string m_main_activity_id; ///< configuration id of the main activity

    parameters_t m_parameters;             ///< parameters given in configuration
    value_parameters_t m_value_parameters; ///< parameters from <inout> with literal values
    // NOLINTEND(cppcoreguidelines-non-private-member-variables-in-classes)
};

} // namespace sight::activity
