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

#include <data/material.hpp>
#include <data/mesh.hpp>

#include <viz/scene3d/material/generic.hpp>

#include <OGRE/OgreAxisAlignedBox.h>
#include <OGRE/OgreMaterial.h>

namespace sight::viz::scene3d::material
{

/**
 * @brief   Wrapper around the "Default" material used in Sight and defined in the 'Default.material' script.
 *
 * This material is primarily dedicated to offer the features required by sight::data::material.
 * It provides a material with three different types of shading: ambient, flat or phong.
 * It also allows to shows point and cell normals, which can be useful for debugging.
 * Last it also offers the possibility to bind a texture on the diffuse component.
 *
 * This implies this class handles all the shader combinations, which are defined in 'Default.program' script.
 */
class SIGHT_VIZ_SCENE3D_CLASS_API standard : public sight::viz::scene3d::material::generic
{
public:

    using uptr = std::unique_ptr<standard>;
    using sight::viz::scene3d::material::generic::set_layout;

    /// Name of the default used as a base for the instance created by this adaptor
    static const inline std::string TEMPLATE = "Default";
    static const inline std::string TEXTURE  = "diffuseTexture";

    /// Constructor
    SIGHT_VIZ_SCENE3D_API standard(const std::string& _name);

    /// Destructor
    SIGHT_VIZ_SCENE3D_API ~standard() override = default;

    /// Updates material options mode (standard, point normals or cells normals)
    SIGHT_VIZ_SCENE3D_API void update_options_mode(int _options_mode);

    /// Update material shading mode (flat/phong)
    SIGHT_VIZ_SCENE3D_API void set_shading(
        data::material::shading_t _shading_mode,
        int _num_lights,
        bool _has_diffuse_texture = false,
        bool _use_texture_alpha   = false
    );

    /// Set the material template used
    SIGHT_VIZ_SCENE3D_API void set_layout(const sight::viz::scene3d::mesh& _mesh) override;

private:

    /// Size of the mesh used to adjust the size of normals
    Ogre::Real m_mesh_size {1.F};
};

//------------------------------------------------------------------------------

} // namespace sight::viz::scene3d::material
