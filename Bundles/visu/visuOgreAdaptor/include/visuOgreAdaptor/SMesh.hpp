/************************************************************************
 *
 * Copyright (C) 2014-2019 IRCAD France
 * Copyright (C) 2014-2019 IHU Strasbourg
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

#include "visuOgreAdaptor/config.hpp"
#include "visuOgreAdaptor/SMaterial.hpp"
#include "visuOgreAdaptor/STransform.hpp"

#include <fwData/Material.hpp>
#include <fwData/Mesh.hpp>

#include <fwRenderOgre/IAdaptor.hpp>
#include <fwRenderOgre/ITransformable.hpp>
#include <fwRenderOgre/Mesh.hpp>

#include <OGRE/OgreEntity.h>

namespace fwData
{
class Material;
}
namespace fwData
{
class Mesh;
}

namespace visuOgreAdaptor
{

/**
 * @brief This adaptor shows individual meshes.
 *
 * This class handles the conversion of ::fwData::Mesh to Ogre3d. It can handle triangles, edges, quads or tetrahedrons.
 * For the quads and tetrahedrons, we generate the triangles in a pre-process, using the render to vertex buffer (r2vb)
 * feature to avoid the cost of geometry shaders when usign multi-pass rendering techniques.
 *
 * An Ogre entity is created from the mesh. A second mesh and a second entity are created as an input for the r2vb.
 * Thus, the main mesh will contains only triangles or edges, while the second entity contains only quads or tetras.
 * An Ogre material is also created, and then managed by a SMaterial adaptor (specified in the configuration otherwise
 * a new one is generated).
 *
 * To handle the per-primitive color of ::fwData::Mesh we also rely on geometry shaders, and thus on r2vb. We build a
 * texture containing the color for each primitive. This texture is fetched inside the geometry shader using the
 * primitive id.
 *
 * @section Slots Slots
 * - \b updateVisibility(bool): Sets whether the mesh is to be seen or not.
 * - \b update(): Called when the mesh is modified.
 * - \b modifyColors(): Called when the point colors are modified.
 * - \b modifyTexCoords(): Called when the texture coordinates are modified.
 * - \b modifyVertices(): Called when the vertices are modified.
 *
 * @section XML XML Configuration
 * @code{.xml}
    <service uid="..." type="::visuOgreAdaptor::SMesh" >
        <inout key="mesh" uid="..." />
        <config layer="layerId" transform="transformUID" materialAdaptor="materialName" shadingMode="gouraud"
                textureName="texAdaptorUID" />
    </service>
   @endcode
 * @subsection In-Out In-Out
 * - \b mesh [::fwData::Mesh]: adapted mesh. It can not be a read-only data because we may generate normals or add some
 * fields.
 * @subsection Configuration Configuration:
 *  - \b layer (mandatory) : defines the mesh's layer
 *  - \b autoresetcamera (optional, default="yes"): reset the camera when this mesh is modified, "yes" or "no".
 *  - \b transform (optional) : the name of the Ogre transform node where to attach the mesh, as it was specified
 * in the STransform adaptor.
 * Either of the following (whether a material is configured in the XML scene or not) :
 *  - \b materialName (optional) : name of the Ogre material, as defined in the ::visuOgreAdaptor::SMaterial you want
 * to be bound to.
 * Only if there is no material configured in the XML scene (in this case, it has to retrieve the material
 * template, the texture adaptor and the shading mode) :
 *  - \b materialTemplate (optional) : the name of the base Ogre material for the internally created SMaterial.
 *  - \b textureName (optional) : the name of the Ogre texture that the mesh will use.
 *  - \b shadingMode (optional, none/flat/gouraud/phong/ambient, default=phong) : name of the used shading mode.
 *  - \b queryFlags (optional) : Used for picking. Picked only by pickers with the same flag.
 */
class VISUOGREADAPTOR_CLASS_API SMesh : public ::fwRenderOgre::IAdaptor,
                                        public ::fwRenderOgre::ITransformable
{
public:
    fwCoreServiceMacro(SMesh, ::fwRenderOgre::IAdaptor);

    /// Constructor: Sets default parameters and initializes necessary members.
    VISUOGREADAPTOR_API SMesh() noexcept;
    /// Destructor: if an entity exists in the Ogre Scene, asks Ogre to destroy it.
    VISUOGREADAPTOR_API virtual ~SMesh() noexcept;
    /// Returns the material associated to this.
    VISUOGREADAPTOR_API SPTR(::fwData::Material) getMaterial() const;
    /// Sets the current material.
    VISUOGREADAPTOR_API void setMaterial(SPTR(::fwData::Material) material);
    /// Sets the material template Name.
    VISUOGREADAPTOR_API void setMaterialTemplateName(const std::string& materialName);
    /// Active/Inactive automatic reset on camera. By default =true.
    VISUOGREADAPTOR_API void setAutoResetCamera(bool autoResetCamera);
    /// Returns associated entity
    VISUOGREADAPTOR_API ::Ogre::Entity* getEntity() const;
    /// Returns if the SMesh is visible in the scene or not.
    VISUOGREADAPTOR_API bool getVisibility() const;
    /// Sets whether the mesh is to be seen or not.
    VISUOGREADAPTOR_API void updateVisibility(bool isVisible);
    /// Set meshes vertex buffer to dynamic state (only has effect if called before service starting/update)
    VISUOGREADAPTOR_API void setDynamicVertices(bool _isDynamic);
    /// Set meshes and indices buffers to dynamic state (only has effect if called before service starting/update)
    VISUOGREADAPTOR_API void setDynamic(bool _isDynamic);
    /// Set query mask, it's a flag for picking request
    VISUOGREADAPTOR_API void setQueryFlags(std::uint32_t _queryFlags);
    /// This method is called by a reconstruction adaptor after creating the mesh adaptor
    VISUOGREADAPTOR_API void setIsReconstructionManaged(bool _isReconstructionManaged);
    /// Returns proposals to connect service slots to associated object signals
    VISUOGREADAPTOR_API ::fwServices::IService::KeyConnectionsMap getAutoConnections() const override;
    /// Ask the render service (SRender) to update - we also flag the r2vb objects as dirty
    VISUOGREADAPTOR_API virtual void requestRender() override;

private:

    /// Configures the adaptor
    void configuring() override;
    /// Manually creates a Mesh in the Default Ogre Ressource group
    void starting() override;
    /// Deletes the mesh after unregistering the service, and shutting connections.
    void stopping() override;
    /// Called when the mesh is modified
    void updating() override;
    /// Updates the Mesh, checks if color, number of vertices have changed, and updates them.
    void updateMesh(const ::fwData::Mesh::sptr& mesh);
    /// Instantiates a new material adaptor
    ::visuOgreAdaptor::SMaterial::sptr createMaterialService(const std::string& _materialSuffix = "");
    /// Associates a new SMaterial to the managed SMesh.
    /// With this method, SMesh is responsible for creating a SMaterial
    void updateNewMaterialAdaptor();
    /// Updates the associated material adaptor.
    /// This method is called when a material adaptor has been configured in the XML scene
    void updateXMLMaterialAdaptor();
    /// Attach a node in the scene graph
    void attachNode(::Ogre::MovableObject* _node);

    /**
     * @name Slots methods
     * @{
     */
    /// Slot: called when the vertices are modified
    void modifyVertices();
    /// Slot: called when the point colors are modified
    void modifyPointColors();
    /// Slot: called when the texture coordinates are modified
    void modifyTexCoords();
    /** @} */

    /// Sets whether the camera must be auto reset when a mesh is updated or not.
    bool m_autoResetCamera;
    /// Node in the scene graph
    ::Ogre::Entity* m_entity;
    /// SMaterial attached to the mesh
    ::visuOgreAdaptor::SMaterial::sptr m_materialAdaptor;
    /// Attached material name (when configured by XML)
    std::string m_materialName;
    /// Ogre Material related to the mesh
    ::fwData::Material::sptr m_material;
    /// Attached Material's name
    std::string m_materialTemplateName;
    /// Attached texture adaptor UID
    std::string m_textureName;
    /// Indicates if the mesh adaptor is managed by a reconstruction adaptor
    bool m_isReconstructionManaged;
    /// Indicates if the mesh adaptor has to create a new material adaptor or simply use the one that is XML configured
    bool m_useNewMaterialAdaptor;
    /// Is the entity visible or not ? We need to store it in the adaptor because the information may be received
    /// before the entity is created.
    bool m_isVisible;
    /// The configured shading mode
    std::string m_shadingMode;
    /// defines if the mesh changes dynamically, defined in m_configuration
    bool m_isDynamic { false };
    /// defines if the vertices change dynamically, defined in m_configuration
    bool m_isDynamicVertices { false };

    ::fwRenderOgre::Mesh::sptr m_meshGeometry;
    /// SMaterial adaptors attached to the r2vb objects
    std::map< ::fwData::Mesh::CellTypes, ::visuOgreAdaptor::SMaterial::sptr> m_r2vbMaterialAdaptor;
    /// Mask for picking request
    std::uint32_t m_queryFlags {0};
};

//------------------------------------------------------------------------------
// Inline functions

inline SPTR(::fwData::Material) SMesh::getMaterial() const
{
    return m_material;
}

//------------------------------------------------------------------------------

inline void SMesh::setMaterial(::fwData::Material::sptr material)
{
    m_material = material;
}

//------------------------------------------------------------------------------

inline void SMesh::setMaterialTemplateName(const std::string& materialName)
{
    m_materialTemplateName = materialName;
}

//------------------------------------------------------------------------------

inline void SMesh::setAutoResetCamera(bool autoResetCamera)
{
    m_autoResetCamera = autoResetCamera;
}

//------------------------------------------------------------------------------

inline ::Ogre::Entity* SMesh::getEntity() const
{
    return m_entity;
}

//------------------------------------------------------------------------------

inline bool SMesh::getVisibility() const
{
    return m_entity ? m_entity->getVisible() : m_isVisible;
}

//------------------------------------------------------------------------------

inline void SMesh::setDynamic(bool _isDynamic)
{
    m_isDynamic = _isDynamic;
    if(m_meshGeometry)
    {
        m_meshGeometry->setDynamic(_isDynamic);
    }
}

//------------------------------------------------------------------------------

inline void SMesh::setDynamicVertices(bool _isDynamic)
{
    m_isDynamicVertices = _isDynamic;
    if(m_meshGeometry)
    {
        m_meshGeometry->setDynamicVertices(_isDynamic);
    }
}

//------------------------------------------------------------------------------

inline void SMesh::setQueryFlags(uint32_t _queryFlags)
{
    m_queryFlags = _queryFlags;
}

//-----------------------------------------------------------------------------

inline void SMesh::setIsReconstructionManaged(bool _isReconstructionManaged)
{
    m_isReconstructionManaged = _isReconstructionManaged;
}

//------------------------------------------------------------------------------

} //namespace visuOgreAdaptor
