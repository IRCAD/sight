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

#include "activity.hpp"

#include <core/runtime/module.hpp>
#include <core/runtime/path.hpp>
#include <core/runtime/runtime.hpp>

#include <data/activity.hpp>
#include <data/vector.hpp>

#include <boost/range/iterator_range_core.hpp>

#include <limits>
#include <regex>

namespace sight::activity::extension
{

activity_config_param::activity_config_param(const config_t& config) :
    replace(config.get<std::string>("<xmlattr>.replace")),
    // @deprecated This is no longer necessary to use "uid" to get the prefix replacement, since
    // this is now done in config. However we keep that code for a while for backward compatibility
    by(config.get_optional<std::string>("<xmlattr>.uid").get_value_or(""))
{
    if(by.empty())
    {
        by = config.get<std::string>("<xmlattr>.by");
    }
}

//-----------------------------------------------------------------------------

activity_config::activity_config(const config_t& config) :
    id(config.get<std::string>("<xmlattr>.id"))
{
    if(config.count("parameters") == 1)
    {
        const config_t& configParameters = config.get_child("parameters");

        for(const auto& v : boost::make_iterator_range(configParameters.equal_range("parameter")))
        {
            activity_config_param parameter(v.second);
            parameters.push_back(parameter);
        }
    }

    SIGHT_ASSERT("At most 1 <parameters> tag is allowed", config.count("parameters") < 2);
}

//-----------------------------------------------------------------------------

activity_requirement_key::activity_requirement_key(const config_t& config) :
    key(config.get_value<std::string>())
{
}

//-----------------------------------------------------------------------------

activity_requirement::activity_requirement(const config_t& config) :
    name(config.get<std::string>("<xmlattr>.name")),
    type(config.get<std::string>("<xmlattr>.type")),
    container(config.get<std::string>("<xmlattr>.container", "")),
    description(config.get<std::string>("desc", "")),
    validator(config.get<std::string>("validator", "")),
    minOccurs(config.get<unsigned int>("<xmlattr>.minOccurs", 1)),
    maxOccurs(config.get<unsigned int>("<xmlattr>.maxOccurs", 1)),
    objectConfig(config.get_child("config", config_t()))
{
    for(const auto& v : boost::make_iterator_range(config.equal_range("key")))
    {
        keys.emplace_back(v.second);
    }

    if(config.get<std::string>("<xmlattr>.maxOccurs", "") == "*")
    {
        this->maxOccurs = std::numeric_limits<unsigned int>::max();
    }

    const std::string& createStr = config.get<std::string>("<xmlattr>.create", "false");
    SIGHT_ASSERT("'create' attribute must be 'true' or 'false'", createStr == "true" || createStr == "false");
    create = (createStr == "true");
    SIGHT_ASSERT(
        "Create option is only available if minOccurs = 0 and maxOccurs = 1",
        !create || (minOccurs == 0 && maxOccurs == 1)
    );

    SIGHT_ASSERT(
        "minOccurs value shall be equal or greater than 0 and lower or equal to maxOccurs (" << maxOccurs << ")",
        minOccurs <= maxOccurs
    );

    const std::string& reset_string = config.get<std::string>("<xmlattr>.reset", "false");
    SIGHT_ASSERT("'reset' attribute must be 'true' or 'false'", reset_string == "true" || reset_string == "false");

    reset = (reset_string == "true");

    SIGHT_ASSERT(
        "'Reset' option is only available if 'create' = 'true' or 'minOccurs' = '0'",
        !reset || create || minOccurs == 0
    );
}

//-----------------------------------------------------------------------------

activity_info::activity_info(const SPTR(core::runtime::extension)& ext) :
    id(ext->get_config().get<std::string>("id")),
    title(ext->get_config().get<std::string>("title")),
    description(ext->get_config().get<std::string>("desc")),
    icon(core::runtime::get_module_resource_file_path(ext->get_config().get<std::string>("icon")).string()),
    tabInfo(title),
    bundleId(ext->get_module()->identifier()),
    appConfig(ext->get_config().get_child("appConfig"))
{
    const auto& config = ext->get_config();
    tabInfo = config.get<std::string>("tabinfo", tabInfo);

    if(const auto& requirementsCfg = config.get_child_optional("requirements"); requirementsCfg.has_value())
    {
        for(const auto& elem : boost::make_iterator_range(requirementsCfg->equal_range("requirement")))
        {
            activity_requirement requirement(elem.second);
            requirements.push_back(requirement);

            MinMaxType& minMax = m_requirementCount[requirement.type];

            minMax.first += requirement.minOccurs;

            if(requirement.maxOccurs < (std::numeric_limits<unsigned int>::max() - minMax.second))
            {
                minMax.second += requirement.maxOccurs;
            }
            else
            {
                minMax.second = std::numeric_limits<unsigned int>::max();
            }
        }
    }

    builderImpl = config.get<std::string>("builder", "sight::activity::builder::Activity");

    // backward compatibility
    if(const auto& validatorCfg = config.get_optional<std::string>("validator"); validatorCfg.has_value())
    {
        const auto& validatorImplStr = validatorCfg.value();
        if(!validatorImplStr.empty())
        {
            validatorsImpl.push_back(validatorImplStr);
        }
    }

    if(const auto& validators = config.get_child_optional("validators"); validators.has_value())
    {
        for(const auto& validator : boost::make_iterator_range(validators->equal_range("validator")))
        {
            validatorsImpl.push_back(validator.second.get_value<std::string>());
        }
    }

    // Set Default validator if none is defined
    if(validatorsImpl.empty())
    {
        validatorsImpl.emplace_back("sight::activity::validator::default_activity");
    }
}

//-----------------------------------------------------------------------------

bool activity_info::usableWith(DataCountType dataCounts) const
{
    bool ok = dataCounts.size() <= m_requirementCount.size();

    if(ok)
    {
        for(const RequirementsMinMaxCount::value_type& reqCount : m_requirementCount)
        {
            const MinMaxType& reqMinMax = reqCount.second;
            auto iter                   = dataCounts.find(reqCount.first);
            if(iter != dataCounts.end())
            {
                unsigned int dataCount = iter->second;
                ok = (dataCount != 0U) && reqMinMax.first <= dataCount && dataCount <= reqMinMax.second;
                dataCounts.erase(iter);
            }
            else
            {
                ok = (reqMinMax.first == 0);
            }

            if(!ok)
            {
                break;
            }
        }

        if(ok)
        {
            for(const DataCountType::value_type& dataCount : dataCounts)
            {
                if(m_requirementCount.find(dataCount.first) == m_requirementCount.end())
                {
                    ok = false;
                    break;
                }
            }
        }
    }

    return ok;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------

activity::sptr activity::getDefault()
{
    static auto s_activity = std::make_shared<sight::activity::extension::activity>();
    return s_activity;
}

//-----------------------------------------------------------------------------

void activity::parse_plugin_infos()
{
    std::vector<SPTR(core::runtime::extension)> extensions =
        core::runtime::get_all_extensions_for_point("sight::activity::extension");

    for(const SPTR(core::runtime::extension) & ext : extensions)
    {
        SIGHT_DEBUG("Parsing <" << ext->get_module()->identifier() << "> Activity");
        activity_info info(ext);

        core::mt::write_lock lock(m_registryMutex);
        SIGHT_ASSERT(
            "The id " << info.id << "(" << info.title << ")"
            << " already exists in the Activity registry",
            m_reg.find(info.id) == m_reg.end()
        );
        m_reg.insert(Registry::value_type(info.id, info));
    }
}

//-----------------------------------------------------------------------------

void activity::clear_registry()
{
    core::mt::write_lock lock(m_registryMutex);
    m_reg.clear();
}

//-----------------------------------------------------------------------------

bool activity::hasInfo(const std::string& extensionId) const
{
    core::mt::read_lock lock(m_registryMutex);
    auto iter = m_reg.find(extensionId);
    return iter != m_reg.end();
}

//-----------------------------------------------------------------------------

std::vector<activity_info> activity::getInfos() const
{
    std::vector<activity_info> infos;

    core::mt::read_lock lock(m_registryMutex);

    for(const Registry::value_type& val : m_reg)
    {
        infos.push_back(val.second);
    }

    return infos;
}

//-----------------------------------------------------------------------------

activity_info::DataCountType activity::getDataCount(const data::vector::csptr& data)
{
    activity_info::DataCountType dataCount;

    for(const data::object::csptr obj : *data)
    {
        ++dataCount[obj->get_classname()];
    }

    return dataCount;
}

//-----------------------------------------------------------------------------

std::vector<activity_info> activity::getInfos(const data::vector::csptr& data) const
{
    activity_info::DataCountType dataCount = this->getDataCount(data);
    std::vector<activity_info> infos;

    core::mt::read_lock lock(m_registryMutex);

    for(const Registry::value_type& regValue : m_reg)
    {
        const activity_info& activity = regValue.second;
        if(activity.usableWith(dataCount))
        {
            infos.push_back(activity);
        }
    }

    return infos;
}

//-----------------------------------------------------------------------------

std::vector<std::string> activity::getKeys() const
{
    std::vector<std::string> keys;

    core::mt::read_lock lock(m_registryMutex);

    for(const Registry::value_type& val : m_reg)
    {
        keys.push_back(val.first);
    }

    return keys;
}

//-----------------------------------------------------------------------------

activity_info activity::getInfo(const std::string& extensionId) const
{
    core::mt::read_lock lock(m_registryMutex);
    auto iter = m_reg.find(extensionId);
    SIGHT_ASSERT(
        "The id " << extensionId << " is not found in the application configuration parameter registry",
        iter != m_reg.end()
    );
    return iter->second;
}

//-----------------------------------------------------------------------------

std::tuple<activity_info, std::map<std::string, std::string> > activity::getInfoAndReplacementMap(
    const data::activity& activity,
    const activity_config_params_type& parameters
) const
{
    // Retrieve the activity informations
    const auto& info = getInfo(activity.getActivityConfigId());

    return {info, getReplacementMap(activity, info, parameters)};
}

//------------------------------------------------------------------------------

std::map<std::string, std::string> activity::getReplacementMap(
    const data::activity& activity,
    const activity_info& info,
    const activity_config_params_type& parameters
)
{
    std::map<std::string, std::string> replacement_map;

    // First, use requirements to populate replacement map with an object from the root composite
    for(const auto& requirement : info.requirements)
    {
        // Use the name as "key" for the config parameter
        const auto& it = activity.find(requirement.name);

        if(it != activity.end() && it->second)
        {
            replacement_map[requirement.name] = it->second->get_id();
        }
    }

    const auto add_parameters =
        [&replacement_map](const auto& parameters)
        {
            for(const auto& parameter : parameters)
            {
                // @TODO This should be removed later when legacy xml code is cleaned.
                SIGHT_FATAL_IF(
                    "Cannot replace '" << parameter.replace << "' by CAMP path '" << parameter.by << "'.",
                    parameter.isObjectPath()
                );

                replacement_map[parameter.replace] = parameter.by;
            }
        };

    // Then, use config parameters
    // Parameters will override already present requirements with the same name
    add_parameters(info.appConfig.parameters);

    // Finally, use additional parameters...
    add_parameters(parameters);

    // Store the activity UID
    replacement_map["AS_UID"] = activity.get_id();

    return replacement_map;
}

} // namespace sight::activity::extension
