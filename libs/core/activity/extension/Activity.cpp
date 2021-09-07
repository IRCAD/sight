/************************************************************************
 *
 * Copyright (C) 2009-2021 IRCAD France
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

#include "activity/extension/Activity.hpp"

#include <core/runtime/ConfigurationElement.hpp>
#include <core/runtime/Convert.hpp>
#include <core/runtime/helper.hpp>
#include <core/runtime/Module.hpp>
#include <core/runtime/Runtime.hpp>

#include <data/Vector.hpp>

#include <boost/foreach.hpp>

#include <limits>
#include <regex>

namespace sight::activity
{

namespace extension
{

ActivityAppConfigParam::ActivityAppConfigParam(const ConfigType& config) :
    replace(config.get<std::string>("<xmlattr>.replace"))
{
    // @deprecated This is no longer necessary to use "uid" to get the prefix replacement, since
    // this is now done in AppConfig. However we keep that code for a while for backward compatibility
    by = config.get_optional<std::string>("<xmlattr>.uid").get_value_or("");
    if(by.empty())
    {
        by = config.get<std::string>("<xmlattr>.by");
    }
}

//-----------------------------------------------------------------------------

ActivityAppConfig::ActivityAppConfig(const ConfigType& config) :
    id(config.get<std::string>("<xmlattr>.id"))
{
    if(config.count("parameters") == 1)
    {
        const ConfigType& configParameters = config.get_child("parameters");
        BOOST_FOREACH(const ConfigType::value_type& v, configParameters.equal_range("parameter"))
        {
            ActivityAppConfigParam parameter(v.second);
            parameters.push_back(parameter);
        }
    }

    SIGHT_ASSERT("At most 1 <parameters> tag is allowed", config.count("parameters") < 2);
}

//-----------------------------------------------------------------------------

ActivityRequirementKey::ActivityRequirementKey(const ConfigType& config) :
    key(config.get_value<std::string>()),
    path(config.get_optional<std::string>("<xmlattr>.path").get_value_or(""))
{
}

//-----------------------------------------------------------------------------

ActivityRequirement::ActivityRequirement(const ConfigType& config) :
    name(config.get<std::string>("<xmlattr>.name")),
    type(config.get<std::string>("<xmlattr>.type")),
    container(config.get_optional<std::string>("<xmlattr>.container").get_value_or("")),
    description(config.get_optional<std::string>("desc").get_value_or("")),
    validator(config.get_optional<std::string>("validator").get_value_or("")),
    minOccurs(config.get_optional<unsigned int>("<xmlattr>.minOccurs").get_value_or(1)),
    maxOccurs(config.get_optional<unsigned int>("<xmlattr>.maxOccurs").get_value_or(1))
{
    BOOST_FOREACH(const ConfigType::value_type& v, config.equal_range("key"))
    {
        keys.push_back(ActivityRequirementKey(v.second));
    }

    if(config.get_optional<std::string>("<xmlattr>.maxOccurs").get_value_or("") == "*")
    {
        this->maxOccurs = std::numeric_limits<unsigned int>::max();
    }

    std::string createStr = config.get_optional<std::string>("<xmlattr>.create").get_value_or("false");
    SIGHT_ASSERT("'create' attribute must be 'true' or 'false'", createStr == "true" || createStr == "false");
    create = (createStr == "true");
    SIGHT_ASSERT(
        "Create option is only available if minOccurs = 0 and maxOccurs = 1",
        !create || (minOccurs == 0 && maxOccurs == 1)
    );

    SIGHT_ASSERT(
        "minOccurs value shall be equal or greater than 0 and lower or equal to maxOccurs (" << maxOccurs << ")",
        0 <= minOccurs && minOccurs <= maxOccurs
    );
}

//-----------------------------------------------------------------------------

ActivityInfo::ActivityInfo(const SPTR(core::runtime::Extension)& ext) :
    id(ext->findConfigurationElement("id")->getValue()),
    title(ext->findConfigurationElement("title")->getValue()),
    description(ext->findConfigurationElement("desc")->getValue()),
    icon(core::runtime::getModuleResourceFilePath(ext->findConfigurationElement("icon")->getValue()).string()),
    tabInfo(title),
    bundleId(ext->getModule()->getIdentifier()),
    appConfig(core::runtime::Convert::toPropertyTree(ext->findConfigurationElement("appConfig")).get_child("appConfig"))
{
    if(ext->findConfigurationElement("tabinfo"))
    {
        tabInfo = ext->findConfigurationElement("tabinfo")->getValue();
    }

    core::runtime::ConfigurationElement::sptr req = ext->findConfigurationElement("requirements");
    for(core::runtime::ConfigurationElementContainer::Iterator elem = req->begin() ;
        elem != req->end() ;
        ++elem)
    {
        ActivityRequirement requirement(core::runtime::Convert::toPropertyTree(*elem).get_child("requirement"));
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

    core::runtime::ConfigurationElement::csptr builderCfg = ext->findConfigurationElement("builder");
    if(builderCfg)
    {
        builderImpl = builderCfg->getValue();
    }
    else
    {
        builderImpl = "::sight::activity::builder::ActivitySeriesInitData";
    }

    // backward compatibility
    core::runtime::ConfigurationElement::csptr validatorCfg = ext->findConfigurationElement("validator");
    if(validatorCfg)
    {
        std::string validatorImplStr = validatorCfg->getValue();
        if(!validatorImplStr.empty())
        {
            validatorsImpl.push_back(validatorImplStr);
        }
    }

    core::runtime::ConfigurationElement::sptr validatorsCfg = ext->findConfigurationElement("validators");
    if(validatorsCfg)
    {
        auto validators = core::runtime::Convert::toPropertyTree(validatorsCfg).get_child("validators");
        BOOST_FOREACH(auto const& validator, validators.equal_range("validator"))
        {
            validatorsImpl.push_back(validator.second.get_value<std::string>());
        }
    }

    // Set Default validator if none is defined
    if(validatorsImpl.empty())
    {
        validatorsImpl.push_back("::sight::activity::validator::DefaultActivity");
    }
}

//-----------------------------------------------------------------------------

bool ActivityInfo::usableWith(DataCountType dataCounts) const
{
    bool ok = dataCounts.size() <= m_requirementCount.size();

    if(ok)
    {
        for(const RequirementsMinMaxCount::value_type& reqCount : m_requirementCount)
        {
            const MinMaxType& reqMinMax  = reqCount.second;
            DataCountType::iterator iter = dataCounts.find(reqCount.first);
            if(iter != dataCounts.end())
            {
                unsigned int dataCount = iter->second;
                ok = dataCount && reqMinMax.first <= dataCount && dataCount <= reqMinMax.second;
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

Activity::sptr Activity::s_activity = Activity::New();

//-----------------------------------------------------------------------------

Activity::sptr Activity::getDefault()
{
    return s_activity;
}

//-----------------------------------------------------------------------------

Activity::~Activity()
{
}

//-----------------------------------------------------------------------------

void Activity::parseBundleInformation()
{
    std::vector<SPTR(core::runtime::Extension)> extensions =
        core::runtime::getAllExtensionsForPoint("sight::activity::extension::Activity");

    for(const SPTR(core::runtime::Extension) & ext : extensions)
    {
        SIGHT_DEBUG("Parsing <" << ext->getModule()->getIdentifier() << "> Activity");
        ActivityInfo info(ext);

        core::mt::WriteLock lock(m_registryMutex);
        SIGHT_ASSERT(
            "The id " << info.id << "(" << info.title << ")"
            << " already exists in the Activity registry",
            m_reg.find(info.id) == m_reg.end()
        );
        m_reg.insert(Registry::value_type(info.id, info));
    }
}

//-----------------------------------------------------------------------------

Activity::Activity()
{
}

//-----------------------------------------------------------------------------

void Activity::clearRegistry()
{
    core::mt::WriteLock lock(m_registryMutex);
    m_reg.clear();
}

//-----------------------------------------------------------------------------

bool Activity::hasInfo(const std::string& extensionId) const
{
    core::mt::ReadLock lock(m_registryMutex);
    Registry::const_iterator iter = m_reg.find(extensionId);
    return iter != m_reg.end();
}

//-----------------------------------------------------------------------------

std::vector<ActivityInfo> Activity::getInfos() const
{
    std::vector<ActivityInfo> infos;

    core::mt::ReadLock lock(m_registryMutex);

    for(Registry::value_type val : m_reg)
    {
        infos.push_back(val.second);
    }

    return infos;
}

//-----------------------------------------------------------------------------

ActivityInfo::DataCountType Activity::getDataCount(const data::Vector::csptr& data) const
{
    ActivityInfo::DataCountType dataCount;

    for(const data::Object::csptr& obj : *data)
    {
        ++dataCount[obj->getClassname()];
    }

    return dataCount;
}

//-----------------------------------------------------------------------------

std::vector<ActivityInfo> Activity::getInfos(const data::Vector::csptr& data) const
{
    ActivityInfo::DataCountType dataCount = this->getDataCount(data);
    std::vector<ActivityInfo> infos;

    core::mt::ReadLock lock(m_registryMutex);

    for(const Registry::value_type& regValue : m_reg)
    {
        const ActivityInfo& activity = regValue.second;
        if(activity.usableWith(dataCount))
        {
            infos.push_back(activity);
        }
    }

    return infos;
}

//-----------------------------------------------------------------------------

std::vector<std::string> Activity::getKeys() const
{
    std::vector<std::string> keys;

    core::mt::ReadLock lock(m_registryMutex);

    for(Registry::value_type val : m_reg)
    {
        keys.push_back(val.first);
    }

    return keys;
}

//-----------------------------------------------------------------------------

const ActivityInfo Activity::getInfo(const std::string& extensionId) const
{
    core::mt::ReadLock lock(m_registryMutex);
    Registry::const_iterator iter = m_reg.find(extensionId);
    SIGHT_ASSERT(
        "The id " << extensionId << " is not found in the application configuration parameter registry",
        iter != m_reg.end()
    );
    return iter->second;
}

//-----------------------------------------------------------------------------

} // namespace extension

} // namespace sight::activity
