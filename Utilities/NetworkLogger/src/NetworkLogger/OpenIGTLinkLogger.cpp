/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <boost/lexical_cast.hpp>
#include "NetworkLogger/OpenIGTLinkLogger.hpp"

namespace networkLogger
{
OpenIGTLinkLogger::OpenIGTLinkLogger()
{

}

OpenIGTLinkLogger::~OpenIGTLinkLogger()
{

}

void OpenIGTLinkLogger::log(std::ostream& outputStream) throw (std::runtime_error)
{
    m_client.connect(m_host, m_port);
    m_logFunction(outputStream, m_client);
    m_client.disconnect();
}

void OpenIGTLinkLogger::configure(int argc, char** argv) throw (std::runtime_error)
{
    if (argc < 2)
    {
        throw std::runtime_error("./NetworkLogger <type> <file> <zmq_connect_str|port>");
    }
    else
    {
        m_host = std::string(argv[0]);
        m_port = ::boost::lexical_cast<boost::uint16_t>(argv[1]);
    }
}

INetworkLogger::sptr OpenIGTLinkLogger::create(FunctionType const& logFunction)
{
    OpenIGTLinkLogger* logger;

    logger                = new OpenIGTLinkLogger();
    logger->m_logFunction = logFunction;
    return INetworkLogger::sptr(logger);
}
}