/************************************************************************
 *
 * Copyright (C) 2017-2026 IRCAD France
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

#include <data/integer.hpp>
#include <data/real.hpp>

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
 * - \b clearMaskTL() : Slot to clear the output foreground mask timeline and reset the last timestamp.
 *
 * @section XML XML Configuration
 * @code{.xml}
        <service uid="..." type="sight::module::filter::vision::colour_image_masking" >
            <in key="videoTL" uid="..." auto_connect="true" />
            <inout key="mask" uid="..." />
            <inout key="videoMaskTL" uid="..." />
            <config scaleFactor="0.5" noise_level="30" foreground_components="5" background_components="5"/>
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
 * - \b noise_level (optional)(default: 0.0) : standard deviation value for the additive noise used during the
 * foreground
 * learning step. It allows to perform a more robust learning step by providing different values close to the initial
 * pixel. It is sometimes needed when real values are a little bit different than the given input image used during the
 * learning step. For example, to avoid missing pixels when brightness or shadows is changing.
 * - \b foreground_components (optional)(default: 5) : number of components learned in the foreground color model
 * - \b background_components (optional)(default: 5) : number of components learned in the foreground color model
 * - \b HSV (optional) : values in HSV defined by <lower>(default: 0,0,0) and <upper> (default: 255,255,255) tags
 * allowing to compute automatically the mask during the foreground color model learning step
 *
 * @subsection ConfigBindings Configuration data bindings
 * Configuration inputs accept literals or shared data objects.
 * Default values apply when no configuration value is provided.
 * - \b config.threshold [sight::data::integer]: threshold.
 * - \b config.noise_level [sight::data::real]: noise level.
 * - \b config.background_components [sight::data::integer]: background components.
 * - \b config.foreground_components [sight::data::integer]: foreground components.
 */
class colour_image_masking final : public service::filter
{
public:

    /// Generates default methods as New, dynamicCast, ...
    SIGHT_DECLARE_SERVICE(colour_image_masking, sight::service::filter);

    struct slots
    {
        static inline const slot_key_t SET_BACKGROUND = "set_background";
        static inline const slot_key_t SET_FOREGROUND = "set_foreground";
        static inline const slot_key_t SET_THRESHOLD  = "set_threshold";
        static inline const slot_key_t CLEAR_MASKTL   = "clearMaskTL";
    };

    /// Initializes the slots and member variables.
    colour_image_masking() noexcept;

    /// Destroys the service.
    ~colour_image_masking() noexcept override;

protected:

    /// Defines auto connection for this service (update slot) to the frame timeline (objectPushed)
    service::connections_t auto_connections() const override;

    /// Initializes segmentation method parameters.
    void configuring() override;

    /// Initializes the colour image masker.
    void starting() override;

    /// Does nothing.
    void stopping() override;

    /// Computes the image mask on a frame.
    void updating() override;

private:

    /// Apply settings from bound data at startup and after external modifications.
    void apply_config_threshold();

    /// SLOT: Sets background image and learn background model.
    void set_background();

    /// SLOT: Sets foreground image and learn foreground model.
    void set_foreground();

    /// SLOT: Sets the threshold value to compute final binary image.
    void set_threshold(int _threshold);

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

    static constexpr std::string_view MASK_KEY          = "mask";
    static constexpr std::string_view VIDEO_TL_KEY      = "videoTL";
    static constexpr std::string_view VIDEO_MASK_TL_KEY = "videoMaskTL";

    /// Typed config.threshold configuration binding.
    ptr_in<sight::data::integer> m_threshold {this, "config.threshold", 1};

    /// Noise level to add during the foreground learning step.
    ptr_in<sight::data::real> m_noise {this, "config.noise_level", 0.};

    /// Number of background components.
    ptr_in<sight::data::integer> m_background_components {this, "config.background_components", 5};

    /// Number of foreground components.
    ptr_in<sight::data::integer> m_foreground_components {this, "config.foreground_components", 5};

    ptr_in<sight::data::image> m_mask {this, MASK_KEY};
    ptr_in<sight::data::frame_tl> m_video_tl {this, VIDEO_TL_KEY};

    ptr_inout<sight::data::frame_tl> m_video_mask_tl {this, VIDEO_MASK_TL_KEY};
};

} // namespace sight::module::filter::vision.
