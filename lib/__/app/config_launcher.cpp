/************************************************************************
 *
 * Copyright (C) 2009-2025 IRCAD France
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
#include <core/com/signal.hxx>
#include <core/com/slots.hxx>

namespace sight::app
{

static const std::string S_CLOSE_CONFIG_CHANNEL_ID = "CLOSE_CONFIG_CHANNEL";

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
        {m_config_id, sight::data::object::MODIFIED_SIG, slots::UPDATE}
    };
}

//------------------------------------------------------------------------------

void config_launcher::configuring(const config_t& _config)
{
    m_config_launcher->parse_config(_config, this->get_sptr());
    m_proxy_channel = this->get_id() + "_stop_config";
}

//------------------------------------------------------------------------------

void config_launcher::starting()
{
    const auto config = *m_config_id;
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

    const auto new_config = *m_config_id;

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
        replace_map[S_CLOSE_CONFIG_CHANNEL_ID] = m_proxy_channel;
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
