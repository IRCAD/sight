/************************************************************************
 *
 * Copyright (C) 2009-2019 IRCAD France
 * Copyright (C) 2012-2019 IHU Strasbourg
 *
 * This file is part of Sight.
 *
 * Sight is free software: you can redistribute it and/or modify it under
 * the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Sight is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with Sight. If not, see <https://www.gnu.org/licenses/>.
 *
 ***********************************************************************/

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

    fwCoreServiceMacro(SThreshold, ::fwServices::IOperator);

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
