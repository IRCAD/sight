/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2018.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#pragma once

#include "visuVTKAdaptor/config.hpp"

#include <fwDataTools/helper/MedicalImage.hpp>

#include <fwRenderVTK/IAdaptor.hpp>

class vtkCamera;

namespace visuVTKAdaptor
{

/**
 * @brief Update the camera according to slice move.
 *
 * @section Slots Slots
 * - \b updateSliceType(int from, int to): update image slice type
 * - \b updateSliceIndex(int axial, int frontal, int sagittal) : update image slice index
 *
 * @section XML XML Configuration
 *
 * @code{.xml}
   <service type="::visuVTKAdaptor::SSliceFollowerCamera" autoConnect="yes">
       <inout key="image" uid="..." />
       <config renderer="default" sliceIndex="axial" />
   </service>
   @endcode
 * @subsection In-Out In-Out
 * - \b image [::fwData::Image]: image to display.
 *
 * @subsection Configuration Configuration:
 * - \b config(mandatory) : contains the adaptor configuration
 *    - \b renderer (mandatory): defines the renderer to show the image
 *    - \b sliceIndex (optional, axial/frontal/sagittal, default=axial): orientation of the negato
 */
class VISUVTKADAPTOR_CLASS_API SSliceFollowerCamera : public ::fwDataTools::helper::MedicalImage,
                                                      public ::fwRenderVTK::IAdaptor
{
public:
    fwCoreServiceClassDefinitionsMacro( (SSliceFollowerCamera)(::fwRenderVTK::IAdaptor) );

    VISUVTKADAPTOR_API SSliceFollowerCamera() noexcept;

    VISUVTKADAPTOR_API virtual ~SSliceFollowerCamera() noexcept;

protected:

    VISUVTKADAPTOR_API void configuring() override;
    VISUVTKADAPTOR_API void starting() override;
    VISUVTKADAPTOR_API void updating() override;
    VISUVTKADAPTOR_API void stopping() override;

    /**
     * @brief Returns proposals to connect service slots to associated object signals,
     * this method is used for obj/srv auto connection
     *
     * Connect Image::s_MODIFIED_SIG to this::s_UPDATE_SLOT
     * Connect Image::s_SLICE_INDEX_MODIFIED_SIG to this::s_UPDATE_SLICE_INDEX_SLOT
     * Connect Image::s_SLICE_TYPE_MODIFIED_SIG to this::s_UPDATE_SLICE_TYPE_SLOT
     * Connect Image::s_BUFFER_MODIFIED_SIG to this::s_UPDATE_SLOT
     */
    VISUVTKADAPTOR_API virtual KeyConnectionsMap getAutoConnections() const override;

    void initializeCamera();
    void updateCamera(double distance = -1., double size = -1);

    vtkCamera* m_camera;

private:
    /**
     * @name Slots
     * @{
     */
    /// Slot: update image slice index
    void updateSliceIndex(int axial, int frontal, int sagittal);

    /// Slot: update image slice type
    void updateSliceType(int from, int to);
    /**
     * @}
     */
};

} //namespace visuVTKAdaptor
