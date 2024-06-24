/************************************************************************
 *
 * Copyright (C) 2020-2024 IRCAD France
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

#include <data/image.hpp>

#include <service/filter.hpp>

#include <opencv2/opencv.hpp>

namespace sight::module::filter::vision
{

/**
 * @brief   Service that learns a background depth image in a mask to perform a depth difference and output a foregroud
 * image.
 *
 * @see service::filter
 *
 * @section Slots Slots
 * - \b set_background() : Slot to learn the background depth image.
 * - \b set_threshold() : Slot to change the difference value tolerance between the larned depth image and the current.
 *
 * @section XML XML Configuration
 *
 * @code{.xml}
    <service uid="..." type="sight::module::filter::vision::depth_image_masking" worker="...">
        <in key="maskImage" uid="..." />
        <in key="videoImage" uid="..." />
        <in key="depthImage" uid="..." auto_connect="true" />
        <inout key="foregroundImage" uid="..." />
    </service>
   @endcode
 *
 * @subsection Input Input
 * - \b maskImage [sight::data::image] : Mask image to perform the difference inside. Background value should be set to
 * 0.
 * this mask only.
 * - \b videoImage [sight::data::image] : RGB image on which the foreground part will be extract from the depth
 * difference
 * image.
 * - \b depthImage [sight::data::image] : Depth image on which the difference will be performed with the learned depth
 * background image.
 *
 * @subsection In-Out In-Out
 * - \b foregroundImage [sight::data::image] : image containing the foreground pixels and background pixels are
 * transparent.
 */
class depth_image_masking : public service::filter
{
public:

    SIGHT_DECLARE_SERVICE(depth_image_masking, service::filter);

    /// Initializes slots
    depth_image_masking() noexcept;

    /// Does nothing
    ~depth_image_masking() noexcept override = default;

    /// Defines auto connection for this service (update slot) to the depth image (objectModified)
    service::connections_t auto_connections() const override;

protected:

    /// Does nothing
    void configuring() override;

    /// Does nothing
    void starting() override;

    /// Does nothing
    void stopping() override;

    /// Performs difference on depth images and fills foreground image
    void updating() override;

private:

    /// Slot: Sets background depth image on which the difference will be performed to compute what pixels of a new
    /// depth.
    /// image are in front of this learned background image.
    void set_background();

    /// Slot: Sets the threshold tolerance value when performing the depth images difference.
    void set_threshold(int _threshold);

    /// Mask image to perform computation only inside this mask.
    cv::Mat m_cv_mask_image;

    /// Depth image inside the mask to compute the depth difference with the current image.
    cv::Mat m_cv_depth_mask_image;

    /// Threshold value to manage a tolerance when performing the depth difference because of noise in such images.
    int m_threshold {10};

    static constexpr std::string_view MASK_IMAGE_KEY       = "maskImage";
    static constexpr std::string_view VIDEO_IMAGE_KEY      = "videoImage";
    static constexpr std::string_view DEPTH_IMAGE_KEY      = "depthImage";
    static constexpr std::string_view FOREGROUND_IMAGE_KEY = "foregroundImage";

    sight::data::ptr<sight::data::image, sight::data::access::in> m_mask_image {this, MASK_IMAGE_KEY};
    sight::data::ptr<sight::data::image, sight::data::access::in> m_video_image {this, VIDEO_IMAGE_KEY};
    sight::data::ptr<sight::data::image, sight::data::access::in> m_depth_image {this, DEPTH_IMAGE_KEY};
    sight::data::ptr<sight::data::image, sight::data::access::inout> m_foreground_image {this, FOREGROUND_IMAGE_KEY};
};

} // namespace sight::module::filter::vision
