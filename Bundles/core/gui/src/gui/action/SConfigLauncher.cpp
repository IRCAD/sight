/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2018.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "gui/action/SConfigLauncher.hpp"

#include <fwCom/Signal.hxx>
#include <fwCom/Slots.hxx>

#include <fwServices/macros.hpp>
#include <fwServices/registry/Proxy.hpp>

#include <boost/make_unique.hpp>

namespace gui
{
namespace action
{

//------------------------------------------------------------------------------

fwServicesRegisterMacro( ::fwGui::IActionSrv, ::gui::action::SConfigLauncher );

const ::fwCom::Signals::SignalKeyType SConfigLauncher::s_LAUNCHED_SIG = "launched";

static const ::fwCom::Slots::SlotKeyType s_STOP_CONFIG_SLOT = "stopConfig";

static const std::string s_CLOSE_CONFIG_CHANNEL_ID = "CLOSE_CONFIG_CHANNEL";

//------------------------------------------------------------------------------

SConfigLauncher::SConfigLauncher() noexcept
{
    m_configLauncher = ::boost::make_unique< ::fwServices::helper::ConfigLauncher>();

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
    this->::fwGui::IActionSrv::setIsActive(isActive);
    if ( isActive )
    {
        ::fwServices::registry::Proxy::sptr proxies = ::fwServices::registry::Proxy::getDefault();
        proxies->connect(m_proxychannel, this->slot(s_STOP_CONFIG_SLOT));
        ::fwServices::helper::ConfigLauncher::FieldAdaptorType replaceMap;
        replaceMap[s_CLOSE_CONFIG_CHANNEL_ID] = m_proxychannel;
        m_configLauncher->startConfig(this->getSptr(), replaceMap);
        m_sigLaunched->asyncEmit();
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
    if (m_configLauncher->configIsRunning())
    {
        m_configLauncher->stopConfig();
        ::fwServices::registry::Proxy::sptr proxies = ::fwServices::registry::Proxy::getDefault();
        proxies->disconnect(m_proxychannel, this->slot(s_STOP_CONFIG_SLOT));
        this->setIsActive(false);
    }
}

//------------------------------------------------------------------------------

}
}
