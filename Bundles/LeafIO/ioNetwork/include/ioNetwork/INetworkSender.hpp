/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __IONETWORK_INETWORKSENDER_HPP__
#define __IONETWORK_INETWORKSENDER_HPP__

#include "ioNetwork/config.hpp"

#include <fwData/Object.hpp>

#include <fwServices/IController.hpp>

namespace ioNetwork
{

/**
 * @brief Abstract class for network senders(you need to inherit from this class if you want implement a new network
 * sender).
 *
 * @note The inherited service will automatically be connected to its object. This bypass the "autoConnect" because of
 * the OSLM_FATAL_IF when service receives a message.
 */
class IONETWORK_CLASS_API INetworkSender : public ::fwServices::IController
{

public:

    fwCoreServiceClassDefinitionsMacro( (INetworkSender) (::fwServices::IController));

    /**
     * @brief Server started signal is emitted when the server is started
     */
    IONETWORK_API static const ::fwCom::Signals::SignalKeyType s_SERVER_STARTED_SIGNAL;

    /**
     * @typedef ServerStartedSignalType
     *
     * @brief ServerStartedSignalType is stored and emitted when the sender is started
     */
    typedef ::fwCom::Signal< void () > ServerStartedSignalType;

    /**
     * @brief Server stopped signal is emitted when the sender is stopped
     */
    IONETWORK_API static const ::fwCom::Signals::SignalKeyType s_SERVER_STOPPED_SIGNAL;

    /**
     * @typedef ServerStoppedSignalType
     *
     * @brief ServerStoppedSignalType is stored and emitted when the sender is stopped
     */
    typedef ::fwCom::Signal< void () > ServerStoppedSignalType;

    /// Constructor
    IONETWORK_API INetworkSender();

    /// Destructor
    IONETWORK_API virtual ~INetworkSender();

protected:

    /// Overrides
    IONETWORK_API virtual void configuring() override;

    /// Make connection like autoConnect enable message reception
    IONETWORK_API virtual void starting() override;

    /// Disconnect connection to bypass the OSLM_FATAL_IF when service receives a message
    IONETWORK_API virtual void stopping() override;

    /// Sends the object
    IONETWORK_API virtual void updating() override;

    /// Calls stopping and starting
    IONETWORK_API virtual void swapping() override;

    /**
     * @brief Sends the obj at index
     * Useable if the configuration group exists.
     */
    virtual void sendObject(const ::fwData::Object::csptr& obj, const size_t index) = 0;

    /// Signal when server started
    ServerStartedSignalType::sptr m_sigServerStarted;

    /// Signal when server stopped
    ServerStoppedSignalType::sptr m_sigServerStopped;
};

} // namespace ioNetwork

#endif /*__IONETWORK_INETWORKSENDER_HPP__*/

