/************************************************************************
 *
 * Copyright (C) 2020-2021 IRCAD France
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

#include "modules/filter/vision/config.hpp"

#include <service/IOperator.hpp>

#include <opencv2/opencv.hpp>

namespace sight::module::filter::vision
{

/**
 * @brief   Service that learns a background depth image in a mask to perform a depth difference and output a foregroud
 * image.
 *
 * @see service::IOperator
 *
 * @section Slots Slots
 * - \b setBackground() : Slot to learn the background depth image.
 * - \b setThreshold() : Slot to change the difference value tolerance between the larned depth image and the current.
 *
 * @section XML XML Configuration
 *
 * @code{.xml}
    <service uid="..." type="sight::module::filter::vision::SDepthImageMasking" worker="...">
        <in key="maskImage" uid="..." />
        <in key="videoImage" uid="..." />
        <in key="depthImage" uid="..." autoConnect="yes" />
        <inout key="foregroundImage" uid="..." />
    </service>
   @endcode
 *
 * @subsection Input Input
 * - \b maskImage [sight::data::Image] : Mask image to perform the difference inside. Background value should be set to
 * 0.
 * this mask only.
 * - \b videoImage [sight::data::Image] : RGB image on which the foreground part will be extract from the depth
 * difference
 * image.
 * - \b depthImage [sight::data::Image] : Depth image on which the difference will be performed with the learned depth
 * background image.
 *
 * @subsection In-Out In-Out
 * - \b foregroundImage [sight::data::Image] : Image containing the foreground pixels and background pixels are
 * transparent.
 */
class MODULE_FILTER_VISION_CLASS_API SDepthImageMasking : public service::IOperator
{
public:
    SIGHT_DECLARE_SERVICE(SDepthImageMasking, service::IOperator)

    /// Initializes slots
    MODULE_FILTER_VISION_CLASS_API SDepthImageMasking() noexcept;

    /// Does nothing
    MODULE_FILTER_VISION_CLASS_API virtual ~SDepthImageMasking() noexcept;

    /// Defines auto connection for this service (update slot) to the depth image (objectModified)
    service::IService::KeyConnectionsMap getAutoConnections() const override;

protected:

    /// Does nothing
    MODULE_FILTER_VISION_CLASS_API virtual void configuring() override;

    /// Does nothing
    MODULE_FILTER_VISION_CLASS_API virtual void starting() override;

    /// Does nothing
    MODULE_FILTER_VISION_CLASS_API virtual void stopping() override;

    /// Performs difference on depth images and fills foreground image
    MODULE_FILTER_VISION_CLASS_API virtual void updating() override;

private:

    /// Slot: Sets background depth image on which the difference will be performed to compute what pixels of a new
    /// depth.
    /// image are in front of this learned background image.
    void setBackground();

    /// Slot: Sets the threshold tolerance value when performing the depth images difference.
    void setThreshold(int _threshold);

    /// Mask image to perform computation only inside this mask.
    ::cv::Mat m_cvMaskImage;
    /// Depth image inside the mask to compute the depth difference with the current image.
    ::cv::Mat m_cvDepthMaskImage;
    /// Threshold value to manage a tolerance when performing the depth difference because of noise in such images.
    int m_threshold{10};
};

} // namespace sight::module::filter::vision
