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

#include <sight/service/config.hpp>

#include <core/tools/object.hpp>

#include <boost/property_tree/ptree.hpp>

#include <map>

namespace sight::service::extension
{

/**
 */
class SIGHT_SERVICE_CLASS_API service_config_info : public core::base_object
{
public:

    SIGHT_DECLARE_CLASS(service_config_info, core::base_object);

    /// Constructor, do nothing.
    service_config_info()
    = default;

    /// Destructor, do nothing.
    ~service_config_info() override
    = default;

    std::string service;
    std::string desc;
    boost::property_tree::ptree config;
};

/**
 * @brief This class allows to register all the service configuration which has the point extension
 *        "sight::service::extension::config".
 *
 * Registry config like :
 * @code{.xml}
    <extension implements="sight::service::extension::config">
        <id>SDBOpenIOSelectorConfig</id>
        <service>module::ui::editor::selector</service>
        <desc>"Open" action's IOSelector config</desc>
        <config>
            <type mode="reader" />
            <selection mode="include" />
            <addSelection service="sight::module::io::session::reader" />
            <config id="SightDataConfig" service="sight::module::io::session::reader" />
        </config>
    </extension>
   @endcode
 */
class SIGHT_SERVICE_CLASS_API config : public core::base_object
{
public:

    SIGHT_DECLARE_CLASS(config, core::base_object);

    /// Return the default global instance of config
    SIGHT_SERVICE_API static config::sptr get_default();

    SIGHT_SERVICE_API config()           = default;
    SIGHT_SERVICE_API ~config() override = default;

    /**
     * @brief Parses module information to retrieve service declaration.
     * @warning This method must be launch only once. The same extension will not be parsed twice.
     * @note This method is thread safe.
     */
    SIGHT_SERVICE_API void parse_plugin_infos();

    /**
     * @brief Register a new service configuration
     * @param _config_id the identifier of the registered configuration.
     * @param _service  the implementation of the service
     * @param _desc     the description of the configuration
     * @param _config   the registered config
     * @param _config   the registered config
     * @note This method is thread safe
     */
    SIGHT_SERVICE_API void add_service_config_info(
        const std::string& _config_id,
        const std::string& _service,
        const std::string& _desc,
        const boost::property_tree::ptree& _config
    );

    /**
     * @brief Returns the configuration with the given id for the service with the given implementation
     * @note This method is thread safe
     */
    SIGHT_SERVICE_API boost::property_tree::ptree get_service_config(
        const std::string& _config_id,
        const std::string& _service_impl =
        ""
    ) const;

    /**
     * @brief Returns the description of the given configuration name.
     * @note This method is thread safe
     */
    SIGHT_SERVICE_API const std::string& get_config_desc(const std::string& _config_id) const;

    /**
     * @brief Returns a vector containing the names of the available config for the service with the given
     * implementation
     * @note This method is thread safe
     */
    SIGHT_SERVICE_API std::vector<std::string> get_all_config_for_service(
        std::string _service_impl,
        bool _matching_only = false
    ) const;

    /**
     * @brief Clear the registry.
     * @note This method is thread safe.
     */
    SIGHT_SERVICE_API void clear_registry();

protected:

    using registry = std::map<std::string, service_config_info::sptr>;

    /// Container of service information <configId, service config information>
    registry m_reg;

    const static std::string CONFIG_EXT_POINT;

    /// Used to protect the registry access.
    mutable core::mt::read_write_mutex m_registry_mutex;

    /// The global instance of the service config.
    static config::sptr s_current_service_config;
};

} // namespace sight::service::extension
