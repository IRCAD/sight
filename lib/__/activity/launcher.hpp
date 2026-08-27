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

#include <service/value_parameters.hpp>

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
    using replace_map_t      = service::replace_map_t;
    using in_out_map_t       = std::vector<std::string>;
    using value_parameters_t = service::value_parameters_t;

    using type_resolver_t = service::type_resolver_t;
    using uid_generator_t = service::uid_generator_t;

    /// Constructor. Do nothing.
    SIGHT_ACTIVITY_API launcher() = default;

    /// Destructor. Do nothing.
    SIGHT_ACTIVITY_API virtual ~launcher() = default;

protected:

    /// Parses the configuration
    SIGHT_ACTIVITY_API virtual void parse_configuration(
        const configuration_t& _config,
        const in_out_map_t& _inouts         = in_out_map_t(),
        const in_out_map_t& _bound_data_ids = in_out_map_t()
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
