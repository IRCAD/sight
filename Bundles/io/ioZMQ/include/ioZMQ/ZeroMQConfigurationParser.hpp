/************************************************************************
 *
 * Copyright (C) 2009-2018 IRCAD France
 * Copyright (C) 2012-2018 IHU Strasbourg
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

#pragma once

#include "ioZMQ/config.hpp"
#include "ioZMQ/Patterns.hpp"

#include <fwRuntime/ConfigurationElement.hpp>

#include <zmqNetwork/Socket.hpp>

#include <vector>

namespace ioZMQ
{
/**
 *
 * @code{.xml}
   <service uid="sender" type="::ioNetwork::INetworkSender" impl="::ioZMQ::SZeroMQSender" autoConnect="yes">
      <protocol type="tcp">
          <host>127.0.0.1</host>
          <port>4242</port>
          <mode>Client</mode>
      </protocol>
      <pattern>Subscribe</pattern>
   </service>
 * @endcode
 * - \b protocol: 'tcp' or 'ipc'
 *   - if protocol = tcp
 *      - \b port: port used for connection
 *      - \b mode: 'Client' or 'Server'
 *      - \b host: host/interface address
 *   - if protocol = ipc
 *     - \b file: file to connect
 *     - \b mode: 'Client' or 'Server'
 * @brief class to parse configuration element to get all needed information for all supported mode and protocol
 */
class IOZMQ_CLASS_API ZeroMQConfigurationParser
{
public:

    /// constructor
    IOZMQ_API ZeroMQConfigurationParser(::fwRuntime::ConfigurationElement::sptr configuration);

    /// Destructor
    IOZMQ_API ~ZeroMQConfigurationParser();

    /**
     * @brief method to parse configuration
     *
     * @param[in] supportedPattern supportedPattern by the service
     */
    IOZMQ_API void parse(Patterns::SupportedPatternMapType const& supportedPattern);

    /**
     * @brief get pattern mode
     *
     * @return pattern mode
     */
    IOZMQ_API ::zmqNetwork::Socket::PatternMode getPatternMode() const;

    /**
     * @brief get socket mode
     *
     * @return socket mode
     */
    IOZMQ_API ::zmqNetwork::Socket::SocketMode getSocketMode() const;

    /**
     * @brief get hostname or interface
     *
     * @return hostname or interface
     */
    IOZMQ_API std::string const& getHostname() const;

private:

    /**
     * @brief parse pattern mode for a string value given
     */
    void parsePatternMode(std::string const& key, Patterns::SupportedPatternMapType const& supportedPattern);

    /**
     * @brief parsing for tcp protocol
     */
    void parseTcp(::fwRuntime::ConfigurationElement::sptr protocolElement);

    /**
     * @brief parsing for ipc protocol
     */
    void parseIpc(::fwRuntime::ConfigurationElement::sptr protocolElement);

    /**
     * @brief get type and parse protocol type
     */
    void parseProtocol(::fwRuntime::ConfigurationElement::sptr protocolElement);

    /// configuration element
    ::fwRuntime::ConfigurationElement::sptr m_configuration;

    /// pattern mode
    ::zmqNetwork::Socket::PatternMode m_patternMode;

    /// socket mode
    ::zmqNetwork::Socket::SocketMode m_sockMode;

    /// m_host
    std::string m_host;
};
}
