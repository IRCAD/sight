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

#include "service/extension/config.hpp"

#include <core/mt/types.hpp>
#include <core/object.hpp>
#include <core/runtime/extension.hpp>

#include <data/map.hpp>

#include <map>
#include <unordered_set>

namespace sight::app
{

/// Associations of <pattern, value>.
using field_adaptor_t = std::map<std::string, std::string>;

namespace extension
{

/**
 */
class SIGHT_APP_CLASS_API app_info
{
public:

    using sptr = SPTR(app_info);

    std::string group;
    std::string desc;
    using parameters_t = std::map<std::string, std::string>;
    parameters_t parameters;
    struct objects_info_t
    {
        std::string type;
        bool deferred;
        bool optional;
        std::string value;
    };
    using objects_t = std::map<std::string, objects_info_t>;
    objects_t objects;
    core::runtime::config_t config;
    std::string module_id;      ///< Module identifier (used to start the module when the appConfig is launched)
    std::string module_version; ///< Module version (used to start the module when the appConfig is launched)
};

/**
 * @brief This class allows to register all the configuration which has the point extension
 *        "sight::app::extension::config".
 */
class SIGHT_APP_CLASS_API config final
{
public:

    using sptr = SPTR(config);

    /**
     * @brief Parses module information to retrieve configuration declaration.
     * @warning This method must be launch only once. The same extension will not be parsed twice.
     * @note This method is thread safe.
     */
    SIGHT_APP_API void parse_plugin_infos();

    /**
     * @brief Parses module information to retrieve configuration declaration.
     * @warning This method must be launch only once. The same extension will not be parsed twice.
     * @note This method is thread safe.
     */
    SIGHT_APP_API void parse_plugin_infos(
        const std::vector<std::shared_ptr<core::runtime::extension> >& _extensions
    );

    /**
     * @brief Register a new config.
     * @param _config_id the identifier of the registered config.
     * @param _group the name of the group that owns the config.
     * @param _desc the config description.
     * @param _parameters the list of template parameters in the config.
     * @param _objects the list of objects in the config.
     * @param _config the registered config.
     * @note This method is thread safe
     */
    SIGHT_APP_API void add_app_info(
        const std::string& _config_id,
        const std::string& _group,
        const std::string& _desc,
        const app_info::parameters_t& _parameters,
        const app_info::objects_t& _objects,
        const core::runtime::config_t& _config,
        const std::string& _module_id
    );

    /**
     * @brief  Return the adapted config with the identifier configId.
     * @param _config_id the identifier of the requested config.
     * @param _replace_fields associations between the value and the pattern to replace in the config.
     * @note This method is thread safe.
     */
    SIGHT_APP_API core::runtime::config_t get_adapted_template_config(
        const std::string& _config_id,
        const field_adaptor_t _replace_fields,
        const std::string& _auto_prefix_id
    );

    /**
     * @brief Retrieves the module from the config id
     * @param _config_id the config identifier
     */
    SIGHT_APP_API std::shared_ptr<core::runtime::module> get_module(const std::string& _config_id);

    /**
     * @brief Return all configurations ( standard and template ) register in the registry.
     * @note This method is thread safe.
     */
    SIGHT_APP_API std::vector<std::string> get_all_configs() const;

    /**
     * @brief Return all configurations with specified group.
     * @note This method is thread safe.
     */
    SIGHT_APP_API std::vector<std::string> get_configs_from_group(const std::string& _group) const;

    /**
     * @brief Clear the registry.
     * @note This method is thread safe.
     */
    SIGHT_APP_API void clear_registry();

    /**
     * @brief Create an unique identifier
     * @note This method is thread safe.
     */
    SIGHT_APP_API static std::string get_unique_identifier(const std::string& _service_uid = "");

    /// Return an instance of config.
    SIGHT_APP_API static config::sptr get();

protected:

    using registry = std::map<std::string, app_info::sptr>;

    /// Container of <configId, config information>
    registry m_reg;

private:

    using uid_parameter_replace_t = std::unordered_set<std::string>;

    /// Convert the map into map <pattern, value>.
    static field_adaptor_t map_to_field_adaptor(data::map::csptr _field_adaptors);

    static void collect_uid_for_parameter_replace(
        const std::string& _name,
        const core::runtime::config_t& _cfg_elem,
        uid_parameter_replace_t& _replace_map
    );

    /// Adapts the configuration : replace field thanks to field adaptors
    static core::runtime::config_t adapt_config(
        const core::runtime::config_t& _cfg_elem,
        const field_adaptor_t& _field_adaptors,
        const uid_parameter_replace_t& _uid_parameter_replace,
        const std::string& _auto_prefix_id
    );

    /// Adapts field thanks to field adaptors
    static std::string subst_var(std::string _str, const field_adaptor_t& _variables_map);

    /// Used to protect the registry access.
    mutable core::mt::read_write_mutex m_registry_mutex;

    /// The static identifier for mandatory parameters.
    static std::string s_mandatory_parameter_identifier;

    /// Associations of <tag id, generic-uid attribute>.
    using uid_definition_t = std::multimap<std::string, std::string>;
    static uid_definition_t s_uid_definition_dictionary;
};

} // namespace extension

} // namespace sight::app
