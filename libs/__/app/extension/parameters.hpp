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

#pragma once

#include <sight/app/config.hpp>

#include "app/extension/config.hpp"

#include <core/mt/types.hpp>
#include <core/object.hpp>

#include <map>

namespace sight::app::extension
{

/**
 * @brief This class allows to register all the configuration parameters which has the point extension
 *        "sight::app::extension::parameters".
 */
class SIGHT_APP_CLASS_API parameters : public core::base_object
{
public:

    SIGHT_DECLARE_CLASS(parameters, core::base_object);

    /// Return the default global instance of parameters
    SIGHT_APP_API static parameters::sptr get_default();

    SIGHT_APP_API parameters();
    SIGHT_APP_API ~parameters() override = default;

    /**
     * @brief Parse module information to retrieve config parameters declaration
     * @warning This method must be launch only once. The same extension will not be parsed twice.
     * @note This method is thread safe.
     **/
    SIGHT_APP_API void parse_plugin_infos();

    /**
     * @brief Get the parameters associated to extension id.
     * @note This method is thread safe.
     **/
    SIGHT_APP_API const field_adaptor_t& get_parameters(const std::string& _extension_id) const;

    /**
     * @brief Clear the registry.
     * @note This method is thread safe.
     */
    SIGHT_APP_API void clear_registry();

protected:

    using registry = std::map<std::string, field_adaptor_t>;

    /// Container of parameter information
    registry m_reg;

    /// Used to protect the registry access.
    mutable core::mt::read_write_mutex m_registry_mutex;

    /// The global instance of the app config parameters.
    static parameters::sptr s_app_config_parameters;
};

} // namespace sight::app::extension
