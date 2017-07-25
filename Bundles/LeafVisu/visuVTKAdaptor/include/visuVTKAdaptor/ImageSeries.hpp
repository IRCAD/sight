/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __VISUVTKADAPTOR_IMAGESERIES_HPP__
#define __VISUVTKADAPTOR_IMAGESERIES_HPP__

#include "visuVTKAdaptor/config.hpp"
#include "visuVTKAdaptor/SNegatoMPR.hpp"

#include <fwDataTools/helper/MedicalImageAdaptor.hpp>

#include <fwRenderVTK/IAdaptor.hpp>

#include <vector>

namespace visuVTKAdaptor
{

/**
 * @brief This adaptor shows ImageSeries. Creates an adaptor for the image in the series.
 */
class VISUVTKADAPTOR_CLASS_API ImageSeries : public ::fwDataTools::helper::MedicalImageAdaptor,
                                             public ::fwRenderVTK::IAdaptor
{

public:

    fwCoreServiceClassDefinitionsMacro( (ImageSeries)(::fwRenderVTK::IAdaptor) );

    VISUVTKADAPTOR_API ImageSeries() noexcept;

    VISUVTKADAPTOR_API virtual ~ImageSeries() noexcept;

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

    /// Calls doUpdate()
    VISUVTKADAPTOR_API void doStart();

    /// Configure the adaptor.
    VISUVTKADAPTOR_API void doConfigure();

    /// Calls doUpdate()
    VISUVTKADAPTOR_API void doSwap();

    /// Creates and starts image adaptor. Redraw all (stop then restart sub services).
    VISUVTKADAPTOR_API void doUpdate();

    /// Stops and unregister image subservice.
    VISUVTKADAPTOR_API void doStop();

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

#endif // __VISUVTKADAPTOR_IMAGESERIES_HPP__
