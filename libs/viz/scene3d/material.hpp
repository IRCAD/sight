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

    typedef std::unique_ptr<material> uptr;

    /// Name of the default used as a base for the instance created by this adaptor
    VIZ_SCENE3D_API static const std::string DEFAULT_MATERIAL_TEMPLATE_NAME;

    /// Constructor
    VIZ_SCENE3D_API material(const std::string& _name, const std::string& _template_name);

    /// Destructor
    VIZ_SCENE3D_API ~material();

    /// Updates material options mode (standard, point normals or cells normals)
    VIZ_SCENE3D_API void updateOptionsMode(int _options_mode);

    /// Updates material polygon mode (surface, point or wireframe)
    VIZ_SCENE3D_API void updatePolygonMode(int _polygon_mode);

    /// Update material shading mode (flat/phong)
    VIZ_SCENE3D_API void updateShadingMode(
        int _shading_mode,
        int _num_lights,
        bool _has_diffuse_texture,
        bool _use_texture_alpha
    );

    /// Update material color
    VIZ_SCENE3D_API void updateRGBAMode(data::material::sptr _sight_material);

    void setHasMeshNormal(bool _has_mesh_normal);
    void setHasUV(bool _has_uv);
    void setHasVertexColor(bool _has_vertex_color);
    void setHasPrimitiveColor(bool _has_primitive_color, const std::string& _texture_name);

    /// Set the type of primitive
    void set_primitive_type(data::mesh::cell_type_t _type);

    /// Sets the size of the mesh. This is used either to adjust the size of normals or to expand billboards
    void setMeshSize(float _size);

    /// Set the diffuse texture
    VIZ_SCENE3D_API void setDiffuseTexture(const Ogre::TexturePtr& _texture);

    /// Set the material template used
    VIZ_SCENE3D_API void setTemplate(const std::string& _template_name);

private:

    /// Remove a rendering pass in all techniques on the current material
    void removePass(const std::string& _name);

    /// Remove all techniques related to order independent transparency support
    /// Each time we have to modify the shader programs, we clean everything
    /// and we let the material_mgr_listener generate the techniques from the basic techniques defined in the .material
    void cleanTransparencyTechniques();

    /// Associated Ogre material
    Ogre::MaterialPtr m_material;

    /// Defines if the associated mesh has a normal layer
    bool m_hasMeshNormal {true};

    /// Defines if the associated mesh has a a texture coordinates layer
    bool m_hasUV {false};

    /// Defines if the associated mesh has a a per vertex color layer
    bool m_hasVertexColor {false};

    /// Defines if the associated mesh has a a per primitive color layer
    bool m_hasPrimitiveColor {false};

    /// Primitive type of the associated mesh
    data::mesh::cell_type_t m_primitiveType {data::mesh::cell_type_t::TRIANGLE};

    /// Name of the texture used to store per-primitive color
    std::string m_perPrimitiveColorTextureName;

    /// Size of the mesh, used either to adjust the size of normals or to expand billboards
    Ogre::Real m_meshSize {1.F};

    /// Name of the material template
    std::string m_templateName;
};

//------------------------------------------------------------------------------

inline void material::setHasMeshNormal(bool _has_mesh_normal)
{
    m_hasMeshNormal = _has_mesh_normal;
}

//------------------------------------------------------------------------------

inline void material::setHasUV(bool _has_uv)
{
    m_hasUV = _has_uv;
}

//------------------------------------------------------------------------------

inline void material::setHasVertexColor(bool _has_vertex_color)
{
    m_hasVertexColor = _has_vertex_color;
}

//------------------------------------------------------------------------------

inline void material::setHasPrimitiveColor(bool _has_primitive_color, const std::string& _texture_name)
{
    m_hasPrimitiveColor            = _has_primitive_color;
    m_perPrimitiveColorTextureName = _texture_name;
}

//------------------------------------------------------------------------------

inline void material::set_primitive_type(data::mesh::cell_type_t _type)
{
    m_primitiveType = _type;
}

//------------------------------------------------------------------------------

inline void material::setMeshSize(const float _size)
{
    m_meshSize = _size;
}

//------------------------------------------------------------------------------

} // namespace sight::viz::scene3d
