/************************************************************************
 *
 * Copyright (C) 2018 IRCAD France
 * Copyright (C) 2018 IHU Strasbourg
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

#include "ctrlCom/config.hpp"

#include <fwCom/helper/SigSlotConnection.hpp>

#include <fwServices/IController.hpp>

namespace ctrlCom
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
        <service uid="..." type="::ctrlCom::SSignalGate">
            <signal>data/modified</signal>
            <signal>object/changed</signal>
            <signal>service/computed</signal>
        </service>
   @endcode
 * @subsection Configuration Configuration
 *  - \b signal: list of signals to wait for
 */
class CTRLCOM_CLASS_API SSignalGate : public ::fwServices::IController
{
public:

    fwCoreServiceClassDefinitionsMacro( (SSignalGate)(::fwServices::IController) )

    /**
     * @name Signals API
     * @{
     */
    CTRLCOM_API static const ::fwCom::Signals::SignalKeyType s_ALL_RECEIVED_SIG;
    typedef ::fwCom::Signal< void () > AllReceivedSignalType;
    typedef ::fwCom::Slot< void () > ReceivedSignalType;
    /** @} */

    /// Constructor
    CTRLCOM_API SSignalGate();

    /// Destructor
    CTRLCOM_API virtual ~SSignalGate() override;

    /// Slot: called when one of the signals is called
    void received(size_t _index);

protected:

    /// Does nothing
    CTRLCOM_API virtual void configuring() override;

    /// Does nothing
    CTRLCOM_API virtual void starting() override;

    /// Does nothing
    CTRLCOM_API virtual void stopping() override;

    /// Does nothing
    CTRLCOM_API virtual void updating() override;

private:

    /// Keep track of received signals
    std::vector<bool> m_flags;

    std::vector< ReceivedSignalType::sptr > m_waitingSlots;

    /// Store all internal signal connections
    ::fwCom::helper::SigSlotConnection m_connections;
};

} // namespace ctrlCom
