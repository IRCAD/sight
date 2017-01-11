/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2014-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __VISUOGREADAPTOR_SRECONSTRUCTION_HPP__
#define __VISUOGREADAPTOR_SRECONSTRUCTION_HPP__

#include <fwCom/Slot.hpp>

#include <fwRenderOgre/IAdaptor.hpp>
#include <fwRenderOgre/ITransformable.hpp>

#include "visuOgreAdaptor/config.hpp"
#include "visuOgreAdaptor/SMesh.hpp"

namespace fwData
{
class Mesh;
}

namespace visuOgreAdaptor
{

class VISUOGREADAPTOR_CLASS_API SReconstruction : public ::fwRenderOgre::IAdaptor,
                                                  public ::fwRenderOgre::ITransformable
{

public:

    fwCoreServiceClassDefinitionsMacro ( (SReconstruction)(::fwRenderOgre::IAdaptor) );

    /// Constructor.
    VISUOGREADAPTOR_API SReconstruction() throw();

    /// Destructor. Does nothing
    VISUOGREADAPTOR_API virtual ~SReconstruction() throw();

    /**
     * @name Slots API
     * @{
     */
    typedef ::fwCom::Slot< void (SPTR( ::fwData::Mesh ) )> ChangeMeshSlotType;
    typedef ::fwCom::Slot< void () > VisibilitySlotType;

    VISUOGREADAPTOR_API static const ::fwCom::Slots::SlotKeyType s_CHANGE_MESH_SLOT;
    VISUOGREADAPTOR_API static const ::fwCom::Slots::SlotKeyType s_VISIBILITY_SLOT;
    /** @} */

    /// Forces the Reconstruction to be hidden or not.
    VISUOGREADAPTOR_API void setForceHide(bool _hide);

    /// Active/Inactive automatic reset on camera for triangular mesh adaptor. By default =true.
    VISUOGREADAPTOR_API void setAutoResetCamera(bool _autoResetCamera);
    /// Changes the material's name
    VISUOGREADAPTOR_API void setMaterialTemplateName(const std::string& _materialName);

    VISUOGREADAPTOR_API ::visuOgreAdaptor::SMesh::sptr getMeshAdaptor();

    /// Returns proposals to connect service slots to associated object signals
    ::fwServices::IService::KeyConnectionsType getObjSrvConnections() const;

    /// Set meshes vertex buffer to dynamic state (only has effect if called before service starting/update)
    VISUOGREADAPTOR_API void setDynamicVertices(bool _isDynamic);

    /// Set meshes and indices buffers to dynamic state (only has effect if called before service starting/update)
    VISUOGREADAPTOR_API void setDynamic(bool _isDynamic);

protected:
    /// starts the service; creates the mesh service.
    VISUOGREADAPTOR_API void doStart() throw(::fwTools::Failed);
    /// stops and unregisters the service.
    VISUOGREADAPTOR_API void doStop() throw(::fwTools::Failed);

    /**
     * @brief Configure the Reconstruction adaptor.
     * @code{.xml}
       <adaptor id="reconstructionAdaptorId" class="::visuOgreAdaptor::SReconstruction" objectId="objectId">
        <config transform="transform" autoresetcamera="autoresetcamera" />
       </adaptor>
       @endcode
     * With :
     *  - \b transform (mandatory) : the transformation matrix to associate to the adaptor
     *  - \b autoresetcamera (optional)
     */
    VISUOGREADAPTOR_API void doConfigure() throw(::fwTools::Failed);
    /// Updates the mesh adaptor according to the reconstruction or creates it if it hasn't been yet.
    VISUOGREADAPTOR_API void doUpdate() throw(::fwTools::Failed);
    /// Calls updating
    VISUOGREADAPTOR_API void doSwap() throw(::fwTools::Failed);

private:
    /// Changes the attached mesh
    void changeMesh( SPTR( ::fwData::Mesh) );
    /// modify if the
    void modifyVisibility();
    /// Creates the mesh service.
    void createMeshService();

    /// Pointer to the associated mesh service.
    ::fwRenderOgre::IAdaptor::wptr m_meshAdaptor;

    /// sets if the camera has to be reset automatically.
    bool m_autoResetCamera;

    /// Material's name
    std::string m_materialTemplateName;
    /// defines if the mesh changes dynamically
    bool m_isDynamic;
    /// defines if the vertices change dynamically
    bool m_isDynamicVertices;
};

//------------------------------------------------------------------------------
// Inline function(s)

inline void SReconstruction::setAutoResetCamera(bool _autoResetCamera)
{
    m_autoResetCamera = _autoResetCamera;
}

//------------------------------------------------------------------------------

inline void SReconstruction::setMaterialTemplateName(const std::string& _materialName)
{
    m_materialTemplateName = _materialName;
}

//------------------------------------------------------------------------------

inline void SReconstruction::setDynamic(bool _isDynamic)
{
    m_isDynamic = _isDynamic;
}

//------------------------------------------------------------------------------

inline void SReconstruction::setDynamicVertices(bool _isDynamic)
{
    m_isDynamicVertices = _isDynamic;
}

//------------------------------------------------------------------------------

} // namespace visuOgreAdaptor

#endif // __VISUOGREADAPTOR_SRECONSTRUCTION_HPP__
