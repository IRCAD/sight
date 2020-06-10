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

#include <fwCom/Signal.hpp>

#include <fwServices/IController.hpp>

namespace ioIGTL
{
/// Interface providing behavior of network listeners, if you want create a new network listener you must be inherit.
class IOIGTL_CLASS_API INetworkListener : public ::fwServices::IController
{

public:

    /// Generates default methods as New, dynamicCast, ...
    fwCoreServiceMacro(INetworkListener, ::fwServices::IController)

    /// Initializes signals.
    IOIGTL_API INetworkListener();

    /// Destoys the interface.
    IOIGTL_API ~INetworkListener() override;

protected:

    /// Defines the signal's name emitted when service is connected.
    IOIGTL_API static const ::fwCom::Signals::SignalKeyType s_CONNECTED_SIGNAL;

    /// Defines the signal's name emitted when service is disconnected.
    IOIGTL_API static const ::fwCom::Signals::SignalKeyType s_DISCONNECTED_SIGNAL;

    /// Does nothing.
    IOIGTL_API void updating() override;

    /// Defines the signal emitted when service is connected.
    typedef ::fwCom::Signal< void () > ConnectedSignalType;
    ConnectedSignalType::sptr m_sigConnected;

    /// Defines the signal emitted when service is disconnected.
    typedef ::fwCom::Signal< void () > DisconnectSignalType;
    DisconnectSignalType::sptr m_sigDisconnected;

};

} // namespace ioIGTL.
