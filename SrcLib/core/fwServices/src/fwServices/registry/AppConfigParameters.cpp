/************************************************************************
 *
 * Copyright (C) 2009-2016 IRCAD France
 * Copyright (C) 2012-2016 IHU Strasbourg
 *
 * This file is part of Sight.
 *
 * Sight is free software: you can redistribute it and/or modify it under
 * the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Sight is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with Sight. If not, see <https://www.gnu.org/licenses/>.
 *
 ***********************************************************************/

#include "fwServices/registry/AppConfigParameters.hpp"

#include <fwData/Composite.hpp>
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
    std::vector< std::shared_ptr< ::fwRuntime::Extension > >  extensions = ::fwRuntime::getAllExtensionsForPoint(
        "::fwServices::registry::AppConfigParameters");
    for( std::shared_ptr< ::fwRuntime::Extension > ext :  extensions )
    {
        // Get id
        std::string extensionId = ext->findConfigurationElement("id")->getValue();

        FieldAdaptorType parameters;

        // Get parmeters
        ::fwRuntime::ConfigurationElement::csptr parametersConfig = ext->findConfigurationElement("parameters");
        ::fwRuntime::ConfigurationElement::Container elements     = parametersConfig->getElements();
        for( ::fwRuntime::ConfigurationElement::sptr paramConfig :  elements )
        {
            std::string name = paramConfig->getExistingAttributeValue("name");
            std::string val  = paramConfig->getExistingAttributeValue("value");
            parameters[name] = val;
        }
        ::fwCore::mt::WriteLock lock(m_registryMutex);
        Registry::const_iterator iter = m_reg.find( extensionId );
        SLM_ASSERT("The id " <<  extensionId
                             << " already exists in the application configuration parameter registry",
                   iter == m_reg.end());
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

const FieldAdaptorType & AppConfigParameters::getParameters( const std::string & extensionId ) const
{
    ::fwCore::mt::ReadLock lock(m_registryMutex);
    Registry::const_iterator iter = m_reg.find( extensionId );
    SLM_ASSERT("The id " <<  extensionId << " is not found in the application configuration parameter registry",
               iter != m_reg.end());
    return iter->second;
}

//-----------------------------------------------------------------------------

} // namespace registry

} // namespace fwServices

