/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2018.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "fwServices/helper/ConfigLauncher.hpp"

#include <fwServices/macros.hpp>

#include <fwData/Composite.hpp>
#include <fwData/String.hpp>

#include <fwDataCamp/getObject.hpp>

#include <fwTools/fwID.hpp>

#include <boost/property_tree/xml_parser.hpp>

namespace fwServices
{
namespace helper
{

//------------------------------------------------------------------------------

const std::string ConfigLauncher::s_SELF_KEY        = "self";
const std::string ConfigLauncher::s_GENERIC_UID_KEY = "GENERIC_UID";

//------------------------------------------------------------------------------

ConfigLauncher::ConfigLauncher() :
    m_configIsRunning(false)
{
}

//------------------------------------------------------------------------------

ConfigLauncher::~ConfigLauncher()
{
}

//------------------------------------------------------------------------------

void ConfigLauncher::parseConfig(const ::fwServices::IService::ConfigType& config,
                                 const ::fwServices::IService::sptr& service)
{
    ::fwServices::IService::ConfigType srvCfg;
    const ::fwServices::IService::ConfigType* curConfig = &config;

    const ::fwServices::IService::ConfigType& oldConfig = config;
    SLM_ASSERT("There must be only one <appConfig/> element.", oldConfig.count("appConfig") == 1 );

    const ::fwServices::IService::ConfigType& appConfig = oldConfig.get_child("appConfig");
    const std::string appCfgId                          = appConfig.get<std::string>("<xmlattr>.id");

    srvCfg.add("config.appConfig.<xmlattr>.id", appCfgId);
    ::fwServices::IService::ConfigType& newCfg = srvCfg.get_child("config.appConfig");
    curConfig                                  = &srvCfg;

    auto inoutsCfg = oldConfig.equal_range("inout");
    for (auto itCfg = inoutsCfg.first; itCfg != inoutsCfg.second; ++itCfg)
    {
        ::fwServices::IService::ConfigType parameterCfg;

        const std::string key = itCfg->second.get<std::string>("<xmlattr>.key");
        SLM_ASSERT("[" + appCfgId + "] Missing 'key' tag.", !key.empty());

        const std::string uid = itCfg->second.get<std::string>("<xmlattr>.uid");
        SLM_ASSERT("[" + appCfgId + "] Missing 'uid' tag.", !uid.empty());

        parameterCfg.add("<xmlattr>.replace", key);

        const std::string strOptional = itCfg->second.get<std::string>("<xmlattr>.optional", "no");
        const bool optional           = strOptional == "yes" ? true : false;

        auto obj = service->getInOut< ::fwData::Object>(key);
        if(optional)
        {
            parameterCfg.add("<xmlattr>.uid", uid);
        }
        else
        {
            SLM_ASSERT("Object key '" + key + "'with uid '" + uid + "' does not exist.", obj);
            parameterCfg.add("<xmlattr>.uid", obj->getID());
        }

        newCfg.add_child("parameters.parameter", parameterCfg);
    }

    // @deprecated This is no longer necessary to use "uid" to get the prefix replacement, since
    // this is now done in AppConfig. However we keep that code for a while for backward compatibility
    auto paramsCfg = oldConfig.equal_range("parameter");
    for (auto itCfg = paramsCfg.first; itCfg != paramsCfg.second; ++itCfg)
    {
        ::fwServices::IService::ConfigType parameterCfg;

        const std::string replace = itCfg->second.get<std::string>("<xmlattr>.replace");
        SLM_ASSERT("[" + appCfgId + "] Missing 'replace' tag.", !replace.empty());

        parameterCfg.add("<xmlattr>.replace", replace);

        if(itCfg->second.get_child("<xmlattr>").count("uid") == 1)
        {
            const std::string uid = itCfg->second.get<std::string>("<xmlattr>.uid");
            parameterCfg.add("<xmlattr>.uid", uid);
        }
        else
        {
            const std::string by = itCfg->second.get<std::string>("<xmlattr>.by");
            parameterCfg.add("<xmlattr>.by", by);
        }

        newCfg.add_child("parameters.parameter", parameterCfg);
    }

    SLM_ASSERT("There must be only one <config/> element.", curConfig->count("config") == 1 );

    const ::fwServices::IService::ConfigType& srvconfig = curConfig->get_child("config");

    SLM_ASSERT("There must be only one <appConfig/> element.", srvconfig.count("appConfig") == 1 );

    const ::fwServices::IService::ConfigType& appConfigCfg = srvconfig.get_child("appConfig");
    m_appConfig = ::fwActivities::registry::ActivityAppConfig(appConfigCfg);
}

//------------------------------------------------------------------------------

void ConfigLauncher::startConfig(::fwServices::IService::sptr srv,
                                 const FieldAdaptorType& optReplaceMap )
{
    typedef ::fwActivities::registry::ActivityAppConfig AppConfig;
    FieldAdaptorType replaceMap(optReplaceMap);

    // Generate generic UID
    const std::string genericUidAdaptor = ::fwServices::registry::AppConfig::getUniqueIdentifier( srv->getID() );
    replaceMap[ConfigLauncher::s_GENERIC_UID_KEY] = genericUidAdaptor;

    for(const AppConfig::ActivityAppConfigParamsType::value_type& param :  m_appConfig.parameters)
    {
        replaceMap[param.replace] = param.by;
    }

    // Init manager
    m_appConfigManager = ::fwServices::IAppConfigManager::New();
    m_appConfigManager->setConfig( m_appConfig.id, replaceMap );

    // Launch config
    m_appConfigManager->launch();

    m_configIsRunning = true;
}

//------------------------------------------------------------------------------

void ConfigLauncher::stopConfig()
{
    if( m_configIsRunning )
    {
        // Delete manager
        m_appConfigManager->stopAndDestroy();
        m_appConfigManager.reset();
    }
    m_configIsRunning = false;
}

//------------------------------------------------------------------------------

}     // helper
} // fwServices
