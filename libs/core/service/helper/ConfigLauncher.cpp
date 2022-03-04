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

#include "service/helper/ConfigLauncher.hpp"

#include <core/runtime/helper.hpp>
#include <core/tools/fwID.hpp>

#include <data/Composite.hpp>
#include <data/String.hpp>

#include <service/macros.hpp>

namespace sight::service
{

namespace helper
{

//------------------------------------------------------------------------------

const std::string ConfigLauncher::s_SELF_KEY        = "self";
const std::string ConfigLauncher::s_GENERIC_UID_KEY = "GENERIC_UID";

//------------------------------------------------------------------------------

ConfigLauncher::ConfigLauncher() :
    m_configIsRunning(false)
{
}

//------------------------------------------------------------------------------

ConfigLauncher::~ConfigLauncher()
{
}

//------------------------------------------------------------------------------

void ConfigLauncher::parseConfig(
    const service::IService::ConfigType& _config,
    const service::IService::sptr& _service
)
{
    service::IService::ConfigType srvCfg;
    const service::IService::ConfigType* curConfig = &_config;

    const service::IService::ConfigType& oldConfig = _config;
    SIGHT_ASSERT("There must be only one <appConfig/> element.", oldConfig.count("appConfig") == 1);

    const service::IService::ConfigType& appConfig = oldConfig.get_child("appConfig");
    const std::string appCfgId                     = appConfig.get<std::string>("<xmlattr>.id");

    srvCfg.add("config.appConfig.<xmlattr>.id", appCfgId);
    service::IService::ConfigType& newCfg = srvCfg.get_child("config.appConfig");
    curConfig = &srvCfg;

    auto inouts = _service->getInOuts();

    auto inoutsCfg = oldConfig.equal_range("inout");
    for(auto itCfg = inoutsCfg.first ; itCfg != inoutsCfg.second ; ++itCfg)
    {
        service::IService::ConfigType parameterCfg;

        const std::string key = itCfg->second.get<std::string>("<xmlattr>.key");
        SIGHT_ASSERT("[" + appCfgId + "] Missing 'key' tag.", !key.empty());

        const std::string uid = itCfg->second.get<std::string>("<xmlattr>.uid");
        SIGHT_ASSERT("[" + appCfgId + "] Missing 'uid' tag.", !uid.empty());

        parameterCfg.add("<xmlattr>.replace", key);

        const bool optional = core::runtime::get_ptree_value(itCfg->second, "<xmlattr>.optional", false);

        if(optional)
        {
            m_optionalInputs[key] = uid;
            parameterCfg.add("<xmlattr>.uid", uid);
        }
        else
        {
            const auto it = inouts.find({key, std::nullopt});
            SIGHT_ASSERT("Inout '" + key + "' is not found.", it != inouts.end());
            auto obj = it->second.lock();
            SIGHT_ASSERT("Object key '" + key + "' with uid '" + uid + "' does not exist.", obj);
            parameterCfg.add("<xmlattr>.uid", obj->getID());
        }

        newCfg.add_child("parameters.parameter", parameterCfg);
    }

    auto paramsCfg = oldConfig.equal_range("parameter");
    for(auto itCfg = paramsCfg.first ; itCfg != paramsCfg.second ; ++itCfg)
    {
        service::IService::ConfigType parameterCfg;

        const std::string replace = itCfg->second.get<std::string>("<xmlattr>.replace");
        SIGHT_ASSERT("[" + appCfgId + "] Missing 'replace' tag.", !replace.empty());
        parameterCfg.add("<xmlattr>.replace", replace);

        const std::string by = itCfg->second.get<std::string>("<xmlattr>.by");
        SIGHT_ASSERT("[" + appCfgId + "] Missing 'by' tag.", !by.empty());
        parameterCfg.add("<xmlattr>.by", by);

        newCfg.add_child("parameters.parameter", parameterCfg);
    }

    SIGHT_ASSERT("There must be only one <config/> element.", curConfig->count("config") == 1);

    const service::IService::ConfigType& srvconfig = curConfig->get_child("config");

    SIGHT_ASSERT("There must be only one <appConfig/> element.", srvconfig.count("appConfig") == 1);

    const service::IService::ConfigType& appConfigCfg = srvconfig.get_child("appConfig");
    m_appConfig = activity::extension::ActivityAppConfig(appConfigCfg);
}

//------------------------------------------------------------------------------

void ConfigLauncher::startConfig(
    service::IService::sptr _srv,
    const FieldAdaptorType& _optReplaceMap
)
{
    FieldAdaptorType replaceMap(_optReplaceMap);

    // Generate generic UID
    const std::string genericUidAdaptor = service::extension::AppConfig::getUniqueIdentifier(_srv->getID());
    replaceMap[ConfigLauncher::s_GENERIC_UID_KEY] = genericUidAdaptor;

    for(const auto& param : m_appConfig.parameters)
    {
        replaceMap[param.replace] = param.by;
    }

    // Init manager
    m_appConfigManager = service::AppConfigManager::New();
    m_appConfigManager->setConfig(m_appConfig.id, replaceMap);

    auto inouts = _srv->getInOuts();

    // When a configuration is launched, deferred objects may already exist.
    // This loop allow to notify the app config manager that this data exist and can be used by services.
    // Whitout that, the data is considered as null.
    for(const auto& [key, uid] : m_optionalInputs)
    {
        const auto it = inouts.find({key, 0});
        if(it != inouts.end())
        {
            auto obj = it->second.lock();
            m_appConfigManager->addExistingDeferredObject(obj.get_shared(), uid);
        }
    }

    // Launch config
    m_appConfigManager->launch();

    m_configIsRunning = true;
}

//------------------------------------------------------------------------------

void ConfigLauncher::stopConfig()
{
    if(m_configIsRunning)
    {
        // Delete manager
        m_appConfigManager->stopAndDestroy();
        m_appConfigManager.reset();
    }

    m_configIsRunning = false;
}

//------------------------------------------------------------------------------

} // helper

} // fwServices
