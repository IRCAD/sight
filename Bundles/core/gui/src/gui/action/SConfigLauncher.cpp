/* ***** BEGIN LICENSE BLOCK *****
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <fwServices/Base.hpp>
#include <fwServices/registry/AppConfig.hpp>

#include <fwTools/fwID.hpp>
#include <fwData/Composite.hpp>
#include <fwComEd/CompositeMsg.hpp>

#include <fwAtomConversion/RetreiveObjectVisitor.hpp>

#include "gui/action/SConfigLauncher.hpp"

namespace gui
{
namespace action
{

//------------------------------------------------------------------------------

fwServicesRegisterMacro( ::fwGui::IActionSrv, ::gui::action::SConfigLauncher, ::fwData::Object );

//------------------------------------------------------------------------------

SConfigLauncher::SConfigLauncher() throw() : m_configIsRunning(false)
{
}

//------------------------------------------------------------------------------

SConfigLauncher::~SConfigLauncher() throw()
{}

//------------------------------------------------------------------------------

void SConfigLauncher::starting() throw(::fwTools::Failed)
{
    m_connections = ::fwServices::helper::SigSlotConnection::New();
    this->actionServiceStarting();

    this->updateExecutableState();
}

//------------------------------------------------------------------------------

void SConfigLauncher::stopping() throw(::fwTools::Failed)
{
    bool configIsRunning = this->getIsActive();

    if ( configIsRunning )
    {
        this->stopConfig();
    }

    m_connections.reset();
    this->actionServiceStopping();
}

//------------------------------------------------------------------------------

void SConfigLauncher::configuring() throw(fwTools::Failed)
{
    this->initialize();

    if(this->getConfigTree().get_child("service").count("config") > 0)
    {
        SLM_ASSERT("Sorry you must have one (and only one) <config/> element.",
                this->getConfigTree().get_child("service").count("config") == 1 );
        const ::fwServices::IService::ConfigType srvconfig = this->getConfigTree().get_child("service");
        const ::fwServices::IService::ConfigType &config = srvconfig.get_child("config");

        if(config.count("appConfig") == 1 )
        {
            const ::fwServices::IService::ConfigType &appConfig = config.get_child("appConfig");
            m_appConfig = ::fwActivities::registry::ActivityAppConfig(appConfig);
        }
        OSLM_ASSERT("At most 1 <appConfig> tag is allowed", config.count("appConfig") < 2);
    }
}

//-----------------------------------------------------------------------------

void SConfigLauncher::setIsActive(bool isActive)
{
    this->::fwGui::IActionSrv::setIsActive(isActive);
    if ( isActive )
    {
        this->startConfig();
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

void SConfigLauncher::receiving( ::fwServices::ObjectMsg::csptr msg ) throw(::fwTools::Failed)
{
    if ( msg->hasEvent("WINDOW_CLOSED") )
    {
        this->setIsActive( false );
        this->stopConfig();
    }

    this->updateExecutableState();
}

//------------------------------------------------------------------------------

void SConfigLauncher::info( std::ostream &_sstream )
{
}

//------------------------------------------------------------------------------

void SConfigLauncher::startConfig()
{
    typedef ::fwActivities::registry::ActivityAppConfig AppConfig;
    ::fwData::Object::sptr currentObj = this->getObject();
    ::fwServices::registry::AppConfig::FieldAdaptorType replaceMap;

    // Generate generic UID
    const std::string genericUidAdaptor = ::fwServices::registry::AppConfig::getUniqueIdentifier( this->getID() );
    replaceMap["GENERIC_UID"] = genericUidAdaptor;

    BOOST_FOREACH(const AppConfig::ActivityAppConfigParamsType::value_type& param, m_appConfig.parameters)
    {
        if(!param.isSeshat())
        {
            std::string by = param.by;
            if(by == "self")
            {
                by = currentObj->getID();
            }
            replaceMap[param.replace] = by;
        }
        else
        {
            ::fwData::Object::sptr obj = ::fwAtomConversion::getSubObject(currentObj, param.by);
            OSLM_ASSERT("Invalid seshat path : '"<<param.by<<"'", obj);
            replaceMap[param.replace] = obj->getID();
        }
    }

    // Init manager
    ::fwRuntime::ConfigurationElement::csptr config =
            ::fwServices::registry::AppConfig::getDefault()->getAdaptedTemplateConfig( m_appConfig.id, replaceMap );
    m_appConfigManager = ::fwServices::AppConfigManager::New();
    m_appConfigManager->setConfig( config );

    // Launch config
    m_appConfigManager->launch();

    // Add connection
    this->connectToConfigRoot();

    m_configIsRunning = true;
}

//------------------------------------------------------------------------------

void SConfigLauncher::stopConfig()
{
    if( m_configIsRunning )
    {
        // Remove com channel
        this->disconnectToConfigRoot();

        // Delete manager
        m_appConfigManager->stopAndDestroy();
        m_appConfigManager.reset();
    }
    m_configIsRunning = false;
}

//------------------------------------------------------------------------------

void SConfigLauncher::connectToConfigRoot()
{
    ::fwData::Object::sptr root = m_appConfigManager->getConfigRoot();
    m_connections->connect( root, this->getSptr(), this->getObjSrvConnections() );
}

//------------------------------------------------------------------------------

void SConfigLauncher::disconnectToConfigRoot()
{
    m_connections->disconnect();
}

//------------------------------------------------------------------------------

void SConfigLauncher::updateExecutableState()
{
    typedef ::fwActivities::registry::ActivityAppConfig AppConfig;
    bool executable = true;
    ::fwData::Object::sptr currentObj = this->getObject();
    BOOST_FOREACH(const AppConfig::ActivityAppConfigParamsType::value_type& param, m_appConfig.parameters)
    {
        if(param.isSeshat())
        {
            std::string by = param.by;
            executable &= ::fwAtomConversion::getSubObject(currentObj, param.by, false).get() != 0;
        }
    }

    this->setIsExecutable( executable );
}

//------------------------------------------------------------------------------

}
}

