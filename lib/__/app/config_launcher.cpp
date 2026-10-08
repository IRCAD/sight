/************************************************************************
 *
 * Copyright (C) 2009-2026 IRCAD France
 * Copyright (C) 2012-2019 IHU Strasbourg
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

#include "app/config_launcher.hpp"

#include <core/com/proxy.hpp>

namespace sight::app
{

static const std::string S_CLOSE_CONFIG_ID = "CLOSE_CONFIG";

//------------------------------------------------------------------------------

config_launcher::config_launcher() noexcept :
    m_config_launcher(std::make_unique<app::helper::config_launcher>())
{
    new_signal<signals::launched_t>(signals::LAUNCHED);
}

//------------------------------------------------------------------------------

service::connections_t config_launcher::auto_connections() const
{
    return {
        {m_config_id, sight::data::signals::MODIFIED, slots::UPDATE},
        {m_nested_config_id, sight::data::signals::MODIFIED, slots::UPDATE}
    };
}

//------------------------------------------------------------------------------

std::string config_launcher::config_id() const
{
    const auto nested = *m_nested_config_id;
    return nested.empty() ? *m_config_id : nested;
}

//------------------------------------------------------------------------------

void config_launcher::configuring(const config_t& _config)
{
    m_config_launcher->parse_config(_config, this->get_sptr());
    m_proxy_channel = this->get_id() + "_stop_config";
}

//------------------------------------------------------------------------------

std::optional<std::string> config_launcher::resolve_object_type(
    std::string_view _key,
    std::optional<std::size_t> _index
) const
{
    // The type of the objects forwarded to the sub-configuration is declared by that sub-configuration, which is only
    // known when the service starts, since it may be set at runtime through the "config" property. The objects are
    // thus built at that moment, see app::helper::config_launcher::start_config().
    if(_index.has_value()
       && (_key == helper::config_launcher::OBJECT_GROUP || _key == helper::config_launcher::DATA_GROUP))
    {
        return std::nullopt;
    }

    return base::resolve_object_type(_key, _index);
}

//------------------------------------------------------------------------------

void config_launcher::starting()
{
    const auto config = this->config_id();
    if(not config.empty())
    {
        m_config_launcher->set_config(config);
    }

    if(not m_config_launcher->config().empty())
    {
        this->start_config();
    }
}

//------------------------------------------------------------------------------

void config_launcher::stopping()
{
    this->stop_config();
}

//------------------------------------------------------------------------------

void config_launcher::updating()
{
    bool start = !m_config_launcher->config_is_running();

    const auto new_config = this->config_id();

    // If the configuration is different from the current one
    if(m_config_launcher->config() != new_config)
    {
        // Set the new configuration to start
        m_config_launcher->set_config(new_config);

        // Stop the current configuration and force restart
        if(!start)
        {
            start = true;
            this->stop_config();
        }
    }

    if(start)
    {
        this->start_config();
    }
}

//------------------------------------------------------------------------------

void config_launcher::start_config()
{
    if(!m_config_launcher->config_is_running())
    {
        core::com::proxy::sptr proxies = core::com::proxy::get();
        proxies->connect(m_proxy_channel, this->slot(service::base::slots::STOP));
        app::field_adaptor_t replace_map;
        replace_map[S_CLOSE_CONFIG_ID] = m_proxy_channel;
        try
        {
            m_config_launcher->start_config(this->get_sptr(), replace_map);
            this->async_emit(signals::LAUNCHED);
        }
        catch(const std::exception& /*e*/)
        {
            // Disconnect to avoid inconsistent state
            proxies->disconnect(m_proxy_channel, this->slot(service::base::slots::STOP));

            // Rethrow the same exception
            throw;
        }
    }
}

//------------------------------------------------------------------------------

void config_launcher::stop_config()
{
    if(m_config_launcher->config_is_running())
    {
        m_config_launcher->stop_config();
        core::com::proxy::sptr proxies = core::com::proxy::get();
        proxies->disconnect(m_proxy_channel, this->slot(service::base::slots::STOP));
    }
}

//------------------------------------------------------------------------------

} // namespace sight::app
