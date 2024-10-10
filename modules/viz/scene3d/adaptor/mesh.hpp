/************************************************************************
 *
 * Copyright (C) 2014-2024 IRCAD France
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

#include "modules/viz/scene3d/adaptor/material.hpp"
#include "modules/viz/scene3d/adaptor/transform.hpp"

#include <data/material.hpp>
#include <data/mesh.hpp>

#include <viz/scene3d/adaptor.hpp>
#include <viz/scene3d/mesh.hpp>
#include <viz/scene3d/transformable.hpp>

#include <OGRE/OgreEntity.h>

namespace sight::data
{

class Material;

} // namespace sight::data
namespace sight::data
{

class mesh;

} // namespace sight::data

namespace sight::module::viz::scene3d::adaptor
{

/**
 * @brief This adaptor shows individual meshes.
 *
 * This class handles the conversion of data::mesh to Ogre3d. It can handle triangles, edges, quads or tetrahedrons.
 * For the quads and tetrahedrons, we generate the triangles in a pre-process, using the render to vertex buffer (r2vb)
 * feature to avoid the cost of geometry shaders when using multi-pass rendering techniques.
 *
 * An Ogre entity is created from the mesh. A second mesh and a second entity are created as an input for the r2vb.
 * Thus, the main mesh will contains only triangles or edges, while the second entity contains only quads or tetras.
 * An Ogre material is also created, and then managed by a material adaptor (specified in the configuration otherwise
 * a new one is generated).
 *
 * To handle the per-primitive color of data::mesh we also rely on geometry shaders, and thus on r2vb. We build a
 * texture containing the color for each primitive. This texture is fetched inside the geometry shader using the
 * primitive id.
 *
 * @section Slots Slots
 * - \b update_visibility(bool): sets whether the mesh is to be seen or not.
 * - \b toggle_visibility(): toggle whether the mesh is shown or not.
 * - \b show(): shows the mesh.
 * - \b hide(): hides the mesh.
 * - \b update(): called when the mesh is modified.
 * - \b modifyColors(): called when the point colors are modified.
 * - \b modifyTexCoords(): called when the texture coordinates are modified.
 * - \b modifyVertices(): called when the vertices are modified.
 *
 * @section XML XML Configuration
 * @code{.xml}
    <service uid="..." type="sight::module::viz::scene3d::adaptor::mesh" >
        <in key="mesh" uid="..." />
        <config transform="..." visible="true" materialName="..." shadingMode="phong" textureName="..."
        queryFlags="0x40000000" />
    </service>
   @endcode
 *
 * @subsection Input Input
 * - \b mesh [sight::data::mesh]: adapted mesh.
 *
 * @subsection Configuration Configuration:
 *  - \b autoresetcamera (optional, true/false, default=true): reset the camera when this mesh is modified, "true" or
 *"false".
 *  - \b transform (optional, string, default=""): the name of the Ogre transform node where to attach the mesh, as it
 *       was specified
 *       in the transform adaptor.
 *  - \b visible (optional, bool, default=true): set the initial visibility of the mesh.
 *       Either of the following (whether a material is configured in the XML scene or not) :
 *  - \b materialName (optional, string, default=""): name of the Ogre material, as defined in the
 *       module::viz::scene3d::adaptor::material you want to be bound to.
 *       Only if there is no material configured in the XML scene (in this case, it has to retrieve the material
 *       template, the texture adaptor and the shading mode) :
 *  - \b materialTemplate (optional, string, default=""): the name of the base Ogre material for the internally created
 *       material.
 *  - \b textureName (optional, default=""): the name of the Ogre texture that the mesh will use.
 *  - \b shadingMode (optional, none/flat/phong/ambient, default=phong): name of the used shading mode.
 *  - \b queryFlags (optional, uint32, default=0x40000000): Used for picking. Picked only by pickers whose mask that
 *       match the flag.
 */
class mesh final :
    public sight::viz::scene3d::adaptor,
    public sight::viz::scene3d::transformable
{
public:

    /// Generates default methods as New, dynamicCast, ...
    SIGHT_DECLARE_SERVICE(mesh, sight::viz::scene3d::adaptor);

    /// Sets default parameters and initializes necessary members.
    mesh() noexcept;

    /// Destroys Ogre resources.
    ~mesh() noexcept override;

    /**
     * @brief Gets the associated material.
     * @return The material.
     */
    data::material::sptr get_material() const;

    /**
     * @brief Sets the current material.
     * @param _material new material.
     */
    void set_material(data::material::sptr _material);

    /**
     * @brief Sets the material template Name.
     * @param _material_name material name.
     */
    void set_material_template_name(const std::string& _material_name);

    /**
     * @brief Enables/disables automatic reset on camera.
     * @param _auto_reset_camera use true to activate it.
     */
    void set_auto_reset_camera(bool _auto_reset_camera);

    /**
     * @brief Gets the associated entity.
     * @return The entity.
     */
    Ogre::Entity* get_entity() const;

    /**
     * @brief Gets the mesh visibility.
     * @return True if the mesh is visible.
     */
    bool get_visibility() const;

    /**
     * @brief Sets meshes vertex buffer to dynamic state (only has effect if called before service starting/update).
     * @param _is_dynamic use true to use dynamic vertex buffer.
     */
    void set_dynamic_vertices(bool _is_dynamic);

    /**
     * @brief Sets meshes and indices buffers to dynamic state (only has effect if called before service
     * starting/update).
     * @param _is_dynamic use true to use dynamic vertex and indices buffer.
     */
    void set_dynamic(bool _is_dynamic);

    /**
     * @brief Sets the query flag.
     * @param _query_flags value of the query flag.
     */
    void set_query_flags(std::uint32_t _query_flags);

    /**
     * @brief Sets if the reconstruction is managed or not.
     * @param _is_reconstruction_managed use true if the reconstruction is managed.
     */
    void set_is_reconstruction_managed(bool _is_reconstruction_managed);

    /// Flags the r2vb objects as dirty and asks the render service to update.
    void request_render() override;

    /**
     * @brief Sets the mesh visibility.
     * @param _visible the visibility status of the mesh.
     */
    void set_visible(bool _visible) override;

protected:

    /// Configures the adaptor.
    void configuring() override;

    /// Creates a Mesh in the Default Ogre resource group.
    void starting() override;

    /**
     * @brief Proposals to connect service slots to associated object signals.
     * @return A map of each proposed connection.
     *
     * Connect data::mesh::VERTEX_MODIFIED_SIG to MODIFY_VERTICES_SLOT
     * Connect data::mesh::POINT_COLORS_MODIFIED_SIG to MODIFY_COLORS_SLOT
     * Connect data::mesh::CELL_COLORS_MODIFIED_SIG to MODIFY_COLORS_SLOT
     * Connect data::mesh::POINT_TEX_COORDS_MODIFIED_SIG to MODIFY_POINT_TEX_COORDS_SLOT
     * Connect data::mesh::MODIFIED_SIG to service::slots::UPDATE
     */
    service::connections_t auto_connections() const override;

    /// Deletes the mesh after unregistering the service, and shutting connections.
    void stopping() override;

    /// Updates the mesh.
    void updating() override;

private:

    /// Updates mesh vertices.
    void modify_vertices();

    /// Updates mesh colors.
    void modify_point_colors();

    /// Updates mesh texture coordinates.
    void modify_tex_coords();

    /**
     * @brief Updates the mesh, checks if color, number of vertices have changed, and updates them.
     * @param _mesh used for the update.
     */
    void update_mesh(data::mesh::csptr _mesh);

    /**
     * @brief Instantiates a new material adaptor
     * @param _mesh used to create an unique material name.
     * @param _material_suffix used for the material name.
     */
    module::viz::scene3d::adaptor::material::sptr create_material_service(
        data::mesh::csptr _mesh,
        const std::string& _material_suffix = ""
    );

    /**
     * @brief Associates a new material to the managed mesh.
     * With this method, mesh is responsible for creating a material.
     * @param _mesh used to create the material service.
     */
    void update_new_material_adaptor(data::mesh::csptr _mesh);

    /// Updates the associated material adaptor.
    /// This method is called when a material adaptor has been configured in the XML scene.
    void update_xml_material_adaptor();

    /**
     * @brief Attaches a node in the scene graph.
     * @param _node the node to attach.
     */
    void attach_node(Ogre::MovableObject* _node);

    /// Defines whether the camera must be auto reset when a mesh is updated or not.
    bool m_auto_reset_camera {true};

    /// Contains the node in the scene graph where the mesh is attached.
    Ogre::Entity* m_entity {nullptr};

    /// Contains the Ogre material adaptor.
    module::viz::scene3d::adaptor::material::sptr m_material_adaptor {nullptr};

    /// Defines the attached material name (when configured by XML).
    std::string m_material_name;

    /// Contains the material data.
    data::material::sptr m_material {nullptr};

    /// Defines the attached material's name.
    std::string m_material_template_name {sight::viz::scene3d::material::DEFAULT_MATERIAL_TEMPLATE_NAME};

    /// Defines the attached texture adaptor name.
    std::string m_texture_name;

    /// Defines if the mesh adaptor is managed by a reconstruction adaptor.
    bool m_is_reconstruction_managed {false};

    /// Defines if the mesh adaptor has to create a new material adaptor or simply use the one that is XML configured.
    bool m_use_new_material_adaptor {false};

    /// Defines the configured shading mode.
    std::string m_shading_mode;

    /// Defines if the mesh changes dynamically, defined in m_configuration.
    bool m_is_dynamic {false};

    /// Defines if the vertices change dynamically, defined in m_configuration.
    bool m_is_dynamic_vertices {false};

    /// Ogre mesh.
    sight::viz::scene3d::mesh::sptr m_mesh_geometry {nullptr};

    /// Stores material adaptors attached to the r2vb objects.
    std::map<data::mesh::cell_t, module::viz::scene3d::adaptor::material::sptr> m_r2vb_material_adaptor;

    /// Defines the mask used for picking request.
    std::uint32_t m_query_flags {Ogre::SceneManager::ENTITY_TYPE_MASK};

    enum class update_flags : std::uint8_t
    {
        MESH,
        VERTICES,
        COLORS,
        TEX_COORDS
    };

    static constexpr std::string_view MESH_IN = "mesh";
    data::ptr<data::mesh, data::access::in> m_mesh {this, MESH_IN};
};

//------------------------------------------------------------------------------

inline data::material::sptr mesh::get_material() const
{
    return m_material;
}

//------------------------------------------------------------------------------

inline void mesh::set_material(data::material::sptr _material)
{
    m_material = _material;
}

//------------------------------------------------------------------------------

inline void mesh::set_material_template_name(const std::string& _material_name)
{
    m_material_template_name = _material_name;
}

//------------------------------------------------------------------------------

inline void mesh::set_auto_reset_camera(bool _auto_reset_camera)
{
    m_auto_reset_camera = _auto_reset_camera;
}

//------------------------------------------------------------------------------

inline Ogre::Entity* mesh::get_entity() const
{
    return m_entity;
}

//------------------------------------------------------------------------------

inline bool mesh::get_visibility() const
{
    return m_entity != nullptr ? m_entity->getVisible() : visible();
}

//------------------------------------------------------------------------------

inline void mesh::set_dynamic(bool _is_dynamic)
{
    m_is_dynamic = _is_dynamic;
    if(m_mesh_geometry)
    {
        m_mesh_geometry->set_dynamic(_is_dynamic);
    }
}

//------------------------------------------------------------------------------

inline void mesh::set_dynamic_vertices(bool _is_dynamic)
{
    m_is_dynamic_vertices = _is_dynamic;
    if(m_mesh_geometry)
    {
        m_mesh_geometry->set_dynamic_vertices(_is_dynamic);
    }
}

//------------------------------------------------------------------------------

inline void mesh::set_query_flags(uint32_t _query_flags)
{
    m_query_flags = _query_flags;
}

//-----------------------------------------------------------------------------

inline void mesh::set_is_reconstruction_managed(bool _is_reconstruction_managed)
{
    m_is_reconstruction_managed = _is_reconstruction_managed;
}

//------------------------------------------------------------------------------

} // namespace sight::module::viz::scene3d::adaptor.
