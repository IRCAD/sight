/************************************************************************
 *
 * Copyright (C) 2017-2023 IRCAD France
 * Copyright (C) 2017-2020 IHU Strasbourg
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

#include "viz/scene3d/config.hpp"
#include "viz/scene3d/material.hpp"

#include <data/mesh.hpp>
#include <data/point_list.hpp>

#include <OGRE/OgreMesh.h>

namespace sight::viz::scene3d
{

class r2vb_renderable;

/**
 * @brief   Manages a generic mesh
 */
class VIZ_SCENE3D_CLASS_API mesh
{
public:

    using sptr = std::shared_ptr<mesh>;
    using wptr = std::weak_ptr<mesh>;

    enum buffer_binding
    {
        position_normal = 0,
        colour          = 1,
        texcoord        = 2,
        num_bindings
    };

    /// Constructor
    VIZ_SCENE3D_API mesh(const std::string& _name);

    /// Destructor
    VIZ_SCENE3D_API ~mesh();

    /**
     * @brief Adds the given buffer semantic if it does not exist, and allocate it if necessary.
     *
     * The buffer must contain a data type and must not be interleaved.
     *
     * @param _mesh used to find buffer data.
     * @param _binding layer binding.
     * @param _semantic semantic of the buffer.
     * @param _type data type in the buffer.
     */
    void bind_layer(
        const data::mesh::csptr& _mesh,
        buffer_binding _binding,
        Ogre::VertexElementSemantic _semantic,
        Ogre::VertexElementType _type
    );

    /// Set meshes vertex buffer to dynamic state (only has effect if called before service starting/update)
    VIZ_SCENE3D_API void set_dynamic_vertices(bool _is_dynamic);
    /// Set meshes and indices buffers to dynamic state (only has effect if called before service starting/update)
    VIZ_SCENE3D_API void set_dynamic(bool _is_dynamic);

    VIZ_SCENE3D_API void set_visible(bool _visible);
    VIZ_SCENE3D_API void update_mesh(const data::mesh::csptr& _mesh, bool _points_only = false);
    VIZ_SCENE3D_API void update_mesh(const data::point_list::csptr& _point_list);
    VIZ_SCENE3D_API std::pair<bool, std::vector<r2vb_renderable*> > update_r2_vb(
        const data::mesh::csptr& _mesh,
        Ogre::SceneManager& _scene_mgr,
        const std::string& _material_name
    );

    /// Updates the vertices position
    VIZ_SCENE3D_API void update_vertices(const data::mesh::csptr& _mesh);
    /// Updates the vertices position
    VIZ_SCENE3D_API void update_vertices(const data::point_list::csptr& _mesh);
    /// Updates the vertices colors.
    VIZ_SCENE3D_API void update_colors(const data::mesh::csptr& _mesh);
    /// Updates the vertices texture coordinates.
    VIZ_SCENE3D_API void update_tex_coords(const data::mesh::csptr& _mesh);
    /// Erase the mesh data, called when the configuration change (new layer, etc...), to simplify modifications.
    VIZ_SCENE3D_API void clear_mesh(Ogre::SceneManager& _scene_mgr);

    VIZ_SCENE3D_API void update_material(viz::scene3d::material* _material, bool _is_r2_vb) const;

    [[nodiscard]] VIZ_SCENE3D_API bool has_color_layer_changed(const data::mesh::csptr& _mesh) const;

    VIZ_SCENE3D_API Ogre::Entity* create_entity(Ogre::SceneManager& _scene_mgr);

    VIZ_SCENE3D_API void invalidate_r2_vb();

private:

    /// Returns true if the bounding box of a ogre mesh is valid (not NaN or infinite values)
    static bool are_bounds_valid(const Ogre::MeshPtr& _ogre_mesh);

    /// Maximum size of a texture (TODO: get this from hardware instead)
    static const unsigned int MAX_TEXTURE_SIZE = 2048;

    /// Actual mesh data
    Ogre::MeshPtr m_ogre_mesh;

    /// Binding for each layer
    std::array<std::uint16_t, num_bindings> m_binding {};

    data::mesh::cell_type_t m_cell_type {data::mesh::cell_type_t::size};
    /// Pointers on submeshes need for reallocation check.
    /// For QUADS and TETRAS primitives, they point to r2vb submeshes.
    Ogre::SubMesh* m_sub_mesh {nullptr};

    /// texture used to store per-primitive color
    Ogre::TexturePtr m_per_primitive_color_texture;
    /// Name of the texture used to store per-primitive color
    std::string m_per_primitive_color_texture_name;

    /// Node containing inputs for the r2vb objects - it will never be inserted in the scene
    Ogre::Entity* m_r2vb_entity {nullptr};
    /// mesh data for r2vb input - contains only line lists with adjacency information primitives
    Ogre::MeshPtr m_r2vb_mesh;
    /// List of r2vb objects - these objects triggers the r2vb process and render the output data
    std::map<data::mesh::cell_type_t, viz::scene3d::r2vb_renderable*> m_r2vb_object;

    /// Defines if there is a normal layer
    bool m_has_normal {false};
    /// Defines if there is a vertex color layer
    bool m_has_vertex_color {false};
    /// Defines if there is a primitive color layer
    bool m_has_primitive_color {false};
    /// defines if the mesh has UV coordinates, defined in m_configuration
    bool m_has_uv {false};
    /// defines if the mesh changes dynamically, defined in m_configuration
    bool m_is_dynamic {false};
    /// defines if the vertices change dynamically, defined in m_configuration
    bool m_is_dynamic_vertices {false};
};

//------------------------------------------------------------------------------

inline void mesh::set_dynamic(bool _is_dynamic)
{
    m_is_dynamic = _is_dynamic;
}

//------------------------------------------------------------------------------

inline void mesh::set_dynamic_vertices(bool _is_dynamic)
{
    m_is_dynamic_vertices = _is_dynamic;
}

//------------------------------------------------------------------------------

} // namespace sight::viz::scene3d
