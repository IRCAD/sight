/************************************************************************
 *
 * Copyright (C) 2021-2023 IRCAD France
 * Copyright (C) 2021 IHU Strasbourg
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

#include "modules/viz/scene3d/config.hpp"

#include <data/matrix4.hpp>
#include <data/mesh.hpp>

#include <viz/scene3d/adaptor.hpp>

#include <queue>

namespace sight::module::viz::scene3d::adaptor
{

/**
 * @brief This adaptor shows a list of mesh.
 *
 * @section Slots Slots
 * - \b add(): add a new mesh at the transformation matrix position.
 * - \b clear(): clear all meshes.
 * - \b update_visibility(bool): sets whether the mesh is to be seen or not.
 * - \b toggle_visibility(): toggle whether the mesh is shown or not.
 * - \b show(): shows the mesh.
 * - \b hide(): hides the mesh.
 *
 * @section XML XML Configuration
 * @code{.xml}
    <service uid="..." type="sight::module::viz::scene3d::mesh_list">
        <inout key="mesh" uid="..." />
        <in key="transform" uid="..." />
        <in key="texture" uid="..." />
        <config capacity="50" drop="1" textureAlpha="false" />
    </service>
   @endcode
 *
 * @subsection Input Input
 * - \b texture [sight::data::image]: image displayed as a texture on the mesh
 * - \b transform [sight::data::matrix4]: Sight transform matrix.
 *
 * @subsection In-Out In-Out
 * - \b mesh [sight::data::mesh]: adapted mesh. It can not be a read-only data because we may generate normals or add
 * some
 *      fields.
 *
 * @subsection Configuration Configuration:
 * - \b capacity (optional, int, default=50): maximum capacity of the list.
 * - \b drop (optional, int, default=1): defines the ratio of matrices to display: (1/drop).
 * - \b textureAlpha (optional, bool, default=false): generates alpha value for the texture if the image contains only
 * 1 or 3 channels. It may be slower.
 */
class MODULE_VIZ_SCENE3D_CLASS_API mesh_list final : public sight::viz::scene3d::adaptor
{
public:

    /// Generates default methods as New, dynamicCast, ...
    SIGHT_DECLARE_SERVICE(mesh_list, sight::viz::scene3d::adaptor);

    /// Sets default parameters and initializes necessary members.
    MODULE_VIZ_SCENE3D_API mesh_list() noexcept;

    /// Destroys the service.
    MODULE_VIZ_SCENE3D_API ~mesh_list() noexcept override;

protected:

    /// Configures the adaptor.
    MODULE_VIZ_SCENE3D_API void configuring() override;

    /// Initializes the adaptor.
    MODULE_VIZ_SCENE3D_API void starting() override;

    /**
     * @brief Proposal to connect service slots to associated object signals.
     * @return A map of each proposed connection.
     *
     * Connect data::matrix4::MODIFIED_SIG of s_TRANSFORM_INOUT to ADD_MESH_SLOT
     */
    MODULE_VIZ_SCENE3D_API service::connections_t auto_connections() const override;

    /// Does nothing.
    MODULE_VIZ_SCENE3D_API void updating() override;

    /// Unregisters all services.
    /// @see clear()
    MODULE_VIZ_SCENE3D_API void stopping() override;

    /**
     * @brief Sets the list visibility.
     * @param _visible the visibility status of the list.
     */
    MODULE_VIZ_SCENE3D_API void set_visible(bool _visible) override;

private:

    /// Stores all services and data needed to add a new mesh.
    struct mesh_instance
    {
        data::matrix4::sptr m_matrix;
        data::image::sptr m_image;
        sight::viz::scene3d::adaptor::sptr m_transform;
        sight::viz::scene3d::adaptor::sptr m_mesh;
        sight::viz::scene3d::adaptor::sptr m_texture;
        bool m_is_enabled {false}; ///< set to true when the instance has been added once
    };

    /// Adds a new mesh at the input matrix position.
    void add();

    /// Clear all meshes.
    void clear();

    /// Stores all registered meshes instances.
    std::vector<mesh_instance> m_meshes;

    /// Defines the current index mesh to update in the mesh vector.
    unsigned int m_mesh_count {0};

    /// Defines the maximum capacity of the meshes list.
    unsigned int m_capacity {50};

    /// Defines the number of matrices to drop before adding a new adaptor.
    unsigned int m_drop_data {1};

    /// Counts the number of received signal to add a data, then drop the signal if (m_dropCount%m_dropData != 0).
    unsigned int m_drop_count {0};

    /// Generates alpha value for the texture if the image contains only 1 or 3 channels. It may be slower.
    bool m_generate_alpha {false};

    static constexpr std::string_view TRANSFORM_INPUT = "transform";

    sight::data::ptr<sight::data::image, sight::data::access::in> m_texture {this, "texture"};
    sight::data::ptr<sight::data::matrix4, sight::data::access::in> m_transform {this, TRANSFORM_INPUT};
    sight::data::ptr<sight::data::mesh, sight::data::access::inout> m_mesh {this, "mesh"};
};

} // namespace sight::module::viz::scene3d::adaptor
