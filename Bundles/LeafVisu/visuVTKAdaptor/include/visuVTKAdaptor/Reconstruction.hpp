/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __VISUVTKADAPTOR_RECONSTRUCTION_HPP__
#define __VISUVTKADAPTOR_RECONSTRUCTION_HPP__

#include "visuVTKAdaptor/config.hpp"
#include "visuVTKAdaptor/MeshFactory.hpp"

#include <fwRenderVTK/IVtkAdaptorService.hpp>

namespace fwData
{
class Mesh;
}

namespace visuVTKAdaptor
{

class VISUVTKADAPTOR_CLASS_API Reconstruction : public ::fwRenderVTK::IVtkAdaptorService
{

public:

    fwCoreServiceClassDefinitionsMacro( (Reconstruction)(::fwRenderVTK::IVtkAdaptorService) );

    VISUVTKADAPTOR_API Reconstruction() throw();

    VISUVTKADAPTOR_API virtual ~Reconstruction() throw();

    //------------------------------------------------------------------------------

    void setClippingPlanes(::fwRenderVTK::SRender::VtkObjectIdType id)
    {
        m_clippingPlanesId = id;
    }

    //------------------------------------------------------------------------------

    void setSharpEdgeAngle(double angle)
    {
        m_sharpEdgeAngle = angle;
    }

    VISUVTKADAPTOR_API void setForceHide(bool hide);

    /// Active/Inactive automatic reset on camera for triangular mesh adaptor. By default =true.
    VISUVTKADAPTOR_API void setAutoResetCamera(bool autoResetCamera);

    /// Retrieve a weak_ptr on underlying mesh adaptor service
    ::fwRenderVTK::IVtkAdaptorService::wptr getMeshService() const
    {
        return m_meshService;
    }

    /**
     * @name Slots API
     * @{
     */
    /// slot used to update mesh
    VISUVTKADAPTOR_API static const ::fwCom::Slots::SlotKeyType s_UPDATE_MESH_SLOT;

    /// slot used to update visibility
    VISUVTKADAPTOR_API static const ::fwCom::Slots::SlotKeyType s_UPDATE_VISIBILITY_SLOT;

    /// slot used to update normal mode (0: none, 1: point, 2: cell)
    VISUVTKADAPTOR_API static const ::fwCom::Slots::SlotKeyType s_UPDATE_NORMAL_MODE_SLOT;

    /// Slot: used to update normal display(0: none, 1: point, 2: cell)
    void updateNormalMode(std::uint8_t mode);
    /**
     * @}
     */

    /**
     * @brief Returns proposals to connect service slots to associated object signals,
     * this method is used for obj/srv auto connection
     *
     * Connect Reconstruction::s_MODIFIED_SIG to this::s_UPDATE_SLOT
     * Connect Reconstruction::s_MESH_CHANGED_SIG to this::s_UPDATE_MESH_SLOT
     * Connect Reconstruction::s_VISIBILITY_MODIFIED_SIG to this::s_UPDATE_VISIBILITY_SLOT
     */
    VISUVTKADAPTOR_API virtual KeyConnectionsType getObjSrvConnections() const;

protected:

    VISUVTKADAPTOR_API void doStart() throw(fwTools::Failed);
    VISUVTKADAPTOR_API void doStop() throw(fwTools::Failed);
    VISUVTKADAPTOR_API void doConfigure() throw(fwTools::Failed);
    VISUVTKADAPTOR_API void doSwap() throw(fwTools::Failed);
    VISUVTKADAPTOR_API void doUpdate() throw(fwTools::Failed);

    void createMeshService();

    ::fwRenderVTK::SRender::VtkObjectIdType m_clippingPlanesId;

    ::fwRenderVTK::IVtkAdaptorService::wptr m_meshService;

    double m_sharpEdgeAngle;

    /**
     * @name Slots methods
     * @{
     */

    /// Slot: update mesh
    void updateMesh(SPTR(::fwData::Mesh));

    /// Slot: update visibility
    void updateVisibility(bool visible);

    /**
     * @}
     */

private:
    bool m_autoResetCamera;

};

} //namespace visuVTKAdaptor

#endif // __VISUVTKADAPTOR_RECONSTRUCTION_HPP__
