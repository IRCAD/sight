/************************************************************************
 *
 * Copyright (C) 2017-2024 IRCAD France
 * Copyright (C) 2017-2020 IHU Strasbourg
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

#include <data/frame_tl.hpp>
#include <data/image.hpp>

#include <filter/vision/masker.hpp>

#include <service/filter.hpp>

namespace sight::module::filter::vision
{

/**
 * @brief   Service that learns a foreground and background color model and allow to segment the foreground on a new
 * image using an Expectation Maximization algorithm.
 *
 * @see service::filter
 *
 * @section Slots Slots
 * - \b set_background() : Slot to set the background image to learn the background model color
 * - \b set_foreground() : Slot to set the foreground image to learn the foreground model color
 * - \b set_threshold(int threshold) : Slot to set the threshold value to get the final binary image
 * - \b set_noise_level(double noiseLevel) : Slot to set the noise level added in the learning steps
 * - \b set_background_components(int bgComponents) : Slot to set the number of background components learned
 * - \b set_foreground_components(int fgComponents) : Slot to set the number of foreground components learned
 * - \b clearMaskTL() : Slot to clear the output foreground mask timeline and reset the last timestamp.
 *
 * @section XML XML Configuration
 * @code{.xml}
        <service uid="..." type="sight::module::filter::vision::colour_image_masking" >
            <in key="videoTL" uid="..." auto_connect="true" />
            <inout key="mask" uid="..." />
            <inout key="videoMaskTL" uid="..." />
            <config scaleFactor="0.5" noise="30" foregroundComponents="5" backgroundComponents="5"/>
            <HSV>
                <lower>35,0,0</lower>
                <upper>360.5,255,255</upper>
            </HSV>
        </service>
   @endcode
 *
 * @subsection Input Input
 * - \b mask [sight::data::image] : mask image to perform image segmentation in
 * - \b videoTL [sight::data::frame_tl] : Timeline to extract image from a video to perform the learning steps
 *
 * @subsection In-Out In-Out
 * - \b videoMaskTL [sight::data::frame_tl] : Timeline to put masks inside where the foreground is segmented (outside of
 * the
 * mask = 0)
 *
 * @subsection Configuration Configuration
 * - \b scaleFactor (optional)(default: 1.0) : factor to scale the image to perform image masking on
 * - \b noise (optional)(default: 0.0) : standard deviation value for the additive noise used during the foreground
 * learning step. It allows to perform a more robust learning step by providing different values close to the initial
 * pixel. It is sometimes needed when real values are a little bit different than the given input image used during the
 * learning step. For example, to avoid missing pixels when brightness or shadows is changing.
 * - \b foregroundComponents (optional)(default: 5) : number of components learned in the foreground color model
 * - \b backgroundComponents (optional)(default: 5) : number of components learned in the foreground color model
 * - \b HSV (optional) : values in HSV defined by <lower>(default: 0,0,0) and <upper> (default: 255,255,255) tags
 * allowing to compute automatically the mask during the foreground color model learning step
 */
class colour_image_masking final : public service::filter
{
public:

    /// Generates default methods as New, dynamicCast, ...
    SIGHT_DECLARE_SERVICE(colour_image_masking, sight::service::filter);

    /// Initializes the slots and member variables.
    colour_image_masking() noexcept;

    /// Destroys the service.
    ~colour_image_masking() noexcept override;

    /// Defines auto connection for this service (update slot) to the frame timeline (objectPushed)
    service::connections_t auto_connections() const override;

protected:

    /// Initializes segmentation method parameters.
    void configuring() override;

    /// Initializes the colour image masker.
    void starting() override;

    /// Does nothing.
    void stopping() override;

    /// Computes the image mask on a frame.
    void updating() override;

private:

    /// SLOT: Sets background image and learn background model.
    void set_background();

    /// SLOT: Sets foreground image and learn foreground model.
    void set_foreground();

    /// SLOT: Sets the threshold value to compute final binary image.
    void set_threshold(int _threshold);

    /// SLOT: Sets the noise level added in the learning steps.
    void set_noise_level(double _noise_level);

    /// SLOT: Sets the number of background components learned.
    void set_background_components(int _bg_components);

    /// SLOT: Sets the number of foreground components learned.
    void set_foreground_components(int _fg_components);

    /// SLOT: Clears the output mask timeline and reset the last timestamp.
    void clear_mask_tl();

    /// Object performing the Expectation Maximization segmentation.
    std::unique_ptr<sight::filter::vision::masker> m_masker;

    /// Current timestamp.
    core::clock::type m_last_video_timestamp {0.};

    /// Reduction factor.
    float m_scale_factor {1.};

    /// Opencv scale factor.
    cv::Size m_mask_downsize;

    /// Opencv HSV lower value to threshold the image used during foreground color model learning step.
    cv::Scalar m_lower_color;

    /// Opencv HSV upper value to threshold the image used during foreground color model learning step.
    cv::Scalar m_upper_color;

    /// Noise level to add during the foreground learning step.
    double m_noise {0.};

    /// Number of background components.
    int m_background_components {5};

    /// Number of foreground components.
    int m_foreground_components {5};

    static constexpr std::string_view MASK_KEY          = "mask";
    static constexpr std::string_view VIDEO_TL_KEY      = "videoTL";
    static constexpr std::string_view VIDEO_MASK_TL_KEY = "videoMaskTL";

    sight::data::ptr<sight::data::image, sight::data::access::in> m_mask {this, MASK_KEY};
    sight::data::ptr<sight::data::frame_tl, sight::data::access::in> m_video_tl {this, VIDEO_TL_KEY};
    sight::data::ptr<sight::data::frame_tl, sight::data::access::inout> m_video_mask_tl {this, VIDEO_MASK_TL_KEY};
};

} // namespace sight::module::filter::vision.
