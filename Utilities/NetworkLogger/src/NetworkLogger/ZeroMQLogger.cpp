/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "NetworkLogger/ZeroMQLogger.hpp"

namespace networkLogger
{
ZeroMQLogger::ZeroMQLogger() : m_socket(::zmqNetwork::Socket(zmqNetwork::Socket::Client,
                                                             ::zmqNetwork::Socket::Subscribe))
{

}

ZeroMQLogger::~ZeroMQLogger()
{

}

void ZeroMQLogger::log(std::ostream& outputStream) throw (std::runtime_error)
{
    m_socket.start(m_connectStr);
    m_logFunction(outputStream, m_socket);
    m_socket.stop();
}

void ZeroMQLogger::configure(int argc, char** argv) throw (std::runtime_error)
{
    if (argc == 0)
    {
        throw std::runtime_error("./NetworkLogger <type> <file> <zmq_connect_str|port>");
    }
    else
    {
        m_connectStr = std::string(argv[0]);
    }
}

INetworkLogger::sptr ZeroMQLogger::create(FunctionType const& logFunction)
{
    ZeroMQLogger* logger;

    logger                = new ZeroMQLogger();
    logger->m_logFunction = logFunction;
    return INetworkLogger::sptr(logger);
}
}
