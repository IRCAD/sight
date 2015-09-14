/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2014-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __VISUOGREADAPTOR_SMESH_HPP__
#define __VISUOGREADAPTOR_SMESH_HPP__

#include <fwRenderOgre/IAdaptor.hpp>
#include <fwRenderOgre/ITransformable.hpp>

#include <fwCom/Signal.hpp>
#include <fwCom/Slot.hpp>

#include <fwData/Material.hpp>
#include <fwData/Mesh.hpp>

#include <Ogre.h>

#include "visuOgreAdaptor/config.hpp"
#include "visuOgreAdaptor/SMaterial.hpp"
#include "visuOgreAdaptor/STransform.hpp"

namespace fwData
{
class Mesh;
class Material;
}

namespace visuOgreAdaptor
{

class VISUOGREADAPTOR_CLASS_API SMesh : public ::fwRenderOgre::IAdaptor,
                                        public ::fwRenderOgre::ITransformable
{
public:
    fwCoreServiceClassDefinitionsMacro((SMesh)(::fwRenderOgre::IAdaptor));

    /// Constructor: Sets default parameters and initializes necessary members.
    VISUOGREADAPTOR_API SMesh() throw();
    /// Destructor: if an entity exists in the Ogre Scene, asks Ogre to destroy it.
    VISUOGREADAPTOR_API virtual ~SMesh() throw();

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

    VISUOGREADAPTOR_API void updateOptionsMode();

    /// This method is called by a reconstruction adaptor after creating the mesh adaptor
    VISUOGREADAPTOR_API void setIsReconstructionManaged(bool _isReconstructionManaged);

    /// Returns proposals to connect service slots to associated object signals
    ::fwServices::IService::KeyConnectionsType getObjSrvConnections() const;

private:

    /**
     * @brief Configures the adaptor
     * @verbatim
       <adaptor id="meshAdaptor" class="::visuOgreAdaptor::SMesh" objectId="meshKey">
        <config renderer="rendererId" transform="transformUID" materialAdaptor="materialName" textureAdaptor="texAdaptorUID" />
       </adaptor>
       @endverbatim
     * With :
     *  - \b renderer (mandatory) : defines the mesh's layer
     *  - \b transform (optional) : the transformation matrix to associate to the adaptor
     * Either of the following (whether a material is configured in the XML scene or not) :
     *  - \b materialAdaptor (optional) : the name of the associated material adaptor
     *  - \b materialTemplate (optional) : the name of the base Ogre material for the created SMaterial
     * Only if there is no material adaptor configured in the XML scene (in this case, it has to retrieve the texture
     * adaptor) :
     *  - \b textureAdaptor (optional) : the texture adaptor that the material will be listening to
     */
    void doConfigure() throw(fwTools::Failed);
    /// Manually creates a Mesh in the Default Ogre Ressource group
    void doStart    () throw(fwTools::Failed);
    /// Deletes the mesh after unregistering the service, and shutting connections.
    void doStop     () throw(fwTools::Failed);
    /// Performs a Stop -> Start
    void doSwap     () throw(fwTools::Failed);
    /// Checks if the fwData::Mesh has changed, and updates it if it has.
    void doUpdate   () throw(fwTools::Failed);

    ::Ogre::Entity* newEntity();

    /// Updates the Mesh, checks if color, number of vertices have changed, and updates them.
    void updateMesh(::fwData::Mesh::sptr mesh);
    /// Updates the Mesh, checks if vertices position have changed, and updates them.
    void updateVertices(::fwData::Mesh::sptr mesh);

    /// Associates a new SMaterial to the managed SMesh.
    /// With this method, SMesh is responsible for creating a SMaterial
    void updateNewMaterialAdaptor();
    /// Updates the associated material adaptor.
    /// This method is called when a material adaptor has been configured in the XML scene
    void updateXMLMaterialAdaptor();

    /// Creates a transform Service, and attaches it to a corresponding sceneNode in the Ogre scene.
    void createTransformService();
    /// creates a Normals service
    void createNormalsService();
    /// Unregisters Normals service
    void removeNormalsService();

    /**
     * @name Slots methods
     * @{
     */
    /// Slot called when the mesh is modified
    void modifyMesh();
    /// Slot called when the point colors are modified
    void modifyPointColors();
    /// Slot called when the vertices are modified
    void modifyVertices();
    /// Slot called when the material is modified
    void modifyMaterial();
    /** @} */

    /// Sets whether the camera must be auto reset when a mesh is updated or not.
    bool m_autoResetCamera;

    /// Node in the scene graph
    ::Ogre::Entity* m_entity;
    /// Actual mesh data
    ::Ogre::MeshPtr m_ogreMesh;

    /// SMaterial attached to the Mesh
    ::visuOgreAdaptor::SMaterial::sptr m_materialAdaptor;
    /// Attached material adaptor UID
    std::string m_materialAdaptorUID;
    /// Ogre Material related to the mesh
    ::fwData::Material::sptr m_material;
    /// Attached Material's name
    std::string m_materialTemplateName;

    /// Pointer to the normals service.
    ::fwRenderOgre::IAdaptor::wptr m_normalsService;

    /// Number of primitives types that are handled by ::fwData::Mesh
    static const unsigned int s_numPrimitiveTypes = ::fwData::Mesh::TETRA + 1;

    /// Pointers on submeshes needed for reallocation check
    ::Ogre::SubMesh* m_subMeshes[s_numPrimitiveTypes];

    /// Enum containing the different types of UvGen handled by the service.
    enum UvGenType
    {
        NONE,
        SPHERE,
        CYLINDER,
        PLANE,
    };

    /// Mesh's name in Ogre
    std::string m_meshName;
    /// Stores the enum's value corresponding to this service.
    UvGenType m_uvgen;
    /// Attached texture adaptor UID
    std::string m_texAdaptorUID;

    /// Defines if there is a normal layer
    bool m_hasNormal;
    /// Defines if there is color layer
    bool m_hasColor;
    /// defines if the mesh changes dynamically, defined in m_configuration
    bool m_isDynamic;
    /// defines if the vertices change dynamically, defined in m_configuration
    bool m_isDynamicVertices;
    /// defines if the mesh has UV coordinates, defined in m_configuration
    bool m_hasUV;
    /// Defines if the mesh is supposed to be used to display a video, no by default.
    bool m_isVideo;
    /// Which buffer is the UV buffer. modified according to m_hascolor.
    unsigned short m_UVBinding;
    /// Indicates if the mesh adaptor is managed by a reconstruction adaptor
    bool m_isReconstructionManaged;
    /// Indicates if the mesh adaptor has to create a new material adaptor or simply use the one that is XML configured
    bool m_useNewMaterialAdaptor;
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

inline void SMesh::updateVisibility(bool isVisible)
{
    if(m_entity)
    {
        m_entity->setVisible(isVisible);
        this->requestRender();
    }
}

//------------------------------------------------------------------------------

inline bool SMesh::getVisibility() const
{
    return m_entity ? m_entity->getVisible() : false;
}

//------------------------------------------------------------------------------

inline void SMesh::setDynamic(bool _isDynamic)
{
    m_isDynamic = _isDynamic;
}

//------------------------------------------------------------------------------

inline void SMesh::setDynamicVertices(bool _isDynamic)
{
    m_isDynamicVertices = _isDynamic;
}

//-----------------------------------------------------------------------------

inline void SMesh::setIsReconstructionManaged(bool _isReconstructionManaged)
{
    m_isReconstructionManaged = _isReconstructionManaged;
}

//------------------------------------------------------------------------------

} //namespace visuOgreAdaptor

#endif // __VISUOGREADAPTOR_SMESH_HPP__
