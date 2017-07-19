/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <fwServices/macros.hpp>

#include <fwTools/fwID.hpp>

#include <fwRuntime/Convert.hpp>

#include "fwServices/SConfigController.hpp"

namespace fwServices
{

//------------------------------------------------------------------------------

fwServicesRegisterMacro( ::fwServices::IController, ::fwServices::SConfigController, ::fwData::Object );

//------------------------------------------------------------------------------

SConfigController::SConfigController() noexcept
{
    m_configLauncher = ::fwServices::helper::ConfigLauncher::New();
}

//------------------------------------------------------------------------------

SConfigController::~SConfigController() noexcept
{
}

//------------------------------------------------------------------------------

void SConfigController::starting()
{
    m_configLauncher->startConfig(this->getSptr());
}

//------------------------------------------------------------------------------

void SConfigController::stopping()
{
    m_configLauncher->stopConfig();
}

//------------------------------------------------------------------------------

void SConfigController::swapping()
{
    m_configLauncher->stopConfig();
    m_configLauncher->startConfig(this->getSptr());
}

//------------------------------------------------------------------------------

void SConfigController::configuring()
{
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
                SLM_ASSERT("[" + appCfgId + "] Missing 'uid' or 'by' tag.", paramCfg->hasAttribute("by"));
                std::string by = paramCfg->getAttributeValue("by");
                // 'by' can be empty
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

//------------------------------------------------------------------------------

void SConfigController::updating()
{
}

//------------------------------------------------------------------------------

void SConfigController::info( std::ostream &_sstream )
{
}

//------------------------------------------------------------------------------

}

