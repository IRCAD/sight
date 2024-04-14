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

#include "config_launcher.hpp"

#include <core/com/proxy.hpp>
#include <core/com/signal.hxx>
#include <core/com/slots.hxx>

#include <service/macros.hpp>

namespace sight::module::ui
{

//------------------------------------------------------------------------------

const core::com::signals::key_t config_launcher::LAUNCHED_SIG = "launched";

static const core::com::slots::key_t STOP_CONFIG_SLOT = "stopConfig";

static const std::string CLOSE_CONFIG_CHANNEL_ID = "CLOSE_CONFIG_CHANNEL";

//------------------------------------------------------------------------------

config_launcher::config_launcher() noexcept :
    m_sig_launched(new_signal<launched_signal_t>(LAUNCHED_SIG)),
    m_config_launcher(std::make_unique<app::helper::config_launcher>())
{
    new_slot(STOP_CONFIG_SLOT, &config_launcher::stop_config, this);
}

//------------------------------------------------------------------------------

config_launcher::~config_launcher() noexcept =
    default;

//------------------------------------------------------------------------------

void config_launcher::starting()
{
    m_proxychannel = this->get_id() + "_stopConfig";

    this->action_service_starting();
}

//------------------------------------------------------------------------------

void config_launcher::stopping()
{
    this->stop_config();
    this->action_service_stopping();
}

//------------------------------------------------------------------------------

void config_launcher::configuring()
{
    this->initialize();

    m_config_launcher->parse_config(this->get_config(), this->get_sptr());
}

//-----------------------------------------------------------------------------

void config_launcher::set_checked(bool _is_checked)
{
    this->sight::ui::action::set_checked(_is_checked);
    if(_is_checked)
    {
        // Check if the config is already running, this avoids to start a running config.
        if(!m_config_launcher->config_is_running())
        {
            core::com::proxy::sptr proxies = core::com::proxy::get();
            proxies->connect(m_proxychannel, this->slot(STOP_CONFIG_SLOT));
            app::field_adaptor_t replace_map;
            replace_map[CLOSE_CONFIG_CHANNEL_ID] = m_proxychannel;
            m_config_launcher->start_config(this->get_sptr(), replace_map);
            m_sig_launched->async_emit();
        }
    }
    else
    {
        this->stop_config();
    }
}

//------------------------------------------------------------------------------

void config_launcher::updating()
{
}

//------------------------------------------------------------------------------

void config_launcher::stop_config()
{
    if(m_config_launcher->config_is_running())
    {
        m_config_launcher->stop_config();
        core::com::proxy::sptr proxies = core::com::proxy::get();
        proxies->disconnect(m_proxychannel, this->slot(STOP_CONFIG_SLOT));
        this->set_checked(false);
    }
}

//------------------------------------------------------------------------------

} // namespace sight::module::ui
