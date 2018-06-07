/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2018.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#pragma once

#include "ctrlCom/config.hpp"

#include <fwCom/helper/SigSlotConnection.hpp>

#include <fwServices/IController.hpp>

namespace ctrlCom
{
/**
 * @brief This service extract camera informations in configuration using android device name.
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
 *  - \b signal: list of signals to trigger
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
    CTRLCOM_API virtual ~SSignalGate();

    /// Slot: called when one of the signal is called
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

    /// Kepp track of received signals
    std::vector<bool> m_flags;

    std::vector< ReceivedSignalType::sptr > m_waitingSlots;

    /// Store all internal signals connection
    ::fwCom::helper::SigSlotConnection m_connections;
};

} // namespace ctrlCom
