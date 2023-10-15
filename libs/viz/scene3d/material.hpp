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
    VIZ_SCENE3D_API material(const std::string& _name, const std::string& _templateName);

    /// Destructor
    VIZ_SCENE3D_API ~material();

    /// Updates material options mode (standard, point normals or cells normals)
    VIZ_SCENE3D_API void updateOptionsMode(int _optionsMode);

    /// Updates material polygon mode (surface, point or wireframe)
    VIZ_SCENE3D_API void updatePolygonMode(int _polygonMode);

    /// Update material shading mode (flat/phong)
    VIZ_SCENE3D_API void updateShadingMode(
        int _shadingMode,
        int _numLights,
        bool _hasDiffuseTexture,
        bool useTextureAlpha
    );

    /// Update material color
    VIZ_SCENE3D_API void updateRGBAMode(data::material::sptr _sightMaterial);

    void setHasMeshNormal(bool _hasMeshNormal);
    void setHasUV(bool _hasUV);
    void setHasVertexColor(bool _hasVertexColor);
    void setHasPrimitiveColor(bool _hasPrimitiveColor, const std::string& _textureName);

    /// Set the type of primitive
    void setPrimitiveType(data::mesh::CellType _type);

    /// Sets the size of the mesh. This is used either to adjust the size of normals or to expand billboards
    void setMeshSize(float _size);

    /// Set the diffuse texture
    VIZ_SCENE3D_API void setDiffuseTexture(const Ogre::TexturePtr& _texture);

    /// Set the material template used
    VIZ_SCENE3D_API void setTemplate(const std::string& _templateName);

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
    data::mesh::CellType m_primitiveType {data::mesh::CellType::TRIANGLE};

    /// Name of the texture used to store per-primitive color
    std::string m_perPrimitiveColorTextureName;

    /// Size of the mesh, used either to adjust the size of normals or to expand billboards
    Ogre::Real m_meshSize {1.F};

    /// Name of the material template
    std::string m_templateName;
};

//------------------------------------------------------------------------------

inline void material::setHasMeshNormal(bool _hasMeshNormal)
{
    m_hasMeshNormal = _hasMeshNormal;
}

//------------------------------------------------------------------------------

inline void material::setHasUV(bool _hasUV)
{
    m_hasUV = _hasUV;
}

//------------------------------------------------------------------------------

inline void material::setHasVertexColor(bool _hasVertexColor)
{
    m_hasVertexColor = _hasVertexColor;
}

//------------------------------------------------------------------------------

inline void material::setHasPrimitiveColor(bool _hasPrimitiveColor, const std::string& _textureName)
{
    m_hasPrimitiveColor            = _hasPrimitiveColor;
    m_perPrimitiveColorTextureName = _textureName;
}

//------------------------------------------------------------------------------

inline void material::setPrimitiveType(data::mesh::CellType _type)
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
