/************************************************************************
 *
 * Copyright (C) 2009-2025 IRCAD France
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

//------------------------------------------------------------------------------

const std::string config_launcher::SELF_KEY = "self";

//------------------------------------------------------------------------------

void config_launcher::parse_config(
    const service::config_t& _config,
    const service::base::sptr& _service
)
{
    const service::config_t& old_config = _config;
    if(old_config.count("appConfig") == 1)
    {
        FW_DEPRECATED_MSG(
            "appConfig attribute deprecated, use `config` property instead.",
            "26.0"
        );

        const service::config_t& app_config = old_config.get_child("appConfig");
        m_config_key = app_config.get<std::string>("<xmlattr>.id", "");
    }

    const service::config_t& app_config_cfg = init_config(old_config, _service);

    if(app_config_cfg.count("parameters") == 1)
    {
        const service::config_t& config_parameters = app_config_cfg.get_child("parameters");

        for(const auto& v : boost::make_iterator_range(config_parameters.equal_range("parameter")))
        {
            m_parameters.emplace_back(
                v.second.get<std::string>("<xmlattr>.replace"),
                v.second.get<std::string>("<xmlattr>.by")
            );
        }
    }
}

//------------------------------------------------------------------------------

service::config_t config_launcher::init_config(
    const service::config_t& _old_config,
    service::base::sptr _service
)
{
    service::config_t srv_cfg;

    size_t i = 0;

    if(const auto inouts_cfg = _old_config.get_child_optional("inout"); inouts_cfg.has_value())
    {
        for(const auto& it_cfg : boost::make_iterator_range(inouts_cfg->equal_range("key")))
        {
            service::config_t parameter_cfg;

            const auto key = it_cfg.second.get<std::string>("<xmlattr>.name");
            SIGHT_ASSERT("[" + _service->get_id() + "] Missing 'key' tag.", !key.empty());

            const auto uid = it_cfg.second.get<std::string>("<xmlattr>.uid");
            SIGHT_ASSERT("[" + _service->get_id() + "] Missing 'uid' tag.", !uid.empty());

            parameter_cfg.add("<xmlattr>.replace", key);

            const bool optional = core::runtime::get_ptree_value(it_cfg.second, "<xmlattr>.optional", false);

            if(optional)
            {
                m_optional_inputs[key] = {uid, i};
                parameter_cfg.add("<xmlattr>.by", uid);
            }
            else
            {
                const auto obj = _service->inout(DATA_GROUP, i).lock();
                SIGHT_ASSERT(std::string("Object key '") + key + "' with uid '" + uid + "' does not exist.", obj);
                parameter_cfg.add("<xmlattr>.by", obj->get_id());
            }

            srv_cfg.add_child("parameters.parameter", parameter_cfg);
            ++i;
        }
    }

    for(const auto& it_cfg : boost::make_iterator_range(_old_config.equal_range("parameter")))
    {
        service::config_t parameter_cfg;

        const auto replace = it_cfg.second.get<std::string>("<xmlattr>.replace");
        SIGHT_ASSERT("[" + _service->get_id() + "] Missing 'replace' tag.", !replace.empty());
        parameter_cfg.add("<xmlattr>.replace", replace);

        const auto by = it_cfg.second.get<std::string>("<xmlattr>.by");
        parameter_cfg.add("<xmlattr>.by", by);

        srv_cfg.add_child("parameters.parameter", parameter_cfg);
    }

    return srv_cfg;
}

//------------------------------------------------------------------------------

void config_launcher::start_config(
    service::base::sptr _service,
    const field_adaptor_t& _opt_replace_map
)
{
    field_adaptor_t replace_map(_opt_replace_map);

    for(const auto& param : m_parameters)
    {
        replace_map[param.first] = param.second;
    }

    // Init manager
    auto config_manager = std::make_shared<app::detail::config_manager>();
    config_manager->set_config(m_config_key, replace_map);

    // When a configuration is launched, deferred objects may already exist.
    // This loop allow to notify the app config manager that this data exist and can be used by services.
    // Without that, the data is considered as null.
    for(const auto& [key, value] : m_optional_inputs)
    {
        const auto obj = _service->inout(DATA_GROUP, value.second).lock();
        if(obj)
        {
            config_manager->add_existing_deferred_object(obj.get_shared(), value.first);
        }
    }

    // Launch config
    config_manager->launch();

    m_config_manager    = config_manager;
    m_config_is_running = true;
}

//------------------------------------------------------------------------------

void config_launcher::stop_config()
{
    if(m_config_is_running)
    {
        // Delete manager
        m_config_manager->stop_and_destroy();
        m_config_manager.reset();
    }

    m_config_is_running = false;
}

//------------------------------------------------------------------------------

void config_launcher::set_config(const std::string& _key)
{
    m_config_key = _key;
}

//------------------------------------------------------------------------------

} // namespace sight::app::helper
