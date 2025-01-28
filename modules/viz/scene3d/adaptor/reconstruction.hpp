/************************************************************************
 *
 * Copyright (C) 2014-2025 IRCAD France
 * Copyright (C) 2014-2020 IHU Strasbourg
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

#include "modules/viz/scene3d/adaptor/mesh.hpp"

#include <core/com/slot.hpp>

#include <data/reconstruction.hpp>

#include <viz/scene3d/adaptor.hpp>
#include <viz/scene3d/material/standard.hpp>
#include <viz/scene3d/transformable.hpp>

namespace sight::data
{

class mesh;

} // namespace sight::data

namespace sight::module::viz::scene3d::adaptor
{

/**
 * @brief This adaptor displays a reconstruction.
 *
 * @section Slots Slots
 * - \b update(): called when the associated mesh changes.
 * - \b update_visibility(bool): sets whether the reconstruction is to be seen or not.
 * - \b show(): shows the reconstruction.
 * - \b hide(): hides the reconstruction.
 *
 * @section XML XML Configuration
 * @code{.xml}
    <service type="sight::module::viz::scene3d::adaptor::reconstruction">
        <in key="reconstruction" uid="..." />
        <config transform="..." autoresetcamera="true" queryFlags="0x40000000" />
   </service>
   @endcode
 *
 * @subsection Input Input:
 * - \b reconstruction [sight::data::reconstruction]: reconstruction to display.
 *
 * @subsection Configuration Configuration:
 * - \b transform (optional, string, default=""): the transformation matrix to associate to the adaptor.
 * - \b autoresetcamera (optional, true/false, default=true): reset the camera when this mesh is modified, "true" or
 *"false".
 * - \b queryFlags (optional, unit32, default=0x40000000): Used for picking. Picked only by pickers whose mask that
 *      match the flag.
 */
class reconstruction final : public sight::viz::scene3d::adaptor,
                             public sight::viz::scene3d::transformable
{
public:

    /// Generates default methods as New, dynamicCast, ...
    SIGHT_DECLARE_SERVICE(reconstruction, sight::viz::scene3d::adaptor);

    /// Destroys the adaptor.
    ~reconstruction() noexcept final = default;

    /**
     * @brief Enables/disables automatic reset on camera.
     * @param _auto_reset_camera use true to activate it.
     */
    void set_auto_reset_camera(bool _auto_reset_camera);

    /**
     * @brief Sets the material template Name.
     * @param _material_name material name.
     */
    void set_material_template_name(const std::string& _material_name);

    /**
     * @brief Sets the query flag.
     * @param _query_flags value of the query flag.
     */
    void set_query_flags(std::uint32_t _query_flags);

    /**
     * @brief Gets the mesh adaptor.
     * @return The mesh adaptor.
     */
    module::viz::scene3d::adaptor::mesh::sptr get_mesh_adaptor();

protected:

    /// Configures the adaptor.
    void configuring() final;

    /// Creates the mesh service.
    void starting() final;

    /**
     * @brief Proposals to connect service slots to associated object signals.
     * @return A map of each proposed connection.
     *
     * Connect data::reconstruction::MESH_CHANGED_SIG of s_RECONSTRUCTION_INPUT to adaptor::slots::LAZY_UPDATE
     */
    service::connections_t auto_connections() const final;

    /// Stops and unregisters created services.
    void stopping() final;

    /// Updates the mesh adaptor according to the reconstruction or creates it if it hasn't been yet.
    void updating() final;

    /**
     * @brief Sets the reconstruction visibility.
     * @param _visible the visibility status of the reconstruction.
     */
    void set_visible(bool _visible) final;

private:

    /// Creates the mesh service.
    void create_mesh_service();

    /// Contains the associated mesh service.
    sight::viz::scene3d::adaptor::wptr m_mesh_adaptor;

    /// Defines if the camera has to be reset automatically.
    bool m_auto_reset_camera {true};

    /// Defines the material name.
    std::string m_material_template_name {sight::viz::scene3d::material::standard::TEMPLATE};

    /// Defines if the mesh changes dynamically.
    bool m_is_dynamic {false};

    /// Defines if the vertices change dynamically.
    bool m_is_dynamic_vertices {false};

    /// Defines the mask used for picking request.
    std::uint32_t m_query_flags {Ogre::SceneManager::ENTITY_TYPE_MASK};

    static constexpr std::string_view RECONSTRUCTION_INPUT = "reconstruction";
    data::ptr<data::reconstruction, data::access::in> m_reconstruction {this, RECONSTRUCTION_INPUT};
};

//------------------------------------------------------------------------------

inline void reconstruction::set_auto_reset_camera(bool _auto_reset_camera)
{
    m_auto_reset_camera = _auto_reset_camera;
}

//------------------------------------------------------------------------------

inline void reconstruction::set_material_template_name(const std::string& _material_name)
{
    m_material_template_name = _material_name;
}

//------------------------------------------------------------------------------

inline void reconstruction::set_query_flags(uint32_t _query_flags)
{
    m_query_flags = _query_flags;
}

//------------------------------------------------------------------------------

} // namespace sight::module::viz::scene3d::adaptor.
