/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __IOZMQ_SZEROMQLISTENER_HPP__
#define __IOZMQ_SZEROMQLISTENER_HPP__

#include "ioZMQ/config.hpp"

#include <fwCom/Slot.hpp>

#include <fwThread/Worker.hpp>

#include <zmqNetwork/Socket.hpp>

#include <ioNetwork/INetworkListener.hpp>

namespace ioZMQ
{
/**
 *
 * @brief network listener using ZMQNetwork library
 */
class IOZMQ_CLASS_API SZeroMQListener : public ::ioNetwork::INetworkListener
{

public:

    fwCoreServiceClassDefinitionsMacro( (SZeroMQListener) (::ioNetwork::INetworkListener));

    /**
     * @brief Slot to update configuration
     */
    IOZMQ_API static const ::fwCom::Slots::SlotKeyType s_UPDATE_CONFIGURATION_SLOT;
    typedef ::fwCom::Slot< void (const ::zmqNetwork::Socket::PatternMode,
                                 const ::zmqNetwork::Socket::SocketMode,
                                 const std::string& host) >                 UpdateConfigurationSlotType;

    /// Constructor
    IOZMQ_API SZeroMQListener();

    /// Destructor
    IOZMQ_API virtual ~SZeroMQListener();

protected:
    /**
     * @brief configure the listener and parse configuration with ZeroMQConfigurationParser
     *
     * @see ZeroMQConfigurationParser for the format of the configuration
     */
    IOZMQ_API virtual void configuring() override;

    /**
     * @brief start the socket
     */
    IOZMQ_API virtual void starting() override;

    /**
     * @brief stop the socket
     */
    IOZMQ_API virtual void stopping() override;

    /**
     * @brief slot method to update configuration
     *
     * @param[in] patternMode pattern mode (push, pull,...)
     * @param[in] sockMode socket mode (client or server)
     * @param[in] host new host
     */
    IOZMQ_API void updateConfiguration(   ::zmqNetwork::Socket::PatternMode const patternMode,
                                          ::zmqNetwork::Socket::SocketMode const sockMode,
                                          std::string const& host);

    IOZMQ_API void setHost(std::string const& host, std::uint16_t const port);

private:
    /**
     * @brief This method handle message and notify to all service when received message.
     */
    void runReceiver();

    /// Socket smart pointer
    ::zmqNetwork::Socket::sptr m_socket;

    /// Pattern to use (Push, Pull, Publish,...)
    ::zmqNetwork::Socket::PatternMode m_patternMode;

    /// Socket mode (Client or Server)
    ::zmqNetwork::Socket::SocketMode m_sockMode;

    /// interface to bind or host connect
    std::string m_hostStr;

    /// UpdateConfiguration slot
    UpdateConfigurationSlotType::sptr m_updateConfigurationSlot;

    /// Thread for running runReceiver method
    ::fwThread::Worker::sptr m_receiveWorker;
};

} // namespace ioZMQ

#endif /*__IOZMQ_SZEROMQLISTENER_HPP__*/

