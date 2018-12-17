/************************************************************************
 *
 * Copyright (C) 2009-2017 IRCAD France
 * Copyright (C) 2012-2017 IHU Strasbourg
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

#include "uiNetwork/ConnectorModel.hpp"

#include <fwRuntime/ConfigurationElementContainer.hpp>

#include <fwServices/macros.hpp>
#include <fwServices/registry/ServiceFactory.hpp>

#include <boost/function.hpp>

#include <algorithm>

namespace uiNetwork
{

//-----------------------------------------------------------------------------

ConnectorModel::ConnectorModel ()
{
}

//-----------------------------------------------------------------------------

ConnectorModel::~ConnectorModel()
{
}

//-----------------------------------------------------------------------------

bool ConnectorModel::isRegisteredService(std::string const& serviceName) const
{
    for(std::string const& registeredService :  m_registeredListeners)
    {
        if (registeredService == serviceName)
        {
            return true;
        }
    }

    for(std::string const& registeredService :  m_registeredSenders)
    {
        if (registeredService == serviceName)
        {
            return true;
        }
    }
    return false;
}

//-----------------------------------------------------------------------------

void ConnectorModel::initialize (::fwRuntime::ConfigurationElement::csptr configElement,
                                 ::fwData::Object::csptr obj)
{
    std::string serviceName;
    ::fwServices::registry::ServiceFactory::sptr serviceFactory;
    ::fwRuntime::ConfigurationElement::sptr currentConfig;
    ::fwRuntime::ConfigurationElementContainer serviceConfigs;
    ::fwRuntime::ConfigurationElementContainer::Iterator it;

    serviceFactory        = ::fwServices::registry::ServiceFactory::getDefault();
    m_registeredListeners = serviceFactory->getImplementationIdFromObjectAndType(obj->getClassname(),
                                                                                 "::ioNetwork::INetworkListener");
    m_registeredSenders = serviceFactory->getImplementationIdFromObjectAndType(obj->getClassname(),
                                                                               "::ioNetwork::INetworkSender");
    serviceConfigs = configElement->findAllConfigurationElement("config");
    for (it = serviceConfigs.begin(); it != serviceConfigs.end(); ++it)
    {
        currentConfig = *it;
        if (!currentConfig->hasAttribute("impl"))
        {
            throw ::fwTools::Failed("Missing 'impl' attribute in service configuration");
        }
        serviceName = currentConfig->getAttributeValue("impl");
        if (!this->isRegisteredService(serviceName))
        {
            throw ::fwTools::Failed("Cannot find service name in registered service");
        }
        m_serviceConfigs[serviceName] = currentConfig;
    }
}

//-----------------------------------------------------------------------------

ConnectorModel::ServiceConfigurationContainer const& ConnectorModel::getServiceConfigurations() const
{
    return m_serviceConfigs;
}

//-----------------------------------------------------------------------------

std::vector<std::string> const& ConnectorModel::getRegisteredListeners() const
{
    return m_registeredListeners;
}

//-----------------------------------------------------------------------------

std::vector<std::string> const& ConnectorModel::getRegisteredSenders() const
{
    return m_registeredSenders;
}

//-----------------------------------------------------------------------------

} // namespace uiNetwork
