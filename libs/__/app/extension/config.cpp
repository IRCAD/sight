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

#include "app/extension/config.hpp"

#include <core/runtime/module.hpp>
#include <core/runtime/runtime.hpp>

#include <data/composite.hpp>
#include <data/string.hpp>

#include <boost/algorithm/string.hpp>

#include <regex>

namespace sight::app::extension
{

std::string config::s_mandatoryParameterIdentifier = "@mandatory@";

config::UidDefinitionType config::s_uidDefinitionDictionary = {{"object", "uid"},
    {"service", "uid"},
    {"view", "sid"},
    {"view", "wid"},
    {"editor", "wid"},
    {"slideView", "wid"},
    {"connect", "channel"},
    {"menuItem", "sid"},
    {"channel", "uid"},
};
static const std::regex s_isVariable(R"(\$\{.*\}.*)");

//-----------------------------------------------------------------------------

config::sptr config::getDefault()
{
    static config::sptr s_currentAppConfig = std::make_shared<config>();
    return s_currentAppConfig;
}

//-----------------------------------------------------------------------------

void config::parse_plugin_infos()
{
    auto extensions = core::runtime::get_all_extensions_for_point("sight::app::extension::config");
    for(const auto& ext : extensions)
    {
        const auto& appConfig = ext->get_config();

        const auto configId = appConfig.get<std::string>("id");
        const auto group    = appConfig.get<std::string>("group", "");
        const auto desc     = appConfig.get<std::string>("desc", "No description available");

        app_info::ParametersType parameters;

        if(const auto parametersCfg = appConfig.get_child_optional("parameters"); parametersCfg.has_value())
        {
            for(const auto& param : boost::make_iterator_range(parametersCfg->equal_range("param")))
            {
                const auto name = param.second.get<std::string>("<xmlattr>.name");
                parameters[name] = param.second.get<std::string>("<xmlattr>.default", s_mandatoryParameterIdentifier);
            }
        }

        // Get config
        const auto config = appConfig.get_child("config");

        // Get module
        std::shared_ptr<core::runtime::module> module = ext->get_module();
        std::string moduleId                          = module->identifier();

        // Add app info
        this->addapp_info(configId, group, desc, parameters, config, moduleId);
    }
}

//-----------------------------------------------------------------------------

void config::addapp_info(
    const std::string& configId,
    const std::string& group,
    const std::string& desc,
    const app_info::ParametersType& parameters,
    const core::runtime::config_t& config,
    const std::string& moduleId
)
{
    core::mt::write_lock lock(m_registryMutex);

    SIGHT_DEBUG("New app config registering : configId = " + configId);
    SIGHT_ASSERT(
        "The app config with the id = " << configId << " already exist.",
        m_reg.find(configId) == m_reg.end()
    );

    app_info::sptr info = std::make_shared<app_info>();
    info->group      = group;
    info->desc       = desc;
    info->config     = config;
    info->parameters = parameters;
    info->moduleId   = moduleId;
    m_reg[configId]  = info;
}

//-----------------------------------------------------------------------------

config::config()
= default;

//-----------------------------------------------------------------------------

void config::clear_registry()
{
    core::mt::write_lock lock(m_registryMutex);
    m_reg.clear();
}

//-----------------------------------------------------------------------------

core::runtime::config_t config::getAdaptedTemplateConfig(
    const std::string& configId,
    const field_adaptor_t fieldAdaptors,
    bool autoPrefixId
) const
{
    core::mt::read_lock lock(m_registryMutex);
    // Get config template
    auto iter = m_reg.find(configId);
    SIGHT_ASSERT(
        "The id " << configId << " is not found in the application configuration registry",
        iter != m_reg.end()
    );

    // Adapt config
    core::runtime::config_t newConfig;

    field_adaptor_t fields;
    app_info::ParametersType parameters = iter->second->parameters;

    for(const app_info::ParametersType::value_type& param : parameters)
    {
        auto iterField        = fieldAdaptors.find(param.first);
        const std::string key = "\\$\\{" + param.first + "\\}";
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
        autoPrefixName = sight::app::extension::config::getUniqueIdentifier(configId);
    }

    UidParameterReplaceType parameterReplaceAdaptors;
    sight::app::extension::config::collectUIDForParameterReplace(
        "config",
        iter->second->config,
        parameterReplaceAdaptors
    );
    newConfig = sight::app::extension::config::adaptConfig(
        iter->second->config,
        fields,
        parameterReplaceAdaptors,
        autoPrefixName
    );

    return newConfig;
}

//-----------------------------------------------------------------------------

core::runtime::config_t config::getAdaptedTemplateConfig(
    const std::string& configId,
    data::composite::csptr replaceFields,
    bool autoPrefixId
)
const
{
    field_adaptor_t fieldAdaptors = compositeToFieldAdaptor(replaceFields);
    return this->getAdaptedTemplateConfig(configId, fieldAdaptors, autoPrefixId);
}

//-----------------------------------------------------------------------------

std::shared_ptr<core::runtime::module> config::get_module(const std::string& _configId)
{
    auto iter = m_reg.find(_configId);
    SIGHT_ASSERT(
        "The id " << _configId << " is not found in the application configuration registry",
        iter != m_reg.end()
    );

    auto module = core::runtime::find_module(iter->second->moduleId);

    return module;
}

//-----------------------------------------------------------------------------

std::vector<std::string> config::getAllConfigs() const
{
    core::mt::read_lock lock(m_registryMutex);
    std::vector<std::string> ids;
    for(const Registry::value_type& elem : m_reg)
    {
        ids.push_back(elem.first);
    }

    return ids;
}

//-----------------------------------------------------------------------------

std::vector<std::string> config::getConfigsFromGroup(const std::string& group) const
{
    core::mt::read_lock lock(m_registryMutex);
    std::vector<std::string> ids;
    for(const Registry::value_type& elem : m_reg)
    {
        app_info::sptr info = elem.second;
        if(info->group == group)
        {
            ids.push_back(elem.first);
        }
    }

    return ids;
}

//-----------------------------------------------------------------------------

field_adaptor_t config::compositeToFieldAdaptor(data::composite::csptr fieldAdaptors)
{
    field_adaptor_t fields;
    for(const data::composite::value_type& elem : *fieldAdaptors)
    {
        fields[elem.first] = std::dynamic_pointer_cast<data::string>(elem.second)->value();
    }

    return fields;
}

//-----------------------------------------------------------------------------

std::string config::getUniqueIdentifier(const std::string& serviceUid)
{
    static core::mt::mutex s_idMutex;
    core::mt::scoped_lock lock(s_idMutex);

    static unsigned int srvCpt = 1;
    std::stringstream sstr;

    if(serviceUid.empty())
    {
        sstr << "config_manager_" << srvCpt;
    }
    else
    {
        sstr << serviceUid << "_" << srvCpt;
    }

    ++srvCpt;
    return sstr.str();
}

//-----------------------------------------------------------------------------

void config::collectUIDForParameterReplace(
    const std::string& _name,
    const core::runtime::config_t& _cfgElem,
    UidParameterReplaceType& _replaceMap
)
{
    const auto& attributes = _cfgElem.get_child_optional("<xmlattr>");

    if(attributes)
    {
        for(const auto& attribute : *attributes)
        {
            auto range = s_uidDefinitionDictionary.equal_range(_name);

            for(auto it = range.first ; it != range.second ; ++it)
            {
                const auto attrValue = attribute.second.get_value<std::string>();
                if(it->second == attribute.first && !std::regex_match(attrValue, s_isVariable))
                {
                    _replaceMap.insert(attrValue);
                }
            }
        }
    }

    // Check if a service if used only on signal/slot
    for(const auto& subElem : _cfgElem)
    {
        if(subElem.first == "signal" || subElem.first == "slot")
        {
            std::vector<std::string> tokens;
            boost::split(tokens, subElem.second.get_value<std::string>(), boost::is_any_of("/"));
            _replaceMap.insert(tokens[0]);
        }

        collectUIDForParameterReplace(subElem.first, subElem.second, _replaceMap);
    }
}

//-----------------------------------------------------------------------------

core::runtime::config_t config::adaptConfig(
    const core::runtime::config_t& _cfgElem,
    const field_adaptor_t& _fieldAdaptors,
    const UidParameterReplaceType& _uidParameterReplace,
    const std::string& _autoPrefixId
)
{
    core::runtime::config_t result;
    result.put_value<std::string>(adaptField(_cfgElem.get_value<std::string>(), _fieldAdaptors));

    const auto& attributes = _cfgElem.get_child_optional("<xmlattr>");

    if(attributes)
    {
        for(const auto& attribute : *attributes)
        {
            const auto attributeValue = attribute.second.get_value<std::string>();

            // Add the config prefix for unique identifiers
            if(!_autoPrefixId.empty())
            {
                if(attribute.first == "uid"
                   || attribute.first == "sid"
                   || attribute.first == "wid"
                   || attribute.first == "channel")
                {
                    // Detect if we have a variable name
                    if(!std::regex_match(attributeValue, s_isVariable))
                    {
                        // This is not a variable, add the prefix
                        result.put(
                            "<xmlattr>." + attribute.first,
                            _autoPrefixId + "_" + adaptField(attributeValue, _fieldAdaptors)
                        );
                        continue;
                    }
                }
                // Special case for <parameter replace="..." by="..." />
                else if(attribute.first == "by")
                {
                    // Detect if we have a variable name
                    if(!std::regex_match(attributeValue, s_isVariable))
                    {
                        // Look inside the map of potential replacements
                        auto itParam = _uidParameterReplace.find(attributeValue);
                        if(itParam != _uidParameterReplace.end())
                        {
                            result.put(
                                "<xmlattr>." + attribute.first,
                                _autoPrefixId + "_"
                                + adaptField(attributeValue, _fieldAdaptors)
                            );
                            continue;
                        }
                    }
                }
            }

            result.put("<xmlattr>." + attribute.first, adaptField(attributeValue, _fieldAdaptors));
        }
    }

    for(const auto& subElem : _cfgElem)
    {
        // Add the config prefix for unique identifiers in signal and slot sources
        if(!_autoPrefixId.empty() && (subElem.first == "signal" || subElem.first == "slot"))
        {
            // Detect if we have a variable name
            if(!std::regex_match(subElem.second.get_value<std::string>(), s_isVariable))
            {
                // This is not a variable, add the prefix
                core::runtime::config_t elt;
                elt.put_value(_autoPrefixId + "_" + subElem.second.get_value<std::string>());

                const auto& subAttributes = _cfgElem.get_child_optional("<xmlattr>");

                if(subAttributes)
                {
                    for(const auto& attribute : *subAttributes)
                    {
                        elt.put("<xmlattr>." + attribute.first, attribute.second.get_value<std::string>());
                    }
                }

                result.add_child(subElem.first, elt);
                continue;
            }
        }

        result.add_child(
            subElem.first,
            adaptConfig(subElem.second, _fieldAdaptors, _uidParameterReplace, _autoPrefixId)
        );
    }

    return result;
}

//-----------------------------------------------------------------------------

std::string config::adaptField(const std::string& _str, const field_adaptor_t& _variablesMap)
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

} // namespace sight::app::extension
