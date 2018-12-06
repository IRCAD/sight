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

void ZeroMQLogger::log(std::ostream& outputStream)
{
    m_socket.start(m_connectStr);
    m_logFunction(outputStream, m_socket);
    m_socket.stop();
}

void ZeroMQLogger::configure(int argc, char** argv)
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
