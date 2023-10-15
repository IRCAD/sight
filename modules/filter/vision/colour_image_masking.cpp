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

    const service::config_t hsvConfig = this->get_config().get_child("HSV");
    std::string s_lowerValue          = hsvConfig.get<std::string>("lower", "");
    std::string s_upperValue          = hsvConfig.get<std::string>("upper", "");

    const boost::char_separator<char> sep {",", ";"};

    if(!s_lowerValue.empty())
    {
        const boost::tokenizer<boost::char_separator<char> > tokLower {s_lowerValue, sep};
        int i = 0;
        for(const auto& it : tokLower)
        {
            SIGHT_ASSERT("Only 3 integers needed to define lower HSV value", i < 3);
            m_lowerColor[i++] = boost::lexical_cast<double>(it);
        }
    }

    if(!s_upperValue.empty())
    {
        const boost::tokenizer<boost::char_separator<char> > tokUpper {s_upperValue, sep};
        int i = 0;
        for(const auto& it : tokUpper)
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
        const auto mask    = m_mask.lock();
        const auto videoTL = m_videoTL.lock();
        auto videoMaskTL   = m_videoMaskTL.lock();

        // Sanity checks
        SIGHT_ASSERT("Missing input '" << s_MASK_KEY << "'.", mask);
        SIGHT_ASSERT("Missing input '" << s_VIDEO_TL_KEY << "'.", videoTL);
        SIGHT_ASSERT("Missing inout '" << s_VIDEO_MASK_TL_KEY << "'.", videoMaskTL);
        const auto maskSize = mask->size();
        if(maskSize[0] != videoTL->getWidth() || maskSize[1] != videoTL->getHeight())
        {
            SIGHT_ERROR(
                "Reference mask (" << maskSize[0] << ", " << maskSize[1]
                << ") has different size as the video timeline (" << videoTL->getWidth() << ", "
                << videoTL->getHeight() << ")."
            );
        }

        // This service can take a while to run, this blocker skips frames that arrive while we're already processing
        // one
        auto sig_ = videoTL->signal<data::timeline::signals::pushed_t>(
            data::timeline::signals::PUSHED
        );
        core::com::connection::blocker blocker(sig_->get_connection(slot(service::slots::UPDATE)));

        // Get the timestamp from the latest video frame
        core::hires_clock::type currentTimestamp = videoTL->getNewerTimestamp();

        // Get image from the video timeline
        CSPTR(data::frame_tl::BufferType) videoBuffer = videoTL->getClosestBuffer(currentTimestamp);

        if(!videoBuffer)
        {
            SIGHT_ERROR("Buffer not found with timestamp " << currentTimestamp);
            return;
        }

        const std::uint8_t* frameBuffOutVideo = &videoBuffer->getElement(0);

        core::hires_clock::type videoTimestamp = videoBuffer->getTimestamp();
        if(videoTimestamp <= m_lastVideoTimestamp)
        {
            SIGHT_WARN(
                "Dropping frame with timestamp " << videoTimestamp << " (previous frame had timestamp "
                << m_lastVideoTimestamp << ")"
            );
            return;
        }

        m_lastVideoTimestamp = videoTimestamp;

        // convert the ::fw::Data::image mask to an OpenCV image
        cv::Mat maskCV = io::opencv::image::copyToCv(mask.get_shared());

        cv::cvtColor(maskCV, maskCV, cv::COLOR_BGR2GRAY);

        maskCV = (maskCV > 0);

        //convert the data::frame_tl videoTL to an OpenCV image
        const cv::Mat videoCV = io::opencv::frame_tl::moveToCv(videoTL.get_shared(), frameBuffOutVideo);

        // Create image mask to put inside the timeline
        SPTR(data::frame_tl::BufferType) maskBuffer = videoMaskTL->createBuffer(currentTimestamp);
        std::uint8_t* frameBuffOutMask = maskBuffer->addElement(0);

        cv::Mat videoMaskCV = io::opencv::frame_tl::moveToCv(videoMaskTL.get_shared(), frameBuffOutMask);

        // Get the foreground mask
        cv::Mat foregroundMask = m_masker->makeMask(videoCV, m_maskDownsize, maskCV);

        // Create an openCV mat that aliases the buffer created from the output timeline
        videoCV.copyTo(videoMaskCV, foregroundMask);

        // Push the mask object in the timeline
        videoMaskTL->pushObject(maskBuffer);

        auto sig = videoMaskTL->signal<data::timeline::signals::pushed_t>(
            data::timeline::signals::PUSHED
        );
        sig->async_emit(currentTimestamp);
    }
}

// ------------------------------------------------------------------------------

void colour_image_masking::setBackground()
{
    const auto mask    = m_mask.lock();
    const auto videoTL = m_videoTL.lock();

    core::hires_clock::type currentTimestamp = core::hires_clock::get_time_in_milli_sec();
    CSPTR(data::frame_tl::BufferType) videoBuffer = videoTL->getClosestBuffer(currentTimestamp);
    if(!videoBuffer)
    {
        SIGHT_ERROR("Buffer not found with timestamp " << currentTimestamp);
        return;
    }

    const std::uint8_t* frameBuffOutVideo = &videoBuffer->getElement(0);

    //convert the data::frame_tl videoTL to an OpenCV image
    const cv::Mat videoCV = io::opencv::frame_tl::moveToCv(videoTL.get_shared(), frameBuffOutVideo);

    // convert the data::image mask to an OpenCV image
    cv::Mat maskCV = io::opencv::image::copyToCv(mask.get_shared());

    // Convert color mask to grayscale value
    cv::cvtColor(maskCV, maskCV, cv::COLOR_RGB2GRAY);

    maskCV = (maskCV > 0);

    // Save size to downscale the image (speed up the process but decrease segmentation quality)
    m_maskDownsize = cv::Size(
        static_cast<int>(static_cast<float>(maskCV.size[1]) * m_scaleFactor),
        static_cast<int>(static_cast<float>(maskCV.size[0]) * m_scaleFactor)
    );

    // Erode a little bit the mask to avoid the borders
    // Construct element type
    int elementType = cv::MORPH_ELLIPSE;
    // Choose element size
    int elementErodeSize(1);
    // Construct erosion element
    cv::Mat elementErode = cv::getStructuringElement(
        elementType,
        cv::Size(2 * elementErodeSize + 1, 2 * elementErodeSize + 1),
        cv::Point(elementErodeSize, elementErodeSize)
    );
    // Perform erosion
    cv::erode(maskCV, maskCV, elementErode);

    // Learn background color model
    m_masker->trainBackgroundModel(videoCV, maskCV, unsigned(m_backgroundComponents));

    // Initialize the mask timeline
    const auto videoMaskTL = m_videoMaskTL.lock();
    videoMaskTL->initPoolSize(
        videoTL->getWidth(),
        videoTL->getHeight(),
        core::type::UINT8,
        data::frame_tl::PixelFormat::RGBA
    );
}

// ------------------------------------------------------------------------------

void colour_image_masking::setForeground()
{
    const auto videoTL = m_videoTL.lock();

    core::hires_clock::type currentTimestamp = core::hires_clock::get_time_in_milli_sec();
    CSPTR(data::frame_tl::BufferType) videoBuffer = videoTL->getClosestBuffer(currentTimestamp);
    if(!videoBuffer)
    {
        SIGHT_ERROR("Buffer not found with timestamp " << currentTimestamp);
        return;
    }

    const std::uint8_t* frameBuffOutVideo = &videoBuffer->getElement(0);

    //convert mask to an OpenCV image:
    cv::Mat videoCV = io::opencv::frame_tl::moveToCv(videoTL.get_shared(), frameBuffOutVideo);

    // Convert RGB to HSV
    cv::Mat videoBGR;
    cv::Mat videoHSV;
    cv::cvtColor(videoCV, videoBGR, cv::COLOR_RGBA2BGR);
    cv::cvtColor(videoBGR, videoHSV, cv::COLOR_BGR2HSV);

    // Get the mask to learn the foreground model
    cv::Mat foregroundMask;
    cv::inRange(videoHSV, m_lowerColor, m_upperColor, foregroundMask);

    // Remove small objects by performing an opening
    cv::Mat openForegroundMask;
    // Construct element type
    int elementType = cv::MORPH_ELLIPSE;
    // Choose element size
    int elementErodeSize(2);
    // Construct erosion element
    cv::Mat elementErode = cv::getStructuringElement(
        elementType,
        cv::Size(2 * elementErodeSize + 1, 2 * elementErodeSize + 1),
        cv::Point(elementErodeSize, elementErodeSize)
    );
    // Perform erosion
    cv::erode(foregroundMask, openForegroundMask, elementErode);

    // Learn foreground color model
    m_masker->trainForegroundModel(videoCV, openForegroundMask, unsigned(m_foregroundComponents), m_noise);
}

// ------------------------------------------------------------------------------

void colour_image_masking::setThreshold(int threshold)
{
    if(m_masker)
    {
        m_masker->setThreshold(threshold);
    }
}

// ------------------------------------------------------------------------------

void colour_image_masking::setNoiseLevel(double noiseLevel)
{
    m_noise = noiseLevel;
}

// ------------------------------------------------------------------------------

void colour_image_masking::setBackgroundComponents(int bgComponents)
{
    m_backgroundComponents = bgComponents;
}

// ------------------------------------------------------------------------------

void colour_image_masking::setForegroundComponents(int fgComponents)
{
    m_foregroundComponents = fgComponents;
}

// ------------------------------------------------------------------------------

void colour_image_masking::clearMaskTL()
{
    auto videoMaskTL = m_videoMaskTL.lock();
    videoMaskTL->clearTimeline();
    auto sigTLCleared = videoMaskTL->signal<data::timeline::signals::cleared_t>(data::timeline::signals::CLEARED);
    sigTLCleared->async_emit();
    m_lastVideoTimestamp = 0.;
}

// ------------------------------------------------------------------------------

} // namespace sight::module::filter::vision
