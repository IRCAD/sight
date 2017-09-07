/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __VISUVTKADAPTOR_SIMAGESERIES_HPP__
#define __VISUVTKADAPTOR_SIMAGESERIES_HPP__

#include "visuVTKAdaptor/config.hpp"
#include "visuVTKAdaptor/SNegatoMPR.hpp"

#include <fwDataTools/helper/MedicalImageAdaptor.hpp>

#include <fwRenderVTK/IAdaptor.hpp>

#include <vector>

namespace visuVTKAdaptor
{

/**
 * @brief This adaptor shows ImageSeries. Creates an adaptor for the image in the series.
 *
 * @section XML XML Configuration
 * @code{.xml}
   <service type="::visuVTKAdaptor::SImageSeries" autoConnect="yes">
       <inout key="image" uid="..." />
       <inout key="tf" uid="..." optional="yes" />
       <config renderer="default" picker="negatodefault" mode="2d" slices="1" sliceIndex="axial"
               transform="trf" tfalpha="yes" interpolation="off" vtkimagesource="imgSource" actorOpacity="1.0" />
   </service>
   @endcode
 * @subsection In-Out In-Out
 * - \b image [::fwData::Image]: image to display.
 * - \b tf [::fwData::TransferFunction] (optional): the current TransferFunction. If it is not defined, we use the
 *      image's default transferFunction (CT-GreyLevel). The transferFunction's signals are automatically connected to
 *      the slots 'updateTFPoints' and 'updateTFWindowing'.
 *
 * @subsection Configuration Configuration:
 * - \b config(mandatory) : contains the adaptor configuration
 *    - \b renderer (mandatory): defines the renderer to show the image
 *    - \b picker (mandatory): identifier of the picker
 *    - \b mode (optional, 2d or 3d): defines the scene mode. In 2d mode, the camera follow the negato in
 *    axial/frontal/sagital orientation. In 3d mode, the camera is automatically reset when the image is modified. If
 *    mode is not defined, the camera is free.
 *    - \b slices (optional, default=3): number of slices shown in the adaptor
 *    - \b sliceIndex (optional, axial/frontal/sagittal, default=axial): orientation of the negato
 *    - \b transform (optional): the vtkTransform to associate to the adaptor
 *    - \b tfalpha (optional, yes/no, default=no): if true, the opacity of the transfer function is used in the negato.
 *    - \b interpolation (optional, yes/no, default=yes): if true, the image pixels are interpolated
 *    - \b vtkimagesource (optional): source image, used for blend
 *    - \b actorOpacity (optional, default=1.0): actor opacity (float)
 */
class VISUVTKADAPTOR_CLASS_API SImageSeries : public ::fwDataTools::helper::MedicalImageAdaptor,
                                              public ::fwRenderVTK::IAdaptor
{

public:

    fwCoreServiceClassDefinitionsMacro( (SImageSeries)(::fwRenderVTK::IAdaptor) );

    VISUVTKADAPTOR_API SImageSeries() noexcept;

    VISUVTKADAPTOR_API virtual ~SImageSeries() noexcept;

    //------------------------------------------------------------------------------

    void setAllowAlphaInTF(bool allow)
    {
        m_allowAlphaInTF = allow;
    }
    //------------------------------------------------------------------------------

    void setInterpolation(bool interpolation)
    {
        m_interpolation = interpolation;
    }
    //------------------------------------------------------------------------------

    void setVtkImageSourceId(std::string id)
    {
        m_imageSourceId = id;
    }

protected:

    /// Configure the adaptor.
    VISUVTKADAPTOR_API void configuring();

    /// Calls doUpdate()
    VISUVTKADAPTOR_API void starting();

    /// Creates and starts image adaptor. Redraw all (stop then restart sub services).
    VISUVTKADAPTOR_API void updating();

    /// Stops and unregister image subservice.
    VISUVTKADAPTOR_API void stopping();

    /// Sets adaptor slice mode (NO_SLICE, ONE_SLICE, THREE_SLICES)
    void setSliceMode(SNegatoMPR::SliceMode sliceMode);

    /// Gets adaptor slice mode (NO_SLICE, ONE_SLICE, THREE_SLICES)
    SNegatoMPR::SliceMode getSliceMode();

    /// Returns true if 3d mode is enabled, false if it is not and indeterminate if it is not defined
    ::boost::logic::tribool is3dModeEnabled();

    /// Defines 3D mode
    void set3dMode( bool enabled );

private:
    bool m_allowAlphaInTF;
    bool m_interpolation;

    std::string m_imageSourceId;

    ::boost::logic::tribool m_3dModeEnabled;
    SNegatoMPR::SliceMode m_sliceMode;

};

} //namespace visuVTKAdaptor

#endif // __VISUVTKADAPTOR_SIMAGESERIES_HPP__
