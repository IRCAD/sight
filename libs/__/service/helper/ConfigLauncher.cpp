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

#include "service/detail/app_config_manager.hpp"

#include <core/runtime/helper.hpp>

#include <boost/range/iterator_range_core.hpp>
namespace sight::service::helper
{

//-----------------------------------------------------------------------------

ConfigLauncher::Parameters::Parameters(const service::config_t& config) :
    m_id(config.get<std::string>("<xmlattr>.id"))
{
    if(config.count("parameters") == 1)
    {
        const service::config_t& configParameters = config.get_child("parameters");

        for(const auto& v : boost::make_iterator_range(configParameters.equal_range("parameter")))
        {
            m_parameters.emplace_back(
                v.second.get<std::string>("<xmlattr>.replace"),
                v.second.get<std::string>("<xmlattr>.by")

            );
        }
    }

    SIGHT_ASSERT("At most 1 <parameters> tag is allowed", config.count("parameters") < 2);
}

//------------------------------------------------------------------------------

const std::string ConfigLauncher::s_SELF_KEY        = "self";
const std::string ConfigLauncher::s_GENERIC_UID_KEY = "GENERIC_UID";

//------------------------------------------------------------------------------

void ConfigLauncher::parseConfig(
    const service::config_t& _config,
    const service::base::sptr& _service
)
{
    const service::config_t& oldConfig = _config;
    SIGHT_ASSERT("There must be only one <appConfig/> element.", oldConfig.count("appConfig") == 1);

    const service::config_t& appConfig = oldConfig.get_child("appConfig");
    const auto appCfgIdElement         = appConfig.get_optional<std::string>("<xmlattr>.id");

    //if there is an id attribute in the config, there is a unique config to load
    //else there are several possible configs, and need to load all of them
    if(appCfgIdElement.has_value())
    {
        const auto& appCfgId                  = *appCfgIdElement;
        const service::config_t& appConfigCfg = initConfig(appCfgId, oldConfig, _service);
        m_appConfigParameters[m_configKey] = Parameters(appConfigCfg);
    }
    else
    {
        //there are several config used. Parse them all
        const auto subconfigList = appConfig.equal_range("config");
        for(auto subconfig = subconfigList.first ;
            subconfig != subconfigList.second ;
            ++subconfig)
        {
            const auto configName = subconfig->second.get<std::string>("<xmlattr>.name");
            const auto configId   = subconfig->second.get<std::string>("<xmlattr>.id");
            if(!m_appConfigParameters.contains(configId))
            {
                const service::config_t& appConfigCfg = initConfig(configId, oldConfig, _service);
                m_appConfigParameters[configName] = Parameters(appConfigCfg);
                m_configKey                       = configName;
            }
            else
            {
                SIGHT_ERROR("The key " << configId << " is used twice to identify configs in configLauncher");
                break;
            }
        }

        if(const auto defaultConfig = appConfig.get_optional<std::string>(
               "<xmlattr>.default"
        ); defaultConfig.has_value())
        {
            setConfig(*defaultConfig);
        }
    }
}

//------------------------------------------------------------------------------

service::config_t ConfigLauncher::initConfig(
    const std::string& appCfgId,
    const service::config_t& oldConfig,
    service::base::sptr _service
)
{
    service::config_t srvCfg;
    srvCfg.add("config.appConfig.<xmlattr>.id", appCfgId);
    service::config_t& newCfg          = srvCfg.get_child("config.appConfig");
    const service::config_t* curConfig = &srvCfg;

    size_t i = 0;

    if(const auto inoutsCfg = oldConfig.get_child_optional("inout"); inoutsCfg.has_value())
    {
        for(const auto& itCfg : boost::make_iterator_range(inoutsCfg->equal_range("key")))
        {
            service::config_t parameterCfg;

            const auto key = itCfg.second.get<std::string>("<xmlattr>.name");
            SIGHT_ASSERT("[" + appCfgId + "] Missing 'key' tag.", !key.empty());

            const auto uid = itCfg.second.get<std::string>("<xmlattr>.uid");
            SIGHT_ASSERT("[" + appCfgId + "] Missing 'uid' tag.", !uid.empty());

            parameterCfg.add("<xmlattr>.replace", key);

            const bool optional = core::runtime::get_ptree_value(itCfg.second, "<xmlattr>.optional", false);

            if(optional)
            {
                m_optionalInputs[key] = {uid, i};
                parameterCfg.add("<xmlattr>.by", uid);
            }
            else
            {
                const auto obj = _service->inout(s_DATA_GROUP, i).lock();
                SIGHT_ASSERT(std::string("Object key '") + key + "' with uid '" + uid + "' does not exist.", obj);
                parameterCfg.add("<xmlattr>.by", obj->get_id());
            }

            newCfg.add_child("parameters.parameter", parameterCfg);
            ++i;
        }
    }

    for(const auto& itCfg : boost::make_iterator_range(oldConfig.equal_range("parameter")))
    {
        service::config_t parameterCfg;

        const auto replace = itCfg.second.get<std::string>("<xmlattr>.replace");
        SIGHT_ASSERT("[" + appCfgId + "] Missing 'replace' tag.", !replace.empty());
        parameterCfg.add("<xmlattr>.replace", replace);

        const auto by = itCfg.second.get<std::string>("<xmlattr>.by");
        parameterCfg.add("<xmlattr>.by", by);

        newCfg.add_child("parameters.parameter", parameterCfg);
    }

    SIGHT_ASSERT("There must be only one <config/> element.", curConfig->count("config") == 1);

    const service::config_t& srvconfig = curConfig->get_child("config");

    SIGHT_ASSERT("There must be only one <appConfig/> element.", srvconfig.count("appConfig") == 1);

    return srvconfig.get_child("appConfig");
}

//------------------------------------------------------------------------------

void ConfigLauncher::startConfig(
    service::base::sptr _service,
    const FieldAdaptorType& _optReplaceMap
)
{
    FieldAdaptorType replaceMap(_optReplaceMap);

    // Generate generic UID
    const std::string genericUidAdaptor = service::extension::AppConfig::getUniqueIdentifier(_service->get_id());
    replaceMap[ConfigLauncher::s_GENERIC_UID_KEY] = genericUidAdaptor;

    //get the right appConfig
    const auto appConfig = m_appConfigParameters[m_configKey];

    for(const auto& param : appConfig.m_parameters)
    {
        replaceMap[param.first] = param.second;
    }

    // Init manager
    auto app_config_manager = std::make_shared<service::detail::app_config_manager>();
    app_config_manager->setConfig(appConfig.m_id, replaceMap);

    // When a configuration is launched, deferred objects may already exist.
    // This loop allow to notify the app config manager that this data exist and can be used by services.
    // Without that, the data is considered as null.
    for(const auto& [key, value] : m_optionalInputs)
    {
        const auto obj = _service->inout(s_DATA_GROUP, value.second).lock();
        if(obj)
        {
            app_config_manager->addExistingDeferredObject(obj.get_shared(), value.first);
        }
    }

    // Launch config
    app_config_manager->launch();

    m_app_config_manager = app_config_manager;
    m_configIsRunning    = true;
}

//------------------------------------------------------------------------------

void ConfigLauncher::stopConfig()
{
    if(m_configIsRunning)
    {
        // Delete manager
        m_app_config_manager->stopAndDestroy();
        m_app_config_manager.reset();
    }

    m_configIsRunning = false;
}

//------------------------------------------------------------------------------

void ConfigLauncher::setConfig(const std::string& key)
{
    if(m_appConfigParameters.contains(key))
    {
        m_configKey = key;
    }
    else
    {
        SIGHT_ERROR("The is no config registered with key " << key);
    }
}

//------------------------------------------------------------------------------

} // namespace sight::service::helper
