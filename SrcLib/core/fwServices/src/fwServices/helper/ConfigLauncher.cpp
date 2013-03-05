/* ***** BEGIN LICENSE BLOCK *****
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <fwServices/Base.hpp>
#include <fwServices/registry/AppConfig.hpp>

#include <fwTools/fwID.hpp>
#include <fwData/Composite.hpp>

#include <fwAtomConversion/RetreiveObjectVisitor.hpp>

#include "fwServices/helper/ConfigLauncher.hpp"

namespace fwServices
{
namespace helper
{

//------------------------------------------------------------------------------
const std::string ConfigLauncher::s_SELF_KEY = "self";
const std::string ConfigLauncher::s_GENERIC_UID_KEY = "GENERIC_UID";
//------------------------------------------------------------------------------

ConfigLauncher::ConfigLauncher() : m_configIsRunning(false)
{}

//------------------------------------------------------------------------------

ConfigLauncher::~ConfigLauncher()
{}

//------------------------------------------------------------------------------

void ConfigLauncher::parseConfig(const ::fwServices::IService::ConfigType& config)
{
    if(config.get_child("service").count("config") > 0)
    {
        SLM_ASSERT("Sorry you must have one (and only one) <config/> element.",
                config.get_child("service").count("config") == 1 );
        const ::fwServices::IService::ConfigType srvconfig = config.get_child("service");
        const ::fwServices::IService::ConfigType &config = srvconfig.get_child("config");

        if(config.count("appConfig") == 1 )
        {
            const ::fwServices::IService::ConfigType &appConfig = config.get_child("appConfig");
            m_appConfig = ::fwActivities::registry::ActivityAppConfig(appConfig);
        }
        OSLM_ASSERT("At most 1 <appConfig> tag is allowed", config.count("appConfig") < 2);
    }
}

//------------------------------------------------------------------------------

void ConfigLauncher::startConfig(::fwServices::IService::sptr srv)
{
    ::fwData::Object::sptr currentObj = srv->getObject();
    typedef ::fwActivities::registry::ActivityAppConfig AppConfig;
    ::fwServices::registry::AppConfig::FieldAdaptorType replaceMap;

    // Generate generic UID
    const std::string genericUidAdaptor = ::fwServices::registry::AppConfig::getUniqueIdentifier( srv->getID() );
    replaceMap[ConfigLauncher::s_GENERIC_UID_KEY] = genericUidAdaptor;

    BOOST_FOREACH(const AppConfig::ActivityAppConfigParamsType::value_type& param, m_appConfig.parameters)
    {
        if(!param.isSeshat())
        {
            std::string by = param.by;
            if(by == ConfigLauncher::s_SELF_KEY)
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
    this->connectToConfigRoot(srv);

    m_configIsRunning = true;
}

//------------------------------------------------------------------------------

void ConfigLauncher::stopConfig()
{
    if( m_configIsRunning )
    {
        // Remove connection
        this->disconnectToConfigRoot();

        // Delete manager
        m_appConfigManager->stopAndDestroy();
        m_appConfigManager.reset();
    }
    m_configIsRunning = false;
}

//------------------------------------------------------------------------------

void ConfigLauncher::connectToConfigRoot(::fwServices::IService::sptr srv)
{
    ::fwData::Object::sptr root = m_appConfigManager->getConfigRoot();
    m_connections = ::fwServices::helper::SigSlotConnection::New();
    m_connections->connect( root, srv->getSptr(), srv->getObjSrvConnections() );
}

//------------------------------------------------------------------------------

void ConfigLauncher::disconnectToConfigRoot()
{
    m_connections->disconnect();
    m_connections.reset();
}

//------------------------------------------------------------------------------

bool ConfigLauncher::isExecutable(::fwData::Object::sptr currentObj)
{
    typedef ::fwActivities::registry::ActivityAppConfig AppConfig;
    bool executable = true;
    BOOST_FOREACH(const AppConfig::ActivityAppConfigParamsType::value_type& param, m_appConfig.parameters)
    {
        if(param.isSeshat())
        {
            std::string by = param.by;
            executable &= ::fwAtomConversion::getSubObject(currentObj, param.by, false).get() != 0;
        }
    }

    return executable;
}

//------------------------------------------------------------------------------

} // helper
} // fwServices

