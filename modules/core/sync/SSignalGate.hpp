/************************************************************************
 *
 * Copyright (C) 2018-2021 IRCAD France
 * Copyright (C) 2018-2019 IHU Strasbourg
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

#include "modules/sync/config.hpp"

#include <core/com/helper/SigSlotConnection.hpp>

#include <service/IController.hpp>

namespace sight::module::sync
{

/**
 * @brief   This service triggers a signal each time all configured signals are received. Once it triggers the signal
 * it resets its internal state and wait again for all signals to be received.
 *
 * @section Signals Signals
 * - \b allReceived(int) : Emitted each time all configured signals are received.
 *
 * @section XML XML Configuration
 *
 * @code{.xml}
        <service uid="..." type="::sight::module::sync::SSignalGate">
            <signal>data/modified</signal>
            <signal>object/changed</signal>
            <signal>service/computed</signal>
        </service>
   @endcode
 * @subsection Configuration Configuration
 *  - \b signal: list of signals to wait for
 */
class MODULE_SYNC_CLASS_API SSignalGate : public service::IController
{
public:

    SIGHT_DECLARE_SERVICE(SSignalGate, sight::service::IController)

    /**
     * @name Signals API
     * @{
     */
    MODULE_SYNC_API static const core::com::Signals::SignalKeyType s_ALL_RECEIVED_SIG;
    typedef core::com::Signal< void () > AllReceivedSignalType;
    typedef core::com::Slot< void () > ReceivedSignalType;
    /** @} */

    /// Constructor
    MODULE_SYNC_API SSignalGate();

    /// Destructor
    MODULE_SYNC_API virtual ~SSignalGate() override;

    /// Slot: called when one of the signals is called
    void received(size_t _index);

protected:

    /// Does nothing
    MODULE_SYNC_API virtual void configuring() override;

    /// Does nothing
    MODULE_SYNC_API virtual void starting() override;

    /// Does nothing
    MODULE_SYNC_API virtual void stopping() override;

    /// Does nothing
    MODULE_SYNC_API virtual void updating() override;

private:

    /// Keep track of received signals
    std::vector<bool> m_flags;

    std::vector< ReceivedSignalType::sptr > m_waitingSlots;

    /// Store all internal signal connections
    core::com::helper::SigSlotConnection m_connections;
};

} // namespace sight::module::sync
