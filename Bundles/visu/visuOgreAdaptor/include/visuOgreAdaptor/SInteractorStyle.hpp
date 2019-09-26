/************************************************************************
 *
 * Copyright (C) 2014-2019 IRCAD France
 * Copyright (C) 2014-2019 IHU Strasbourg
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
 * @brief Manage interactor style for Ogre.
 *
 * @section Signals Signals
 * - \b picked(::fwDataTools::PickingInfo) : Emit the picked informations.
 *
 * @section Slots Slots
 * - \b pick(::fwDataTools::PickingInfo) : Transmit the picked informations.

 * @section XML XML Configuration
 * @code{.xml}
        <service type=" ::visuOgreAdaptor::SInteractorStyle ">
            <config layer=" ... " movement=" ... " picker=" ..." queryMask="..."/>
       </service>
   @endcode
 * @subsection Configuration Configuration:
 * - \b layer (mandatory): Layer on which the interactions will be done.
 * - \b movement (optional, 'Trackball'/'Fixed'/'Negato2D'/'VR', default=''): Style of the movement
 * interactor.
 * - \b picker (optional, 'Mesh'/'Video', default=''): Style of the picker interactor.
 * - \b queryMask (optional, uint32, default=0x40000000): (uint32) Used for picking. Picked only displayed entities with
 * the matching flag.
 */
class VISUOGREADAPTOR_CLASS_API SInteractorStyle : public ::fwRenderOgre::IAdaptor
{

public:

    fwCoreServiceMacro(SInteractorStyle, ::fwRenderOgre::IAdaptor);

    /**
     * @name Slots API
     * @{
     */
    /// Slots used to forward the picker sinal.
    VISUOGREADAPTOR_API static const ::fwCom::Slots::SlotKeyType s_PICK_SLOT;
    /** @} */

    /**
     * @name Signals API
     * @{
     */
    /// Signal used to forward the interactor sinal.
    typedef ::fwCom::Signal< void ( ::fwDataTools::PickingInfo ) > PointClickedSigType;
    VISUOGREADAPTOR_API static const ::fwCom::Signals::SignalKeyType s_PICKED_SIG;
    /** @} */

    /// Creates signals and slots.
    VISUOGREADAPTOR_API SInteractorStyle() noexcept;

    /// Does nothing.
    VISUOGREADAPTOR_API virtual ~SInteractorStyle() noexcept;

protected:

    /// Select the interactor style.
    VISUOGREADAPTOR_API void configuring() override;

    /// Starting method.
    VISUOGREADAPTOR_API void starting() override;

    /// Update the interactor.
    VISUOGREADAPTOR_API void updating() override;

    /// Stopping method.
    VISUOGREADAPTOR_API void stopping() override;

private:

    /**
     * @name Slots methods
     * @{
     */
    /**
     * @brief Called by the interactor, forward the signal.
     * @param _info Picking informations.
     */
    void picked(::fwDataTools::PickingInfo _info);
    /**
     * @}
     */

    /// Set interactor style.
    void setInteractorStyle();

    /**
     * @name Signals attributes
     * @{
     */
    /// Pointer to the generic signal.
    PointClickedSigType::sptr m_sigPicked;
    /**
     * @}
     */

    /// Type of the picker style.
    std::string m_pickerStyle {""};

    /// Type of the movement style.
    std::string m_movementStyle {""};

    ///Connection service, needed for slot/signal association.
    ::fwCom::helper::SigSlotConnection m_connections;

    /// Mask for picking requests.
    std::uint32_t m_queryMask {::Ogre::SceneManager::ENTITY_TYPE_MASK};

    /// Pointer on move interactor.
    ::fwRenderOgre::interactor::IInteractor::sptr m_moveInteractor {nullptr};

    /// Pointer on select interactor.
    ::fwRenderOgre::interactor::IInteractor::sptr m_selectInteractor {nullptr};
};

} //namespace visuOgreAdaptor
