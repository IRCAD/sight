/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <fwServices/macros.hpp>

#include <fwTools/fwID.hpp>
#include <fwData/Composite.hpp>
#include <fwData/String.hpp>


#include <fwDataCamp/getObject.hpp>

#include "fwServices/helper/ConfigLauncher.hpp"

namespace fwServices
{
namespace helper
{

//------------------------------------------------------------------------------
const std::string ConfigLauncher::s_SELF_KEY        = "self";
const std::string ConfigLauncher::s_GENERIC_UID_KEY = "GENERIC_UID";
//------------------------------------------------------------------------------

ConfigLauncher::ConfigLauncher() : m_configIsRunning(false)
{
}

//------------------------------------------------------------------------------

ConfigLauncher::~ConfigLauncher()
{
}

//------------------------------------------------------------------------------

void ConfigLauncher::parseConfig(const ::fwServices::IService::ConfigType& config)
{
    if(config.get_child("service").count("config") > 0)
    {
        SLM_ASSERT("There must be one (and only one) <config/> element.",
                   config.get_child("service").count("config") == 1 );
        const ::fwServices::IService::ConfigType srvconfig = config.get_child("service");
        const ::fwServices::IService::ConfigType& config   = srvconfig.get_child("config");

        if(config.count("appConfig") == 1 )
        {
            const ::fwServices::IService::ConfigType& appConfig = config.get_child("appConfig");
            m_appConfig = ::fwActivities::registry::ActivityAppConfig(appConfig);
        }
        OSLM_ASSERT("At most 1 <appConfig> tag is allowed", config.count("appConfig") < 2);
    }
}

//------------------------------------------------------------------------------

void ConfigLauncher::startConfig(::fwServices::IService::sptr srv,
                                 const FieldAdaptorType& optReplaceMap )
{
    ::fwData::Object::sptr currentObj = srv->getObject();
    typedef ::fwActivities::registry::ActivityAppConfig AppConfig;
    FieldAdaptorType replaceMap(optReplaceMap);

    // Generate generic UID
    const std::string genericUidAdaptor = ::fwServices::registry::AppConfig::getUniqueIdentifier( srv->getID() );
    replaceMap[ConfigLauncher::s_GENERIC_UID_KEY] = genericUidAdaptor;

    for(const AppConfig::ActivityAppConfigParamsType::value_type& param :  m_appConfig.parameters)
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
            std::string parameterToReplace = param.by;
            if (parameterToReplace.substr(0,1) == "!")
            {
                parameterToReplace.replace(0, 1, "@");
            }

            ::fwData::Object::sptr obj = ::fwDataCamp::getObject(currentObj, parameterToReplace);
            OSLM_ASSERT("Invalid seshat path : '"<<param.by<<"'", obj);
            ::fwData::String::sptr stringParameter = ::fwData::String::dynamicCast(obj);

            std::string parameterValue = obj->getID();

            if(stringParameter && param.by.substr(0,1) == "!")
            {
                parameterValue = stringParameter->getValue();
            }
            replaceMap[param.replace] = parameterValue;
        }
    }

    // Init manager
    m_appConfigManager = ::fwServices::IAppConfigManager::New();
    m_appConfigManager->setConfig( m_appConfig.id, replaceMap );

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
    m_connections.connect( root, srv, srv->getObjSrvConnections() );
}

//------------------------------------------------------------------------------

void ConfigLauncher::disconnectToConfigRoot()
{
    m_connections.disconnect();
}

//------------------------------------------------------------------------------

bool ConfigLauncher::isExecutable(::fwData::Object::sptr currentObj)
{
    typedef ::fwActivities::registry::ActivityAppConfig AppConfig;
    bool executable = true;
    for(const AppConfig::ActivityAppConfigParamsType::value_type& param :  m_appConfig.parameters)
    {
        if(param.isSeshat())
        {
            std::string by = param.by;
            executable &= ::fwDataCamp::getObject(currentObj, param.by, false).get() != 0;
        }
    }

    return executable;
}

//------------------------------------------------------------------------------

}     // helper
} // fwServices
