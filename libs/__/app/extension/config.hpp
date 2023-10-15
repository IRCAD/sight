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

#include "service/extension/config.hpp"

#include <core/mt/types.hpp>
#include <core/runtime/extension.hpp>
#include <core/tools/object.hpp>

#include <data/composite.hpp>

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
class APP_CLASS_API app_info : public core::base_object
{
public:

    SIGHT_DECLARE_CLASS(app_info, core::base_object);

    /// Constructor, do nothing.
    app_info() = default;

    /// Destructor, do nothing.
    ~app_info() override = default;

    std::string group;
    std::string desc;
    typedef std::map<std::string, std::string> ParametersType;
    ParametersType parameters;
    core::runtime::config_t config;
    std::string moduleId;      ///< Module identifier (used to start the module when the appConfig is launched)
    std::string moduleVersion; ///< Module version (used to start the module when the appConfig is launched)
};

/**
 * @brief This class allows to register all the configuration which has the point extension
 *        "sight::app::extension::config".
 */
class APP_CLASS_API config : public core::base_object
{
public:

    SIGHT_DECLARE_CLASS(config, core::base_object);

    APP_API config();
    APP_API ~config() override = default;

    /**
     * @brief Parses module information to retrieve configuration declaration.
     * @warning This method must be launch only once. The same extension will not be parsed twice.
     * @note This method is thread safe.
     */
    APP_API void parse_plugin_infos();

    /**
     * @brief Parses module information to retrieve configuration declaration.
     * @warning This method must be launch only once. The same extension will not be parsed twice.
     * @note This method is thread safe.
     */
    APP_API void parse_plugin_infos(
        const std::vector<std::shared_ptr<core::runtime::extension> >& extensions
    );

    /**
     * @brief Register a new config.
     * @param configId the identifier of the registered config.
     * @param type the type of the added config. It can be TEMPLATE, STANDARD or PARAMETERS.
     * @param group the name of the group that owns the config.
     * @param desc the config description.
     * @param parameters the list of template parameters in the config.
     * @param config the registered config.
     * @note This method is thread safe
     */
    APP_API void addapp_info(
        const std::string& configId,
        const std::string& group,
        const std::string& desc,
        const app_info::ParametersType& parameters,
        const core::runtime::config_t& config,
        const std::string& moduleId
    );

    /**
     * @brief  Return the adapted config with the identifier configId.
     * @param configId the identifier of the requested config.
     * @param replaceFields associations between the value and the pattern to replace in the config.
     * @note This method is thread safe.
     */
    APP_API core::runtime::config_t getAdaptedTemplateConfig(
        const std::string& configId,
        const field_adaptor_t replaceFields,
        bool autoPrefixId
    ) const;

    /**
     * @brief  Return the adapted config with the identifier configId.
     * @param configId the identifier of the requested config.
     * @param replaceFields composite of association between the value and the pattern to replace in the config.
     * @note This method is thread safe.
     */
    APP_API core::runtime::config_t getAdaptedTemplateConfig(
        const std::string& configId,
        data::composite::csptr replaceFields,
        bool autoPrefixId
    )
    const;

    /**
     * @brief Retrieves the module from the config id
     * @param configId the config identifier
     */
    APP_API std::shared_ptr<core::runtime::module> get_module(const std::string& _configId);

    /**
     * @brief Return all configurations ( standard and template ) register in the registry.
     * @note This method is thread safe.
     */
    APP_API std::vector<std::string> getAllConfigs() const;

    /**
     * @brief Return all configurations with specified group.
     * @note This method is thread safe.
     */
    APP_API std::vector<std::string> getConfigsFromGroup(const std::string& group) const;

    /**
     * @brief Clear the registry.
     * @note This method is thread safe.
     */
    APP_API void clear_registry();

    /**
     * @brief Create an unique identifier
     * @note This method is thread safe.
     */
    APP_API static std::string getUniqueIdentifier(const std::string& serviceUid = "");

    /// Return an instance of config.
    APP_API static config::sptr getDefault();

protected:

    typedef std::map<std::string, app_info::sptr> Registry;

    /// Container of <configId, config information>
    Registry m_reg;

private:

    typedef std::unordered_set<std::string> UidParameterReplaceType;

    /// Convert the composite into map <pattern, value>.
    static field_adaptor_t compositeToFieldAdaptor(data::composite::csptr fieldAdaptors);

    static void collectUIDForParameterReplace(
        const std::string& _name,
        const core::runtime::config_t& _cfgElem,
        UidParameterReplaceType& replaceMap
    );

    /// Adapts the configuration : replace field thanks to field adaptors
    static core::runtime::config_t adaptConfig(
        const core::runtime::config_t& _cfgElem,
        const field_adaptor_t& _fieldAdaptors,
        const UidParameterReplaceType& _uidParameterReplace,
        const std::string& _autoPrefixId
    );

    /// Adapts field thanks to field adaptors
    static std::string adaptField(const std::string& _str, const field_adaptor_t& _variablesMap);

    /// Used to protect the registry access.
    mutable core::mt::read_write_mutex m_registryMutex;

    /// The static identifier for mandatory parameters.
    static std::string s_mandatoryParameterIdentifier;

    /// Associations of <tag id, generic-uid attribute>.
    typedef std::multimap<std::string, std::string> UidDefinitionType;
    static UidDefinitionType s_uidDefinitionDictionary;
};

} // namespace extension

} // namespace sight::app
