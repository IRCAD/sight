/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2014-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __ZMQNETWORK_SOCKET_HPP__
#define __ZMQNETWORK_SOCKET_HPP__

#include "zmqNetwork/config.hpp"
#include "zmqNetwork/Exception.hpp"

#include <fwCore/Exception.hpp>

#include <fwData/Object.hpp>

#include <igtlProtocol/DataConverter.hpp>

#include <zmq.hpp>

#include <string>

namespace zmqNetwork
{
/**
 *
 * @brief class to handle a zmq::socket
 */
class ZMQNETWORK_CLASS_API Socket
{

public:

    /**
     * @typedef sptr
     *
     * @brief Shared pointer to a socket
     */
    typedef SPTR (Socket) sptr;

    /**
     * @enum SocketMode
     *
     * @brief Socket mode (client or server)
     */
    enum SocketMode
    {
        Server,
        Client
    };

    /**
     * @enum PatternMode
     *
     * @brief zmq pattern supported
     */
    enum PatternMode
    {
        Publish   = ZMQ_PUB,
        Subscribe = ZMQ_SUB,
        Push      = ZMQ_PUSH,
        Pull      = ZMQ_PULL,
        Request   = ZMQ_REQ,
        Reply     = ZMQ_REP
    };

    /**
     * @brief constructor
     */
    ZMQNETWORK_API Socket(SocketMode sockMode, PatternMode patternMode);

    /**
     * @brief destructor
     */
    ZMQNETWORK_API ~Socket();

    /**
     * @brief method to send a fwData object
     *
     * @param[in] data data to send
     */
    ZMQNETWORK_API void sendObject(const ::fwData::Object::csptr& data);

    /**
     * @brief method to receive fwData object from network
     *
     * @throw ::fwCore::Exception
     * @return object received null if response size equal 0
     */
    ZMQNETWORK_API ::fwData::Object::sptr receiveObject();

    /**
     * @brief start socket (connect or bind)
     *
     * @param[in] str string in zmq format
     */
    ZMQNETWORK_API void start(const std::string& str);

    /**
     * @brief stop socket(close socket and destroy context)
     */
    ZMQNETWORK_API void stop();

    /**
     * @brief set status to false(used to quit server event loop in a other thread)
     */
    ZMQNETWORK_API void terminate();

    /**
     * @brief get status of socket
     *
     * @return true if is started false otherwise
     */
    ZMQNETWORK_API bool isStarted() const;

    /**
     * @brief set receive timeout
     *
     * @param[in] msec timeout in msec
     */
    ZMQNETWORK_API void setReceiveTimeout(int msec);

    /**
     * @brief get pattern mode
     *
     * @see PatternMode
     * @return pattern mode
     */
    ZMQNETWORK_API PatternMode getPatternMode() const;

    /**
     * @brief get socket mode
     *
     * @see SocketMode
     * @return socket mode
     */
    ZMQNETWORK_API SocketMode getSocketMode() const;

    /**
     * @brief set device Name when igtl message are sended
     * @param[in] deviceName std::string deviceName
     */
    ZMQNETWORK_API void setDeviceNameOut(const std::string& deviceName);

    /**
     * @brief get device Name when igtl message are sended
     * @return std::string deviceName
     */
    ZMQNETWORK_API std::string getDeviceNameOut() const;

private:

    /**
     * @brief method to delete a message after it has been sended (for zero-copy send)
     *
     * @see http://zeromq.org/blog:zero-copy
     */
    static void deleteMessage(void* data, void* hint);

    /**
     * @brief initialize socket(create context)
     */
    void initialize();

    /**
     * @typedef SocketSPtr
     *
     * @brief Shared pointer to a zmq::socket_t
     */
    typedef SPTR (::zmq::socket_t) SocketSPtr;

    /**
     * @typedef ContextSPtr
     *
     * @brief Shared pointer to a ::zmq::context_t
     */
    typedef SPTR (::zmq::context_t) ContextSPtr;

    /// Shared pointer to a ::zmq::socket
    SocketSPtr m_socket;

    /// status of socket, true if is started false otherwise
    bool m_isStarted;

    /// DataConverter instance(used for conversion between F4S data and OpenIGTLinkProtocol data)
    ::igtlProtocol::DataConverter::sptr m_dataConverter;

    /// Socket context
    ContextSPtr m_context;

    /// Socket Mode
    SocketMode m_sockMode;

    /// Pattern mode
    PatternMode m_patternMode;

    /// timeout
    std::uint32_t m_timeout;

    /// header size
    static const int s_HEADER_SIZE = 58;

    /// device name in sent IGTL message
    std::string m_deviceNameOut;

};
}//namespace zmqNetwork

#endif //__ZMQNETWORK_SOCKET_HPP__
