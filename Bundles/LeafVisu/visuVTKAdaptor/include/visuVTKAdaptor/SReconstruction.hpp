/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __VISUVTKADAPTOR_SRECONSTRUCTION_HPP__
#define __VISUVTKADAPTOR_SRECONSTRUCTION_HPP__

#include "visuVTKAdaptor/config.hpp"

#include <fwRenderVTK/IAdaptor.hpp>

namespace fwData
{
class Mesh;
}

namespace visuVTKAdaptor
{

/**
 * @brief Displays a Reconstruction
 *
 * @section Slots Slots
 * - \b updateVisibility(bool) : shows/hides the adaptor
 * - \b updateNormalMode(std::uint8_t) : updates the normal mode (0: hidden, 1: point normals, 2: cell normals)
 *
 * @section XML XML Configuration
 *
 * @code{.xml}
   <service type="::visuVTKAdaptor::SReconstruction">
       <in key="reconstruction" uid="..." />
       <config renderer="default" transform="..." picker="" />
   </service>
   @endcode
 * @subsection Input Input
 * - \b reconstruction [::fwData::Reconstruction]: reconstruction to display.
 * @subsection Configuration Configuration
 * - \b config(mandatory) : contains the adaptor configuration
 *    - \b renderer(mandatory) : renderer where the reconstruction is displayed
 *    - \b picker(optional) : picker used to pick on the reconstruction
 *    - \b autoresetcamera : reset the camera point of view when the mesh is modified ("yes" or "no", default: "yes") .
 */
class VISUVTKADAPTOR_CLASS_API SReconstruction : public ::fwRenderVTK::IAdaptor
{

public:

    fwCoreServiceClassDefinitionsMacro( (SReconstruction)(::fwRenderVTK::IAdaptor) );

    VISUVTKADAPTOR_API SReconstruction() noexcept;

    VISUVTKADAPTOR_API virtual ~SReconstruction() noexcept;

    static const ::fwServices::IService::KeyType s_RECONSTRUCTION_INPUT;

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
    VISUVTKADAPTOR_API virtual KeyConnectionsMap getAutoConnections() const;

protected:

    VISUVTKADAPTOR_API void configuring();
    VISUVTKADAPTOR_API void starting();
    VISUVTKADAPTOR_API void updating();
    VISUVTKADAPTOR_API void stopping();

    void createMeshService();

    ::fwRenderVTK::SRender::VtkObjectIdType m_clippingPlanesId;

    ::fwRenderVTK::IAdaptor::wptr m_meshService;

    double m_sharpEdgeAngle;

private:

    /**
     * @name Slots methods
     * @{
     */

    /// Slot: update visibility
    void updateVisibility(bool visible);
    /**
     * @}
     */

    bool m_autoResetCamera;

};

} //namespace visuVTKAdaptor

#endif // __VISUVTKADAPTOR_SRECONSTRUCTION_HPP__
