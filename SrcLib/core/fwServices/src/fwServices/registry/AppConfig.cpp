/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2012.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <boost/foreach.hpp>
#include <boost/regex.hpp>

#include <fwRuntime/ConfigurationElement.hpp>
#include <fwRuntime/Runtime.hpp>
#include <fwRuntime/helper.hpp>

#include <fwData/Composite.hpp>
#include <fwData/String.hpp>
#include <fwData/Composite.hpp>

#include "fwServices/registry/AppConfig.hpp"

namespace fwServices
{
namespace registry
{
AppConfig::sptr AppConfig::s_currentAppConfig = AppConfig::New();
::fwCore::mt::Mutex AppConfig::s_idMutex;

//-----------------------------------------------------------------------------

AppConfig::sptr AppConfig::getDefault()
{
    return s_currentAppConfig;
}

//-----------------------------------------------------------------------------

AppConfig::~AppConfig()
{
    SLM_TRACE_FUNC();
}

//-----------------------------------------------------------------------------

void AppConfig::parseBundleInformation()
{
    std::vector< ::boost::shared_ptr< ::fwRuntime::Extension > >  extensions =
            ::fwRuntime::getAllExtensionsForPoint("::fwServices::registry::AppConfig");
    BOOST_FOREACH( ::boost::shared_ptr< ::fwRuntime::Extension > ext, extensions )
    {
        // Get id
        std::string configId = ext->findConfigurationElement("id")->getValue();

        // Get group
        std::string group = "";
        if ( ext->hasConfigurationElement("group") )
        {
            group = ext->findConfigurationElement("group")->getValue();
        }
        // Get desc
        std::string desc = "No description available";
        if ( ext->hasConfigurationElement("desc") )
        {
            desc = ext->findConfigurationElement("desc")->getValue();
        }

        // get type
        OSLM_ASSERT("Sorry, xml element \"type\" must be equal to \"parameters\" (here = "
                    << ext->findConfigurationElement("type")->getValue() << ") ",
                    ext->findConfigurationElement("type")->getValue()=="parameters" );
        AppInfo::ConfigType type = AppInfo::PARAMETERS;

        // Get parameters
        AppInfo::ParamatersType parameters;
        if ( ext->hasConfigurationElement("parameters") )
        {
            ::fwRuntime::ConfigurationElement::csptr parametersConfig = ext->findConfigurationElement("parameters");
            ::fwRuntime::ConfigurationElement::Container elements = parametersConfig->getElements();
            BOOST_FOREACH( ::fwRuntime::ConfigurationElement::sptr paramConfig, elements )
            {
                std::string name = paramConfig->getExistingAttributeValue("name");
                std::string defaultValue = paramConfig->getAttributeValue("default");
                parameters[name] = defaultValue;
            }
        }

        // Get config
        ::fwRuntime::ConfigurationElement::csptr config = *(ext->findConfigurationElement("config")->begin());

        // Add app info
        this->addAppInfo( configId, type, group, desc, parameters, config );
    }
}

//-----------------------------------------------------------------------------

void AppConfig::addAppInfo
(   const std::string & configId,
    AppInfo::ConfigType type,
    const std::string & group,
    const std::string & desc,
    const AppInfo::ParamatersType & parameters,
    ::fwRuntime::ConfigurationElement::csptr config)
{
    ::fwCore::mt::WriteLock lock(m_registryMutex);

    OSLM_DEBUG( "New app config registering : "
            << " configId =" << configId
            << " type=" << type
    );

    SLM_ASSERT("Sorry, app config id = "<< configId <<" already exist.", m_reg.find( configId ) == m_reg.end() );

    AppInfo::sptr info = AppInfo::New();
    info->type = type;
    info->group = group;
    info->desc = desc;
    info->config =  config;
    info->parameters = parameters;
    m_reg[configId] = info;
}

//-----------------------------------------------------------------------------

AppConfig::AppConfig()
{
}

//-----------------------------------------------------------------------------

void AppConfig::clearRegistry()
{
    ::fwCore::mt::WriteLock lock(m_registryMutex);
    m_reg.clear();
}

//-----------------------------------------------------------------------------

::fwRuntime::ConfigurationElement::csptr AppConfig::getAdaptedTemplateConfig(
        const std::string & configId,
        const FieldAdaptorType fieldAdaptors ) const
{
    ::fwCore::mt::ReadLock lock(m_registryMutex);
    // Get config template
    Registry::const_iterator iter = m_reg.find( configId );
    SLM_ASSERT("Sorry, the id " <<  configId << " is not found in the application configuration registry", iter != m_reg.end());

    // Adapt config
    ::fwRuntime::ConfigurationElement::sptr newConfig;
    SLM_ASSERT("Config has not good type, PARAMETERS type is required", iter->second->type == AppInfo::PARAMETERS );

    FieldAdaptorType fields;
    AppInfo::ParamatersType parameters = iter->second->parameters;
    BOOST_FOREACH( AppInfo::ParamatersType::value_type param, parameters )
    {
        FieldAdaptorType::const_iterator iter = fieldAdaptors.find( param.first );
        std::stringstream key;
        key << "\\$\\{" << param.first << "\\}";
        if ( iter != fieldAdaptors.end() )
        {
            fields[key.str()] = iter->second;
        }
        else if ( param.second != "" )
        {
            fields[key.str()] = param.second;
        }
        else
        {
            FW_RAISE("Parameter : '" << param.first << "' is needed by the app configuration id='"<< configId <<"'.");
        }
    }
    newConfig = this->adaptConfig(  iter->second->config, fields );

    return newConfig;
}

//-----------------------------------------------------------------------------

::fwRuntime::ConfigurationElement::csptr AppConfig::getAdaptedTemplateConfig( const std::string & configId, ::fwData::Composite::csptr replaceFields ) const
{
    FieldAdaptorType fieldAdaptors = compositeToFieldAdaptor( replaceFields );
    return this->getAdaptedTemplateConfig( configId, fieldAdaptors );
}

//-----------------------------------------------------------------------------

std::vector< std::string > AppConfig::getAllConfigs() const
{
    ::fwCore::mt::ReadLock lock(m_registryMutex);
    std::vector< std::string > ids;
    BOOST_FOREACH( Registry::value_type elem, m_reg )
    {
        ids.push_back( elem.first );
    }
    return ids;
}

//-----------------------------------------------------------------------------

std::vector< std::string > AppConfig::getConfigsFromGroup(const std::string & group) const
{
    ::fwCore::mt::ReadLock lock(m_registryMutex);
    std::vector< std::string > ids;
    BOOST_FOREACH( Registry::value_type elem, m_reg )
    {
        AppInfo::sptr info = elem.second;
        if(info->group == group)
        {
            ids.push_back( elem.first );
        }
    }
    return ids;
}

//-----------------------------------------------------------------------------

AppConfig::FieldAdaptorType AppConfig::compositeToFieldAdaptor( ::fwData::Composite::csptr fieldAdaptors ) const
{
    FieldAdaptorType fields;
    BOOST_FOREACH(const ::fwData::Composite::value_type &elem, *fieldAdaptors )
    {
        fields[elem.first] = ::fwData::String::dynamicCast( elem.second )->value();
    }

    return fields;
}

//-----------------------------------------------------------------------------

std::string AppConfig::getUniqueIdentifier(const std::string& serviceUid )
{
    ::fwCore::mt::ScopedLock lock(s_idMutex);
    static unsigned int srvCpt = 1;
    std::stringstream sstr;

    if ( serviceUid.empty() )
    {
        sstr <<  "AppConfigManager_" << srvCpt;
    }
    else
    {
        sstr <<  serviceUid << "_" << srvCpt;
    }
    ++srvCpt;
    return sstr.str();
}

//-----------------------------------------------------------------------------

::fwRuntime::EConfigurationElement::sptr AppConfig::adaptConfig( ::fwRuntime::ConfigurationElement::csptr _cfgElem, const FieldAdaptorType & fieldAdaptors ) const
{
    SLM_TRACE_FUNC();

    ::fwRuntime::EConfigurationElement::sptr result = ::fwRuntime::EConfigurationElement::New( _cfgElem->getName() );
    result->setValue( this->adaptField( _cfgElem->getValue(), fieldAdaptors ) );

    typedef std::map<std::string, std::string> MapAttributesType;
    BOOST_FOREACH( MapAttributesType::value_type attribute, _cfgElem->getAttributes() )
    {
        result->setAttributeValue( attribute.first, this->adaptField( attribute.second, fieldAdaptors ) );
    }

    BOOST_FOREACH ( ::fwRuntime::ConfigurationElement::csptr subElem, _cfgElem->getElements())
    {
        result->addConfigurationElement( this->adaptConfig( subElem, fieldAdaptors ) );
    }

    return result;
}

//-----------------------------------------------------------------------------

std::string AppConfig::adaptField( const std::string & _str, const FieldAdaptorType & fieldAdaptors ) const
{
    std::string newStr = _str;
    if(!_str.empty())
    {
        BOOST_FOREACH(FieldAdaptorType::value_type fieldAdaptor, fieldAdaptors)
        {
            std::stringstream sstr;
            sstr << "(.*)" << fieldAdaptor.first << "(.*)";
            ::boost::regex machine_regex ( sstr.str() );
            if ( ::boost::regex_match( _str, machine_regex ) )
            {
                std::stringstream machine_format;
                machine_format << "\\1" << fieldAdaptor.second << "\\2";
                newStr = ::boost::regex_replace( newStr,
                                machine_regex, machine_format.str(),
                                ::boost::match_default | ::boost::format_sed );
            }
        }
    }
    return newStr;
}

//-----------------------------------------------------------------------------

} // namespace registry

} // namespace fwServices

