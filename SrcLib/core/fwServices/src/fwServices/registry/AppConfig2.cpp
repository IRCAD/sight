/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "fwServices/registry/AppConfig2.hpp"

#include <fwData/Composite.hpp>
#include <fwData/String.hpp>

#include <fwRuntime/ConfigurationElement.hpp>
#include <fwRuntime/helper.hpp>
#include <fwRuntime/Runtime.hpp>

#include <boost/regex.hpp>

namespace fwServices
{
namespace registry
{
AppConfig2::sptr AppConfig2::s_currentAppConfig2 = AppConfig2::New();
::fwCore::mt::Mutex AppConfig2::s_idMutex;

std::string AppConfig2::s_mandatoryParameterIdentifier = "@mandatory@";

//-----------------------------------------------------------------------------

AppConfig2::sptr AppConfig2::getDefault()
{
    return s_currentAppConfig2;
}

//-----------------------------------------------------------------------------

AppConfig2::~AppConfig2()
{
}

//-----------------------------------------------------------------------------

void AppConfig2::parseBundleInformation()
{
    auto extensions = ::fwRuntime::getAllExtensionsForPoint("::fwServices::registry::AppConfig2");
    for( const auto& ext : extensions )
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

        AppInfo::ParametersType parameters;
        if ( ext->hasConfigurationElement("parameters") )
        {
            ::fwRuntime::ConfigurationElement::csptr parametersConfig = ext->findConfigurationElement("parameters");
            ::fwRuntime::ConfigurationElement::Container elements     = parametersConfig->getElements();
            for( ::fwRuntime::ConfigurationElement::sptr paramConfig :  elements )
            {
                std::string name = paramConfig->getExistingAttributeValue("name");

                if(paramConfig->hasAttribute("default"))
                {
                    parameters[name] = paramConfig->getAttributeValue("default");
                }
                else
                {
                    parameters[name] = s_mandatoryParameterIdentifier;
                }

            }
        }

        // Get config
        ::fwRuntime::ConfigurationElement::csptr config = ext->findConfigurationElement("config");

        // Get bundle
        std::shared_ptr< ::fwRuntime::Bundle> bundle = ext->getBundle();
        std::string bundleId                         = bundle->getIdentifier();
        std::string bundleVersion                    = bundle->getVersion().string();

        // Add app info
        this->addAppInfo( configId, group, desc, parameters, config, bundleId, bundleVersion );
    }
}

//-----------------------------------------------------------------------------

void AppConfig2::addAppInfo
    (   const std::string& configId,
    const std::string& group,
    const std::string& desc,
    const AppInfo::ParametersType& parameters,
    ::fwRuntime::ConfigurationElement::csptr config,
    const std::string bundleId,
    const std::string bundleVersion)
{
    ::fwCore::mt::WriteLock lock(m_registryMutex);

    OSLM_DEBUG( "New app config registering : "
                << " configId =" << configId
                );

    SLM_ASSERT("The app config with the id = "<< configId <<" already exists.", m_reg.find( configId ) == m_reg.end() );

    AppInfo::sptr info = AppInfo::New();
    info->group         = group;
    info->desc          = desc;
    info->config        = config;
    info->parameters    = parameters;
    info->bundleId      = bundleId;
    info->bundleVersion = bundleVersion;
    m_reg[configId]     = info;
}

//-----------------------------------------------------------------------------

AppConfig2::AppConfig2()
{
}

//-----------------------------------------------------------------------------

void AppConfig2::clearRegistry()
{
    ::fwCore::mt::WriteLock lock(m_registryMutex);
    m_reg.clear();
}

//-----------------------------------------------------------------------------

::fwRuntime::ConfigurationElement::csptr AppConfig2::getAdaptedTemplateConfig(
    const std::string& configId,
    const FieldAdaptorType fieldAdaptors,
    bool autoPrefixId) const
{
    ::fwCore::mt::ReadLock lock(m_registryMutex);
    // Get config template
    Registry::const_iterator iter = m_reg.find( configId );
    SLM_ASSERT("The id " <<  configId << " is not found in the application configuration registry",
               iter != m_reg.end());

    // Adapt config
    ::fwRuntime::ConfigurationElement::sptr newConfig;

    FieldAdaptorType fields;
    AppInfo::ParametersType parameters = iter->second->parameters;

    for( AppInfo::ParametersType::value_type param :  parameters )
    {
        FieldAdaptorType::const_iterator iter = fieldAdaptors.find( param.first );
        std::stringstream key;
        key << "\\$\\{" << param.first << "\\}";
        if ( iter != fieldAdaptors.end() )
        {
            fields[key.str()] = iter->second;
        }
        else if ( param.second != s_mandatoryParameterIdentifier)
        {
            fields[key.str()] = param.second;
        }
        else
        {
            FW_RAISE("Parameter : '" << param.first << "' is needed by the app configuration id='"<< configId <<"'.");
        }
    }

    std::string autoPrefixName;
    if(autoPrefixId)
    {
        autoPrefixName = this->getUniqueIdentifier(configId);
    }

    newConfig = this->adaptConfig(  iter->second->config, fields, autoPrefixName);

    return newConfig;
}

//-----------------------------------------------------------------------------

::fwRuntime::ConfigurationElement::csptr AppConfig2::getAdaptedTemplateConfig( const std::string& configId,
                                                                               ::fwData::Composite::csptr replaceFields,
                                                                               bool autoPrefixId )
const
{
    FieldAdaptorType fieldAdaptors = compositeToFieldAdaptor( replaceFields );
    return this->getAdaptedTemplateConfig( configId, fieldAdaptors, autoPrefixId );
}

//-----------------------------------------------------------------------------

std::vector< std::string > AppConfig2::getAllConfigs() const
{
    ::fwCore::mt::ReadLock lock(m_registryMutex);
    std::vector< std::string > ids;
    for( const Registry::value_type& elem :  m_reg )
    {
        ids.push_back( elem.first );
    }
    return ids;
}

//-----------------------------------------------------------------------------

std::vector< std::string > AppConfig2::getConfigsFromGroup(const std::string& group) const
{
    ::fwCore::mt::ReadLock lock(m_registryMutex);
    std::vector< std::string > ids;
    for( const Registry::value_type& elem :  m_reg )
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

FieldAdaptorType AppConfig2::compositeToFieldAdaptor( ::fwData::Composite::csptr fieldAdaptors ) const
{
    FieldAdaptorType fields;
    for(const ::fwData::Composite::value_type& elem :  * fieldAdaptors )
    {
        fields[elem.first] = ::fwData::String::dynamicCast( elem.second )->value();
    }

    return fields;
}

//-----------------------------------------------------------------------------

std::string AppConfig2::getUniqueIdentifier(const std::string& serviceUid )
{
    ::fwCore::mt::ScopedLock lock(s_idMutex);
    static unsigned int srvCpt = 1;
    std::stringstream sstr;

    if ( serviceUid.empty() )
    {
        sstr <<  "AppConfig2Manager_" << srvCpt;
    }
    else
    {
        sstr <<  serviceUid << "_" << srvCpt;
    }
    ++srvCpt;
    return sstr.str();
}

//-----------------------------------------------------------------------------

::fwRuntime::EConfigurationElement::sptr AppConfig2::adaptConfig(::fwRuntime::ConfigurationElement::csptr _cfgElem,
                                                                 const FieldAdaptorType& fieldAdaptors,
                                                                 const std::string& autoPrefixId) const
{
    ::fwRuntime::EConfigurationElement::sptr result = ::fwRuntime::EConfigurationElement::New( _cfgElem->getName() );
    result->setValue( this->adaptField( _cfgElem->getValue(), fieldAdaptors ) );

    typedef std::map<std::string, std::string> MapAttributesType;
    for( MapAttributesType::value_type attribute :  _cfgElem->getAttributes() )
    {
        // Add the config prefix for unique identifiers
        if(!autoPrefixId.empty() &&
           (attribute.first == "uid" || attribute.first == "sid" || attribute.first == "wid" ||
            attribute.first == "channel" ))
        {
            // Detect if we have a variable name
            ::boost::regex isVariable( "\\${.*}.*" );
            if ( !::boost::regex_match( attribute.second, isVariable ) )
            {
                // This is not a variable, add the prefix
                result->setAttributeValue( attribute.first,
                                           autoPrefixId + "_" + this->adaptField( attribute.second, fieldAdaptors ));
                continue;
            }
        }
        result->setAttributeValue( attribute.first, this->adaptField( attribute.second, fieldAdaptors ) );
    }

    for ( ::fwRuntime::ConfigurationElement::csptr subElem : _cfgElem->getElements())
    {
        // Add the config prefix for unique identifiers in signal and slot sources
        if( !autoPrefixId.empty() && (subElem->getName() == "signal" || subElem->getName() == "slot" ) )
        {
            // Detect if we have a variable name
            ::boost::regex isVariable( "\\${.*}.*" );
            if ( !::boost::regex_match( subElem->getValue(), isVariable ) )
            {
                // This is not a variable, add the prefix
                auto elt = ::fwRuntime::EConfigurationElement::New( subElem->getName() );
                elt->setValue( autoPrefixId + "_" + subElem->getValue() );

                for (const auto& attr : subElem->getAttributes())
                {
                    elt->setAttributeValue(attr.first, attr.second);
                }

                result->addConfigurationElement( elt );
                continue;
            }
        }

        result->addConfigurationElement( this->adaptConfig( subElem, fieldAdaptors, autoPrefixId ) );
    }

    return result;
}

//-----------------------------------------------------------------------------

std::string AppConfig2::adaptField( const std::string& _str, const FieldAdaptorType& fieldAdaptors ) const
{
    std::string newStr = _str;
    if(!_str.empty())
    {
        for(FieldAdaptorType::value_type fieldAdaptor :  fieldAdaptors)
        {
            ::boost::regex varRegex( "(.*)" + fieldAdaptor.first + "(.*)" );
            if ( ::boost::regex_match( _str, varRegex ) )
            {
                const std::string varReplace("\\1" + fieldAdaptor.second + "\\2");
                newStr = ::boost::regex_replace( newStr, varRegex, varReplace,
                                                 ::boost::match_default | ::boost::format_sed );
            }
        }
    }
    return newStr;
}

//-----------------------------------------------------------------------------

std::shared_ptr< ::fwRuntime::Bundle > AppConfig2::getBundle(const std::string& configId)
{
    Registry::const_iterator iter = m_reg.find( configId );
    SLM_ASSERT("The id " <<  configId << " is not found in the application configuration registry",
               iter != m_reg.end());

    std::shared_ptr< ::fwRuntime::Bundle > bundle = ::fwRuntime::findBundle(iter->second->bundleId,
                                                                            iter->second->bundleVersion);

    return bundle;
}

//-----------------------------------------------------------------------------

} // namespace registry

} // namespace fwServices

