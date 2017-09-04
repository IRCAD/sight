/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2014-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __VISUVTKARADAPTOR_SVIDEO_HPP__
#define __VISUVTKARADAPTOR_SVIDEO_HPP__

#include "visuVTKARAdaptor/config.hpp"

#include <fwDataTools/helper/MedicalImageAdaptor.hpp>

#include <fwRenderVTK/IAdaptor.hpp>

#include <vtkLookupTable.h>
#include <vtkSmartPointer.h>

class vtkImageData;
class vtkImageActor;

namespace visuVTKARAdaptor
{

/**
 * @brief   Adaptor rendering a video frame from a 2D-image.
 *
 * @section Slots Slots
 * - \b updateImage() : update the image content.
 * - \b updateImageOpacity() : update the opacity of the frame.
 * - \b show(bool) : show or hide the frame.
 * - \b calibrate() : call if the calibration of the camera has changed.
 * - \b updateTF() : update the displayed transfer function

 * @section XML XML Configuration
 *
 * @code{.xml}
        <service type="::visuVTKARAdaptor::SVideo" autoConnect="yes" >
            <in key="frame" uid="..." />
            <in key="camera" uid="..." />
            <in key="tf" uid="..." optional="yes" />
            <config renderer="default" reversed="true" selectedTFKey="..." />
        </service>
   @endcode
 * @subsection Input Input
 * - \b frame [::fwData::Image]: frame displayed.
 * - \b camera [::arData::Camera] (optional): camera calibration, recenters the video using the (cx, cy) offsets.
 * - \b tf [::fwData::TransferFunction] (optional): a transfer function that can be applied to the video.
 *
 * @subsection Configuration Configuration
 * - \b renderer : ID of the renderer the adaptor must use
 * - \b reverse (optional)(default: true) : if true, the actor is rotated by 180° along the z and y axis.
 */
class VISUVTKARADAPTOR_CLASS_API SVideo : public ::fwRenderVTK::IAdaptor
{

public:
    fwCoreServiceClassDefinitionsMacro( (SVideo)(::fwRenderVTK::IAdaptor) );

    /// Constructor
    SVideo() noexcept;

    /// Destructor
    virtual ~SVideo() noexcept;

    /**
     * @brief Returns proposals to connect service slots to associated object signals,
     * this method is used for obj/srv auto connection
     *
     * Connect Image::s_MODIFIED_SIG to this::s_UPDATE_IMAGE_SLOT
     * Connect Image::s_VISIBILITY_MODIFIED_SIG to this::s_UPDATE_IMAGE_OPACITY_SLOT
     * Connect Image::s_TRANSPARENCY_MODIFIED_SIG to this::s_UPDATE_IMAGE_OPACITY_SLOT
     * Connect Image::s_BUFFER_MODIFIED_SIG to this::s_UPDATE_SLOT
     *
     * Connect Camera::s_MODIFIED_SIG to this::s_CALIBRATE_SLOT
     * Connect Camera::s_INTRINSIC_CALIBRATED_SIG to this::s_CALIBRATE_SLOT
     *
     * Connect TransferFunction::s_POINTS_MODIFIED_SIG to this::s_UPDATE_TF_SLOT
     * Connect TransferFunction::s_WINDOWING_MODIFIED_SIG to this::s_UPDATE_TF_SLOT
     */
    VISUVTKARADAPTOR_API virtual KeyConnectionsMap getAutoConnections() const override;

protected:

    /// Create the actor and mapper used to show the video frame.
    VISUVTKARADAPTOR_API void starting() override;

    /// Configure the adaptor.
    VISUVTKARADAPTOR_API void configuring() override;

    /// Updated the frame from the current Image.
    VISUVTKARADAPTOR_API void updating() override;

    /// Removes the actor from the renderer
    VISUVTKARADAPTOR_API void stopping() override;

    /// Selects the current tf
    VISUVTKARADAPTOR_API void swapping(const KeyType& key) override;

private:

    /// Slot: Update image opacity and visibility
    void updateImageOpacity();

    /// Slot: update image
    void updateImage();

    /// Slot: Updates the displayed transfer function
    void updateTF();

    /// Slot: set the visibility  of the image
    void show(bool visible);

    /// Slot: apply the optical center offset to our video plane
    void offsetOpticalCenter();

    vtkImageData* m_imageData; ///< vtk image created from current data Image. It is shown in the frame.
    vtkImageActor* m_actor;   ///< actor to show frame
    vtkSmartPointer<vtkLookupTable> m_lookupTable; ///< Vtk LUT representing the fw4spl TF

    bool m_isTextureInit; /// true if the texture is initialized

    bool m_reverse; ///< if true, the actor is rotated by 180° along the z and y axis.
};

} //namespace visuVTKARAdaptor

#endif /* __VISUVTKARADAPTOR_SVIDEO_HPP__ */
