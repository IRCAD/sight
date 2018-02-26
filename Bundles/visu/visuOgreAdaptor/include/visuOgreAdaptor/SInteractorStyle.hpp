/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2014-2018.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#pragma once

#include "visuOgreAdaptor/config.hpp"

#include <fwCom/Signal.hpp>
#include <fwCom/Signals.hpp>
#include <fwCom/Slot.hpp>
#include <fwCom/Slots.hpp>

#include <fwData/Object.hpp>

#include <fwRenderOgre/IAdaptor.hpp>

namespace visuOgreAdaptor
{

/**
 * @brief   Manage interactor style for Ogre
 *
 * @section Signals Signals
 * - \b pointClickedSignal(int) : Emitted when a point is clicked.
 *
 * @section Slots Slots
 * - \b pointClickedSlot(int) : Transmit the clicked point.

 * @section XML XML Configuration
 * @code{.xml}
        <service type=" ::visuOgreAdaptor::SInteractorStyle ">
            <config render=" ... " movement=" ... " picker=" ..." />
       </service>
   @endcode
 * @subsection Configuration Configuration:
 * - \b render (mandatory) : Layer on which the interactions will be done.
 * - \b movement (optional) : Style of the movement interactor: 'Trackball', 'Fixed', 'Negato2D', or 'VR'.
 * - \b picker (optional) : Style of the picker interactor: 'Mesh' or 'Video'.
 */
class VISUOGREADAPTOR_CLASS_API SInteractorStyle : public ::fwRenderOgre::IAdaptor
{

public:

    fwCoreServiceClassDefinitionsMacro((SInteractorStyle)(::fwRenderOgre::IAdaptor))

    /**
     * @name Signals API
     * @{
     */
    /// Signal sent when a point is clicked
    typedef ::fwCom::Signal< void ( ::fwData::Object::sptr ) > PointClickedSignalType;
    VISUOGREADAPTOR_API static const ::fwCom::Signals::SignalKeyType s_ADD_POINT_SIG;
    VISUOGREADAPTOR_API static const ::fwCom::Signals::SignalKeyType s_REMOVE_POINT_SIG;
    /** @} */

    /**
     * @name Slots API
     * @{
     */

    /// Slots used when a point is clicked
    VISUOGREADAPTOR_API static const ::fwCom::Slots::SlotKeyType s_ADD_POINT_SLOT;
    VISUOGREADAPTOR_API static const ::fwCom::Slots::SlotKeyType s_REMOVE_POINT_SLOT;
    /** @} */

    /// Constructor. Creates signals and slots
    VISUOGREADAPTOR_API SInteractorStyle() noexcept;
    /// Destructor. Does nothing
    VISUOGREADAPTOR_API virtual ~SInteractorStyle() noexcept;

protected:

    /// Select the interactor style
    VISUOGREADAPTOR_API void configuring() override;
    /// Starting method
    VISUOGREADAPTOR_API void starting() override;
    /// Update the interactor
    VISUOGREADAPTOR_API void updating() override;
    /// Stopping method
    VISUOGREADAPTOR_API void stopping() override;

private:

    /// Slot: sends a signal when the interactor has recieved a clicked point signal
    void addPoint(fwData::Object::sptr obj);
    /// Slot: sends a signal when the interactor has recieved a clicked point signal
    void removePoint(fwData::Object::sptr obj);
    /// Set interactor style
    void setInteractorStyle();
    /// Type of the picker style
    std::string m_pickerStyle;
    /// Type of the movement style
    std::string m_movementStyle;
    /// Pointer to the generic signal
    PointClickedSignalType::sptr m_sigAddPoint;
    /// Pointer to the generic signal
    PointClickedSignalType::sptr m_sigRemovePoint;
    ///Connection service, needed for slot/signal association
    ::fwCom::helper::SigSlotConnection m_connections;
    /// Mask for picking requests
    std::uint32_t m_queryFlags {0};
};

} //namespace visuOgreAdaptor
