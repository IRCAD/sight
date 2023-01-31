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

#include "service/helper/ConfigLauncher.hpp"

#include "service/detail/AppConfigManager.hpp"

#include <core/runtime/helper.hpp>

#include <boost/range/iterator_range_core.hpp>
namespace sight::service::helper
{

//------------------------------------------------------------------------------

const std::string ConfigLauncher::s_SELF_KEY        = "self";
const std::string ConfigLauncher::s_GENERIC_UID_KEY = "GENERIC_UID";

//------------------------------------------------------------------------------

void ConfigLauncher::parseConfig(
    const service::IService::ConfigType& _config,
    const service::IService::sptr& _service
)
{
    service::IService::ConfigType srvCfg;

    const service::IService::ConfigType& oldConfig = _config;
    SIGHT_ASSERT("There must be only one <appConfig/> element.", oldConfig.count("appConfig") == 1);

    const service::IService::ConfigType& appConfig = oldConfig.get_child("appConfig");
    const auto appCfgId                            = appConfig.get<std::string>("<xmlattr>.id");

    srvCfg.add("config.appConfig.<xmlattr>.id", appCfgId);
    service::IService::ConfigType& newCfg          = srvCfg.get_child("config.appConfig");
    const service::IService::ConfigType* curConfig = &srvCfg;

    size_t i = 0;

    if(const auto inoutsCfg = oldConfig.get_child_optional("inout"); inoutsCfg.has_value())
    {
        for(const auto& itCfg : boost::make_iterator_range(inoutsCfg->equal_range("key")))
        {
            service::IService::ConfigType parameterCfg;

            const auto key = itCfg.second.get<std::string>("<xmlattr>.name");
            SIGHT_ASSERT("[" + appCfgId + "] Missing 'key' tag.", !key.empty());

            const auto uid = itCfg.second.get<std::string>("<xmlattr>.uid");
            SIGHT_ASSERT("[" + appCfgId + "] Missing 'uid' tag.", !uid.empty());

            parameterCfg.add("<xmlattr>.replace", key);

            const bool optional = core::runtime::get_ptree_value(itCfg.second, "<xmlattr>.optional", false);

            if(optional)
            {
                m_optionalInputs[key] = {uid, i};
                parameterCfg.add("<xmlattr>.uid", uid);
            }
            else
            {
                const auto obj = _service->getInOut(s_DATA_GROUP, i).lock();
                SIGHT_ASSERT(std::string("Object key '") + key + "' with uid '" + uid + "' does not exist.", obj);
                parameterCfg.add("<xmlattr>.uid", obj->getID());
            }

            newCfg.add_child("parameters.parameter", parameterCfg);
            ++i;
        }
    }

    for(const auto& itCfg : boost::make_iterator_range(oldConfig.equal_range("parameter")))
    {
        service::IService::ConfigType parameterCfg;

        const auto replace = itCfg.second.get<std::string>("<xmlattr>.replace");
        SIGHT_ASSERT("[" + appCfgId + "] Missing 'replace' tag.", !replace.empty());
        parameterCfg.add("<xmlattr>.replace", replace);

        const auto by = itCfg.second.get<std::string>("<xmlattr>.by");
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
    service::IService::sptr _service,
    const FieldAdaptorType& _optReplaceMap
)
{
    FieldAdaptorType replaceMap(_optReplaceMap);

    // Generate generic UID
    const std::string genericUidAdaptor = service::extension::AppConfig::getUniqueIdentifier(_service->getID());
    replaceMap[ConfigLauncher::s_GENERIC_UID_KEY] = genericUidAdaptor;

    for(const auto& param : m_appConfig.parameters)
    {
        replaceMap[param.replace] = param.by;
    }

    // Init manager
    auto appConfigManager = service::detail::AppConfigManager::New();
    appConfigManager->setConfig(m_appConfig.id, replaceMap);

    // When a configuration is launched, deferred objects may already exist.
    // This loop allow to notify the app config manager that this data exist and can be used by services.
    // Without that, the data is considered as null.
    for(const auto& [key, value] : m_optionalInputs)
    {
        const auto obj = _service->getInOut(s_DATA_GROUP, value.second).lock();
        if(obj)
        {
            appConfigManager->addExistingDeferredObject(obj.get_shared(), value.first);
        }
    }

    // Launch config
    appConfigManager->launch();

    m_appConfigManager = appConfigManager;
    m_configIsRunning  = true;
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

} // namespace sight::service::helper
