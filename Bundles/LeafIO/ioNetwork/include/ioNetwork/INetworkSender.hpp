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
 * @brief Abstract class for network senders
 * You need to inherit from this class if you want implement a new network sender
 */
class IONETWORK_CLASS_API INetworkSender : public ::fwServices::IController
{

public:

    fwCoreServiceClassDefinitionsMacro( (INetworkSender) (::fwServices::IController));

    /**
     * @brief Service connected signal is emitted when the server is started
     */
    IONETWORK_API static const ::fwCom::Signals::SignalKeyType s_CONNECTED_SIGNAL;

    /**
     * @typedef ConnectedSignalType
     *
     * @brief ConnectedSignalType is stored and emitted when the sender is started
     */
    typedef ::fwCom::Signal< void () > ConnectedSignalType;

    /**
     * @brief Service disconnected signal is emitted when the sender is stopped
     */
    IONETWORK_API static const ::fwCom::Signals::SignalKeyType s_DISCONNECTED_SIGNAL;

    /**
     * @typedef DisconnectSignalType
     *
     * @brief DisconnectSignalType is stored and emitted when the sender is stopped
     */
    typedef ::fwCom::Signal< void () > DisconnectSignalType;

    /// Constructor
    IONETWORK_API INetworkSender();

    /// Destructor
    IONETWORK_API virtual ~INetworkSender();

protected:

    /// Sends the object
    IONETWORK_API virtual void updating() override;

    /// Calls stopping and starting
    IONETWORK_API virtual void swapping() override;

    /**
     * @brief Sends the obj at index
     * Usable if the configuration group exists.
     */
    virtual void sendObject(const ::fwData::Object::csptr& obj, const size_t index) = 0;

    /// Signal emitted when service is connected
    ConnectedSignalType::sptr m_sigConnected;

    /// Signal emitted when service is disconnected
    DisconnectSignalType::sptr m_sigDisconnected;
};

} // namespace ioNetwork

#endif /*__IONETWORK_INETWORKSENDER_HPP__*/

