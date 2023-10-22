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

#include "app/helper/config_launcher.hpp"

#include "app/detail/config_manager.hpp"

#include <core/runtime/helper.hpp>

#include <boost/range/iterator_range_core.hpp>
namespace sight::app::helper
{

//-----------------------------------------------------------------------------

config_launcher::Parameters::Parameters(const service::config_t& _config) :
    m_id(_config.get<std::string>("<xmlattr>.id"))
{
    if(_config.count("parameters") == 1)
    {
        const service::config_t& config_parameters = _config.get_child("parameters");

        for(const auto& v : boost::make_iterator_range(config_parameters.equal_range("parameter")))
        {
            m_parameters.emplace_back(
                v.second.get<std::string>("<xmlattr>.replace"),
                v.second.get<std::string>("<xmlattr>.by")

            );
        }
    }

    SIGHT_ASSERT("At most 1 <parameters> tag is allowed", _config.count("parameters") < 2);
}

//------------------------------------------------------------------------------

const std::string config_launcher::s_SELF_KEY        = "self";
const std::string config_launcher::s_GENERIC_UID_KEY = "GENERIC_UID";

//------------------------------------------------------------------------------

void config_launcher::parseConfig(
    const service::config_t& _config,
    const service::base::sptr& _service
)
{
    const service::config_t& old_config = _config;
    SIGHT_ASSERT("There must be only one <appConfig/> element.", old_config.count("appConfig") == 1);

    const service::config_t& app_config = old_config.get_child("appConfig");
    const auto app_cfg_id_element       = app_config.get_optional<std::string>("<xmlattr>.id");

    //if there is an id attribute in the config, there is a unique config to load
    //else there are several possible configs, and need to load all of them
    if(app_cfg_id_element.has_value())
    {
        const auto& app_cfg_id                  = *app_cfg_id_element;
        const service::config_t& app_config_cfg = initConfig(app_cfg_id, old_config, _service);
        m_appConfigParameters[m_configKey] = Parameters(app_config_cfg);
    }
    else
    {
        //there are several config used. Parse them all
        const auto subconfig_list = app_config.equal_range("config");
        for(auto subconfig = subconfig_list.first ;
            subconfig != subconfig_list.second ;
            ++subconfig)
        {
            const auto config_name = subconfig->second.get<std::string>("<xmlattr>.name");
            const auto config_id   = subconfig->second.get<std::string>("<xmlattr>.id");
            if(!m_appConfigParameters.contains(config_id))
            {
                const service::config_t& app_config_cfg = initConfig(config_id, old_config, _service);
                m_appConfigParameters[config_name] = Parameters(app_config_cfg);
                m_configKey                        = config_name;
            }
            else
            {
                SIGHT_ERROR("The key " << config_id << " is used twice to identify configs in configLauncher");
                break;
            }
        }

        if(const auto default_config = app_config.get_optional<std::string>(
               "<xmlattr>.default"
        ); default_config.has_value())
        {
            setConfig(*default_config);
        }
    }
}

//------------------------------------------------------------------------------

service::config_t config_launcher::initConfig(
    const std::string& _app_cfg_id,
    const service::config_t& _old_config,
    service::base::sptr _service
)
{
    service::config_t srv_cfg;
    srv_cfg.add("config.appConfig.<xmlattr>.id", _app_cfg_id);
    service::config_t& new_cfg          = srv_cfg.get_child("config.appConfig");
    const service::config_t* cur_config = &srv_cfg;

    size_t i = 0;

    if(const auto inouts_cfg = _old_config.get_child_optional("inout"); inouts_cfg.has_value())
    {
        for(const auto& it_cfg : boost::make_iterator_range(inouts_cfg->equal_range("key")))
        {
            service::config_t parameter_cfg;

            const auto key = it_cfg.second.get<std::string>("<xmlattr>.name");
            SIGHT_ASSERT("[" + _app_cfg_id + "] Missing 'key' tag.", !key.empty());

            const auto uid = it_cfg.second.get<std::string>("<xmlattr>.uid");
            SIGHT_ASSERT("[" + _app_cfg_id + "] Missing 'uid' tag.", !uid.empty());

            parameter_cfg.add("<xmlattr>.replace", key);

            const bool optional = core::runtime::get_ptree_value(it_cfg.second, "<xmlattr>.optional", false);

            if(optional)
            {
                m_optionalInputs[key] = {uid, i};
                parameter_cfg.add("<xmlattr>.by", uid);
            }
            else
            {
                const auto obj = _service->inout(s_DATA_GROUP, i).lock();
                SIGHT_ASSERT(std::string("Object key '") + key + "' with uid '" + uid + "' does not exist.", obj);
                parameter_cfg.add("<xmlattr>.by", obj->get_id());
            }

            new_cfg.add_child("parameters.parameter", parameter_cfg);
            ++i;
        }
    }

    for(const auto& it_cfg : boost::make_iterator_range(_old_config.equal_range("parameter")))
    {
        service::config_t parameter_cfg;

        const auto replace = it_cfg.second.get<std::string>("<xmlattr>.replace");
        SIGHT_ASSERT("[" + _app_cfg_id + "] Missing 'replace' tag.", !replace.empty());
        parameter_cfg.add("<xmlattr>.replace", replace);

        const auto by = it_cfg.second.get<std::string>("<xmlattr>.by");
        parameter_cfg.add("<xmlattr>.by", by);

        new_cfg.add_child("parameters.parameter", parameter_cfg);
    }

    SIGHT_ASSERT("There must be only one <config/> element.", cur_config->count("config") == 1);

    const service::config_t& srvconfig = cur_config->get_child("config");

    SIGHT_ASSERT("There must be only one <appConfig/> element.", srvconfig.count("appConfig") == 1);

    return srvconfig.get_child("appConfig");
}

//------------------------------------------------------------------------------

void config_launcher::startConfig(
    service::base::sptr _service,
    const field_adaptor_t& _opt_replace_map
)
{
    field_adaptor_t replace_map(_opt_replace_map);

    // Generate generic UID
    const std::string generic_uid_adaptor = app::extension::config::getUniqueIdentifier(_service->get_id());
    replace_map[config_launcher::s_GENERIC_UID_KEY] = generic_uid_adaptor;

    //get the right appConfig
    const auto app_config = m_appConfigParameters[m_configKey];

    for(const auto& param : app_config.m_parameters)
    {
        replace_map[param.first] = param.second;
    }

    // Init manager
    auto config_manager = std::make_shared<app::detail::config_manager>();
    config_manager->setConfig(app_config.m_id, replace_map);

    // When a configuration is launched, deferred objects may already exist.
    // This loop allow to notify the app config manager that this data exist and can be used by services.
    // Without that, the data is considered as null.
    for(const auto& [key, value] : m_optionalInputs)
    {
        const auto obj = _service->inout(s_DATA_GROUP, value.second).lock();
        if(obj)
        {
            config_manager->addExistingDeferredObject(obj.get_shared(), value.first);
        }
    }

    // Launch config
    config_manager->launch();

    m_config_manager  = config_manager;
    m_configIsRunning = true;
}

//------------------------------------------------------------------------------

void config_launcher::stopConfig()
{
    if(m_configIsRunning)
    {
        // Delete manager
        m_config_manager->stopAndDestroy();
        m_config_manager.reset();
    }

    m_configIsRunning = false;
}

//------------------------------------------------------------------------------

void config_launcher::setConfig(const std::string& _key)
{
    if(m_appConfigParameters.contains(_key))
    {
        m_configKey = _key;
    }
    else
    {
        SIGHT_ERROR("The is no config registered with key " << _key);
    }
}

//------------------------------------------------------------------------------

} // namespace sight::app::helper
