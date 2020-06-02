/************************************************************************
 *
 * Copyright (C) 2020 IRCAD France
 * Copyright (C) 2020 IHU Strasbourg
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

#include "ioIGTL/config.hpp"

#include <fwData/Object.hpp>

#include <fwServices/IController.hpp>

namespace ioIGTL
{

/// Interface providing behavior of network senders, if you want create a new network sender you must be inherit.
class IOIGTL_CLASS_API INetworkSender : public ::fwServices::IController
{

public:

    /// Generates default methods as New, dynamicCast, ...
    fwCoreServiceMacro(INetworkSender, ::fwServices::IController)

    /// Initializes signals.
    IOIGTL_API INetworkSender();

    /// Destoys the interface.
    IOIGTL_API virtual ~INetworkSender();

protected:

    /**
     * @brief Sends input objects
     * @pre The service must be started.
     */
    IOIGTL_API virtual void updating() override;

    /// Stops and restarts the service.
    IOIGTL_API virtual void swapping() override;

    /**
     * @brief Proposals to connect service slots to associated object signals.
     * @return A map of each proposed connection.
     *
     * Connect ::fwData::Object::s_MODIFIED_SIG to s_UPDATE_SLOT.
     */
    IOIGTL_API virtual KeyConnectionsMap getAutoConnections() const override;

    /**
     * @brief Sends the obj at the specified index.
     * @pre the configuration group must exists.
     */
    virtual void sendObject(const ::fwData::Object::csptr& obj, const size_t index) = 0;

    /// Defines the signal emitted when service is connected.
    typedef ::fwCom::Signal< void () > ConnectedSignalType;
    ConnectedSignalType::sptr m_sigConnected;

    /// Defines the signal emitted when service is disconnected.
    typedef ::fwCom::Signal< void () > DisconnectSignalType;
    DisconnectSignalType::sptr m_sigDisconnected;

};

} // namespace ioIGTL.
