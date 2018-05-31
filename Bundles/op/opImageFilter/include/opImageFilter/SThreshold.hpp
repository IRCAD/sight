/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2018.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#pragma once

#include "opImageFilter/config.hpp"

#include <fwServices/IOperator.hpp>

namespace opImageFilter
{

/**
 * @brief   Apply a threshold on an image.
 *
 * This Service needs two image: the source and the target image.
 *
 * @section XML XML Configuration
 *
 * @code{.xml}
        <service type="::opImageFilter::SThreshold">
            <in key="source" uid="..." />
            <out key="target" uid="..." />
            <config>
                <threshold>50</threshold>
            </config>
       </service>
   @endcode
 *
 * @subsection Input Input:
 * - \b source [::fwData::Object]: Source image, can be a ::fwMedData::ImageSeries or a ::fwData::Image.
 * @subsection Output Output:
 * - \b target [::fwData::Object]: Target image (ie the filtered image),
 * if the type of the source is a ::fwMedData::ImageSeries then target will be a ::fwMedData::ImageSeries,
 * otherwise it will be an simple ::fwData::Image.
 * @subsection Configuration Configuration
 * - \b threshold : Specify the desired threshold used in filter
 * (Pixels with value lower than this value will be set to 0,
 * otherwise it will be set to the maximum value of the pixel type of the image.).
 */
class OPIMAGEFILTER_CLASS_API SThreshold : public ::fwServices::IOperator
{

public:

    fwCoreServiceClassDefinitionsMacro( (SThreshold)(::fwServices::IOperator) );

    OPIMAGEFILTER_API SThreshold() noexcept;

    OPIMAGEFILTER_API virtual ~SThreshold() noexcept override;

protected:

    OPIMAGEFILTER_API void starting() override;

    OPIMAGEFILTER_API void stopping() override;

    /// Configure the service.
    OPIMAGEFILTER_API void configuring() override;

    /// Apply the threshold.
    OPIMAGEFILTER_API void updating() override;

private:

    /// Threshold value used in filter
    double m_threshold;
};

} // namespace opImageFilter
