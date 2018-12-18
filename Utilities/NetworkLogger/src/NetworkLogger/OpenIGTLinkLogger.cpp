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

#include "NetworkLogger/OpenIGTLinkLogger.hpp"

#include <boost/lexical_cast.hpp>

namespace networkLogger
{
OpenIGTLinkLogger::OpenIGTLinkLogger()
{

}

OpenIGTLinkLogger::~OpenIGTLinkLogger()
{

}

//------------------------------------------------------------------------------

void OpenIGTLinkLogger::log(std::ostream& outputStream)
{
    m_client.connect(m_host, m_port);
    m_logFunction(outputStream, m_client);
    m_client.disconnect();
}

//------------------------------------------------------------------------------

void OpenIGTLinkLogger::configure(int argc, char** argv)
{
    if (argc < 2)
    {
        throw std::runtime_error("./NetworkLogger <type> <file> <zmq_connect_str|port>");
    }
    else
    {
        m_host = std::string(argv[0]);
        m_port = ::boost::lexical_cast<std::uint16_t>(argv[1]);
    }
}

//------------------------------------------------------------------------------

INetworkLogger::sptr OpenIGTLinkLogger::create(FunctionType const& logFunction)
{
    OpenIGTLinkLogger* logger;

    logger                = new OpenIGTLinkLogger();
    logger->m_logFunction = logFunction;
    return INetworkLogger::sptr(logger);
}
}