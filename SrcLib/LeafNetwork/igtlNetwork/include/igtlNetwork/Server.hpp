/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2014-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __IGTLNETWORK_SERVER_HPP__
#define __IGTLNETWORK_SERVER_HPP__

#include "igtlNetwork/Client.hpp"
#include "igtlNetwork/INetwork.hpp"
#include "igtlNetwork/config.hpp"

#include <boost/type.hpp>

#include <fwCore/Exception.hpp>
#include <fwCore/mt/types.hpp>

#include <igtl/igtlServerSocket.h>

#include <list>
#include <string>

namespace igtlNetwork
{

/**
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
     * @brief method to broadcast to all client a msg
     */
    IGTLNETWORK_API void broadcast(::igtl::MessageBase::Pointer msg);

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

    /**
     * @brief method to have the current number of clients
     */
    IGTLNETWORK_API unsigned int getNumberOfClients();
    /**
     * @brief methdo to receive all headers of all connected clients
     *
     * @return vector of igl::MessageHeader::Pointer
     */

    IGTLNETWORK_API std::vector< ::igtl::MessageHeader::Pointer > receiveHeader();

    /** @brief receive body pack of a specific connected client
     *
     *  @param[in] header msg header
     *  @param[in] client num
     *
     *  @return Message
     */
    IGTLNETWORK_API ::igtl::MessageBase::Pointer receiveBody (::igtl::MessageHeader::Pointer header,
                                                              unsigned int client) throw (::fwCore::Exception);

    /**
     * @brief set the device name when a message is sended
     */
    IGTLNETWORK_API void setMessageDeviceName(std::string deviceName);

private:

    /// server socket
    ::igtl::ServerSocket::Pointer m_serverSocket;

    ///mutex
    mutable ::fwCore::mt::Mutex m_mutex;

    /// state of server
    bool m_isStarted;

    /// vector of clients
    std::vector< Client::sptr > m_clients;

    /// Server port
    ::boost::uint16_t m_port;

    /// integer constant for success
    static const int s_SUCCESS = 0;

    /// device name in the sended message
    std::string m_deviceNameOut;

};

}

#endif // __IGTLNETWORK_SERVER_HPP__
