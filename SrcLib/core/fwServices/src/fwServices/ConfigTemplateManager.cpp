/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <sstream>

#include <boost/foreach.hpp>
#include <boost/regex.hpp>

#include "fwServices/ConfigTemplateManager.hpp"
#include "fwServices/bundle/runtime.hpp"
#include "fwServices/Base.hpp"

namespace fwServices
{

//-----------------------------------------------------------------------------

ConfigTemplateManager::ConfigTemplateManager() :
                            m_configName(""),
                            m_configType(""),
                            m_state ( CONFIG_IS_STOPPED )
{}

//-----------------------------------------------------------------------------

ConfigTemplateManager::ConfigTemplateManager( const std::string & _configName, const std::string & _configType, const std::map< std::string, std::string > & _fieldAdaptors )
{
    setConfig( _configName, _configType );
    setFieldAdaptors( _fieldAdaptors );
}

//-----------------------------------------------------------------------------

ConfigTemplateManager::~ConfigTemplateManager()
{
    SLM_ASSERT("Sorry, you must stop the manager first before destroying it.", m_state == CONFIG_IS_STOPPED );
}

//-----------------------------------------------------------------------------

void ConfigTemplateManager::setConfig( const std::string & _configName, const std::string & _configType )
{
    m_configName = _configName;
    m_configType = _configType;
}

//-----------------------------------------------------------------------------

void ConfigTemplateManager::setFieldAdaptors( const std::map< std::string, std::string > & _fieldAdaptors )
{
    m_fieldAdaptors = _fieldAdaptors;
}

//-----------------------------------------------------------------------------

::fwTools::Object::sptr ConfigTemplateManager::getConfigRoot() const{

    return m_configRoot;
}

//-----------------------------------------------------------------------------

void ConfigTemplateManager::create()
{
    SLM_ASSERT("Sorry, manager is already running and you try creating config.", m_state == CONFIG_IS_STOPPED );

    // Load config
    loadConfig();

    // Adapt config
    m_adaptedConfig = adaptConfig( m_configTemplate );

    // Create object and services
    m_configRoot = ::fwServices::New( m_adaptedConfig );

    m_state = CONFIG_IS_CREATED;
}

//-----------------------------------------------------------------------------

void ConfigTemplateManager::start()
{
    SLM_ASSERT("Sorry, manager is not created and you try starting it.", m_state == CONFIG_IS_CREATED );

    ::fwServices::start( m_adaptedConfig ) ;

    m_state = CONFIG_IS_STARTED;
}

//-----------------------------------------------------------------------------

void ConfigTemplateManager::update()
{
    ::fwServices::update( m_adaptedConfig ) ;
}

//-----------------------------------------------------------------------------

void ConfigTemplateManager::launch()
{
    create();
    start();
    update();
}

//-----------------------------------------------------------------------------

void ConfigTemplateManager::stopAndDestroy()
{
    SLM_ASSERT("Sorry, manager is not started and you try stopping it.", m_state == CONFIG_IS_STARTED );

    ::fwServices::stopAndUnregister( m_adaptedConfig ) ;
    m_adaptedConfig.reset();
    m_configTemplate.reset();
    m_configRoot.reset();

    m_state = CONFIG_IS_STOPPED;
}

//-----------------------------------------------------------------------------

std::string ConfigTemplateManager::getUniqueIdentifier( std::string _serviceUid, bool _useCpt )
{
    std::string id;
    static unsigned int srvCpt = 1;
    std::stringstream sstr;

    if ( _serviceUid.empty() )
    {
        sstr <<  "ConfigTemplateManager_" << srvCpt;
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

::fwRuntime::EConfigurationElement::sptr ConfigTemplateManager::adaptConfig( ::fwRuntime::ConfigurationElement::sptr _cfgElem )
{
    SLM_TRACE_FUNC();

    ::fwRuntime::EConfigurationElement::NewSptr result ( _cfgElem->getName() );
    result->setValue( this->adaptField( _cfgElem->getValue() ) );

    typedef std::map<std::string, std::string> MapAttributesType;
    BOOST_FOREACH( MapAttributesType::value_type attribute, _cfgElem->getAttributes() )
    {
        result->setAttributeValue( attribute.first, this->adaptField( attribute.second ) );
    }

    BOOST_FOREACH ( ::fwRuntime::ConfigurationElement::sptr subElem, _cfgElem->getElements())
    {
        result->addConfigurationElement( this->adaptConfig( subElem ) );
    }

    return result;
}

//-----------------------------------------------------------------------------

std::string ConfigTemplateManager::adaptField( const std::string & _str ) const
{
    std::string newStr = _str;

    for (   std::map< std::string, std::string >::const_iterator fieldAdaptor = m_fieldAdaptors.begin();
            fieldAdaptor != m_fieldAdaptors.end();
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

void ConfigTemplateManager::loadConfig()
{
    m_configTemplate = ::fwServices::bundle::findConfigurationForPoint( m_configName, m_configType );
}

//-----------------------------------------------------------------------------

//void ConfigTemplateManager::read( ::fwRuntime::ConfigurationElement::sptr _cfgElem )
//{
//    SLM_TRACE_FUNC();
//
//    ::fwRuntime::ConfigurationElement::sptr result;
//
//    OSLM_INFO( " Name : " << _cfgElem->getName() );
//    OSLM_INFO( " Value : " << _cfgElem->getValue() );
//
//    typedef std::map<std::string, std::string> MapAttributesType;
//    BOOST_FOREACH( MapAttributesType::value_type attribute, _cfgElem->getAttributes() )
//    {
//        OSLM_INFO( " Attribute Name : " << attribute.first  << "  -  Attribute Value : " << attribute.second );
//    }
//
//    BOOST_FOREACH(::fwRuntime::ConfigurationElement::sptr subElem, _cfgElem->getElements())
//    {
//        this->read( subElem );
//    }
//}

//-----------------------------------------------------------------------------

}

