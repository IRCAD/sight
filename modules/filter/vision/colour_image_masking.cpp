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

    m_maskDownsize(cv::Size(0, 0)),
    m_lowerColor(cv::Scalar(0, 0, 0)),
    m_upperColor(cv::Scalar(255, 255, 255))
{
    new_slot(SET_BACKGROUND_SLOT, &colour_image_masking::setBackground, this);
    new_slot(SET_FOREGROUND_SLOT, &colour_image_masking::setForeground, this);
    new_slot(SET_THRESHOLD_SLOT, &colour_image_masking::setThreshold, this);
    new_slot(SET_NOISE_LEVEL_SLOT, &colour_image_masking::setNoiseLevel, this);
    new_slot(SET_BACKGROUND_COMPONENTS_SLOT, &colour_image_masking::setBackgroundComponents, this);
    new_slot(SET_FOREGROUND_COMPONENTS_SLOT, &colour_image_masking::setForegroundComponents, this);
    new_slot(CLEAR_MASKTL_SLOT, &colour_image_masking::clearMaskTL, this);
}

// ------------------------------------------------------------------------------

colour_image_masking::~colour_image_masking() noexcept =
    default;

// ------------------------------------------------------------------------------

void colour_image_masking::configuring()
{
    const service::config_t config = this->get_config().get_child("config.<xmlattr>");

    m_scaleFactor          = config.get<float>("scaleFactor", 1.0);
    m_noise                = config.get<double>("noise", 0.0);
    m_foregroundComponents = config.get<int>("foregroundComponents", 5);
    m_backgroundComponents = config.get<int>("backgroundComponents", 5);

    SIGHT_ASSERT(
        "Scale factor must be between 0 and 1. Current value: " << m_scaleFactor,
        (m_scaleFactor > 0 && m_scaleFactor <= 1)
    );
    SIGHT_ASSERT(
        "The number of background components must be greater than 0. Current value: " << m_backgroundComponents,
        m_backgroundComponents > 0
    );
    SIGHT_ASSERT("Noise value must be >= 0. Current value:" << m_noise, m_noise >= 0);
    SIGHT_ASSERT(
        "The number of foreground components must be greater than 0. Current value: " << m_foregroundComponents,
        m_foregroundComponents > 0
    );

    m_lowerColor = cv::Scalar(0, 0, 0);
    m_upperColor = cv::Scalar(255, 255, 255);

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
            m_lowerColor[i++] = boost::lexical_cast<double>(it);
        }
    }

    if(!s_upper_value.empty())
    {
        const boost::tokenizer<boost::char_separator<char> > tok_upper {s_upper_value, sep};
        int i = 0;
        for(const auto& it : tok_upper)
        {
            SIGHT_ASSERT("Only 3 integers needed to define upper HSV value", i < 3);
            m_upperColor[i++] = boost::lexical_cast<double>(it);
        }
    }
}

// ------------------------------------------------------------------------------

void colour_image_masking::starting()
{
    namespace vision = sight::filter::vision;
    m_masker         = std::make_unique<vision::masker>(vision::HSv, vision::LLRatio);
    m_masker->setThreshold(1.);

    m_lastVideoTimestamp = 0.;
}

// ------------------------------------------------------------------------------

void colour_image_masking::stopping()
{
}

//-----------------------------------------------------------------------------

service::connections_t colour_image_masking::auto_connections() const
{
    connections_t connections;

    connections.push(s_VIDEO_TL_KEY, data::timeline::signals::PUSHED, service::slots::UPDATE);
    connections.push(s_VIDEO_TL_KEY, data::timeline::signals::CLEARED, CLEAR_MASKTL_SLOT);

    return connections;
}

// ------------------------------------------------------------------------------

void colour_image_masking::updating()
{
    if(m_masker->isModelLearned())
    {
        const auto mask     = m_mask.lock();
        const auto video_tl = m_videoTL.lock();
        auto video_mask_tl  = m_videoMaskTL.lock();

        // Sanity checks
        SIGHT_ASSERT("Missing input '" << s_MASK_KEY << "'.", mask);
        SIGHT_ASSERT("Missing input '" << s_VIDEO_TL_KEY << "'.", video_tl);
        SIGHT_ASSERT("Missing inout '" << s_VIDEO_MASK_TL_KEY << "'.", video_mask_tl);
        const auto mask_size = mask->size();
        if(mask_size[0] != video_tl->getWidth() || mask_size[1] != video_tl->getHeight())
        {
            SIGHT_ERROR(
                "Reference mask (" << mask_size[0] << ", " << mask_size[1]
                << ") has different size as the video timeline (" << video_tl->getWidth() << ", "
                << video_tl->getHeight() << ")."
            );
        }

        // This service can take a while to run, this blocker skips frames that arrive while we're already processing
        // one
        auto sig = video_tl->signal<data::timeline::signals::pushed_t>(
            data::timeline::signals::PUSHED
        );
        core::com::connection::blocker blocker(sig->get_connection(slot(service::slots::UPDATE)));

        // Get the timestamp from the latest video frame
        core::hires_clock::type current_timestamp = video_tl->getNewerTimestamp();

        // Get image from the video timeline
        CSPTR(data::frame_tl::buffer_t) video_buffer = video_tl->getClosestBuffer(current_timestamp);

        if(!video_buffer)
        {
            SIGHT_ERROR("Buffer not found with timestamp " << current_timestamp);
            return;
        }

        const std::uint8_t* frame_buff_out_video = &video_buffer->getElement(0);

        core::hires_clock::type video_timestamp = video_buffer->getTimestamp();
        if(video_timestamp <= m_lastVideoTimestamp)
        {
            SIGHT_WARN(
                "Dropping frame with timestamp " << video_timestamp << " (previous frame had timestamp "
                << m_lastVideoTimestamp << ")"
            );
            return;
        }

        m_lastVideoTimestamp = video_timestamp;

        // convert the ::fw::Data::image mask to an OpenCV image
        cv::Mat mask_cv = io::opencv::image::copyToCv(mask.get_shared());

        cv::cvtColor(mask_cv, mask_cv, cv::COLOR_BGR2GRAY);

        mask_cv = (mask_cv > 0);

        //convert the data::frame_tl videoTL to an OpenCV image
        const cv::Mat video_cv = io::opencv::frame_tl::move_to_cv(video_tl.get_shared(), frame_buff_out_video);

        // Create image mask to put inside the timeline
        SPTR(data::frame_tl::buffer_t) mask_buffer = video_mask_tl->createBuffer(current_timestamp);
        std::uint8_t* frame_buff_out_mask = mask_buffer->addElement(0);

        cv::Mat video_mask_cv = io::opencv::frame_tl::move_to_cv(video_mask_tl.get_shared(), frame_buff_out_mask);

        // Get the foreground mask
        cv::Mat foreground_mask = m_masker->makeMask(video_cv, m_maskDownsize, mask_cv);

        // Create an openCV mat that aliases the buffer created from the output timeline
        video_cv.copyTo(video_mask_cv, foreground_mask);

        // Push the mask object in the timeline
        video_mask_tl->pushObject(mask_buffer);

        auto sig_mask = video_mask_tl->signal<data::timeline::signals::pushed_t>(data::timeline::signals::PUSHED);
        sig_mask->async_emit(current_timestamp);
    }
}

// ------------------------------------------------------------------------------

void colour_image_masking::setBackground()
{
    const auto mask     = m_mask.lock();
    const auto video_tl = m_videoTL.lock();

    core::hires_clock::type current_timestamp = core::hires_clock::get_time_in_milli_sec();
    CSPTR(data::frame_tl::buffer_t) video_buffer = video_tl->getClosestBuffer(current_timestamp);
    if(!video_buffer)
    {
        SIGHT_ERROR("Buffer not found with timestamp " << current_timestamp);
        return;
    }

    const std::uint8_t* frame_buff_out_video = &video_buffer->getElement(0);

    //convert the data::frame_tl videoTL to an OpenCV image
    const cv::Mat video_cv = io::opencv::frame_tl::move_to_cv(video_tl.get_shared(), frame_buff_out_video);

    // convert the data::image mask to an OpenCV image
    cv::Mat mask_cv = io::opencv::image::copyToCv(mask.get_shared());

    // Convert color mask to grayscale value
    cv::cvtColor(mask_cv, mask_cv, cv::COLOR_RGB2GRAY);

    mask_cv = (mask_cv > 0);

    // Save size to downscale the image (speed up the process but decrease segmentation quality)
    m_maskDownsize = cv::Size(
        static_cast<int>(static_cast<float>(mask_cv.size[1]) * m_scaleFactor),
        static_cast<int>(static_cast<float>(mask_cv.size[0]) * m_scaleFactor)
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
    m_masker->trainBackgroundModel(video_cv, mask_cv, unsigned(m_backgroundComponents));

    // Initialize the mask timeline
    const auto video_mask_tl = m_videoMaskTL.lock();
    video_mask_tl->initPoolSize(
        video_tl->getWidth(),
        video_tl->getHeight(),
        core::type::UINT8,
        data::frame_tl::PixelFormat::RGBA
    );
}

// ------------------------------------------------------------------------------

void colour_image_masking::setForeground()
{
    const auto video_tl = m_videoTL.lock();

    core::hires_clock::type current_timestamp = core::hires_clock::get_time_in_milli_sec();
    CSPTR(data::frame_tl::buffer_t) video_buffer = video_tl->getClosestBuffer(current_timestamp);
    if(!video_buffer)
    {
        SIGHT_ERROR("Buffer not found with timestamp " << current_timestamp);
        return;
    }

    const std::uint8_t* frame_buff_out_video = &video_buffer->getElement(0);

    //convert mask to an OpenCV image:
    cv::Mat video_cv = io::opencv::frame_tl::move_to_cv(video_tl.get_shared(), frame_buff_out_video);

    // Convert RGB to HSV
    cv::Mat video_bgr;
    cv::Mat video_hsv;
    cv::cvtColor(video_cv, video_bgr, cv::COLOR_RGBA2BGR);
    cv::cvtColor(video_bgr, video_hsv, cv::COLOR_BGR2HSV);

    // Get the mask to learn the foreground model
    cv::Mat foreground_mask;
    cv::inRange(video_hsv, m_lowerColor, m_upperColor, foreground_mask);

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
    m_masker->trainForegroundModel(video_cv, open_foreground_mask, unsigned(m_foregroundComponents), m_noise);
}

// ------------------------------------------------------------------------------

void colour_image_masking::setThreshold(int _threshold)
{
    if(m_masker)
    {
        m_masker->setThreshold(_threshold);
    }
}

// ------------------------------------------------------------------------------

void colour_image_masking::setNoiseLevel(double _noise_level)
{
    m_noise = _noise_level;
}

// ------------------------------------------------------------------------------

void colour_image_masking::setBackgroundComponents(int _bg_components)
{
    m_backgroundComponents = _bg_components;
}

// ------------------------------------------------------------------------------

void colour_image_masking::setForegroundComponents(int _fg_components)
{
    m_foregroundComponents = _fg_components;
}

// ------------------------------------------------------------------------------

void colour_image_masking::clearMaskTL()
{
    auto video_mask_tl = m_videoMaskTL.lock();
    video_mask_tl->clearTimeline();
    auto sig_tl_cleared = video_mask_tl->signal<data::timeline::signals::cleared_t>(data::timeline::signals::CLEARED);
    sig_tl_cleared->async_emit();
    m_lastVideoTimestamp = 0.;
}

// ------------------------------------------------------------------------------

} // namespace sight::module::filter::vision
