/************************************************************************
 *
 * Copyright (C) 2020-2025 IRCAD France
 * Copyright (C) 2020-2021 IHU Strasbourg
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

#include <viz/scene3d/adaptor.hpp>
#include <viz/scene3d/material/standard.hpp>
#include <viz/scene3d/transformable.hpp>

namespace sight::module::viz::scene3d::adaptor
{

/**
 * @brief Displays an orientation "marker", marker is represented by a human body mesh,
 * its orientation follows camera movement.
 *
 * @section Slots Slots
 *
 * @section XML XML Configuration
 * @code{.xml}
    <service uid="..." type="sight::module::viz::scene3d::adaptor::orientation_marker" auto_connect="true">
        <in key="matrix" uid="..." />
        <config resource="..." depth="-32.0" />
    </service>
   @endcode
 *
 * @subsection Input Input
 *  - \b matrix [sight::data::matrix4]: matrix to follow (usually camera Matrix).
 * @subsection Configuration Configuration:
 * - \b resource (optional, string): name of the resource to use for the marker.
 * - \b depth (optional, float): value of depth (z) where marker will be positioned, greater value to zoom-in , lower
 * to zoom-out.
 */
class orientation_marker final :
    public sight::viz::scene3d::adaptor
{
public:

    /// Generates default methods as New, dynamicCast, ...
    SIGHT_DECLARE_SERVICE(orientation_marker, sight::viz::scene3d::adaptor);

    /// Constructor
    orientation_marker() noexcept = default;

    /// Destructor
    ~orientation_marker() noexcept final = default;

protected:

    /// Configures the service's parameters
    void configuring() final;

    /// Initializes and starts child services
    void starting() final;

    /// Updates internal matrix from the input transform
    void updating() final;

    /// Unregisters child services
    void stopping() final;

    /// Sets the visibility of the adaptor
    void set_visible(bool _visible) final;

    /// Connects input matrix MODIFIED to UPDATE slot.
    service::connections_t auto_connections() const final;

private:

    /// Contains the material used to display the patient.
    sight::viz::scene3d::material::standard::uptr m_material {nullptr};

    /// Updates the internal camera matrix from the input transform
    void update_camera_matrix();

    /// Contains the scene node where all of manual objects are attached.
    Ogre::SceneNode* m_scene_node {nullptr};

    /// Stores the entity associated to the marker mesh
    Ogre::Entity* m_patient_entity {nullptr};

    /// Resource used for the marker
    std::string m_patient_mesh_rc {"human.mesh"};

    /// Z coordinate of marker position, increase to zoom in, decrease to zoom out.
    float m_marker_depth = -32.F;

    static constexpr std::string_view MATRIX_IN = "matrix";
    sight::data::ptr<sight::data::matrix4, sight::data::access::in> m_matrix {this, MATRIX_IN};
};

} // namespace sight::module::viz::scene3d::adaptor.
