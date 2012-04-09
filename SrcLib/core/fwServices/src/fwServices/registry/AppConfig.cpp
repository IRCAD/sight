/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
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

//-----------------------------------------------------------------------------

AppConfig::sptr AppConfig::getDefault()
{
    SLM_TRACE_FUNC();
    static AppConfig::sptr m_instance = AppConfig::New();
    return m_instance;
}

//-----------------------------------------------------------------------------

AppConfig::~AppConfig()
{
    SLM_TRACE_FUNC();
}

//-----------------------------------------------------------------------------

void AppConfig::parseBundleInformation()
{
    std::vector< ::boost::shared_ptr< ::fwRuntime::Extension > >  extensions = ::fwRuntime::getAllExtensionsForPoint("::fwServices::registry::AppConfig");
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
        std::string typeStr = ext->findConfigurationElement("type")->getValue();
        OSLM_ASSERT("Sorry, xml elment \"type\" must be equal to \"standard\" or \"template\" (here = " << typeStr << ") ", typeStr=="standard" || typeStr=="template" );
        AppInfo::ConfigType type = ( typeStr == "standard" ) ? AppInfo::STANDARD : AppInfo::TEMPLATE ;

        // Get config
        ::fwRuntime::ConfigurationElement::csptr config = *(ext->findConfigurationElement("config")->begin());

        // Add app info
        addAppInfo( configId, type, group, desc, config );
    }
}

//-----------------------------------------------------------------------------

void AppConfig::addAppInfo
(   const std::string & configId,
    AppInfo::ConfigType type,
    const std::string & group,
    const std::string & desc,
    ::fwRuntime::ConfigurationElement::csptr config)
{
    SLM_TRACE_FUNC();
    OSLM_DEBUG( "New app config registring : "
            << " configId =" << configId
            << " type=" << type
    );

    SLM_ASSERT("Sorry, app config id = "<< configId <<" already exist.", m_reg.find( configId ) == m_reg.end() );

    AppInfo::NewSptr info;
    info->type = type;
    info->group = group;
    info->desc = desc;
    info->config =  config;
    m_reg[configId] = info;
}

//-----------------------------------------------------------------------------

AppConfig::AppConfig()
{
    SLM_TRACE_FUNC();
}

//-----------------------------------------------------------------------------

void AppConfig::clearRegistry()
{
    SLM_TRACE_FUNC();
    m_reg.clear();
}

//-----------------------------------------------------------------------------

::fwRuntime::ConfigurationElement::csptr AppConfig::getStandardConfig( const std::string & configId ) const
{
    SLM_TRACE_FUNC();
    Registry::const_iterator iter = m_reg.find( configId );
    SLM_ASSERT("Sorry, the id " <<  configId << " is not found in the application configuration registry", iter != m_reg.end());
    return iter->second->config;
}

//-----------------------------------------------------------------------------

::fwRuntime::ConfigurationElement::csptr AppConfig::getAdaptedTemplateConfig( const std::string & configId, const FieldAdaptorType & fieldAdaptors ) const
{
    // Get config template
    Registry::const_iterator iter = m_reg.find( configId );
    SLM_ASSERT("Sorry, the id " <<  configId << " is not found in the application configuration registry", iter != m_reg.end());

    // Adapt config
    ::fwRuntime::ConfigurationElement::csptr newConfig = adaptConfig(  iter->second->config, fieldAdaptors );
    return newConfig;
}

//-----------------------------------------------------------------------------

::fwRuntime::ConfigurationElement::csptr AppConfig::getAdaptedTemplateConfig( const std::string & configId, ::fwData::Composite::csptr replaceFields ) const
{
    FieldAdaptorType fieldAdaptors = compositeToFieldAdaptor( replaceFields );
    return getAdaptedTemplateConfig( configId, fieldAdaptors );
}

//-----------------------------------------------------------------------------

std::vector< std::string > AppConfig::getAllConfigs() const
{
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

std::string AppConfig::getUniqueIdentifier( std::string _serviceUid, bool _useCpt )
{
    std::string id;
    static unsigned int srvCpt = 1;
    std::stringstream sstr;

    if ( _serviceUid.empty() )
    {
        sstr <<  "AppConfigManager_" << srvCpt;
        srvCpt++;
    }
    else
    {
        sstr <<  _serviceUid;
        if ( _useCpt )
        {
            sstr << "_" << srvCpt;
            srvCpt++;
        }
    }
    id = sstr.str();

    return id;
}

//-----------------------------------------------------------------------------

::fwRuntime::EConfigurationElement::sptr AppConfig::adaptConfig( ::fwRuntime::ConfigurationElement::csptr _cfgElem, const FieldAdaptorType & fieldAdaptors ) const
{
    SLM_TRACE_FUNC();

    ::fwRuntime::EConfigurationElement::NewSptr result ( _cfgElem->getName() );
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

    for (   std::map< std::string, std::string >::const_iterator fieldAdaptor = fieldAdaptors.begin();
            fieldAdaptor != fieldAdaptors.end();
            ++fieldAdaptor )
    {
        std::stringstream sstr;
        sstr << "(.*)" << fieldAdaptor->first << "(.*)";
        ::boost::regex machine_regex ( sstr.str() );
        if ( ::boost::regex_match( _str, machine_regex ) )
        {
            std::stringstream machine_format;
            machine_format << "\\1" << fieldAdaptor->second << "\\2";
            newStr = ::boost::regex_replace( newStr, machine_regex, machine_format.str(), ::boost::match_default | ::boost::format_sed );
        }
    }

    return newStr;
}

//-----------------------------------------------------------------------------

} // namespace registry

} // namespace fwServices

