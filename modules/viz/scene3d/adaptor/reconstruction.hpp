/************************************************************************
 *
 * Copyright (C) 2014-2023 IRCAD France
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
#include "modules/viz/scene3d/config.hpp"

#include <core/com/slot.hpp>

#include <data/reconstruction.hpp>

#include <viz/scene3d/adaptor.hpp>
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
 * - \b changeMesh(data::mesh::sptr): called when the associated mesh changes.
 * - \b updateVisibility(bool): sets whether the reconstruction is to be seen or not.
 * - \b toggleVisibility(): toggle whether the reconstruction is shown or not.
 * - \b show(): shows the reconstruction.
 * - \b hide(): hides the reconstruction.
 * - \b modifyVisibility(): called to show or hide the reconstruction.
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
class MODULE_VIZ_SCENE3D_CLASS_API reconstruction final :
    public sight::viz::scene3d::adaptor,
    public sight::viz::scene3d::transformable
{
public:

    /// Generates default methods as New, dynamicCast, ...
    SIGHT_DECLARE_SERVICE(reconstruction, sight::viz::scene3d::adaptor);

    /// Initialise slots.
    MODULE_VIZ_SCENE3D_API reconstruction() noexcept;

    /// Destroys the adaptor.
    MODULE_VIZ_SCENE3D_API ~reconstruction() noexcept final = default;

    /**
     * @brief Enables/disables automatic reset on camera.
     * @param _autoResetCamera use true to activate it.
     */
    MODULE_VIZ_SCENE3D_API void setAutoResetCamera(bool _auto_reset_camera);

    /**
     * @brief Sets the material template Name.
     * @param _materialName material name.
     */
    MODULE_VIZ_SCENE3D_API void setMaterialTemplateName(const std::string& _material_name);

    /**
     * @brief Sets the query flag.
     * @param _queryFlags value of the query flag.
     */
    MODULE_VIZ_SCENE3D_API void setQueryFlags(std::uint32_t _query_flags);

    /**
     * @brief Gets the mesh adaptor.
     * @return The mesh adaptor.
     */
    MODULE_VIZ_SCENE3D_API module::viz::scene3d::adaptor::mesh::sptr getMeshAdaptor();

protected:

    /// Configures the adaptor.
    MODULE_VIZ_SCENE3D_API void configuring() final;

    /// Creates the mesh service.
    MODULE_VIZ_SCENE3D_API void starting() final;

    /**
     * @brief Proposals to connect service slots to associated object signals.
     * @return A map of each proposed connection.
     *
     * Connect data::reconstruction::MESH_CHANGED_SIG of s_RECONSTRUCTION_INPUT to CHANGE_MESH_SLOT
     * Connect data::reconstruction::VISIBILITY_MODIFIED_SIG of s_RECONSTRUCTION_INPUT to VISIBILITY_SLOT
     */
    MODULE_VIZ_SCENE3D_API service::connections_t auto_connections() const final;

    /// Stops and unregisters created services.
    MODULE_VIZ_SCENE3D_API void stopping() final;

    /// Updates the mesh adaptor according to the reconstruction or creates it if it hasn't been yet.
    MODULE_VIZ_SCENE3D_API void updating() final;

    /**
     * @brief Sets the reconstruction visibility.
     * @param _visible the visibility status of the reconstruction.
     */
    MODULE_VIZ_SCENE3D_API void setVisible(bool _visible) final;

private:

    /// Changes the attached mesh.
    void changeMesh(data::mesh::sptr /*unused*/);

    /// Modifies the visibility.
    void modifyVisibility();

    /// Creates the mesh service.
    void createMeshService();

    /// Contains the associated mesh service.
    sight::viz::scene3d::adaptor::wptr m_meshAdaptor;

    /// Defines if the camera has to be reset automatically.
    bool m_autoResetCamera {true};

    /// Defines the material name.
    std::string m_materialTemplateName {sight::viz::scene3d::material::DEFAULT_MATERIAL_TEMPLATE_NAME};

    /// Defines if the mesh changes dynamically.
    bool m_isDynamic {false};

    /// Defines if the vertices change dynamically.
    bool m_isDynamicVertices {false};

    /// Defines the mask used for picking request.
    std::uint32_t m_queryFlags {Ogre::SceneManager::ENTITY_TYPE_MASK};

    static constexpr std::string_view s_RECONSTRUCTION_INPUT = "reconstruction";
    data::ptr<data::reconstruction, data::Access::in> m_reconstruction {this, s_RECONSTRUCTION_INPUT, true};
};

//------------------------------------------------------------------------------

inline void reconstruction::setAutoResetCamera(bool _auto_reset_camera)
{
    m_autoResetCamera = _auto_reset_camera;
}

//------------------------------------------------------------------------------

inline void reconstruction::setMaterialTemplateName(const std::string& _material_name)
{
    m_materialTemplateName = _material_name;
}

//------------------------------------------------------------------------------

inline void reconstruction::setQueryFlags(uint32_t _query_flags)
{
    m_queryFlags = _query_flags;
}

//------------------------------------------------------------------------------

} // namespace sight::module::viz::scene3d::adaptor.
