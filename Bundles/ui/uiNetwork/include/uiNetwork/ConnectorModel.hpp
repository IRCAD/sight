/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __UINETWORK_CONNECTORMODEL_HPP__
#define __UINETWORK_CONNECTORMODEL_HPP__

#include "uiNetwork/config.hpp"

#include <fwTools/Failed.hpp>
#include <fwRuntime/ConfigurationElement.hpp>
#include <fwData/Object.hpp>

#include <vector>
#include <string>
#include <map>

namespace uiNetwork
{
/**
 *
 * @brief A model used to retreive service configuration and network service in current F4S app
 */
class UINETWORK_CLASS_API ConnectorModel
{
public:
    /**
     * @typedef ServiceConfigurationContainer
     *
     * @brief map when each key have a configuration element used to store services configuration
     */
    typedef std::map<std::string, ::fwRuntime::ConfigurationElement::sptr> ServiceConfigurationContainer;

    /// Constructor
    ConnectorModel();

    /// Destructor
    ~ConnectorModel();

    /**
     * @brief parse the configuration element, retrieve all service configuration and retrieve all network service
     * registered
     */
    void initialize(::fwRuntime::ConfigurationElement::csptr configElement,
                    ::fwData::Object::csptr obj);

    /**
     * @brief getter for registered ioNetwork::INetworkListener in a F4S app
     *
     * @return a vector of service name in a F4S app
     */
    std::vector<std::string> const& getRegisteredListeners() const;

    /**
     * @brief getter for registered ioNetwork::INetworkSender in a F4S app
     *
     * @return  a vector of service name in a F4S app
     */
    std::vector<std::string> const& getRegisteredSenders() const;

    /**
     * @brief getter for services configuration
     *
     * @return @see ServiceConfigurationContainer
     */
    ServiceConfigurationContainer const& getServiceConfigurations() const;


    bool isRegisteredService(std::string const& str) const;
private:
    /// registered listeners name
    std::vector<std::string> m_registeredListeners;

    /// registered senders name
    std::vector<std::string> m_registeredSenders;

    /// services configuration
    ServiceConfigurationContainer m_serviceConfigs;

};
} //namespace uiNetwork

#endif // __UINETWORK_CONNECTORMODEL_HPP__
