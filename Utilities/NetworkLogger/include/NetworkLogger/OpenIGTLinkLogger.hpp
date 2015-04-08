/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2014.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _NETWORKLOGGER_OPENIGTLINKLOGGER_HPP_
#define _NETWORKLOGGER_OPENIGTLINKLOGGER_HPP_

#include "NetworkLogger/INetworkLogger.hpp"
#include "NetworkLogger/types.hpp"

#include <igtlNetwork/Client.hpp>

#include <boost/type.hpp>

namespace networkLogger
{
/**
 * @class OpenIGTLinkLogger
 *
 * @brief OpenIGTLink logger version use OpenIGTLinkNetwork socket to receive/connect
 */
class OpenIGTLinkLogger : public INetworkLogger
{
public:

    /**
     * @typedef SocketType
     *
     * @brief define the OpenIGTLink::Client socket to be the socket type to use
     */
    typedef igtlNetwork::Client SocketType;

    /**
     * @typedef FunctionType
     *
     * @brief logger function type to use
     */
    typedef LoggerFunction<SocketType>::FunctionType FunctionType;

    ///Constructor
    OpenIGTLinkLogger();

    ///Destructor
    ~OpenIGTLinkLogger();

    /**
     * @brief log receive object from network to outputStream
     * @throw std::runtime_error
     */
    void log(std::ostream &outputStream) throw (std::runtime_error);

    /**
     * @brief configure OpenIGTLinkLogger(hostname + port)
     *
     * @param[in] argc number of arguments
     * @param[in] argv arguments
     * @throw std::runtime_error
     */
    void configure(int argc, char **argv) throw (std::runtime_error);

    /**
     * @brief create a OpenIGTLinkLogger with a user-defined function
     *
     * @param[in] logFunction log is like a incomplete method and the user-defined function complete the log method
     * @return smart pointer contain a instance of OpenIGTLinkLogger
     */
    static INetworkLogger::sptr create(FunctionType const &logFunction);
private:

    /// client socket
    ::igtlNetwork::Client m_client;

    /// port
    ::boost::uint16_t m_port;

    /// host
    std::string m_host;

    /// user-defined function
    FunctionType m_logFunction;
};
}

#endif // _NETWORKLOGGER_OPENIGTLINKLOGGER_HPP_
