/************************************************************************
 *
 * Copyright (C) 2018-2023 IRCAD France
 * Copyright (C) 2018-2021 IHU Strasbourg
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

// cspell:ignore NOLINTNEXTLINE

#include "modules/filter/vision/optical_flow.hpp"

#include <core/com/signal.hxx>
#include <core/com/slots.hxx>

#include <io/opencv/frame_tl.hpp>

#include <opencv2/highgui.hpp>
#include <opencv2/video/tracking.hpp>

namespace sight::module::filter::vision
{

static const core::com::signals::key_t CAMERA_MOVED_SIG    = "cameraMoved";
static const core::com::signals::key_t CAMERA_REMAINED_SIG = "cameraRemained";

// ----------------------------------------------------------------------------

optical_flow::optical_flow() noexcept
{
    m_motionSignal   = new_signal<motion_signal_t>(CAMERA_MOVED_SIG);
    m_noMotionSignal = new_signal<no_motion_signal_t>(CAMERA_REMAINED_SIG);
}

// ----------------------------------------------------------------------------

optical_flow::~optical_flow() noexcept =
    default;

// ----------------------------------------------------------------------------

void optical_flow::configuring()
{
    const auto config_tree = this->get_config();
    const auto config      = config_tree.get_child_optional("config.<xmlattr>");

    if(config)
    {
        m_latency          = config->get<unsigned int>("latency", m_latency);
        m_imageScaleFactor = config->get<float>("scaleFactor", m_imageScaleFactor);
    }
}

// ----------------------------------------------------------------------------

void optical_flow::starting()
{
    m_initialization = false;
    m_motion         = false;
}

// ----------------------------------------------------------------------------

void optical_flow::updating()
{
    if(!this->started())
    {
        SIGHT_ERROR("Cannot call `update` when service is stopped.");
        return;
    }

    cv::Mat temp_img;
    cv::Mat gray_img;

    // Scope to lock frameTL
    {
        const auto frame_tl = m_timeline.lock();
        SIGHT_ASSERT(" Input " << s_FRAME_TIMELINE_INPUT << " cannot be null", frame_tl);
        core::hires_clock::type timestamp = frame_tl->getNewerTimestamp();
        if(timestamp < m_lastTimestamp + m_latency)
        {
            return;
        }

        m_lastTimestamp = timestamp;

        CSPTR(data::frame_tl::buffer_t) buffer = frame_tl->getClosestBuffer(frame_tl->getNewerTimestamp());
        // NOLINTNEXTLINE(cppcoreguidelines-pro-type-const-cast)
        auto* frame_buff = const_cast<std::uint8_t*>(&buffer->getElement(0));

        temp_img = io::opencv::frame_tl::move_to_cv(frame_tl.get_shared(), frame_buff);

        // Use a specific size for images:
        // good balance between computation time & image quality for features tracking.
        const cv::MatSize size_of_frame = temp_img.size;
        cv::Size s;

        // If image is small enough no need to re-scale it.
        if(size_of_frame[0] > 640)
        {
            s.width  = static_cast<int>(std::round(static_cast<float>(size_of_frame[0]) / m_imageScaleFactor));
            s.height = static_cast<int>(std::round(static_cast<float>(size_of_frame[1]) / m_imageScaleFactor));
            cv::resize(temp_img, temp_img, s);
        }

        if(frame_tl->numComponents() == 1)
        {
            gray_img = temp_img;
        }
        else if(frame_tl->numComponents() == 3)
        {
            cv::cvtColor(temp_img, gray_img, cv::COLOR_RGB2GRAY);
        }
        else if(frame_tl->numComponents() == 4)
        {
            cv::cvtColor(temp_img, gray_img, cv::COLOR_RGBA2GRAY);
        }
        else
        {
            SIGHT_FATAL("Wrong type of image (nb of components = " << frame_tl->numComponents() << ").");
        }
    }

    if(!m_initialization)
    {
        m_mainMutex.lock();

        m_lastGrayImg = gray_img;
        // Detect "good" features in frame. (parameters coming from opencv/samples/cpp/lkdemo.cpp).
        cv::goodFeaturesToTrack(m_lastGrayImg, m_lastCorners, 2000, 0.01, 10);
        m_initialization = true;

        m_mainMutex.unlock();

        return;
    }

    cv::Mat current_corners;
    cv::Vec2f corners_diff;
    std::vector<uchar> status;
    std::vector<float> err;
    int acc         = 0; // Incremented each time the flow of a feature has been found.
    long double rms = 0; // Root mean square difference between each detected corners.
    int n_move      = 0; // Incremented each time a corners has moved.

    m_mainMutex.lock();

    SIGHT_ASSERT(
        "last image and current image should have same size: " << m_lastGrayImg.size << " , "
        << gray_img.size
        ,
        m_lastGrayImg.size == gray_img.size
    );

    // Optical flow (Lucas-Kanade version).
    cv::calcOpticalFlowPyrLK(m_lastGrayImg, gray_img, m_lastCorners, current_corners, status, err);

    for(int index = 0 ; index < m_lastCorners.size().height ; index++)
    {
        // Check if flow for feature 'index' has been found.
        // Opencv doc for 'status' vector: Each element of the vector is set to 1 if
        // the flow for the corresponding features has been found, otherwise, it is set to 0.
        if(status[static_cast<std::size_t>(index)] != 0U)
        {
            corners_diff = m_lastCorners.at<cv::Vec2f>(index) - current_corners.at<cv::Vec2f>(index);
            rms         +=
                static_cast<long double>(corners_diff[0] * corners_diff[0] + corners_diff[1] * corners_diff[1]);

            // Check if corners has moved.
            if((corners_diff[0] * corners_diff[0] + corners_diff[1] * corners_diff[1]) > 2)
            {
                ++n_move;
            }

            ++acc;
        }
    }

    if(acc != 0)
    {
        rms = rms / static_cast<long double>(acc);
        rms = std::sqrt(rms);
    }

    // If movement is > 100 pixel and at least 80% of detected points has moved:
    // we can say that the camera is moving (values find empirically).
    if((rms > 100) && ((static_cast<float>(n_move) / (static_cast<float>(acc))) > 0.8F) && !m_motion)
    {
        m_motion = !m_motion;
        m_motionSignal->async_emit();
    }
    // No movement or movement on few points (ex: an object moving on the video):
    // we can assume that camera is not moving.
    else if((rms < 1 || ((static_cast<float>(n_move) / (static_cast<float>(acc))) < 0.5F)) && m_motion)
    {
        m_motion = !m_motion;
        m_noMotionSignal->async_emit();
    }

    // Keep last image.
    m_lastGrayImg = gray_img;
    // Detect "good" features in the frame. (parameters coming from opencv/samples/cpp/lkdemo.cpp).
    cv::goodFeaturesToTrack(m_lastGrayImg, m_lastCorners, 2000, 0.01, 10);

    m_mainMutex.unlock();
}

// ----------------------------------------------------------------------------

void optical_flow::stopping()
{
    m_initialization = false;
    m_motion         = false;
}

// ----------------------------------------------------------------------------

service::connections_t optical_flow::auto_connections() const
{
    connections_t connections;

    connections.push(s_FRAME_TIMELINE_INPUT, data::timeline::signals::PUSHED, service::slots::UPDATE);

    return connections;
}

} //namespace sight::module::filter::vision
