/************************************************************************
 *
 * Copyright (C) 2009-2020 IRCAD France
 * Copyright (C) 2012-2020 IHU Strasbourg
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

#include "visuOgreBasic/config.hpp"

#include <fwGui/IGuiContainerSrv.hpp>

#include <boost/shared_array.hpp>

namespace visuOgreBasic
{

/**
 * @brief   Render a 3D mesh.
 *
 * @section XML XML Configuration
 *
 * @code{.xml}
   <service uid="..." type="::visuOgreBasic::SMesh" >
      <in key="mesh" uid="..." />
   </service>
 * @subsection Input Input
 * - \b mesh [::fwData::Mesh]: mesh to display.
 */
class VISUOGREBASIC_CLASS_API SMesh : public ::fwGui::IGuiContainerSrv
{

public:

    fwCoreServiceMacro(SMesh, ::fwGui::IGuiContainerSrv)

    VISUOGREBASIC_API static const ::fwCom::Slots::SlotKeyType s_UPDATE_CAM_POSITION_SLOT;

    VISUOGREBASIC_API static const ::fwCom::Signals::SignalKeyType s_CAM_UPDATED_SIG;

    typedef ::boost::shared_array< double > SharedArray;
    typedef ::fwCom::Signal< void (SharedArray, SharedArray, SharedArray) > CamUpdatedSignalType;

    /// Constructor. Does nothing.
    VISUOGREBASIC_API SMesh() noexcept;

    /// Destructor. Does nothing.
    VISUOGREBASIC_API virtual ~SMesh() noexcept;

    /**
     * @brief Returns proposals to connect service slots to associated object signals,
     * this method is used for obj/srv auto connection
     *
     * Connect Mesh::s_MODIFIED_SIG to this::s_UPDATE_SLOT
     */
    VISUOGREBASIC_API virtual KeyConnectionsMap getAutoConnections() const override;

private:

    ///  This method does nothing.
    VISUOGREBASIC_API void configuring() override;

    ///  This method starts sub-services.
    VISUOGREBASIC_API virtual void starting() override;

    ///  This method launches sub-services.
    VISUOGREBASIC_API virtual void updating() override;

    ///  This method stops sub-services.
    VISUOGREBASIC_API virtual void stopping() override;

    /// Render service
    ::fwServices::IService::sptr m_renderSrv;

    /// Basic trackball interactor adaptor
    ::fwServices::IService::sptr m_interactorSrv;

    /// Negatoscope adaptor
    ::fwServices::IService::sptr m_meshSrv;

    /// Whether of or not to autoConnect to the image, needed for the purpose of tutorials
    bool m_meshAutoConnect { false };

    /// Signal emitted when camera position is updated.
    CamUpdatedSignalType::sptr m_sigCamUpdated;
};

} // namespace visuOgreBasic
