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

#pragma once

#include "app/config.hpp"
#include "app/extension/config.hpp"

#include <core/mt/types.hpp>
#include <core/tools/object.hpp>

#include <map>

namespace sight::app::extension
{

/**
 * @brief This class allows to register all the configuration parameters which has the point extension
 *        "sight::app::extension::parameters".
 */
class APP_CLASS_API parameters : public core::base_object
{
public:

    SIGHT_DECLARE_CLASS(parameters, core::base_object);

    /// Return the default global instance of parameters
    APP_API static parameters::sptr getDefault();

    APP_API parameters();
    APP_API ~parameters() override = default;

    /**
     * @brief Parse module information to retrieve config parameters declaration
     * @warning This method must be launch only once. The same extension will not be parsed twice.
     * @note This method is thread safe.
     **/
    APP_API void parse_plugin_infos();

    /**
     * @brief Get the parameters associated to extension id.
     * @note This method is thread safe.
     **/
    APP_API const field_adaptor_t& getParameters(const std::string& extensionId) const;

    /**
     * @brief Clear the registry.
     * @note This method is thread safe.
     */
    APP_API void clear_registry();

protected:

    typedef std::map<std::string, field_adaptor_t> Registry;

    /// Container of parameter information
    Registry m_reg;

    /// Used to protect the registry access.
    mutable core::mt::read_write_mutex m_registryMutex;

    /// The global instance of the app config parameters.
    static parameters::sptr s_appConfigParameters;
};

} // namespace sight::app::extension
