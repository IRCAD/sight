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

#include "fwServices/registry/AppConfigParameters.hpp"

namespace fwServices
{
namespace registry
{

AppConfigParameters::sptr AppConfigParameters::s_appConfigParameters = AppConfigParameters::New();

//-----------------------------------------------------------------------------

AppConfigParameters::sptr AppConfigParameters::getDefault()
{
    return s_appConfigParameters;
}

//-----------------------------------------------------------------------------

AppConfigParameters::~AppConfigParameters()
{
}

//-----------------------------------------------------------------------------

void AppConfigParameters::parseBundleInformation()
{
    std::vector< ::boost::shared_ptr< ::fwRuntime::Extension > >  extensions = ::fwRuntime::getAllExtensionsForPoint("::fwServices::registry::AppConfigParameters");
    BOOST_FOREACH( ::boost::shared_ptr< ::fwRuntime::Extension > ext, extensions )
    {
        // Get id
        std::string extensionId = ext->findConfigurationElement("id")->getValue();

        AppConfig::FieldAdaptorType parameters;

        // Get parmeters
        ::fwRuntime::ConfigurationElement::csptr parametersConfig = ext->findConfigurationElement("parameters");
        ::fwRuntime::ConfigurationElement::Container elements = parametersConfig->getElements();
        BOOST_FOREACH( ::fwRuntime::ConfigurationElement::sptr paramConfig, elements )
        {
            std::string name = paramConfig->getExistingAttributeValue("name");
            std::string val = paramConfig->getExistingAttributeValue("value");
            parameters[name] = val;
        }
        ::fwCore::mt::WriteLock lock(m_registryMutex);
        Registry::const_iterator iter = m_reg.find( extensionId );
        SLM_ASSERT("Sorry, the id " <<  extensionId
                   << " already exists in the application configuration parameter registry", iter == m_reg.end());
        m_reg[extensionId] = parameters;
    }
}

//-----------------------------------------------------------------------------

AppConfigParameters::AppConfigParameters()
{
}

//-----------------------------------------------------------------------------

void AppConfigParameters::clearRegistry()
{
    ::fwCore::mt::WriteLock lock(m_registryMutex);
    m_reg.clear();
}

//-----------------------------------------------------------------------------

const AppConfig::FieldAdaptorType & AppConfigParameters::getParameters( const std::string & extensionId ) const
{
    ::fwCore::mt::ReadLock lock(m_registryMutex);
    Registry::const_iterator iter = m_reg.find( extensionId );
    SLM_ASSERT("Sorry, the id " <<  extensionId << " is not found in the application configuration parameter registry", iter != m_reg.end());
    return iter->second;
}

//-----------------------------------------------------------------------------

} // namespace registry

} // namespace fwServices

