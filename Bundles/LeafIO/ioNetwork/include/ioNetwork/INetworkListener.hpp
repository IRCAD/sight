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

namespace ioNetwork
{

/**
 *
 * @brief Abstract class for network listeners if you want create a new network listener you must be inherit
 *
 */
class IONETWORK_CLASS_API INetworkListener : public ::fwServices::IController
{

public:
    fwCoreServiceClassDefinitionsMacro( (INetworkListener) (::fwServices::IController));

    /**
     * @brief Service connected signal is emitted when listener is connected
     */
    IONETWORK_API static const ::fwCom::Signals::SignalKeyType s_CONNECTED_SIGNAL;

    /**
     * @typedef ConnectedSignalType
     *
     * @brief ConnectedSignalType is stored and emit it when listener is connected
     */
    typedef ::fwCom::Signal< void () >  ConnectedSignalType;

    /**
     * @brief Service disconnected signal is emitted when listener is disconnected
     */
    IONETWORK_API static const ::fwCom::Signals::SignalKeyType s_DISCONNECTED_SIGNAL;

    /**
     * @typedef DisconnectSignalType
     *
     * @brief DisconnectSignalType is stored and emit it when listener is disconnected
     */
    typedef ::fwCom::Signal< void () > DisconnectSignalType;

    /// Constructor
    IONETWORK_API INetworkListener();

    /// Destructor
    IONETWORK_API virtual ~INetworkListener();

protected:

    /// Overrides
    IONETWORK_API virtual void updating() override;

    /// Overrides
    IONETWORK_API virtual void swapping() override;

    /// Signal emitted when service is connected
    ConnectedSignalType::sptr m_sigConnected;

    /// Signal emitted when service is disconnected
    DisconnectSignalType::sptr m_sigDisconnected;

};

} // namespace ioNetwork

#endif /*__IONETWORK_INETWORKLISTENER_HPP__*/

