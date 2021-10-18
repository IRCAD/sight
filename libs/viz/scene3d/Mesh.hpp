/************************************************************************
 *
 * Copyright (C) 2017-2021 IRCAD France
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
#include "viz/scene3d/Material.hpp"

#include <data/Mesh.hpp>
#include <data/PointList.hpp>

#include <OGRE/OgreMesh.h>

namespace sight::viz::scene3d
{

class R2VBRenderable;

/**
 * @brief   Manages a generic mesh
 */
class VIZ_SCENE3D_CLASS_API Mesh
{
public:

    typedef std::shared_ptr<Mesh> sptr;
    typedef std::weak_ptr<Mesh> wptr;

    enum BufferBinding
    {
        POSITION_NORMAL = 0,
        COLOUR          = 1,
        TEXCOORD        = 2,
        NUM_BINDINGS
    };

    /// Constructor
    VIZ_SCENE3D_API Mesh(const std::string& _name);

    /// Destructor
    VIZ_SCENE3D_API ~Mesh();

    /**
     * @brief Adds the given buffer semantic if it does not exist, and allocate it if necessary.
     *
     * The buffer must contain a data type and must not be interleaved.
     *
     * @param _mesh used to find buffer data.
     * @param _binding layer binfing.
     * @param _semantic semantic of the buffer.
     * @param _type data type in the buffer.
     */
    void bindLayer(
        const data::Mesh::csptr& _mesh,
        BufferBinding _binding,
        ::Ogre::VertexElementSemantic _semantic,
        ::Ogre::VertexElementType _type
    );

    /// Set meshes vertex buffer to dynamic state (only has effect if called before service starting/update)
    VIZ_SCENE3D_API void setDynamicVertices(bool _isDynamic);
    /// Set meshes and indices buffers to dynamic state (only has effect if called before service starting/update)
    VIZ_SCENE3D_API void setDynamic(bool _isDynamic);

    VIZ_SCENE3D_API void setVisible(bool _visible);
    VIZ_SCENE3D_API void updateMesh(const data::Mesh::sptr& _mesh, bool _pointsOnly = false);
    VIZ_SCENE3D_API void updateMesh(const data::PointList::csptr& _pointList);
    VIZ_SCENE3D_API std::pair<bool, std::vector<R2VBRenderable*> > updateR2VB(
        const data::Mesh::sptr& _mesh,
        ::Ogre::SceneManager& _sceneMgr,
        const std::string& _materialName
    );

    /// Updates the vertices position
    VIZ_SCENE3D_API void updateVertices(const data::Mesh::csptr& _mesh);
    /// Updates the vertices position
    VIZ_SCENE3D_API void updateVertices(const data::PointList::csptr& mesh);
    /// Updates the vertices colors.
    VIZ_SCENE3D_API void updateColors(const data::Mesh::csptr& _mesh);
    /// Updates the vertices texture coordinates.
    VIZ_SCENE3D_API void updateTexCoords(const data::Mesh::csptr& _mesh);
    /// Erase the mesh data, called when the configuration change (new layer, etc...), to simplify modifications.
    VIZ_SCENE3D_API void clearMesh(::Ogre::SceneManager& _sceneMgr);

    VIZ_SCENE3D_API void updateMaterial(viz::scene3d::Material* _material, bool _isR2VB) const;

    VIZ_SCENE3D_API bool hasColorLayerChanged(const data::Mesh::csptr& _mesh);

    VIZ_SCENE3D_API ::Ogre::Entity* createEntity(::Ogre::SceneManager& _sceneMgr);

    VIZ_SCENE3D_API void invalidateR2VB();

private:

    /// Returns true if the bounding box of a ogre mesh is valid (not NaN or infinite values)
    static bool areBoundsValid(const ::Ogre::MeshPtr& _ogreMesh);

    /// Maximum size of a texture (TODO: get this from hardware instead)
    static const unsigned int s_maxTextureSize = 2048;

    /// Actual mesh data
    ::Ogre::MeshPtr m_ogreMesh;

    /// Binding for each layer
    unsigned short m_binding[NUM_BINDINGS];

    data::Mesh::CellType m_cellType {data::Mesh::CellType::_SIZE};
    /// Pointers on submeshes need for reallocation check.
    /// For QUADS and TETRAS primitives, they point to r2vb submeshes.
    ::Ogre::SubMesh* m_subMesh;

    /// Texture used to store per-primitive color
    ::Ogre::TexturePtr m_perPrimitiveColorTexture;
    /// Name of the texture used to store per-primitive color
    std::string m_perPrimitiveColorTextureName;

    /// Node containing inputs for the r2vb objects - it will never be inserted in the scene
    ::Ogre::Entity* m_r2vbEntity {nullptr};
    /// Mesh data for r2vb input - contains only line lists with adjacency information primitives
    ::Ogre::MeshPtr m_r2vbMesh;
    /// List of r2vb objects - these objects triggers the r2vb process and render the output data
    std::map<data::Mesh::CellType, viz::scene3d::R2VBRenderable*> m_r2vbObject;

    /// Defines if there is a normal layer
    bool m_hasNormal {false};
    /// Defines if there is a vertex color layer
    bool m_hasVertexColor {false};
    /// Defines if there is a primitive color layer
    bool m_hasPrimitiveColor {false};
    /// defines if the mesh has UV coordinates, defined in m_configuration
    bool m_hasUV {false};
    /// defines if the mesh changes dynamically, defined in m_configuration
    bool m_isDynamic {false};
    /// defines if the vertices change dynamically, defined in m_configuration
    bool m_isDynamicVertices {false};
};

//------------------------------------------------------------------------------

inline void Mesh::setDynamic(bool _isDynamic)
{
    m_isDynamic = _isDynamic;
}

//------------------------------------------------------------------------------

inline void Mesh::setDynamicVertices(bool _isDynamic)
{
    m_isDynamicVertices = _isDynamic;
}

//------------------------------------------------------------------------------

} // namespace sight::viz::scene3d
