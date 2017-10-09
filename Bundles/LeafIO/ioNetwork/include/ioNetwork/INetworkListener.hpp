/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __IONETWORK_INETWORKLISTENER_HPP__
#define __IONETWORK_INETWORKLISTENER_HPP__

#include "ioNetwork/config.hpp"

#include <fwCom/Signal.hpp>

#include <fwServices/IController.hpp>

#include <boost/type.hpp>

#include <map>
#include <string>

namespace ioNetwork
{

/**
 *
 * @brief Abstract class for network listeners if you want create a new network listener you must be inherit
 *
 */
class IONETWORK_CLASS_API INetworkListener :
    public ::fwServices::IController
{

public:
    fwCoreServiceClassDefinitionsMacro( (INetworkListener) (::fwServices::IController));

    /**
     * @brief Receive object signal is emitted when listener receive a new object from network
     */
    IONETWORK_API static const ::fwCom::Signals::SignalKeyType s_RECEIVED_OBJECT_SIGNAL;

    /**
     * @typedef ReceivedObjectSignalType
     *
     * @brief ReceivedObjectSignalType is stored and emit it when receive new object from network
     */
    typedef ::fwCom::Signal< void (::fwData::Object::sptr) > ReceivedObjectSignalType;

    /**
     * @brief Client connected signal is emitted when listener is connected to a server
     */
    IONETWORK_API static const ::fwCom::Signals::SignalKeyType s_CLIENT_CONNECTED_SIGNAL;

    /**
     * @typedef ClientConnectedSignalType
     *
     * @brief ClientConnectedSignalType is stored and emit it when listener is connected
     */
    typedef ::fwCom::Signal< void () >  ClientConnectedSignalType;

    /**
     * @brief Client disconnected signal is emitted when listener is disconnected from the server
     */
    IONETWORK_API static const ::fwCom::Signals::SignalKeyType s_CLIENT_DISCONNECTED_SIGNAL;

    /**
     * @typedef ClientDisconnectSignalType
     *
     * @brief ClientDisconnectSignalType is stored and emit it when listener is disconnected
     */
    typedef ::fwCom::Signal< void () > ClientDisconnectSignalType;

    /// Constructor
    IONETWORK_API INetworkListener();

    /// Destructor
    IONETWORK_API virtual ~INetworkListener();

protected:
    /// Overrides
    IONETWORK_API virtual void configuring() override;

    /// Overrides
    IONETWORK_API virtual void starting() override;

    /// Overrides
    IONETWORK_API virtual void stopping() override;

    /// Overrides
    IONETWORK_API virtual void updating() override;

    /// Overrides
    IONETWORK_API virtual void swapping() override;

    /// Signal emitted when you receive an object
    ReceivedObjectSignalType::sptr m_sigReceivedObject;

    /// Signal emitted when client is connected
    ClientConnectedSignalType::sptr m_sigClientConnected;

    /// Signal emitted when client is disconnected
    ClientDisconnectSignalType::sptr m_sigClientDisconnected;

};

} // namespace ioNetwork

#endif /*__IONETWORK_INETWORKLISTENER_HPP__*/

