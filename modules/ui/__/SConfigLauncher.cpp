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

#include "SConfigLauncher.hpp"

#include <core/com/proxy.hpp>
#include <core/com/signal.hxx>
#include <core/com/slots.hxx>

#include <service/macros.hpp>

namespace sight::module::ui
{

//------------------------------------------------------------------------------

const core::com::signals::key_t SConfigLauncher::LAUNCHED_SIG = "launched";

static const core::com::slots::key_t STOP_CONFIG_SLOT = "stopConfig";

static const std::string s_CLOSE_CONFIG_CHANNEL_ID = "CLOSE_CONFIG_CHANNEL";

//------------------------------------------------------------------------------

SConfigLauncher::SConfigLauncher() noexcept :
    m_sigLaunched(new_signal<LaunchedSignalType>(LAUNCHED_SIG)),
    m_configLauncher(std::make_unique<service::helper::ConfigLauncher>())
{
    new_slot(STOP_CONFIG_SLOT, &SConfigLauncher::stopConfig, this);
}

//------------------------------------------------------------------------------

SConfigLauncher::~SConfigLauncher() noexcept =
    default;

//------------------------------------------------------------------------------

void SConfigLauncher::starting()
{
    m_proxychannel = this->get_id() + "_stopConfig";

    this->actionServiceStarting();
}

//------------------------------------------------------------------------------

void SConfigLauncher::stopping()
{
    this->stopConfig();
    this->actionServiceStopping();
}

//------------------------------------------------------------------------------

void SConfigLauncher::configuring()
{
    this->initialize();

    m_configLauncher->parseConfig(this->getConfiguration(), this->get_sptr());
}

//-----------------------------------------------------------------------------

void SConfigLauncher::setChecked(bool isChecked)
{
    this->sight::ui::action::setChecked(isChecked);
    if(isChecked)
    {
        // Check if the config is already running, this avoids to start a running config.
        if(!m_configLauncher->configIsRunning())
        {
            core::com::proxy::sptr proxies = core::com::proxy::get();
            proxies->connect(m_proxychannel, this->slot(STOP_CONFIG_SLOT));
            service::FieldAdaptorType replaceMap;
            replaceMap[s_CLOSE_CONFIG_CHANNEL_ID] = m_proxychannel;
            m_configLauncher->startConfig(this->get_sptr(), replaceMap);
            m_sigLaunched->async_emit();
        }
    }
    else
    {
        this->stopConfig();
    }
}

//------------------------------------------------------------------------------

void SConfigLauncher::updating()
{
}

//------------------------------------------------------------------------------

void SConfigLauncher::stopConfig()
{
    if(m_configLauncher->configIsRunning())
    {
        m_configLauncher->stopConfig();
        core::com::proxy::sptr proxies = core::com::proxy::get();
        proxies->disconnect(m_proxychannel, this->slot(STOP_CONFIG_SLOT));
        this->setChecked(false);
    }
}

//------------------------------------------------------------------------------

} // namespace sight::module::ui
