/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "gui/action/SConfigLauncher.hpp"

#include <fwCom/Signal.hpp>
#include <fwCom/Signal.hxx>
#include <fwCom/Slot.hpp>
#include <fwCom/Slot.hxx>
#include <fwCom/Slots.hpp>
#include <fwCom/Slots.hxx>

#include <fwRuntime/Convert.hpp>

#include <fwServices/macros.hpp>
#include <fwServices/registry/AppConfig.hpp>
#include <fwServices/registry/Proxy.hpp>

#include <fwTools/fwID.hpp>

namespace gui
{
namespace action
{

//------------------------------------------------------------------------------

fwServicesRegisterMacro( ::fwGui::IActionSrv, ::gui::action::SConfigLauncher, ::fwData::Object );

const ::fwCom::Signals::SignalKeyType SConfigLauncher::s_LAUNCHED_SIG = "launched";

static const ::fwCom::Slots::SlotKeyType s_CHECK_IF_EXECUTABLE_SLOT = "checkIfExecutable";
static const ::fwCom::Slots::SlotKeyType s_STOP_CONFIG_SLOT         = "stopConfig";

static const std::string s_CLOSE_CONFIG_CHANNEL_ID = "CLOSE_CONFIG_CHANNEL";

//------------------------------------------------------------------------------

SConfigLauncher::SConfigLauncher() throw()
{
    m_configLauncher = ::fwServices::helper::ConfigLauncher::New();

    m_sigLaunched = newSignal<LaunchedSignalType>(s_LAUNCHED_SIG);

    newSlot(s_CHECK_IF_EXECUTABLE_SLOT, &SConfigLauncher::checkIfExecutable, this);
    newSlot(s_STOP_CONFIG_SLOT, &SConfigLauncher::stopConfig, this);
}

//------------------------------------------------------------------------------

SConfigLauncher::~SConfigLauncher() throw()
{
}

//------------------------------------------------------------------------------

void SConfigLauncher::starting() throw(::fwTools::Failed)
{

    m_proxychannel = this->getID() + "_stopConfig";

    this->actionServiceStarting();
    ::fwData::Object::sptr currentObj = this->getObject();
    bool executable = m_configLauncher->isExecutable(currentObj);
    this->setIsExecutable( executable );
}

//------------------------------------------------------------------------------

void SConfigLauncher::stopping() throw(::fwTools::Failed)
{
    this->stopConfig();
    this->actionServiceStopping();
}

//------------------------------------------------------------------------------

void SConfigLauncher::configuring() throw(fwTools::Failed)
{
    this->initialize();

    if(this->isVersion2())
    {
        typedef ::fwRuntime::ConfigurationElement::sptr ConfigType;
        typedef ::fwRuntime::EConfigurationElement::sptr EditableConfigType;

        ConfigType cfg = this->getConfiguration();

        ConfigType appCfg = cfg->findConfigurationElement("appConfig");
        SLM_ASSERT("Missing 'appConfig' tag.", appCfg);
        std::string appCfgId = appCfg->getAttributeValue("id");


        EditableConfigType srvCfg    = ::fwRuntime::EConfigurationElement::New( "service" );
        EditableConfigType newCfg    = srvCfg->addConfigurationElement("config");
        EditableConfigType newAppCfg = newCfg->addConfigurationElement("appConfig");
        newAppCfg->setAttributeValue("id", appCfgId);

        EditableConfigType newParamsCfg = newAppCfg->addConfigurationElement("parameters");

        const std::vector< ConfigType > inoutsCfg = cfg->find("inout");
        for (const auto& inoutCfg : inoutsCfg)
        {
            std::string key = inoutCfg->getAttributeValue("key");
            SLM_ASSERT("[" + appCfgId + "] Missing 'key' tag.", !key.empty());

            std::string uid = inoutCfg->getAttributeValue("uid");
            SLM_ASSERT("[" + appCfgId + "] Missing 'uid' tag.", !uid.empty());

            EditableConfigType newParamCfg = newParamsCfg->addConfigurationElement("parameter");
            newParamCfg->setAttributeValue("replace", key);

            auto obj = this->getInOut< ::fwData::Object>(key);
            newParamCfg->setAttributeValue("uid", obj->getID());
        }

        const std::vector< ConfigType > paramsCfg = cfg->find("parameter");
        for (const auto& paramCfg : paramsCfg)
        {
            std::string replace = paramCfg->getAttributeValue("replace");
            SLM_ASSERT("[" + appCfgId + "] Missing 'replace' tag.", !replace.empty());

            EditableConfigType newParamCfg = newParamsCfg->addConfigurationElement("parameter");
            newParamCfg->setAttributeValue("replace", replace);

            std::string uid = paramCfg->getAttributeValue("uid");
            if(!uid.empty())
            {
                newParamCfg->setAttributeValue("uid", uid);
            }
            else
            {
                std::string by = paramCfg->getAttributeValue("by");
                SLM_ASSERT("[" + appCfgId + "] Missing 'uid' or 'by' tag.", !by.empty());

                newParamCfg->setAttributeValue("by", by);
            }
        }

        m_configLauncher->parseConfig(::fwRuntime::Convert::toPropertyTree(srvCfg));
    }
    else
    {
        m_configLauncher->parseConfig(this->getConfigTree());
    }
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

void SConfigLauncher::updating() throw(::fwTools::Failed)
{
}

//------------------------------------------------------------------------------

void SConfigLauncher::checkIfExecutable()
{
    ::fwData::Object::sptr currentObj = this->getObject();
    bool executable = m_configLauncher->isExecutable(currentObj);
    this->setIsExecutable( executable );
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

void SConfigLauncher::info( std::ostream &_sstream )
{
}

//-----------------------------------------------------------------------------

SConfigLauncher::KeyConnectionsType SConfigLauncher::getObjSrvConnections() const
{
    KeyConnectionsType connections;
    connections.push_back( std::make_pair( ::fwData::Object::s_MODIFIED_SIG, s_CHECK_IF_EXECUTABLE_SLOT ) );
    return connections;
}

//------------------------------------------------------------------------------

}
}

