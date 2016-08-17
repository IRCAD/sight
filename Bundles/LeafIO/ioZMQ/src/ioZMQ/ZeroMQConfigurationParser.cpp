/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "ioZMQ/ZeroMQConfigurationParser.hpp"

#include <string>

namespace ioZMQ
{

//-----------------------------------------------------------------------------

ZeroMQConfigurationParser::ZeroMQConfigurationParser (::fwRuntime::ConfigurationElement::sptr configuration)
{
    m_configuration = configuration;
}

//-----------------------------------------------------------------------------

ZeroMQConfigurationParser::~ZeroMQConfigurationParser()
{
}

//-----------------------------------------------------------------------------

void ZeroMQConfigurationParser::parse (
    Patterns::SupportedPatternMapType const& supportedPatterns) throw (::fwTools::Failed)
{

    ::fwRuntime::ConfigurationElement::sptr protocolElement;
    ::fwRuntime::ConfigurationElement::sptr patternElement;

    SLM_ASSERT("Configuration not found", m_configuration != NULL);
    if ( (protocolElement = m_configuration->findConfigurationElement ("protocol"))
         && (patternElement = m_configuration->findConfigurationElement ("pattern")))
    {
        this->parsePatternMode(patternElement->getValue(), supportedPatterns);
        this->parseProtocol(protocolElement);
    }
    else
    {
        throw ::fwTools::Failed("Cannot find protocol element or pattern element");
    }
}

//-----------------------------------------------------------------------------

void ZeroMQConfigurationParser::parseProtocol(
    ::fwRuntime::ConfigurationElement::sptr protocolElement) throw (::fwTools::Failed)
{
    std::string protocol;

    if (!(protocol = protocolElement->getAttributeValue ("type")).empty())
    {
        if (protocol == "tcp")
        {
            m_host = "tcp://";
            this->parseTcp(protocolElement);
        }
        else if (protocol == "ipc")
        {
            m_host = "ipc://";
            this->parseIpc(protocolElement);
        }
        else
        {
            throw ::fwTools::Failed("No such protocol : " + protocol);
        }
    }
    else
    {
        throw ::fwTools::Failed("Cannot find type attribute on protocol element");
    }
}

//-----------------------------------------------------------------------------

void ZeroMQConfigurationParser::parsePatternMode(std::string const& key,
                                                 Patterns::SupportedPatternMapType const& supportedPatterns) throw (::
                                                                                                                    fwTools
                                                                                                                    ::
                                                                                                                    Failed)
{
    Patterns::SupportedPatternMapType::const_iterator it;

    if ((it = supportedPatterns.find(key)) != supportedPatterns.end())
    {
        m_patternMode = it->second;
    }
    else
    {
        throw ::fwTools::Failed("Pattern : " + key + " is not supported");
    }
}

//-----------------------------------------------------------------------------

void ZeroMQConfigurationParser::parseTcp (
    ::fwRuntime::ConfigurationElement::sptr protocolElement) throw (::fwTools::Failed)
{
    ::fwRuntime::ConfigurationElement::sptr hostElement;
    ::fwRuntime::ConfigurationElement::sptr portElement;
    ::fwRuntime::ConfigurationElement::sptr modeElement;

    if ((modeElement = protocolElement->findConfigurationElement("mode")))
    {
        if (modeElement->getValue() == "Client")
        {
            m_sockMode = ::zmqNetwork::Socket::Client;
        }
        else if (modeElement->getValue() == "Server")
        {
            m_sockMode = ::zmqNetwork::Socket::Server;
        }
        else
        {
            throw ::fwTools::Failed("No such mode : " + modeElement->getValue());
        }

        if ((hostElement = protocolElement->findConfigurationElement("host"))
            && (portElement = protocolElement->findConfigurationElement("port")))
        {
            m_host = m_host + hostElement->getValue() + ":" + portElement->getValue();

        }
        else
        {
            throw ::fwTools::Failed("Cannot find host element or port element");
        }
    }
    else
    {
        throw ::fwTools::Failed("Cannot find mode element");
    }
}

//-----------------------------------------------------------------------------

void ZeroMQConfigurationParser::parseIpc (
    ::fwRuntime::ConfigurationElement::sptr protocolElement) throw (::fwTools::Failed)
{
    ::fwRuntime::ConfigurationElement::sptr modeElement;
    ::fwRuntime::ConfigurationElement::sptr fileElement;

    if ((fileElement = protocolElement->findConfigurationElement("file"))
        && (modeElement = protocolElement->findConfigurationElement("mode")))
    {
        if (modeElement->getValue() == "Client")
        {
            m_sockMode = ::zmqNetwork::Socket::Client;
        }
        else if (modeElement->getValue() == "Server")
        {
            m_sockMode = ::zmqNetwork::Socket::Server;
        }
        else
        {
            throw ::fwTools::Failed("No such mode : " + modeElement->getValue());
        }
        m_host = m_host + fileElement->getValue();
    }
}

//-----------------------------------------------------------------------------

::zmqNetwork::Socket::PatternMode const ZeroMQConfigurationParser::getPatternMode() const
{
    return m_patternMode;
}

//-----------------------------------------------------------------------------

::zmqNetwork::Socket::SocketMode const ZeroMQConfigurationParser::getSocketMode() const
{
    return m_sockMode;
}

//-----------------------------------------------------------------------------

std::string const& ZeroMQConfigurationParser::getHostname() const
{
    return m_host;
}

//-----------------------------------------------------------------------------

}//namespace zmqNetwork
