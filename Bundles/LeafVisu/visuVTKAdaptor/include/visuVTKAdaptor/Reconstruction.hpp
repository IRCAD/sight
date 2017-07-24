/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __VISUVTKADAPTOR_RECONSTRUCTION_HPP__
#define __VISUVTKADAPTOR_RECONSTRUCTION_HPP__

#include "visuVTKAdaptor/config.hpp"

#include <fwRenderVTK/IAdaptor.hpp>

namespace fwData
{
class Mesh;
}

namespace visuVTKAdaptor
{

class VISUVTKADAPTOR_CLASS_API Reconstruction : public ::fwRenderVTK::IAdaptor
{

public:

    fwCoreServiceClassDefinitionsMacro( (Reconstruction)(::fwRenderVTK::IAdaptor) );

    VISUVTKADAPTOR_API Reconstruction() noexcept;

    VISUVTKADAPTOR_API virtual ~Reconstruction() noexcept;

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
    ::fwRenderVTK::IAdaptor::wptr getMeshService() const
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

    VISUVTKADAPTOR_API void doStart();
    VISUVTKADAPTOR_API void doStop();
    VISUVTKADAPTOR_API void doConfigure();
    VISUVTKADAPTOR_API void doSwap();
    VISUVTKADAPTOR_API void doUpdate();

    void createMeshService();

    ::fwRenderVTK::SRender::VtkObjectIdType m_clippingPlanesId;

    ::fwRenderVTK::IAdaptor::wptr m_meshService;

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
