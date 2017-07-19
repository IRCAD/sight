/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __VISUVTKADAPTOR_SLICEFOLLOWERCAMERA_HPP__
#define __VISUVTKADAPTOR_SLICEFOLLOWERCAMERA_HPP__

#include "visuVTKAdaptor/config.hpp"

#include <fwDataTools/helper/MedicalImageAdaptor.hpp>

#include <fwRenderVTK/IVtkAdaptorService.hpp>

#include <vector>

class vtkCamera;

namespace visuVTKAdaptor
{

/**
 * @brief Update the camera according to slice move.
 */
class VISUVTKADAPTOR_CLASS_API SliceFollowerCamera : public ::fwDataTools::helper::MedicalImageAdaptor,
                                                     public ::fwRenderVTK::IVtkAdaptorService
{
public:
    fwCoreServiceClassDefinitionsMacro ( (SliceFollowerCamera)(::fwRenderVTK::IVtkAdaptorService) );

    VISUVTKADAPTOR_API SliceFollowerCamera() noexcept;

    VISUVTKADAPTOR_API virtual ~SliceFollowerCamera() noexcept;

    /**
     * @brief Returns proposals to connect service slots to associated object signals,
     * this method is used for obj/srv auto connection
     *
     * Connect Image::s_MODIFIED_SIG to this::s_UPDATE_SLOT
     * Connect Image::s_SLICE_INDEX_MODIFIED_SIG to this::s_UPDATE_SLICE_INDEX_SLOT
     * Connect Image::s_SLICE_TYPE_MODIFIED_SIG to this::s_UPDATE_SLICE_TYPE_SLOT
     * Connect Image::s_BUFFER_MODIFIED_SIG to this::s_UPDATE_SLOT
     */
    VISUVTKADAPTOR_API virtual KeyConnectionsType getObjSrvConnections() const;

protected:

    VISUVTKADAPTOR_API void doStart();
    VISUVTKADAPTOR_API void doStop();

    VISUVTKADAPTOR_API void doConfigure();
    VISUVTKADAPTOR_API void doSwap();
    // redraw all (stop then restart sub services)
    VISUVTKADAPTOR_API void doUpdate();

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

#endif // __VISUVTKADAPTOR_SLICEFOLLOWERCAMERA_HPP__
