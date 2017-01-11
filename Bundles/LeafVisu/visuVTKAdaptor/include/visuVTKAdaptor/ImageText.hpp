/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __VISUVTKADAPTOR_IMAGETEXT_HPP__
#define __VISUVTKADAPTOR_IMAGETEXT_HPP__

#include "visuVTKAdaptor/Text.hpp"
#include "visuVTKAdaptor/config.hpp"

#include <fwDataTools/helper/MedicalImageAdaptor.hpp>

#include <fwRenderVTK/IVtkAdaptorService.hpp>

#include <string>

class vtkTextActor;

namespace visuVTKAdaptor
{

/**
 * @brief Adaptor to display window image information (width, heigth, picked
 * position)
 */
class VISUVTKADAPTOR_CLASS_API ImageText : public Text,
                                           public ::fwDataTools::helper::MedicalImageAdaptor
{

public:

    fwCoreServiceClassDefinitionsMacro ( (ImageText)(::fwRenderVTK::IVtkAdaptorService) );

    VISUVTKADAPTOR_API ImageText() throw();

    VISUVTKADAPTOR_API virtual ~ImageText() throw();

    /**
     * @brief Returns proposals to connect service slots to associated object signals,
     * this method is used for obj/srv auto connection
     *
     * Connect Image::s_MODIFIED_SIG to this::s_UPDATE_SLOT
     * Connect Image::s_SLICE_INDEX_MODIFIED_SIG to this::s_UPDATE_SLICE_INDEX_SLOT
     * Connect Image::s_BUFFER_MODIFIED_SIG to this::s_UPDATE_BUFFER_SLOT
     */
    VISUVTKADAPTOR_API virtual KeyConnectionsType getObjSrvConnections() const;

protected:

    VISUVTKADAPTOR_API void doStart() throw(fwTools::Failed);

    VISUVTKADAPTOR_API void doStop() throw(fwTools::Failed);

    VISUVTKADAPTOR_API void doUpdate() throw(fwTools::Failed);

    VISUVTKADAPTOR_API void doSwap() throw(fwTools::Failed);

    VISUVTKADAPTOR_API void doConfigure() throw(fwTools::Failed);

    /// Update tranfer function points
    VISUVTKADAPTOR_API virtual void updatingTFPoints();

    /// Update transfer function windowing
    VISUVTKADAPTOR_API virtual void updatingTFWindowing(double window, double level);

    /**
     * @name Slots
     * @{
     */
    /// Slot: update image slice index
    void updateSliceIndex(int axial, int frontal, int sagittal);
    /**
     * @}
     */
};

} //namespace visuVTKAdaptor

#endif //__VISUVTKADAPTOR_IMAGETEXT_HPP__
