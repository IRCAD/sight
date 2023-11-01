/************************************************************************
 *
 * Copyright (C) 2009-2023 IRCAD France
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

#include <core/com/helper/sig_slot_connection.hpp>
#include <core/macros.hpp>

#include <data/matrix4.hpp>
#include <data/mesh.hpp>

#include <ui/__/service.hpp>

namespace sight::module::viz::sample
{

/**
 * @brief Render a 3D mesh.
 *
 * @section Slots Slots
 * - \b update_cam_position(): update the camera position.
 * - \b updateCamTransform(): send the signal camUpdated().
 *
 * @section Signals Signals
 * - \b cam_updated(): emitted when the camera move.
 *
 * @section XML XML Configuration
 * @code{.xml}
   <service uid="..." type="sight::module::viz::sample::mesh" >
      <in key="mesh" uid="..." />
   </service>
 *
 * @subsection Input Input
 * - \b mesh [sight::data::mesh]: mesh to display.
 */
class MODULE_VIZ_SAMPLE_CLASS_API mesh : public sight::ui::service
{
public:

    /// Generates default methods as New, dynamicCast, ...
    SIGHT_DECLARE_SERVICE(mesh, sight::ui::service);

    MODULE_VIZ_SAMPLE_API static const core::com::slots::key_t UPDATE_CAM_POSITION_SLOT;

    MODULE_VIZ_SAMPLE_API static const core::com::signals::key_t CAM_UPDATED_SIG;

    using cam_updated_signal_t = core::com::signal<void (data::matrix4::sptr)>;

    /// Creates slots and the signal.
    MODULE_VIZ_SAMPLE_API mesh() noexcept;

    /// Destroys the service.
    MODULE_VIZ_SAMPLE_API ~mesh() noexcept override;

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
     * Connect data::mesh::MODIFIED_SIG to service::slots::UPDATE
     */
    MODULE_VIZ_SAMPLE_API connections_t auto_connections() const override;

    /// Does nothing.
    MODULE_VIZ_SAMPLE_API void updating() override;

    /// Stops and unregisters sub-services.
    MODULE_VIZ_SAMPLE_API void stopping() override;

private:

    /// SLOT: receives new camera transform and update the camera.
    void update_cam_position(data::matrix4::sptr _transform);

    /// SLOT: receives new camera transform from the camera service and trigger the signal.
    void update_cam_transform();

    /// Contains the render service.
    sight::service::base::sptr m_render_srv;

    /// Contains the trackball interactor adaptor.
    service::base::sptr m_interactor_srv;

    /// Contains the mesh adaptor.
    service::base::sptr m_mesh_srv;

    /// Contains the camera adaptor.
    service::base::sptr m_camera_srv;

    /// Contains the transformation adaptor.
    data::matrix4::sptr m_camera_transform;

    /// Contains the signal emitted when camera position is updated.
    cam_updated_signal_t::sptr m_sig_cam_updated;

    /// Stores connection with the camera transform.
    core::com::helper::sig_slot_connection m_connections;

    data::ptr<data::mesh, data::access::in> m_mesh {this, "mesh", false};
};

} // namespace sight::module::viz::sample.
