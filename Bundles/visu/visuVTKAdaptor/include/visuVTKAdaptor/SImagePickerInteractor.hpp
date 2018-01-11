/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __VISUVTKADAPTOR_SIMAGEPICKERINTERACTOR_HPP__
#define __VISUVTKADAPTOR_SIMAGEPICKERINTERACTOR_HPP__

#include "visuVTKAdaptor/config.hpp"
#include "visuVTKAdaptor/SPickerInteractor.hpp"

#include <fwCom/Signal.hpp>
#include <fwCom/Signals.hpp>

#include <fwDataTools/helper/MedicalImageAdaptor.hpp>
#include <fwDataTools/PickingInfo.hpp>

#include <fwRenderVTK/IAdaptor.hpp>

#include <vtkCommand.h>

#include <vector>

namespace visuVTKAdaptor
{

/**
 * @brief This service emits a signal when the user click on the associated image in the scene
 *
 * @section Signals Signals
 * - \b picked(::fwDataTools::PickingInfo) : emitted when the user clicks in the scene, the picking info contains the
 *      coordinates in the image space
 *
 * @section XML XML Configuration
 *
 * @code{.xml}
   <service type="::visuVTKAdaptor::SPickerInteractor" autoConnect="yes">
        <inout key=""image" uid="..." />
        <config renderer="default" picker="myPicker" event="MOUSE_RIGHT_UP" />
   </service>
   @endcode
 * @subsection In-Out In-Out
 * - \b image [::fwData::Image]: image to pick.
 *
 * @subsection Configuration Configuration
 * - \b renderer : the identifier of the renderer.
 * - \b picker : the identifier of the picker.
 * - \b event : the identifier(s) of the event on which the adaptor is picking.
 *   Possible values are:
 *   - MOUSE_LEFT_UP
 *   - MOUSE_RIGHT_UP
 *   - MOUSE_MIDDLE_UP
 *   - MOUSE_WHEELFORWARD
 *   - MOUSE_LEFT_DOWN
 *   - MOUSE_RIGHT_DOWN
 *   - MOUSE_MIDDLE_DOWN
 *   - MOUSE_WHEELBACKWARD
 *   - MOUSE_MOVE
 */
class VISUVTKADAPTOR_CLASS_API SImagePickerInteractor : public ::fwDataTools::helper::MedicalImageAdaptor,
                                                        public ::visuVTKAdaptor::SPickerInteractor
{

public:

    fwCoreServiceClassDefinitionsMacro( (SImagePickerInteractor)(::fwRenderVTK::IAdaptor) );

    VISUVTKADAPTOR_API SImagePickerInteractor() noexcept;

    VISUVTKADAPTOR_API virtual ~SImagePickerInteractor() noexcept;

protected:
    friend class ImagePickerInteractorCallback;

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
     * Connect Image::s_BUFFER_MODIFIED_SIG to this::s_UPDATE_SLOT
     */
    VISUVTKADAPTOR_API virtual KeyConnectionsMap getAutoConnections() const override;

private:
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

#endif // __VISUVTKADAPTOR_SIMAGEPICKERINTERACTOR_HPP__
