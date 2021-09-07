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

#include "SConfigLauncher.hpp"

#include <core/com/Signal.hxx>
#include <core/com/Slots.hxx>

#include <service/macros.hpp>
#include <service/registry/Proxy.hpp>

namespace sight::module::ui::base
{

//------------------------------------------------------------------------------

const core::com::Signals::SignalKeyType SConfigLauncher::s_LAUNCHED_SIG = "launched";

static const core::com::Slots::SlotKeyType s_STOP_CONFIG_SLOT = "stopConfig";

static const std::string s_CLOSE_CONFIG_CHANNEL_ID = "CLOSE_CONFIG_CHANNEL";

//------------------------------------------------------------------------------

SConfigLauncher::SConfigLauncher() noexcept
{
    m_configLauncher = std::make_unique<service::helper::ConfigLauncher>();

    m_sigLaunched = newSignal<LaunchedSignalType>(s_LAUNCHED_SIG);

    newSlot(s_STOP_CONFIG_SLOT, &SConfigLauncher::stopConfig, this);
}

//------------------------------------------------------------------------------

SConfigLauncher::~SConfigLauncher() noexcept
{
}

//------------------------------------------------------------------------------

void SConfigLauncher::starting()
{
    m_proxychannel = this->getID() + "_stopConfig";

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

    m_configLauncher->parseConfig(this->getConfigTree(), this->getSptr());
}

//-----------------------------------------------------------------------------

void SConfigLauncher::setIsActive(bool isActive)
{
    this->::sight::ui::base::IAction::setIsActive(isActive);
    if(isActive)
    {
        // Check if the config is already running, this avoids to start a running config.
        if(!m_configLauncher->configIsRunning())
        {
            service::registry::Proxy::sptr proxies = service::registry::Proxy::getDefault();
            proxies->connect(m_proxychannel, this->slot(s_STOP_CONFIG_SLOT));
            service::FieldAdaptorType replaceMap;
            replaceMap[s_CLOSE_CONFIG_CHANNEL_ID] = m_proxychannel;
            m_configLauncher->startConfig(this->getSptr(), replaceMap);
            m_sigLaunched->asyncEmit();
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
        service::registry::Proxy::sptr proxies = service::registry::Proxy::getDefault();
        proxies->disconnect(m_proxychannel, this->slot(s_STOP_CONFIG_SLOT));
        this->setIsActive(false);
    }
}

//------------------------------------------------------------------------------

} // namespace sight::module
