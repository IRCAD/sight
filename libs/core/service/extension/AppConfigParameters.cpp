/************************************************************************
 *
 * Copyright (C) 2009-2021 IRCAD France
 * Copyright (C) 2012-2019 IHU Strasbourg
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

#include "service/extension/AppConfigParameters.hpp"

#include <core/runtime/ConfigurationElement.hpp>
#include <core/runtime/helper.hpp>
#include <core/runtime/Runtime.hpp>

#include <data/Composite.hpp>
#include <data/String.hpp>

#include <regex>

namespace sight::service
{
namespace extension
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
    std::vector< std::shared_ptr< core::runtime::Extension > >  extensions = core::runtime::getAllExtensionsForPoint(
        "::sight::service::extension::AppConfigParameters");
    for( std::shared_ptr< core::runtime::Extension > ext :  extensions )
    {
        // Get id
        std::string extensionId = ext->findConfigurationElement("id")->getValue();

        FieldAdaptorType parameters;

        // Get parmeters
        core::runtime::ConfigurationElement::csptr parametersConfig = ext->findConfigurationElement("parameters");
        core::runtime::ConfigurationElement::Container elements     = parametersConfig->getElements();
        for( core::runtime::ConfigurationElement::sptr paramConfig :  elements )
        {
            std::string name = paramConfig->getExistingAttributeValue("name");
            std::string val  = paramConfig->getExistingAttributeValue("value");
            parameters[name] = val;
        }
        core::mt::WriteLock lock(m_registryMutex);
#ifdef _DEBUG
        Registry::const_iterator iter = m_reg.find( extensionId );
#endif
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
    core::mt::WriteLock lock(m_registryMutex);
    m_reg.clear();
}

//-----------------------------------------------------------------------------

const FieldAdaptorType& AppConfigParameters::getParameters( const std::string& extensionId ) const
{
    core::mt::ReadLock lock(m_registryMutex);
    Registry::const_iterator iter = m_reg.find( extensionId );
    SLM_ASSERT("The id " <<  extensionId << " is not found in the application configuration parameter registry",
               iter != m_reg.end());
    return iter->second;
}

//-----------------------------------------------------------------------------

} // namespace extension

} // namespace sight::service
