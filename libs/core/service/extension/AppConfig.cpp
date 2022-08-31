/************************************************************************
 *
 * Copyright (C) 2009-2022 IRCAD France
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

#include "service/extension/AppConfig.hpp"

#include <core/runtime/ConfigurationElement.hpp>
#include <core/runtime/helper.hpp>
#include <core/runtime/Module.hpp>
#include <core/runtime/Runtime.hpp>

#include <data/Composite.hpp>
#include <data/String.hpp>

#include <boost/algorithm/string/classification.hpp>
#include <boost/algorithm/string/split.hpp>

#include <regex>

namespace sight::service
{

namespace extension
{

AppConfig::sptr AppConfig::s_currentAppConfig = AppConfig::New();
core::mt::Mutex AppConfig::s_idMutex;

std::string AppConfig::s_mandatoryParameterIdentifier = "@mandatory@";

AppConfig::UidDefinitionType AppConfig::s_uidDefinitionDictionary = {{"object", "uid"},
    {"service", "uid"},
    {"view", "sid"},
    {"view", "wid"},
    {"editor", "wid"},
    {"slideView", "wid"},
    {"connect", "channel"},
    {"menuItem", "sid"},
};
static const std::regex s_isVariable("\\$\\{.*\\}.*");

//-----------------------------------------------------------------------------

AppConfig::sptr AppConfig::getDefault()
{
    return s_currentAppConfig;
}

//-----------------------------------------------------------------------------

AppConfig::~AppConfig()
{
}

//-----------------------------------------------------------------------------

void AppConfig::parseBundleInformation()
{
    auto extensions = core::runtime::getAllExtensionsForPoint("sight::service::extension::AppConfig");
    for(const auto& ext : extensions)
    {
        // Get id
        std::string configId = ext->findConfigurationElement("id")->getValue();

        // Get group
        std::string group = "";
        if(ext->hasConfigurationElement("group"))
        {
            group = ext->findConfigurationElement("group")->getValue();
        }

        // Get desc
        std::string desc = "No description available";
        if(ext->hasConfigurationElement("desc"))
        {
            desc = ext->findConfigurationElement("desc")->getValue();
        }

        AppInfo::ParametersType parameters;
        if(ext->hasConfigurationElement("parameters"))
        {
            core::runtime::ConfigurationElement::csptr parametersConfig = ext->findConfigurationElement("parameters");
            core::runtime::ConfigurationElement::Container elements     = parametersConfig->getElements();
            for(core::runtime::ConfigurationElement::sptr paramConfig : elements)
            {
                std::string name = paramConfig->getExistingAttributeValue("name");

                if(paramConfig->hasAttribute("default"))
                {
                    parameters[name] = paramConfig->getAttributeValue("default");
                }
                else
                {
                    parameters[name] = s_mandatoryParameterIdentifier;
                }
            }
        }

        // Get config
        core::runtime::ConfigurationElement::csptr config = ext->findConfigurationElement("config");

        // Get module
        std::shared_ptr<core::runtime::Module> module = ext->getModule();
        std::string moduleId                          = module->getIdentifier();

        // Add app info
        this->addAppInfo(configId, group, desc, parameters, config, moduleId);
    }
}

//-----------------------------------------------------------------------------

void AppConfig::addAppInfo(
    const std::string& configId,
    const std::string& group,
    const std::string& desc,
    const AppInfo::ParametersType& parameters,
    const core::runtime::ConfigurationElement::csptr& config,
    const std::string& moduleId
)
{
    core::mt::WriteLock lock(m_registryMutex);

    SIGHT_DEBUG("New app config registering : configId = " + configId);
    SIGHT_ASSERT(
        "The app config with the id = " << configId << " already exist.",
        m_reg.find(configId) == m_reg.end()
    );

    AppInfo::sptr info = AppInfo::New();
    info->group      = group;
    info->desc       = desc;
    info->config     = config;
    info->parameters = parameters;
    info->moduleId   = moduleId;
    m_reg[configId]  = info;
}

//-----------------------------------------------------------------------------

AppConfig::AppConfig()
{
}

//-----------------------------------------------------------------------------

void AppConfig::clearRegistry()
{
    core::mt::WriteLock lock(m_registryMutex);
    m_reg.clear();
}

//-----------------------------------------------------------------------------

core::runtime::ConfigurationElement::csptr AppConfig::getAdaptedTemplateConfig(
    const std::string& configId,
    const FieldAdaptorType fieldAdaptors,
    bool autoPrefixId
) const
{
    core::mt::ReadLock lock(m_registryMutex);
    // Get config template
    Registry::const_iterator iter = m_reg.find(configId);
    SIGHT_ASSERT(
        "The id " << configId << " is not found in the application configuration registry",
        iter != m_reg.end()
    );

    // Adapt config
    core::runtime::ConfigurationElement::sptr newConfig;

    FieldAdaptorType fields;
    AppInfo::ParametersType parameters = iter->second->parameters;

    for(AppInfo::ParametersType::value_type param : parameters)
    {
        FieldAdaptorType::const_iterator iterField = fieldAdaptors.find(param.first);
        const std::string key                      = "\\$\\{" + param.first + "\\}";
        if(iterField != fieldAdaptors.end())
        {
            fields[key] = iterField->second;
        }
        else if(param.second != s_mandatoryParameterIdentifier)
        {
            fields[key] = param.second;
        }
        else
        {
            SIGHT_THROW(
                "Parameter : '" << param.first << "' is needed by the app configuration id='" << configId
                << "'."
            );
        }
    }

    std::string autoPrefixName;
    if(autoPrefixId)
    {
        autoPrefixName = this->getUniqueIdentifier(configId);
    }

    UidParameterReplaceType parameterReplaceAdaptors;
    this->collectUIDForParameterReplace(iter->second->config, parameterReplaceAdaptors);
    newConfig = this->adaptConfig(iter->second->config, fields, parameterReplaceAdaptors, autoPrefixName);

    return newConfig;
}

//-----------------------------------------------------------------------------

core::runtime::ConfigurationElement::csptr AppConfig::getAdaptedTemplateConfig(
    const std::string& configId,
    data::Composite::csptr replaceFields,
    bool autoPrefixId
)
const
{
    FieldAdaptorType fieldAdaptors = compositeToFieldAdaptor(replaceFields);
    return this->getAdaptedTemplateConfig(configId, fieldAdaptors, autoPrefixId);
}

//-----------------------------------------------------------------------------

std::shared_ptr<core::runtime::Module> AppConfig::getModule(const std::string& _configId)
{
    Registry::const_iterator iter = m_reg.find(_configId);
    SIGHT_ASSERT(
        "The id " << _configId << " is not found in the application configuration registry",
        iter != m_reg.end()
    );

    auto module = core::runtime::findModule(iter->second->moduleId);

    return module;
}

//-----------------------------------------------------------------------------

std::vector<std::string> AppConfig::getAllConfigs() const
{
    core::mt::ReadLock lock(m_registryMutex);
    std::vector<std::string> ids;
    for(const Registry::value_type& elem : m_reg)
    {
        ids.push_back(elem.first);
    }

    return ids;
}

//-----------------------------------------------------------------------------

std::vector<std::string> AppConfig::getConfigsFromGroup(const std::string& group) const
{
    core::mt::ReadLock lock(m_registryMutex);
    std::vector<std::string> ids;
    for(const Registry::value_type& elem : m_reg)
    {
        AppInfo::sptr info = elem.second;
        if(info->group == group)
        {
            ids.push_back(elem.first);
        }
    }

    return ids;
}

//-----------------------------------------------------------------------------

FieldAdaptorType AppConfig::compositeToFieldAdaptor(data::Composite::csptr fieldAdaptors) const
{
    FieldAdaptorType fields;
    for(const data::Composite::value_type& elem : *fieldAdaptors)
    {
        fields[elem.first] = data::String::dynamicCast(elem.second)->value();
    }

    return fields;
}

//-----------------------------------------------------------------------------

std::string AppConfig::getUniqueIdentifier(const std::string& serviceUid)
{
    core::mt::ScopedLock lock(s_idMutex);
    static unsigned int srvCpt = 1;
    std::stringstream sstr;

    if(serviceUid.empty())
    {
        sstr << "AppConfigManager_" << srvCpt;
    }
    else
    {
        sstr << serviceUid << "_" << srvCpt;
    }

    ++srvCpt;
    return sstr.str();
}

//-----------------------------------------------------------------------------

void AppConfig::collectUIDForParameterReplace(
    core::runtime::ConfigurationElement::csptr _cfgElem,
    UidParameterReplaceType& _replaceMap
)
{
    const auto& name = _cfgElem->getName();
    for(const auto& attribute : _cfgElem->getAttributes())
    {
        auto range = s_uidDefinitionDictionary.equal_range(name);

        for(auto it = range.first ; it != range.second ; ++it)
        {
            if(it->second == attribute.first && !std::regex_match(attribute.second, s_isVariable))
            {
                _replaceMap.insert(attribute.second);
            }
        }
    }

    // Check if a service if used only on signal/slot
    for(const auto& subElem : _cfgElem->getElements())
    {
        if(subElem->getName() == "signal" || subElem->getName() == "slot")
        {
            std::vector<std::string> tokens;
            boost::split(tokens, subElem->getValue(), boost::is_any_of("/"));
            _replaceMap.insert(tokens[0]);
        }

        collectUIDForParameterReplace(subElem, _replaceMap);
    }
}

//-----------------------------------------------------------------------------

core::runtime::EConfigurationElement::sptr AppConfig::adaptConfig(
    core::runtime::ConfigurationElement::csptr _cfgElem,
    const FieldAdaptorType& _fieldAdaptors,
    const UidParameterReplaceType& _uidParameterReplace,
    const std::string& _autoPrefixId
)
{
    core::runtime::EConfigurationElement::sptr result =
        core::runtime::EConfigurationElement::New(_cfgElem->getName());
    result->setValue(adaptField(_cfgElem->getValue(), _fieldAdaptors));

    for(const auto& attribute : _cfgElem->getAttributes())
    {
        // Add the config prefix for unique identifiers
        if(!_autoPrefixId.empty())
        {
            if(attribute.first == "uid"
               || attribute.first == "sid"
               || attribute.first == "wid"
               || attribute.first == "channel")
            {
                // Detect if we have a variable name
                if(!std::regex_match(attribute.second, s_isVariable))
                {
                    // This is not a variable, add the prefix
                    result->setAttributeValue(
                        attribute.first,
                        _autoPrefixId + "_" + adaptField(attribute.second, _fieldAdaptors)
                    );
                    continue;
                }
            }
            // Special case for <parameter replace="..." by="..." />
            else if(attribute.first == "by")
            {
                // Detect if we have a variable name
                if(!std::regex_match(attribute.second, s_isVariable))
                {
                    // Look inside the map of potential replacements
                    auto itParam = _uidParameterReplace.find(attribute.second);
                    if(itParam != _uidParameterReplace.end())
                    {
                        result->setAttributeValue(
                            attribute.first,
                            _autoPrefixId + "_"
                            + adaptField(attribute.second, _fieldAdaptors)
                        );
                        continue;
                    }
                }
            }
        }

        result->setAttributeValue(attribute.first, adaptField(attribute.second, _fieldAdaptors));
    }

    for(const auto& subElem : _cfgElem->getElements())
    {
        // Add the config prefix for unique identifiers in signal and slot sources
        if(!_autoPrefixId.empty() && (subElem->getName() == "signal" || subElem->getName() == "slot"))
        {
            // Detect if we have a variable name
            if(!std::regex_match(subElem->getValue(), s_isVariable))
            {
                // This is not a variable, add the prefix
                auto elt = core::runtime::EConfigurationElement::New(subElem->getName());
                elt->setValue(_autoPrefixId + "_" + subElem->getValue());

                for(const auto& attr : subElem->getAttributes())
                {
                    elt->setAttributeValue(attr.first, attr.second);
                }

                result->addConfigurationElement(elt);
                continue;
            }
        }

        result->addConfigurationElement(adaptConfig(subElem, _fieldAdaptors, _uidParameterReplace, _autoPrefixId));
    }

    return result;
}

//-----------------------------------------------------------------------------

std::string AppConfig::adaptField(const std::string& _str, const FieldAdaptorType& _variablesMap)
{
    std::string newStr = _str;
    if(!_str.empty())
    {
        // Discriminate first variable expressions only, looking through all keys of the replace map is not for free
        // However we look inside the whole string instead of only at the beginning because we want  to replace "inner"
        // variables as well, i.e. not only ${uid} but also uid${suffix}
        if(std::regex_search(_str, s_isVariable))
        {
            // Iterate over all variables
            for(const auto& fieldAdaptor : _variablesMap)
            {
                const std::regex varRegex("(.*)" + fieldAdaptor.first + "(.*)");
                if(std::regex_match(_str, varRegex))
                {
                    const std::string varReplace("\\1" + fieldAdaptor.second + "\\2");
                    newStr = std::regex_replace(
                        newStr,
                        varRegex,
                        varReplace,
                        std::regex_constants::match_default
                        | std::regex_constants::format_sed
                    );
                }
            }
        }
    }

    return newStr;
}

//-----------------------------------------------------------------------------

} // namespace extension

} // namespace sight::service
