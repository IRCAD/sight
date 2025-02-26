/************************************************************************
 *
 * Copyright (C) 2025 IRCAD France
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

#include <sight/viz/scene3d/config.hpp>

#include <viz/scene3d/material/generic.hpp>

namespace sight::viz::scene3d::material
{

/**
 * @brief   Handle the material side of the render-to-vertex-buffer process used to
 * - transform quad meshes into triangle meshes
 * - transform per-primitive colors into vertices colors
 */
class SIGHT_VIZ_SCENE3D_CLASS_API r2vb : public sight::viz::scene3d::material::generic
{
public:

    using uptr = std::unique_ptr<r2vb>;

    /// Constructor
    SIGHT_VIZ_SCENE3D_API r2vb(const std::string& _name, const std::string& _template_name);

    /// Destructor
    SIGHT_VIZ_SCENE3D_API ~r2vb() override = default;

    /// Update material shading mode (flat/phong)
    SIGHT_VIZ_SCENE3D_API void set_shading(data::material::shading_t _shading_mode);

    /// @copydoc sight::viz::scene3d::material::generic::set_layout
    SIGHT_VIZ_SCENE3D_API void set_layout(const sight::viz::scene3d::mesh& _mesh) override;

    /// Set the type of primitive in input used for the render-to-vertex-buffer process
    void set_primitive_type(data::mesh::cell_type_t _type);

private:

    /// Name of the texture used to store per-primitive color
    std::string m_per_primitive_color_texture_name;

    /// Primitive type of the input mesh
    data::mesh::cell_type_t m_primitive_type {data::mesh::cell_type_t::triangle};
};

//------------------------------------------------------------------------------

inline void r2vb::set_primitive_type(data::mesh::cell_type_t _type)
{
    m_primitive_type = _type;
}

//------------------------------------------------------------------------------

} // namespace sight::viz::scene3d::material
