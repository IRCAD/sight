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

    fwCoreServiceMacro(INetworkSender, ::fwServices::IController);

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
