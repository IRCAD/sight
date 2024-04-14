/************************************************************************
 *
 * Copyright (C) 2018-2024 IRCAD France
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

#include <core/com/helper/sig_slot_connection.hpp>

#include <service/controller.hpp>

namespace sight::module::sync
{

/**
 * @brief   This service triggers a signal each time all configured signals are received. Once it triggers the signal
 * it resets its internal state and wait again for all signals to be received.
 *
 * @section Signals Signals
 * - \b all_received(int) : Emitted each time all configured signals are received.
 *
 * @section XML XML Configuration
 *
 * @code{.xml}
        <service uid="..." type="sight::module::sync::signal_gate">
            <signal>data/modified</signal>
            <signal>object/changed</signal>
            <signal>service/computed</signal>
        </service>
   @endcode
 * @subsection Configuration Configuration
 *  - \b signal: list of signals to wait for
 */
class signal_gate : public service::controller
{
public:

    SIGHT_DECLARE_SERVICE(signal_gate, sight::service::controller);

    /**
     * @name Signals API
     * @{
     */
    static const core::com::signals::key_t ALL_RECEIVED_SIG;
    using all_received_signal_t = core::com::signal<void ()>;
    using received_signal_t     = core::com::slot<void ()>;
    /** @} */

    /// Constructor
    signal_gate();

    /// Destructor
    ~signal_gate() override;

    /// Slot: called when one of the signals is called
    void received(std::size_t _index);

protected:

    /// Does nothing
    void configuring() override;

    /// Does nothing
    void starting() override;

    /// Does nothing
    void stopping() override;

    /// Does nothing
    void updating() override;

private:

    /// Keep track of received signals
    std::vector<bool> m_flags;

    std::vector<received_signal_t::sptr> m_waiting_slots;

    /// Store all internal signal connections
    core::com::helper::sig_slot_connection m_connections;
};

} // namespace sight::module::sync
