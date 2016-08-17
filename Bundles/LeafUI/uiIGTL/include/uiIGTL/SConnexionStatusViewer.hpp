/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __UIIGTL_SCONNEXIONSTATUSVIEWER_HPP__
#define __UIIGTL_SCONNEXIONSTATUSVIEWER_HPP__

#include "uiIGTL/config.hpp"

#include <gui/editor/IEditor.hpp>
#include <fwCom/Slot.hpp>
#include <fwCom/Slot.hxx>
#include <fwCom/Slots.hpp>
#include <fwCom/Slots.hxx>
#include <fwCom/Signal.hpp>
#include <fwCom/Signal.hxx>

#include <QLabel>
#include <QPointer>

namespace uiIGTL
{
/**
 *
 *
 * @brief Class to manage status of server and client
 */
class UIIGTL_CLASS_API SConnexionStatusViewer : public ::gui::editor::IEditor
{

public:

    /**
     * @enum ConnexionType
     */
    enum ConnexionType
    {
        SERVER,
        CLIENT
    };

    UIIGTL_API static const ::fwCom::Slots::SlotKeyType s_CONNECT_SLOT;
    typedef ::fwCom::Slot<void () > ConnectSlotType;

    UIIGTL_API static const ::fwCom::Slots::SlotKeyType s_DISCONNECT_SLOT;
    typedef ::fwCom::Slot<void () > DisconnectSlotType;

    fwCoreServiceClassDefinitionsMacro ( (SConnexionStatusViewer) (::gui::editor::IEditor));

    /// Constructor
    UIIGTL_API SConnexionStatusViewer();

    /// Destructor
    UIIGTL_API virtual ~SConnexionStatusViewer() throw();

protected:
    /**
     * Sample of configuration :
     * @code{.xml}
        < service *uid="GENERIC_UID_writer" type="::gui::editor::IDialogEditor" impl="::uiIGTL::SClientConfigEditor" autoConnect="no">
            <type mode="Client" />
        </service>
        @endcode
     */
    UIIGTL_API virtual void configuring() throw (::fwTools::Failed);

    /// Overrides
    UIIGTL_API virtual void starting() throw (::fwTools::Failed);

    /// Overrides
    UIIGTL_API virtual void stopping() throw (::fwTools::Failed);

    /// Overrides
    UIIGTL_API virtual void updating() throw (::fwTools::Failed);

    /// Overrides
    UIIGTL_API virtual void swapping() throw (::fwTools::Failed);

private:

    /**
     * @brief slot called when listener or sender is started
     */
    UIIGTL_API void connect();

    /**
     * @brief slot called when listener or sender is stopped
     */
    UIIGTL_API void disconnect();

    /// Status label
    QPointer<QLabel> m_status;

    /// Connexion type server or client
    ConnexionType m_connexionType;

    /// Connect slot
    ConnectSlotType::sptr m_connectSlot;

    /// Disconnect slot
    DisconnectSlotType::sptr m_disconnectSlot;
};



} // namespace uiIGTL

#endif /*__UIIGTL_SCONNEXIONSTATUSVIEWER_HPP__*/

