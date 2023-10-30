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

#include <data/material.hpp>
#include <data/mesh.hpp>

#include <OGRE/OgreAxisAlignedBox.h>
#include <OGRE/OgreMaterial.h>

namespace sight::viz::scene3d
{

/**
 * @brief   Manages a generic material
 */
class VIZ_SCENE3D_CLASS_API material
{
public:

    using uptr = std::unique_ptr<material>;

    /// Name of the default used as a base for the instance created by this adaptor
    VIZ_SCENE3D_API static const std::string DEFAULT_MATERIAL_TEMPLATE_NAME;

    /// Constructor
    VIZ_SCENE3D_API material(const std::string& _name, const std::string& _template_name);

    /// Destructor
    VIZ_SCENE3D_API ~material();

    /// Updates material options mode (standard, point normals or cells normals)
    VIZ_SCENE3D_API void update_options_mode(int _options_mode);

    /// Updates material polygon mode (surface, point or wireframe)
    VIZ_SCENE3D_API void update_polygon_mode(int _polygon_mode);

    /// Update material shading mode (flat/phong)
    VIZ_SCENE3D_API void update_shading_mode(
        data::material::shading_t _shading_mode,
        int _num_lights,
        bool _has_diffuse_texture,
        bool _use_texture_alpha
    );

    /// Update material color
    VIZ_SCENE3D_API void update_rgba_mode(data::material::sptr _sight_material);

    void set_has_mesh_normal(bool _has_mesh_normal);
    void set_has_uv(bool _has_uv);
    void set_has_vertex_color(bool _has_vertex_color);
    void set_has_primitive_color(bool _has_primitive_color, const std::string& _texture_name);

    /// Set the type of primitive
    void set_primitive_type(data::mesh::cell_type_t _type);

    /// Sets the size of the mesh. This is used either to adjust the size of normals or to expand billboards
    void set_mesh_size(float _size);

    /// Set the diffuse texture
    VIZ_SCENE3D_API void set_diffuse_texture(const Ogre::TexturePtr& _texture);

    /// Set the material template used
    VIZ_SCENE3D_API void set_template(const std::string& _template_name);

private:

    /// Remove a rendering pass in all techniques on the current material
    void remove_pass(const std::string& _name);

    /// Remove all techniques related to order independent transparency support
    /// Each time we have to modify the shader programs, we clean everything
    /// and we let the material_mgr_listener generate the techniques from the basic techniques defined in the .material
    void clean_transparency_techniques();

    /// Associated Ogre material
    Ogre::MaterialPtr m_material;

    /// Defines if the associated mesh has a normal layer
    bool m_has_mesh_normal {true};

    /// Defines if the associated mesh has a a texture coordinates layer
    bool m_has_uv {false};

    /// Defines if the associated mesh has a a per vertex color layer
    bool m_has_vertex_color {false};

    /// Defines if the associated mesh has a a per primitive color layer
    bool m_has_primitive_color {false};

    /// Primitive type of the associated mesh
    data::mesh::cell_type_t m_primitive_type {data::mesh::cell_type_t::triangle};

    /// Name of the texture used to store per-primitive color
    std::string m_per_primitive_color_texture_name;

    /// Size of the mesh, used either to adjust the size of normals or to expand billboards
    Ogre::Real m_mesh_size {1.F};

    /// Name of the material template
    std::string m_template_name;
};

//------------------------------------------------------------------------------

inline void material::set_has_mesh_normal(bool _has_mesh_normal)
{
    m_has_mesh_normal = _has_mesh_normal;
}

//------------------------------------------------------------------------------

inline void material::set_has_uv(bool _has_uv)
{
    m_has_uv = _has_uv;
}

//------------------------------------------------------------------------------

inline void material::set_has_vertex_color(bool _has_vertex_color)
{
    m_has_vertex_color = _has_vertex_color;
}

//------------------------------------------------------------------------------

inline void material::set_has_primitive_color(bool _has_primitive_color, const std::string& _texture_name)
{
    m_has_primitive_color              = _has_primitive_color;
    m_per_primitive_color_texture_name = _texture_name;
}

//------------------------------------------------------------------------------

inline void material::set_primitive_type(data::mesh::cell_type_t _type)
{
    m_primitive_type = _type;
}

//------------------------------------------------------------------------------

inline void material::set_mesh_size(const float _size)
{
    m_mesh_size = _size;
}

//------------------------------------------------------------------------------

} // namespace sight::viz::scene3d
