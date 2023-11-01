/************************************************************************
 *
 * Copyright (C) 2020-2023 IRCAD France
 * Copyright (C) 2020 IHU Strasbourg
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

#include "modules/viz/qt3d/config.hpp"

#include <viz/qt3d/adaptor.hpp>
#include <viz/qt3d/data/mesh.hpp>

namespace sight::module::viz::qt3d::adaptor
{

/**
 * @brief This adaptor shows individual meshes.
 *
 * This class handles the conversion of data::mesh to Qt3D. It can handle triangles.
 *
 * @section Slot Slot
 * - \b update_visibility(bool): shows or hides the mesh.
 * - \b toggle_visibility(): Toggle whether the adaptor is shown or not.
 *
 * @section XML XML Configuration
 * @code{.xml}
    <service uid="..." type="sight::module::viz::qt3d::adaptor::mesh" >
        <in key="mesh" uid="..." />
        <config autoresetcamera="true" visible="true" materialName="..." />
    </service>
   @endcode
 *
 * @subsection Input Input
 * - \b mesh [sight::data::mesh]: adapted mesh.
 *
 * @subsection Configuration Configuration:
 *  - \b autoresetcamera (optional, bool, default=false): reset the camera when this mesh is modified.
 *  - \b visible (optional, bool, default=true): set the initial visibility of the mesh.
 *  - \b materialName (optional): need to be used when a material adaptor is defined in XML file.
 *        Must be set according to ::visuQt3D::material materialName property
 *        to retrieve the material managed by the adaptor.
 */
class MODULE_VIZ_QT3D_CLASS_API mesh : public sight::viz::qt3d::adaptor
{
public:

    /// Generates default methods as New, dynamicCast, ...
    SIGHT_DECLARE_SERVICE(mesh, sight::viz::qt3d::adaptor);

    /// Sets default parameters and initializes necessary members.
    MODULE_VIZ_QT3D_API mesh() noexcept;

    /// Destroys the adaptor.
    MODULE_VIZ_QT3D_API ~mesh() noexcept override;

protected:

    /// Configures the adaptor.
    MODULE_VIZ_QT3D_API void configuring() override;

    /// Starts the adaptor.
    MODULE_VIZ_QT3D_API void starting() override;

    /**
     * @brief Proposals to connect service slots to associated object signals.
     * @return A map of each proposed connection.
     *
     * Connect data::image::MODIFIED_SIG of s_MESH_INOUT to service::slots::UPDATE.
     * Connect data::image::VERTEX_MODIFIED_SIG of s_MESH_INOUT to MODIFY_VERTICES_SLOT.
     */
    MODULE_VIZ_QT3D_API service::connections_t auto_connections() const override;

    /// Updates the mesh.
    MODULE_VIZ_QT3D_API void updating() override;

    /// Does nothing.
    MODULE_VIZ_QT3D_API void stopping() override;

    /**
     * @brief Sets whether the mesh is to be seen or not.
     * @param _visible the visibility status of the mesh.
     */
    MODULE_VIZ_QT3D_API void set_visible(bool _visible) override;

private:

    /// Updates mesh vertices.
    void modify_vertices();

    /// Contains a Qt3D mesh.
    QPointer<sight::viz::qt3d::data::mesh> m_mesh;

    /// Specifies the material adaptor used to configure mesh material.
    std::string m_material_name;

    /// Defines whether the camera must be auto reset when a mesh is updated or not.
    bool m_auto_reset_camera {false};

    static constexpr std::string_view MESH_INOUT = "mesh";
    data::ptr<data::mesh, data::access::inout> m_mesh_data {this, MESH_INOUT, true};
};

} // namespace sight::module::viz::qt3d::adaptor.
