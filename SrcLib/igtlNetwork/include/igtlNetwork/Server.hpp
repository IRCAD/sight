/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2014.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __IGTLNETWORK_SERVER_HPP__
#define __IGTLNETWORK_SERVER_HPP__

#include "igtlNetwork/config.hpp"
#include "igtlNetwork/Client.hpp"
#include "igtlNetwork/INetwork.hpp"

#include <fwCore/Exception.hpp>
#include <fwCore/mt/types.hpp>

#include <fwTools/macros.hpp>

#include <boost/type.hpp>
#include <boost/thread.hpp>

#include <igtl/igtlServerSocket.h>

#include <list>
#include <string>

namespace igtlNetwork
{

/**
 * @class Server
 *
 * @brief a network server class use igtl::ServerSocket
 */
class IGTLNETWORK_CLASS_API Server : public ::igtlNetwork::INetwork
{
public:

    typedef SPTR (Server) sptr;

    /**
     * @brief constructor
     */
    IGTLNETWORK_API Server();

    /**
     * @brief destructor
     */
    IGTLNETWORK_API ~Server();

    /**
     * @brief method to start the server on a port
     *
     * @param[in] port the port to listen
     */
    IGTLNETWORK_API void start(::boost::uint16_t port) throw (::fwCore::Exception);

    /**
     * @brief method to wait a connection
     *
     * @param[in] msec time to wait in millisecond
     */
    IGTLNETWORK_API Client::sptr waitForConnection (int msec = 0);

    /**
     * @brief method to stop the server
     */
    IGTLNETWORK_API void stop() throw (::fwCore::Exception);

    /**
     * @brief method to test if the server started
     */
    IGTLNETWORK_API bool isStarted() const;

    /**
     * @brief method to broadcast to all client the obj
     */
    IGTLNETWORK_API void broadcast(::fwData::Object::sptr obj);

    /**
     * @brief get the port
     *
     * @return the port listened by server instance
     */
    IGTLNETWORK_API ::boost::uint16_t getPort() const;


    /**
     * @brief method to run server and start event loop of server
     */
    IGTLNETWORK_API void runServer();


private:

    /// server socket
    ::igtl::ServerSocket::Pointer m_serverSocket;

    ///mutex
    mutable ::fwCore::mt::Mutex m_mutex;

    /// state of server
    bool m_isStarted;

    /// Map of client each client have own thread it's dirty
    /// but i have no found other solution for multi client with openigtlink
    std::list<Client::sptr> m_clients;

    /// Server port
    ::boost::uint16_t m_port;

    /// integer constant for success
    static const int s_SUCCESS = 0;

};

}


#endif // __IGTLNETWORK_SERVER_HPP__
