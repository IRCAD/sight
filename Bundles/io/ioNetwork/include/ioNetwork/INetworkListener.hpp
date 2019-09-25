/************************************************************************
 *
 * Copyright (C) 2009-2019 IRCAD France
 * Copyright (C) 2012-2019 IHU Strasbourg
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

#pragma once

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
    fwCoreServiceMacro(INetworkListener, ::fwServices::IController);

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
