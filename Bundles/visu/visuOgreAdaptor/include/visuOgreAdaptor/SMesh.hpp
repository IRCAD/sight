/************************************************************************
 *
 * Copyright (C) 2014-2020 IRCAD France
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
        <config layer="..." transform="..." visible="true" materialName="..." shadingMode="gouraud" textureName="..."
        queryFlags="0x40000000" />
    </service>
   @endcode
 *
 * @subsection In-Out In-Out
 * - \b mesh [::fwData::Mesh]: adapted mesh. It can not be a read-only data because we may generate normals or add some
 * fields.
 *
 * @subsection Configuration Configuration:
 *  - \b layer (mandatory, string) : defines the mesh's layer
 *  - \b autoresetcamera (optional, yes/no, default=yes): reset the camera when this mesh is modified, "yes" or "no".
 *  - \b transform (optional, string, default="") : the name of the Ogre transform node where to attach the mesh, as it
 * was specified
 * in the STransform adaptor.
 *  - \b visible (optional, bool, default=true): set the initial visibility of the mesh.
 * Either of the following (whether a material is configured in the XML scene or not) :
 *  - \b materialName (optional, string, default="") : name of the Ogre material, as defined in the
 *::visuOgreAdaptor::SMaterial you want
 * to be bound to.
 * Only if there is no material configured in the XML scene (in this case, it has to retrieve the material
 * template, the texture adaptor and the shading mode) :
 *  - \b materialTemplate (optional, string, default="") : the name of the base Ogre material for the internally created
 * SMaterial.
 *  - \b textureName (optional, default="") : the name of the Ogre texture that the mesh will use.
 *  - \b shadingMode (optional, none/flat/gouraud/phong/ambient, default=phong) : name of the used shading mode.
 *  - \b queryFlags (optional, uint32, default=0x40000000) : Used for picking. Picked only by pickers whose mask that
 * match the
 * flag.
 */
class VISUOGREADAPTOR_CLASS_API SMesh final :
    public ::fwRenderOgre::IAdaptor,
    public ::fwRenderOgre::ITransformable
{
public:

    fwCoreServiceMacro(SMesh, ::fwRenderOgre::IAdaptor)

    /// Sets default parameters and initializes necessary members.
    VISUOGREADAPTOR_API SMesh() noexcept;

    /// Destroys Ogre resources..
    VISUOGREADAPTOR_API virtual ~SMesh() noexcept;

    /**
     * @brief Gets the associated material.
     * @return The material.
     */
    VISUOGREADAPTOR_API ::fwData::Material::sptr getMaterial() const;

    /**
     * @brief Sets the current material.
     * @param _material new material.
     */
    VISUOGREADAPTOR_API void setMaterial(::fwData::Material::sptr _material);

    /**
     * @brief Sets the material template Name.
     * @param _materialName material name.
     */
    VISUOGREADAPTOR_API void setMaterialTemplateName(const std::string& _materialName);

    /**
     * @brief Actives/deactives automatic reset on camera.
     * @param _autoResetCamera use true to activate it.
     */
    VISUOGREADAPTOR_API void setAutoResetCamera(bool _autoResetCamera);

    /**
     * @brief Gets the associated entity.
     * @return The entity.
     */
    VISUOGREADAPTOR_API ::Ogre::Entity* getEntity() const;

    /**
     * @brief Gets the mesh visibility.
     * @return True if the mesh is visible.
     */
    VISUOGREADAPTOR_API bool getVisibility() const;

    /**
     * @brief Sets whether the mesh is to be seen or not.
     * @param _isVisible use true to show the mesh.
     */
    VISUOGREADAPTOR_API void updateVisibility(bool _isVisible);

    /**
     * @brief Sets meshes vertex buffer to dynamic state (only has effect if called before service starting/update).
     * @param _isDynamic use true to use dynamic vertex buffer.
     */
    VISUOGREADAPTOR_API void setDynamicVertices(bool _isDynamic);

    /**
     * @brief Sets meshes and indices buffers to dynamic state (only has effect if called before service
     * starting/update).
     * @param _isDynamic use true to use dynamic vertex and indices buffer.
     */
    VISUOGREADAPTOR_API void setDynamic(bool _isDynamic);

    /**
     * @brief Sets the query flag.
     * @param _queryFlags value of the query flag.
     */
    VISUOGREADAPTOR_API void setQueryFlags(std::uint32_t _queryFlags);

    /**
     * @brief Sets if the reconstruction is managed or not.
     * @param _isReconstructionManaged use true if the reconstruction is managed.
     */
    VISUOGREADAPTOR_API void setIsReconstructionManaged(bool _isReconstructionManaged);

    /// Flags the r2vb objects as dirty and asks the render service to update.
    VISUOGREADAPTOR_API virtual void requestRender() override;

private:

    /// Configures the adaptor.
    virtual void configuring() override;

    /// Creates a Mesh in the Default Ogre resource group.
    virtual void starting() override;

    /**
     * @brief Proposals to connect service slots to associated object signals.
     * @return A map of each proposed connection.
     *
     * Connect ::fwData::Mesh::s_VERTEX_MODIFIED_SIG to s_MODIFY_VERTICES_SLOT
     * Connect ::fwData::Mesh::s_POINT_COLORS_MODIFIED_SIG to s_MODIFY_COLORS_SLOT
     * Connect ::fwData::Mesh::s_CELL_COLORS_MODIFIED_SIG to s_MODIFY_COLORS_SLOT
     * Connect ::fwData::Mesh::s_POINT_TEX_COORDS_MODIFIED_SIG to s_MODIFY_POINT_TEX_COORDS_SLOT
     * Connect ::fwData::Mesh::s_MODIFIED_SIG to s_UPDATE_SLOT
     */
    virtual ::fwServices::IService::KeyConnectionsMap getAutoConnections() const override;

    /// Deletes the mesh after unregistering the service, and shutting connections.
    virtual void stopping() override;

    /// Updates the mesh.
    virtual void updating() override;

    /// Updates mesh vertices.
    void modifyVertices();

    /// Updates mesh colors.
    void modifyPointColors();

    /// Updates mesh texture coordinates.
    void modifyTexCoords();

    /**
     * @brief Updates the mesh, checks if color, number of vertices have changed, and updates them.
     * @param _mesh used for the update.
     */
    void updateMesh(const ::fwData::Mesh::sptr& _mesh);

    /**
     * @brief Instantiates a new material adaptor
     * @param _materialSuffix use for the material name.
     */
    ::visuOgreAdaptor::SMaterial::sptr createMaterialService(const std::string& _materialSuffix = "");

    /// Associates a new SMaterial to the managed SMesh.
    /// With this method, SMesh is responsible for creating a SMaterial.
    void updateNewMaterialAdaptor();

    /// Updates the associated material adaptor.
    /// This method is called when a material adaptor has been configured in the XML scene.
    void updateXMLMaterialAdaptor();

    /**
     * @brief Attachs a node in the scene graph.
     * @param _node the node to attach.
     */
    void attachNode(::Ogre::MovableObject* _node);

    /// Defines whether the camera must be auto reset when a mesh is updated or not.
    bool m_autoResetCamera {true};

    /// Contains the node in the scene graph whwre the mesh is attached.
    ::Ogre::Entity* m_entity {nullptr};

    /// Contains the Ogre material adaptor.
    ::visuOgreAdaptor::SMaterial::sptr m_materialAdaptor {nullptr};

    /// Defines the attached material name (when configured by XML).
    std::string m_materialName {""};

    /// Contains the material data.
    ::fwData::Material::sptr m_material {nullptr};

    /// Defines the attached material's name.
    std::string m_materialTemplateName {::fwRenderOgre::Material::DEFAULT_MATERIAL_TEMPLATE_NAME};

    /// Defines the attached texture adaptor name.
    std::string m_textureName {""};

    /// Defines if the mesh adaptor is managed by a reconstruction adaptor.
    bool m_isReconstructionManaged {false};

    /// Defines if the mesh adaptor has to create a new material adaptor or simply use the one that is XML configured.
    bool m_useNewMaterialAdaptor {false};

    /// Enables the mesh visibility.
    bool m_isVisible {true};

    /// Defines the configured shading mode.
    std::string m_shadingMode {""};

    /// Defines if the mesh changes dynamically, defined in m_configuration.
    bool m_isDynamic {false};

    /// Defines if the vertices change dynamically, defined in m_configuration.
    bool m_isDynamicVertices {false};

    /// Ogre mesh.
    ::fwRenderOgre::Mesh::sptr m_meshGeometry {nullptr};

    /// Stores material adaptors attached to the r2vb objects.
    std::map< ::fwData::Mesh::CellTypes, ::visuOgreAdaptor::SMaterial::sptr> m_r2vbMaterialAdaptor;

    /// Defines the mask used for picking request.
    std::uint32_t m_queryFlags {::Ogre::SceneManager::ENTITY_TYPE_MASK};
};

//------------------------------------------------------------------------------

inline ::fwData::Material::sptr SMesh::getMaterial() const
{
    return m_material;
}

//------------------------------------------------------------------------------

inline void SMesh::setMaterial(::fwData::Material::sptr _material)
{
    m_material = _material;
}

//------------------------------------------------------------------------------

inline void SMesh::setMaterialTemplateName(const std::string& _materialName)
{
    m_materialTemplateName = _materialName;
}

//------------------------------------------------------------------------------

inline void SMesh::setAutoResetCamera(bool _autoResetCamera)
{
    m_autoResetCamera = _autoResetCamera;
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

} // namespace visuOgreAdaptor.
