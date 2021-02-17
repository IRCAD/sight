/************************************************************************
 *
 * Copyright (C) 2009-2021 IRCAD France
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

#include "modules/viz/sample/config.hpp"

#include <data/Matrix4.hpp>

#include <ui/base/IGuiContainer.hpp>

namespace sight::modules::viz::sample
{

/**
 * @brief Render a 3D mesh.
 *
 * @section Slots Slots
 * - \b updateCamPosition(): update the camera position.
 * - \b updateCamTransform(): send the signal camUpdated().
 *
 * @section Signals Signals
 * - \b camUpdated(): emitted when the camera move.
 *
 * @section XML XML Configuration
 * @code{.xml}
   <service uid="..." type="::sight::modules::viz::sample::SMesh" >
      <in key="mesh" uid="..." />
   </service>
 *
 * @subsection Input Input
 * - \b mesh [sight::data::Mesh]: mesh to display.
 */
class MODULE_VIZ_SAMPLE_CLASS_API SMesh : public sight::ui::base::IGuiContainer
{

public:

    /// Generates default methods as New, dynamicCast, ...
    fwCoreServiceMacro(SMesh, sight::ui::base::IGuiContainer)

    MODULE_VIZ_SAMPLE_API static const core::com::Slots::SlotKeyType s_UPDATE_CAM_POSITION_SLOT;

    MODULE_VIZ_SAMPLE_API static const core::com::Signals::SignalKeyType s_CAM_UPDATED_SIG;

    typedef core::com::Signal< void (data::Matrix4::sptr) > CamUpdatedSignalType;

    /// Creates slots and the signal.
    MODULE_VIZ_SAMPLE_API SMesh() noexcept;

    /// Destroys the service.
    MODULE_VIZ_SAMPLE_API virtual ~SMesh() noexcept;

private:

    /// Checks if the input mesh is autoconnect.
    MODULE_VIZ_SAMPLE_API void configuring() override;

    /// Registers and starts sub-services.
    MODULE_VIZ_SAMPLE_API void starting() override;

    /**
     * @brief Proposals to connect service slots to associated object signals.
     * @return A map of each proposed connection.
     * @note This is actually useless since the sub-service already listens to the data,
     * but this prevents a warning in fwServices from being raised.
     *
     * Connect data::Mesh::s_MODIFIED_SIG to s_UPDATE_SLOT
     */
    MODULE_VIZ_SAMPLE_API KeyConnectionsMap getAutoConnections() const override;

    /// Does nothing.
    MODULE_VIZ_SAMPLE_API void updating() override;

    /// Stops and unregisters sub-services.
    MODULE_VIZ_SAMPLE_API void stopping() override;

private:

    /// SLOT: receives new camera transform and update the camera.
    void updateCamPosition(data::Matrix4::sptr _transform);

    /// SLOT: receives new camera transform from the camera service and trigger the signal.
    void updateCamTransform();

    /// Contains the render service.
    service::IService::sptr m_renderSrv;

    /// Contains the trackball interactor adaptor.
    service::IService::sptr m_interactorSrv;

    /// Contains the mesh adaptor.
    service::IService::sptr m_meshSrv;

    /// Contains the camera adaptor.
    service::IService::sptr m_cameraSrv;

    /// Contains the transformation adaptor.
    data::Matrix4::sptr m_cameraTransform;

    /// Defines whether or not to autoConnect to the mesh, needed for the purpose of tutorials.
    bool m_meshAutoConnect { false };

    /// Contains the signal emitted when camera position is updated.
    CamUpdatedSignalType::sptr m_sigCamUpdated;

    /// Stores connection with the camera transform.
    core::com::helper::SigSlotConnection m_connections;

};

} // namespace sight::modules::viz::sample.
