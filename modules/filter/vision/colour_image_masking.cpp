/************************************************************************
 *
 * Copyright (C) 2017-2023 IRCAD France
 * Copyright (C) 2017-2021 IHU Strasbourg
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

#include "modules/filter/vision/colour_image_masking.hpp"

#include <core/com/signal.hxx>
#include <core/com/slots.hxx>

#include <io/opencv/frame_tl.hpp>
#include <io/opencv/image.hpp>

#include <boost/lexical_cast.hpp>
#include <boost/tokenizer.hpp>

namespace sight::module::filter::vision
{

const core::com::slots::key_t SET_BACKGROUND_SLOT            = "setBackground";
const core::com::slots::key_t SET_FOREGROUND_SLOT            = "setForeground";
const core::com::slots::key_t SET_THRESHOLD_SLOT             = "setThreshold";
const core::com::slots::key_t SET_NOISE_LEVEL_SLOT           = "setNoiseLevel";
const core::com::slots::key_t SET_BACKGROUND_COMPONENTS_SLOT = "setBackgroundComponents";
const core::com::slots::key_t SET_FOREGROUND_COMPONENTS_SLOT = "setForegroundComponents";
const core::com::slots::key_t CLEAR_MASKTL_SLOT              = "clearMaskTL";

// ------------------------------------------------------------------------------

colour_image_masking::colour_image_masking() noexcept :

    m_mask_downsize(cv::Size(0, 0)),
    m_lower_color(cv::Scalar(0, 0, 0)),
    m_upper_color(cv::Scalar(255, 255, 255))
{
    new_slot(SET_BACKGROUND_SLOT, &colour_image_masking::set_background, this);
    new_slot(SET_FOREGROUND_SLOT, &colour_image_masking::set_foreground, this);
    new_slot(SET_THRESHOLD_SLOT, &colour_image_masking::set_threshold, this);
    new_slot(SET_NOISE_LEVEL_SLOT, &colour_image_masking::set_noise_level, this);
    new_slot(SET_BACKGROUND_COMPONENTS_SLOT, &colour_image_masking::set_background_components, this);
    new_slot(SET_FOREGROUND_COMPONENTS_SLOT, &colour_image_masking::set_foreground_components, this);
    new_slot(CLEAR_MASKTL_SLOT, &colour_image_masking::clear_mask_tl, this);
}

// ------------------------------------------------------------------------------

colour_image_masking::~colour_image_masking() noexcept =
    default;

// ------------------------------------------------------------------------------

void colour_image_masking::configuring()
{
    const service::config_t config = this->get_config().get_child("config.<xmlattr>");

    m_scale_factor          = config.get<float>("scaleFactor", 1.0);
    m_noise                 = config.get<double>("noise", 0.0);
    m_foreground_components = config.get<int>("foregroundComponents", 5);
    m_background_components = config.get<int>("backgroundComponents", 5);

    SIGHT_ASSERT(
        "Scale factor must be between 0 and 1. Current value: " << m_scale_factor,
        (m_scale_factor > 0 && m_scale_factor <= 1)
    );
    SIGHT_ASSERT(
        "The number of background components must be greater than 0. Current value: " << m_background_components,
        m_background_components > 0
    );
    SIGHT_ASSERT("Noise value must be >= 0. Current value:" << m_noise, m_noise >= 0);
    SIGHT_ASSERT(
        "The number of foreground components must be greater than 0. Current value: " << m_foreground_components,
        m_foreground_components > 0
    );

    m_lower_color = cv::Scalar(0, 0, 0);
    m_upper_color = cv::Scalar(255, 255, 255);

    const service::config_t hsv_config = this->get_config().get_child("HSV");
    std::string s_lower_value          = hsv_config.get<std::string>("lower", "");
    std::string s_upper_value          = hsv_config.get<std::string>("upper", "");

    const boost::char_separator<char> sep {",", ";"};

    if(!s_lower_value.empty())
    {
        const boost::tokenizer<boost::char_separator<char> > tok_lower {s_lower_value, sep};
        int i = 0;
        for(const auto& it : tok_lower)
        {
            SIGHT_ASSERT("Only 3 integers needed to define lower HSV value", i < 3);
            m_lower_color[i++] = boost::lexical_cast<double>(it);
        }
    }

    if(!s_upper_value.empty())
    {
        const boost::tokenizer<boost::char_separator<char> > tok_upper {s_upper_value, sep};
        int i = 0;
        for(const auto& it : tok_upper)
        {
            SIGHT_ASSERT("Only 3 integers needed to define upper HSV value", i < 3);
            m_upper_color[i++] = boost::lexical_cast<double>(it);
        }
    }
}

// ------------------------------------------------------------------------------

void colour_image_masking::starting()
{
    namespace vision = sight::filter::vision;
    m_masker         = std::make_unique<vision::masker>(vision::h_sv, vision::ll_ratio);
    m_masker->set_threshold(1.);

    m_last_video_timestamp = 0.;
}

// ------------------------------------------------------------------------------

void colour_image_masking::stopping()
{
}

//-----------------------------------------------------------------------------

service::connections_t colour_image_masking::auto_connections() const
{
    connections_t connections;

    connections.push(VIDEO_TL_KEY, data::timeline::signals::PUSHED, service::slots::UPDATE);
    connections.push(VIDEO_TL_KEY, data::timeline::signals::CLEARED, CLEAR_MASKTL_SLOT);

    return connections;
}

// ------------------------------------------------------------------------------

void colour_image_masking::updating()
{
    if(m_masker->is_model_learned())
    {
        const auto mask     = m_mask.lock();
        const auto video_tl = m_video_tl.lock();
        auto video_mask_tl  = m_video_mask_tl.lock();

        // Sanity checks
        SIGHT_ASSERT("Missing input '" << MASK_KEY << "'.", mask);
        SIGHT_ASSERT("Missing input '" << VIDEO_TL_KEY << "'.", video_tl);
        SIGHT_ASSERT("Missing inout '" << VIDEO_MASK_TL_KEY << "'.", video_mask_tl);
        const auto mask_size = mask->size();
        if(mask_size[0] != video_tl->get_width() || mask_size[1] != video_tl->get_height())
        {
            SIGHT_ERROR(
                "Reference mask (" << mask_size[0] << ", " << mask_size[1]
                << ") has different size as the video timeline (" << video_tl->get_width() << ", "
                << video_tl->get_height() << ")."
            );
        }

        // This service can take a while to run, this blocker skips frames that arrive while we're already processing
        // one
        auto sig = video_tl->signal<data::timeline::signals::pushed_t>(
            data::timeline::signals::PUSHED
        );
        core::com::connection::blocker blocker(sig->get_connection(slot(service::slots::UPDATE)));

        // Get the timestamp from the latest video frame
        core::hires_clock::type current_timestamp = video_tl->get_newer_timestamp();

        // Get image from the video timeline
        CSPTR(data::frame_tl::buffer_t) video_buffer = video_tl->get_closest_buffer(current_timestamp);

        if(!video_buffer)
        {
            SIGHT_ERROR("Buffer not found with timestamp " << current_timestamp);
            return;
        }

        const std::uint8_t* frame_buff_out_video = &video_buffer->get_element(0);

        core::hires_clock::type video_timestamp = video_buffer->get_timestamp();
        if(video_timestamp <= m_last_video_timestamp)
        {
            SIGHT_WARN(
                "Dropping frame with timestamp " << video_timestamp << " (previous frame had timestamp "
                << m_last_video_timestamp << ")"
            );
            return;
        }

        m_last_video_timestamp = video_timestamp;

        // convert the ::fw::Data::image mask to an OpenCV image
        cv::Mat mask_cv = io::opencv::image::copy_to_cv(mask.get_shared());

        cv::cvtColor(mask_cv, mask_cv, cv::COLOR_BGR2GRAY);

        mask_cv = (mask_cv > 0);

        //convert the data::frame_tl videoTL to an OpenCV image
        const cv::Mat video_cv = io::opencv::frame_tl::move_to_cv(video_tl.get_shared(), frame_buff_out_video);

        // Create image mask to put inside the timeline
        SPTR(data::frame_tl::buffer_t) mask_buffer = video_mask_tl->create_buffer(current_timestamp);
        std::uint8_t* frame_buff_out_mask = mask_buffer->add_element(0);

        cv::Mat video_mask_cv = io::opencv::frame_tl::move_to_cv(video_mask_tl.get_shared(), frame_buff_out_mask);

        // Get the foreground mask
        cv::Mat foreground_mask = m_masker->make_mask(video_cv, m_mask_downsize, mask_cv);

        // Create an openCV mat that aliases the buffer created from the output timeline
        video_cv.copyTo(video_mask_cv, foreground_mask);

        // Push the mask object in the timeline
        video_mask_tl->push_object(mask_buffer);

        auto sig_mask = video_mask_tl->signal<data::timeline::signals::pushed_t>(data::timeline::signals::PUSHED);
        sig_mask->async_emit(current_timestamp);
    }
}

// ------------------------------------------------------------------------------

void colour_image_masking::set_background()
{
    const auto mask     = m_mask.lock();
    const auto video_tl = m_video_tl.lock();

    core::hires_clock::type current_timestamp = core::hires_clock::get_time_in_milli_sec();
    CSPTR(data::frame_tl::buffer_t) video_buffer = video_tl->get_closest_buffer(current_timestamp);
    if(!video_buffer)
    {
        SIGHT_ERROR("Buffer not found with timestamp " << current_timestamp);
        return;
    }

    const std::uint8_t* frame_buff_out_video = &video_buffer->get_element(0);

    //convert the data::frame_tl videoTL to an OpenCV image
    const cv::Mat video_cv = io::opencv::frame_tl::move_to_cv(video_tl.get_shared(), frame_buff_out_video);

    // convert the data::image mask to an OpenCV image
    cv::Mat mask_cv = io::opencv::image::copy_to_cv(mask.get_shared());

    // Convert color mask to grayscale value
    cv::cvtColor(mask_cv, mask_cv, cv::COLOR_RGB2GRAY);

    mask_cv = (mask_cv > 0);

    // Save size to downscale the image (speed up the process but decrease segmentation quality)
    m_mask_downsize = cv::Size(
        static_cast<int>(static_cast<float>(mask_cv.size[1]) * m_scale_factor),
        static_cast<int>(static_cast<float>(mask_cv.size[0]) * m_scale_factor)
    );

    // Erode a little bit the mask to avoid the borders
    // Construct element type
    int element_type = cv::MORPH_ELLIPSE;
    // Choose element size
    int element_erode_size(1);
    // Construct erosion element
    cv::Mat element_erode = cv::getStructuringElement(
        element_type,
        cv::Size(2 * element_erode_size + 1, 2 * element_erode_size + 1),
        cv::Point(element_erode_size, element_erode_size)
    );
    // Perform erosion
    cv::erode(mask_cv, mask_cv, element_erode);

    // Learn background color model
    m_masker->train_background_model(video_cv, mask_cv, unsigned(m_background_components));

    // Initialize the mask timeline
    const auto video_mask_tl = m_video_mask_tl.lock();
    video_mask_tl->init_pool_size(
        video_tl->get_width(),
        video_tl->get_height(),
        core::type::UINT8,
        data::frame_tl::pixel_format::rgba
    );
}

// ------------------------------------------------------------------------------

void colour_image_masking::set_foreground()
{
    const auto video_tl = m_video_tl.lock();

    core::hires_clock::type current_timestamp = core::hires_clock::get_time_in_milli_sec();
    CSPTR(data::frame_tl::buffer_t) video_buffer = video_tl->get_closest_buffer(current_timestamp);
    if(!video_buffer)
    {
        SIGHT_ERROR("Buffer not found with timestamp " << current_timestamp);
        return;
    }

    const std::uint8_t* frame_buff_out_video = &video_buffer->get_element(0);

    //convert mask to an OpenCV image:
    cv::Mat video_cv = io::opencv::frame_tl::move_to_cv(video_tl.get_shared(), frame_buff_out_video);

    // Convert RGB to HSV
    cv::Mat video_bgr;
    cv::Mat video_hsv;
    cv::cvtColor(video_cv, video_bgr, cv::COLOR_RGBA2BGR);
    cv::cvtColor(video_bgr, video_hsv, cv::COLOR_BGR2HSV);

    // Get the mask to learn the foreground model
    cv::Mat foreground_mask;
    cv::inRange(video_hsv, m_lower_color, m_upper_color, foreground_mask);

    // Remove small objects by performing an opening
    cv::Mat open_foreground_mask;
    // Construct element type
    int element_type = cv::MORPH_ELLIPSE;
    // Choose element size
    int element_erode_size(2);
    // Construct erosion element
    cv::Mat element_erode = cv::getStructuringElement(
        element_type,
        cv::Size(2 * element_erode_size + 1, 2 * element_erode_size + 1),
        cv::Point(element_erode_size, element_erode_size)
    );
    // Perform erosion
    cv::erode(foreground_mask, open_foreground_mask, element_erode);

    // Learn foreground color model
    m_masker->train_foreground_model(video_cv, open_foreground_mask, unsigned(m_foreground_components), m_noise);
}

// ------------------------------------------------------------------------------

void colour_image_masking::set_threshold(int _threshold)
{
    if(m_masker)
    {
        m_masker->set_threshold(_threshold);
    }
}

// ------------------------------------------------------------------------------

void colour_image_masking::set_noise_level(double _noise_level)
{
    m_noise = _noise_level;
}

// ------------------------------------------------------------------------------

void colour_image_masking::set_background_components(int _bg_components)
{
    m_background_components = _bg_components;
}

// ------------------------------------------------------------------------------

void colour_image_masking::set_foreground_components(int _fg_components)
{
    m_foreground_components = _fg_components;
}

// ------------------------------------------------------------------------------

void colour_image_masking::clear_mask_tl()
{
    auto video_mask_tl = m_video_mask_tl.lock();
    video_mask_tl->clear_timeline();
    auto sig_tl_cleared = video_mask_tl->signal<data::timeline::signals::cleared_t>(data::timeline::signals::CLEARED);
    sig_tl_cleared->async_emit();
    m_last_video_timestamp = 0.;
}

// ------------------------------------------------------------------------------

} // namespace sight::module::filter::vision
