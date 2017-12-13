/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWRENDEROGRE_MESH_HPP__
#define __FWRENDEROGRE_MESH_HPP__

#include "fwRenderOgre/config.hpp"
#include "fwRenderOgre/Material.hpp"

#include <fwData/Mesh.hpp>
#include <fwData/PointList.hpp>

#include <OGRE/OgreMesh.h>

namespace fwRenderOgre
{

class R2VBRenderable;

/**
 * @brief   Manages a generic mesh
 */
class FWRENDEROGRE_CLASS_API Mesh
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
    FWRENDEROGRE_API Mesh(const std::string& _name);

    /// Destructor
    FWRENDEROGRE_API ~Mesh();

    /// Bind a vertex layer
    void bindLayer(const ::fwData::Mesh::csptr& _mesh, BufferBinding _binding, ::Ogre::VertexElementSemantic _semantic,
                   ::Ogre::VertexElementType _type);

    /// Set meshes vertex buffer to dynamic state (only has effect if called before service starting/update)
    FWRENDEROGRE_API void setDynamicVertices(bool _isDynamic);
    /// Set meshes and indices buffers to dynamic state (only has effect if called before service starting/update)
    FWRENDEROGRE_API void setDynamic(bool _isDynamic);

    FWRENDEROGRE_API void setVisible(bool _visible);
    FWRENDEROGRE_API void updateMesh(const ::fwData::Mesh::sptr& _mesh);
    FWRENDEROGRE_API void updateMesh(const ::fwData::PointList::csptr& _pointList);
    FWRENDEROGRE_API std::vector<R2VBRenderable*> updateR2VB(const ::fwData::Mesh::sptr& _mesh,
                                                             ::Ogre::SceneManager& _sceneMgr,
                                                             const std::string& _materialName, bool _hasTexture);

    /// Updates the vertices position
    FWRENDEROGRE_API void updateVertices(const ::fwData::Mesh::csptr& _mesh);
    /// Updates the vertices position
    FWRENDEROGRE_API void updateVertices(const ::fwData::PointList::csptr& mesh);
    /// Updates the vertices colors.
    FWRENDEROGRE_API void updateColors(const ::fwData::Mesh::csptr& _mesh);
    /// Updates the vertices texture coordinates.
    FWRENDEROGRE_API void updateTexCoords(const ::fwData::Mesh::csptr& _mesh);
    /// Erase the mesh data, called when the configuration change (new layer, etc...), to simplify modifications.
    FWRENDEROGRE_API void clearMesh(::Ogre::SceneManager& _sceneMgr);

    FWRENDEROGRE_API void updateMaterial(::fwRenderOgre::Material* _material, bool _isR2VB) const;

    FWRENDEROGRE_API bool hasColorLayerChanged(const ::fwData::Mesh::csptr& _mesh);

    FWRENDEROGRE_API ::Ogre::Entity* createEntity(::Ogre::SceneManager& _sceneMgr);

    FWRENDEROGRE_API void invalidateR2VB();

private:

    /// Maximum size of a texture (TODO: get this from hardware instead)
    static const unsigned int s_maxTextureSize = 2048;

    /// Actual mesh data
    ::Ogre::MeshPtr m_ogreMesh;

    /// Binding for each layer
    unsigned short m_binding[NUM_BINDINGS];

    /// Number of primitives types that are handled by ::fwData::Mesh
    static const unsigned int s_numPrimitiveTypes = ::fwData::Mesh::TETRA + 1;

    /// Pointers on submeshes needed for reallocation check.
    /// For QUADS and TETRAS primitives, they point to r2vb submeshes.
    std::array< ::Ogre::SubMesh*, s_numPrimitiveTypes> m_subMeshes;

    /// Texture used to store per-primitive color
    ::Ogre::TexturePtr m_perPrimitiveColorTexture;
    /// Name of the texture used to store per-primitive color
    std::string m_perPrimitiveColorTextureName;

    /// Node containing inputs for the r2vb objects - it will never be inserted in the scene
    ::Ogre::Entity* m_r2vbEntity { nullptr };
    /// Mesh data for r2vb input - contains only line lists with adjacency information primitives
    ::Ogre::MeshPtr m_r2vbMesh;
    /// List of r2vb objects - these objects triggers the r2vb process and render the output data
    std::map< ::fwData::Mesh::CellTypes, ::fwRenderOgre::R2VBRenderable*> m_r2vbObject;

    /// Defines if there is a normal layer
    bool m_hasNormal { false };
    /// Defines if there is a vertex color layer
    bool m_hasVertexColor { false };
    /// Defines if there is a primitive color layer
    bool m_hasPrimitiveColor { false };
    /// defines if the mesh has UV coordinates, defined in m_configuration
    bool m_hasUV { false };
    /// defines if the mesh changes dynamically, defined in m_configuration
    bool m_isDynamic { false };
    /// defines if the vertices change dynamically, defined in m_configuration
    bool m_isDynamicVertices { false };
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

} // namespace fwRenderOgre

#endif // __FWRENDEROGRE_MESH_HPP__
