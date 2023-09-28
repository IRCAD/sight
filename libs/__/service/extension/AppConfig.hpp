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

#include "service/config.hpp"
#include "service/extension/AppConfig.hpp"

#include <core/mt/types.hpp>
#include <core/runtime/extension.hpp>
#include <core/tools/object.hpp>

#include <data/Composite.hpp>

#include <map>
#include <unordered_set>

namespace sight::service
{

/// Associations of <pattern, value>.
using FieldAdaptorType = std::map<std::string, std::string>;

namespace extension
{

/**
 */
class SERVICE_CLASS_API AppInfo : public core::base_object
{
public:

    SIGHT_DECLARE_CLASS(AppInfo, core::base_object);

    /// Constructor, do nothing.
    AppInfo() = default;

    /// Destructor, do nothing.
    ~AppInfo() override = default;

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
 *        "sight::service::extension::AppConfig".
 */
class SERVICE_CLASS_API AppConfig : public core::base_object
{
public:

    SIGHT_DECLARE_CLASS(AppConfig, core::base_object);

    SERVICE_API AppConfig();
    SERVICE_API ~AppConfig() override = default;

    /**
     * @brief Parses module information to retrieve configuration declaration.
     * @warning This method must be launch only once. The same extension will not be parsed twice.
     * @note This method is thread safe.
     */
    SERVICE_API void parseBundleInformation();

    /**
     * @brief Parses module information to retrieve configuration declaration.
     * @warning This method must be launch only once. The same extension will not be parsed twice.
     * @note This method is thread safe.
     */
    SERVICE_API void parseBundleInformation(
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
    SERVICE_API void addAppInfo(
        const std::string& configId,
        const std::string& group,
        const std::string& desc,
        const AppInfo::ParametersType& parameters,
        const core::runtime::config_t& config,
        const std::string& moduleId
    );

    /**
     * @brief  Return the adapted config with the identifier configId.
     * @param configId the identifier of the requested config.
     * @param replaceFields associations between the value and the pattern to replace in the config.
     * @note This method is thread safe.
     */
    SERVICE_API core::runtime::config_t getAdaptedTemplateConfig(
        const std::string& configId,
        const FieldAdaptorType replaceFields,
        bool autoPrefixId
    ) const;

    /**
     * @brief  Return the adapted config with the identifier configId.
     * @param configId the identifier of the requested config.
     * @param replaceFields composite of association between the value and the pattern to replace in the config.
     * @note This method is thread safe.
     */
    SERVICE_API core::runtime::config_t getAdaptedTemplateConfig(
        const std::string& configId,
        data::Composite::csptr replaceFields,
        bool autoPrefixId
    )
    const;

    /**
     * @brief Retrieves the module from the config id
     * @param configId the config identifier
     */
    SERVICE_API std::shared_ptr<core::runtime::module> get_module(const std::string& _configId);

    /**
     * @brief Return all configurations ( standard and template ) register in the registry.
     * @note This method is thread safe.
     */
    SERVICE_API std::vector<std::string> getAllConfigs() const;

    /**
     * @brief Return all configurations with specified group.
     * @note This method is thread safe.
     */
    SERVICE_API std::vector<std::string> getConfigsFromGroup(const std::string& group) const;

    /**
     * @brief Clear the registry.
     * @note This method is thread safe.
     */
    SERVICE_API void clearRegistry();

    /**
     * @brief Create an unique identifier
     * @note This method is thread safe.
     */
    SERVICE_API static std::string getUniqueIdentifier(const std::string& serviceUid = "");

    /// Return an instance of AppConfig.
    SERVICE_API static AppConfig::sptr getDefault();

protected:

    typedef std::map<std::string, AppInfo::sptr> Registry;

    /// Container of <configId, AppConfig information>
    Registry m_reg;

private:

    typedef std::unordered_set<std::string> UidParameterReplaceType;

    /// Convert the composite into map <pattern, value>.
    static FieldAdaptorType compositeToFieldAdaptor(data::Composite::csptr fieldAdaptors);

    static void collectUIDForParameterReplace(
        const std::string& _name,
        const core::runtime::config_t& _cfgElem,
        UidParameterReplaceType& replaceMap
    );

    /// Adapts the configuration : replace field thanks to field adaptors
    static core::runtime::config_t adaptConfig(
        const core::runtime::config_t& _cfgElem,
        const FieldAdaptorType& _fieldAdaptors,
        const UidParameterReplaceType& _uidParameterReplace,
        const std::string& _autoPrefixId
    );

    /// Adapts field thanks to field adaptors
    static std::string adaptField(const std::string& _str, const FieldAdaptorType& _variablesMap);

    /// Used to protect the registry access.
    mutable core::mt::read_write_mutex m_registryMutex;

    /// Used to protect the unique identifier creation.
    static core::mt::mutex s_idMutex;

    /// The global instance of the app config.
    static AppConfig::sptr s_currentAppConfig;

    /// The static identifier for mandatory parameters.
    static std::string s_mandatoryParameterIdentifier;

    /// Associations of <tag id, generic-uid attribute>.
    typedef std::multimap<std::string, std::string> UidDefinitionType;
    static UidDefinitionType s_uidDefinitionDictionary;
};

} // namespace extension

} // namespace sight::service
