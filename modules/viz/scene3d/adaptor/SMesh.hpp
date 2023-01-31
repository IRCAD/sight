/************************************************************************
 *
 * Copyright (C) 2014-2023 IRCAD France
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

#include "modules/viz/scene3d/adaptor/SMaterial.hpp"
#include "modules/viz/scene3d/adaptor/STransform.hpp"
#include "modules/viz/scene3d/config.hpp"

#include <data/Material.hpp>
#include <data/Mesh.hpp>

#include <viz/scene3d/IAdaptor.hpp>
#include <viz/scene3d/ITransformable.hpp>
#include <viz/scene3d/Mesh.hpp>

#include <OGRE/OgreEntity.h>

namespace sight::data
{

class Material;

} // namespace sight::data
namespace sight::data
{

class Mesh;

} // namespace sight::data

namespace sight::module::viz::scene3d::adaptor
{

/**
 * @brief This adaptor shows individual meshes.
 *
 * This class handles the conversion of data::Mesh to Ogre3d. It can handle triangles, edges, quads or tetrahedrons.
 * For the quads and tetrahedrons, we generate the triangles in a pre-process, using the render to vertex buffer (r2vb)
 * feature to avoid the cost of geometry shaders when using multi-pass rendering techniques.
 *
 * An Ogre entity is created from the mesh. A second mesh and a second entity are created as an input for the r2vb.
 * Thus, the main mesh will contains only triangles or edges, while the second entity contains only quads or tetras.
 * An Ogre material is also created, and then managed by a SMaterial adaptor (specified in the configuration otherwise
 * a new one is generated).
 *
 * To handle the per-primitive color of data::Mesh we also rely on geometry shaders, and thus on r2vb. We build a
 * texture containing the color for each primitive. This texture is fetched inside the geometry shader using the
 * primitive id.
 *
 * @section Slots Slots
 * - \b updateVisibility(bool): sets whether the mesh is to be seen or not.
 * - \b toggleVisibility(): toggle whether the mesh is shown or not.
 * - \b show(): shows the mesh.
 * - \b hide(): hides the mesh.
 * - \b update(): called when the mesh is modified.
 * - \b modifyColors(): called when the point colors are modified.
 * - \b modifyTexCoords(): called when the texture coordinates are modified.
 * - \b modifyVertices(): called when the vertices are modified.
 *
 * @section XML XML Configuration
 * @code{.xml}
    <service uid="..." type="sight::module::viz::scene3d::adaptor::SMesh" >
        <in key="mesh" uid="..." />
        <config transform="..." visible="true" materialName="..." shadingMode="phong" textureName="..."
        queryFlags="0x40000000" />
    </service>
   @endcode
 *
 * @subsection Input Input
 * - \b mesh [sight::data::Mesh]: adapted mesh.
 *
 * @subsection Configuration Configuration:
 *  - \b autoresetcamera (optional, true/false, default=true): reset the camera when this mesh is modified, "true" or
 *"false".
 *  - \b transform (optional, string, default=""): the name of the Ogre transform node where to attach the mesh, as it
 *       was specified
 *       in the STransform adaptor.
 *  - \b visible (optional, bool, default=true): set the initial visibility of the mesh.
 *       Either of the following (whether a material is configured in the XML scene or not) :
 *  - \b materialName (optional, string, default=""): name of the Ogre material, as defined in the
 *       module::viz::scene3d::adaptor::SMaterial you want to be bound to.
 *       Only if there is no material configured in the XML scene (in this case, it has to retrieve the material
 *       template, the texture adaptor and the shading mode) :
 *  - \b materialTemplate (optional, string, default=""): the name of the base Ogre material for the internally created
 *       SMaterial.
 *  - \b textureName (optional, default=""): the name of the Ogre texture that the mesh will use.
 *  - \b shadingMode (optional, none/flat/phong/ambient, default=phong): name of the used shading mode.
 *  - \b queryFlags (optional, uint32, default=0x40000000): Used for picking. Picked only by pickers whose mask that
 *       match the flag.
 */
class MODULE_VIZ_SCENE3D_CLASS_API SMesh final :
    public sight::viz::scene3d::IAdaptor,
    public sight::viz::scene3d::ITransformable
{
public:

    /// Generates default methods as New, dynamicCast, ...
    SIGHT_DECLARE_SERVICE(SMesh, sight::viz::scene3d::IAdaptor);

    /// Sets default parameters and initializes necessary members.
    MODULE_VIZ_SCENE3D_API SMesh() noexcept;

    /// Destroys Ogre resources.
    MODULE_VIZ_SCENE3D_API ~SMesh() noexcept override;

    /**
     * @brief Gets the associated material.
     * @return The material.
     */
    MODULE_VIZ_SCENE3D_API data::Material::sptr getMaterial() const;

    /**
     * @brief Sets the current material.
     * @param _material new material.
     */
    MODULE_VIZ_SCENE3D_API void setMaterial(data::Material::sptr _material);

    /**
     * @brief Sets the material template Name.
     * @param _materialName material name.
     */
    MODULE_VIZ_SCENE3D_API void setMaterialTemplateName(const std::string& _materialName);

    /**
     * @brief Enables/disables automatic reset on camera.
     * @param _autoResetCamera use true to activate it.
     */
    MODULE_VIZ_SCENE3D_API void setAutoResetCamera(bool _autoResetCamera);

    /**
     * @brief Gets the associated entity.
     * @return The entity.
     */
    MODULE_VIZ_SCENE3D_API Ogre::Entity* getEntity() const;

    /**
     * @brief Gets the mesh visibility.
     * @return True if the mesh is visible.
     */
    MODULE_VIZ_SCENE3D_API bool getVisibility() const;

    /**
     * @brief Sets meshes vertex buffer to dynamic state (only has effect if called before service starting/update).
     * @param _isDynamic use true to use dynamic vertex buffer.
     */
    MODULE_VIZ_SCENE3D_API void setDynamicVertices(bool _isDynamic);

    /**
     * @brief Sets meshes and indices buffers to dynamic state (only has effect if called before service
     * starting/update).
     * @param _isDynamic use true to use dynamic vertex and indices buffer.
     */
    MODULE_VIZ_SCENE3D_API void setDynamic(bool _isDynamic);

    /**
     * @brief Sets the query flag.
     * @param _queryFlags value of the query flag.
     */
    MODULE_VIZ_SCENE3D_API void setQueryFlags(std::uint32_t _queryFlags);

    /**
     * @brief Sets if the reconstruction is managed or not.
     * @param _isReconstructionManaged use true if the reconstruction is managed.
     */
    MODULE_VIZ_SCENE3D_API void setIsReconstructionManaged(bool _isReconstructionManaged);

    /// Flags the r2vb objects as dirty and asks the render service to update.
    MODULE_VIZ_SCENE3D_API void requestRender() override;

    /**
     * @brief Sets the mesh visibility.
     * @param _visible the visibility status of the mesh.
     */
    MODULE_VIZ_SCENE3D_API void setVisible(bool _visible) override;

protected:

    /// Configures the adaptor.
    MODULE_VIZ_SCENE3D_API void configuring() override;

    /// Creates a Mesh in the Default Ogre resource group.
    MODULE_VIZ_SCENE3D_API void starting() override;

    /**
     * @brief Proposals to connect service slots to associated object signals.
     * @return A map of each proposed connection.
     *
     * Connect data::Mesh::s_VERTEX_MODIFIED_SIG to s_MODIFY_VERTICES_SLOT
     * Connect data::Mesh::s_POINT_COLORS_MODIFIED_SIG to s_MODIFY_COLORS_SLOT
     * Connect data::Mesh::s_CELL_COLORS_MODIFIED_SIG to s_MODIFY_COLORS_SLOT
     * Connect data::Mesh::s_POINT_TEX_COORDS_MODIFIED_SIG to s_MODIFY_POINT_TEX_COORDS_SLOT
     * Connect data::Mesh::s_MODIFIED_SIG to IService::slots::s_UPDATE
     */
    MODULE_VIZ_SCENE3D_API service::IService::KeyConnectionsMap getAutoConnections() const override;

    /// Deletes the mesh after unregistering the service, and shutting connections.
    MODULE_VIZ_SCENE3D_API void stopping() override;

    /// Updates the mesh.
    MODULE_VIZ_SCENE3D_API void updating() override;

private:

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
    void updateMesh(data::Mesh::csptr _mesh);

    /**
     * @brief Instantiates a new material adaptor
     * @param _materialSuffix used for the material name.
     * @param _mesh used to create an unique material name.
     */
    module::viz::scene3d::adaptor::SMaterial::sptr createMaterialService(
        data::Mesh::csptr _mesh,
        const std::string& _materialSuffix = ""
    );

    /**
     * @brief Associates a new SMaterial to the managed SMesh.
     * With this method, SMesh is responsible for creating a SMaterial.
     * @param _mesh used to create the material service.
     */
    void updateNewMaterialAdaptor(data::Mesh::csptr _mesh);

    /// Updates the associated material adaptor.
    /// This method is called when a material adaptor has been configured in the XML scene.
    void updateXMLMaterialAdaptor();

    /**
     * @brief Attaches a node in the scene graph.
     * @param _node the node to attach.
     */
    void attachNode(Ogre::MovableObject* _node);

    /// Defines whether the camera must be auto reset when a mesh is updated or not.
    bool m_autoResetCamera {true};

    /// Contains the node in the scene graph where the mesh is attached.
    Ogre::Entity* m_entity {nullptr};

    /// Contains the Ogre material adaptor.
    module::viz::scene3d::adaptor::SMaterial::sptr m_materialAdaptor {nullptr};

    /// Defines the attached material name (when configured by XML).
    std::string m_materialName;

    /// Contains the material data.
    data::Material::sptr m_material {nullptr};

    /// Defines the attached material's name.
    std::string m_materialTemplateName {sight::viz::scene3d::Material::DEFAULT_MATERIAL_TEMPLATE_NAME};

    /// Defines the attached texture adaptor name.
    std::string m_textureName;

    /// Defines if the mesh adaptor is managed by a reconstruction adaptor.
    bool m_isReconstructionManaged {false};

    /// Defines if the mesh adaptor has to create a new material adaptor or simply use the one that is XML configured.
    bool m_useNewMaterialAdaptor {false};

    /// Defines the configured shading mode.
    std::string m_shadingMode;

    /// Defines if the mesh changes dynamically, defined in m_configuration.
    bool m_isDynamic {false};

    /// Defines if the vertices change dynamically, defined in m_configuration.
    bool m_isDynamicVertices {false};

    /// Ogre mesh.
    sight::viz::scene3d::Mesh::sptr m_meshGeometry {nullptr};

    /// Stores material adaptors attached to the r2vb objects.
    std::map<data::Mesh::cell_t, module::viz::scene3d::adaptor::SMaterial::sptr> m_r2vbMaterialAdaptor;

    /// Defines the mask used for picking request.
    std::uint32_t m_queryFlags {Ogre::SceneManager::ENTITY_TYPE_MASK};

    static constexpr std::string_view s_MESH_IN = "mesh";
    data::ptr<data::Mesh, data::Access::in> m_mesh {this, s_MESH_IN, true};
};

//------------------------------------------------------------------------------

inline data::Material::sptr SMesh::getMaterial() const
{
    return m_material;
}

//------------------------------------------------------------------------------

inline void SMesh::setMaterial(data::Material::sptr _material)
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

inline Ogre::Entity* SMesh::getEntity() const
{
    return m_entity;
}

//------------------------------------------------------------------------------

inline bool SMesh::getVisibility() const
{
    return m_entity != nullptr ? m_entity->getVisible() : m_isVisible;
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

} // namespace sight::module::viz::scene3d::adaptor.
