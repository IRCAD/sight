/************************************************************************
 *
 * Copyright (C) 2020 IRCAD France
 * Copyright (C) 2020 IHU Strasbourg
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

#include "depthSegmentation/config.hpp"

#include <fwServices/IOperator.hpp>

#include <opencv2/core.hpp>
#include <opencv2/opencv.hpp>

namespace depthSegmentation
{

/**
 * @brief   Service that learns a foreground and background color model and allow to segment the foreground on a new
 * image using an Eexpectation Maximization algorithm
 *
 * @see ::fwServices::IOperator
 *
 * @section Slots Slots
 * - \b todo() : Slot todo
 *
 * @section XML XML Configuration
 *
 * @code{.xml}
        <service uid="..." type="::depthSegmentation::SDepthImageMasking" >
            <in key="videoTL" uid="..." autoConnect="yes" />
            <inout key="mask" uid="..." />
            <inout key="videoMaskTL" uid="..." />
        </service>
   @endcode
 *
 * @subsection Input Input
 * - \b todo [::todo::todo] : todo
 *
 * @subsection In-Out In-Out
 * - \b todo [::todo::todo] : todo
 *
 * @subsection Configuration Configuration
 * - \b todo (optional)(default: 1.0) : todo
 */
class DEPTHSEGMENTATION_CLASS_API SDepthImageMasking : public ::fwServices::IOperator
{
public:
    fwCoreServiceMacro(SDepthImageMasking, ::fwServices::IOperator)

    ///Constructor
    DEPTHSEGMENTATION_CLASS_API SDepthImageMasking() noexcept;

    /// Destructor
    DEPTHSEGMENTATION_CLASS_API virtual ~SDepthImageMasking() noexcept;

    /// Defines auto connection for this service (update slot) to the frame timeline (objectPushed)
    ::fwServices::IService::KeyConnectionsMap getAutoConnections() const override;

protected:

    /// todo
    DEPTHSEGMENTATION_CLASS_API virtual void configuring() override;

    /// todo
    DEPTHSEGMENTATION_CLASS_API virtual void starting() override;

    /// todo
    DEPTHSEGMENTATION_CLASS_API virtual void stopping() override;

    /// todo
    DEPTHSEGMENTATION_CLASS_API virtual void updating() override;

private:

    void create_mask_from_depth(::cv::Mat& _depth, int _thresh, ::cv::ThresholdTypes _type);

    ::cv::Mat m_elemLess;
    ::cv::Mat m_elemMore;
};

} // namespace colourSegmentation
