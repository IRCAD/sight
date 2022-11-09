/************************************************************************
 *
 * Copyright (C) 2018-2022 IRCAD France
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

#include "modules/filter/vision/SOpticalFlow.hpp"

#include <core/com/Signal.hxx>
#include <core/com/Slots.hxx>

#include <io/opencv/FrameTL.hpp>

#include <opencv2/highgui.hpp>
#include <opencv2/video/tracking.hpp>

namespace sight::module::filter::vision
{

static const core::com::Signals::SignalKeyType s_CAMERA_MOVED_SIG    = "cameraMoved";
static const core::com::Signals::SignalKeyType s_CAMERA_REMAINED_SIG = "cameraRemained";

// ----------------------------------------------------------------------------

SOpticalFlow::SOpticalFlow() noexcept
{
    m_motionSignal   = newSignal<MotionSignalType>(s_CAMERA_MOVED_SIG);
    m_noMotionSignal = newSignal<NoMotionSignalType>(s_CAMERA_REMAINED_SIG);
}

// ----------------------------------------------------------------------------

SOpticalFlow::~SOpticalFlow() noexcept =
    default;

// ----------------------------------------------------------------------------

void SOpticalFlow::configuring()
{
    const auto configTree = this->getConfiguration();
    const auto config     = configTree.get_child_optional("config.<xmlattr>");

    if(config)
    {
        m_latency          = config->get<unsigned int>("latency", m_latency);
        m_imageScaleFactor = config->get<float>("scaleFactor", m_imageScaleFactor);
    }
}

// ----------------------------------------------------------------------------

void SOpticalFlow::starting()
{
    m_initialization = false;
    m_motion         = false;
}

// ----------------------------------------------------------------------------

void SOpticalFlow::updating()
{
    if(!this->isStarted())
    {
        SIGHT_ERROR("Cannot call `update` when service is stopped.");
        return;
    }

    cv::Mat tempImg;
    cv::Mat grayImg;

    // Scope to lock frameTL
    {
        const auto frameTL = m_timeline.lock();
        SIGHT_ASSERT(" Input " << s_FRAME_TIMELINE_INPUT << " cannot be null", frameTL);
        core::HiResClock::HiResClockType timestamp = frameTL->getNewerTimestamp();
        if(timestamp < m_lastTimestamp + m_latency)
        {
            return;
        }

        m_lastTimestamp = timestamp;

        CSPTR(data::FrameTL::BufferType) buffer = frameTL->getClosestBuffer(frameTL->getNewerTimestamp());
        // NOLINTNEXTLINE(cppcoreguidelines-pro-type-const-cast)
        auto* frameBuff = const_cast<std::uint8_t*>(&buffer->getElement(0));

        tempImg = io::opencv::FrameTL::moveToCv(frameTL.get_shared(), frameBuff);

        // Use a specific size for images:
        // good balance between computation time & image quality for features tracking.
        const cv::MatSize sizeOfFrame = tempImg.size;
        cv::Size s;

        // If image is small enough no need to re-scale it.
        if(sizeOfFrame[0] > 640)
        {
            s.width  = static_cast<int>(std::round(static_cast<float>(sizeOfFrame[0]) / m_imageScaleFactor));
            s.height = static_cast<int>(std::round(static_cast<float>(sizeOfFrame[1]) / m_imageScaleFactor));
            cv::resize(tempImg, tempImg, s);
        }

        if(frameTL->numComponents() == 1)
        {
            grayImg = tempImg;
        }
        else if(frameTL->numComponents() == 3)
        {
            cv::cvtColor(tempImg, grayImg, cv::COLOR_RGB2GRAY);
        }
        else if(frameTL->numComponents() == 4)
        {
            cv::cvtColor(tempImg, grayImg, cv::COLOR_RGBA2GRAY);
        }
        else
        {
            SIGHT_FATAL("Wrong type of image (nb of components = " << frameTL->numComponents() << ").");
        }
    }

    if(!m_initialization)
    {
        m_mainMutex.lock();

        m_lastGrayImg = grayImg;
        // Detect "good" features in frame. (parameters coming from opencv/samples/cpp/lkdemo.cpp).
        cv::goodFeaturesToTrack(m_lastGrayImg, m_lastCorners, 2000, 0.01, 10);
        m_initialization = true;

        m_mainMutex.unlock();

        return;
    }

    cv::Mat currentCorners;
    cv::Vec2f cornersDiff;
    std::vector<uchar> status;
    std::vector<float> err;
    int acc         = 0; // Incremented each time the flow of a feature has been found.
    long double RMS = 0; // Root mean square difference between each detected corners.
    int n_move      = 0; // Incremented each time a corners has moved.

    m_mainMutex.lock();

    SIGHT_ASSERT(
        "last image and current image should have same size: " << m_lastGrayImg.size << " , "
        << grayImg.size
        ,
        m_lastGrayImg.size == grayImg.size
    );

    // Optical flow (Lucas-Kanade version).
    cv::calcOpticalFlowPyrLK(m_lastGrayImg, grayImg, m_lastCorners, currentCorners, status, err);

    for(int index = 0 ; index < m_lastCorners.size().height ; index++)
    {
        // Check if flow for feature 'index' has been found.
        // Opencv doc for 'status' vector: Each element of the vector is set to 1 if
        // the flow for the corresponding features has been found, otherwise, it is set to 0.
        if(status[static_cast<std::size_t>(index)] != 0U)
        {
            cornersDiff = m_lastCorners.at<cv::Vec2f>(index) - currentCorners.at<cv::Vec2f>(index);
            RMS        +=
                static_cast<long double>(cornersDiff[0] * cornersDiff[0] + cornersDiff[1] * cornersDiff[1]);

            // Check if corners has moved.
            if((cornersDiff[0] * cornersDiff[0] + cornersDiff[1] * cornersDiff[1]) > 2)
            {
                ++n_move;
            }

            ++acc;
        }
    }

    if(acc != 0)
    {
        RMS = RMS / static_cast<long double>(acc);
        RMS = std::sqrt(RMS);
    }

    // If movement is > 100 pixel and at least 80% of detected points has moved:
    // we can say that the camera is moving (values find empirically).
    if((RMS > 100) && ((static_cast<float>(n_move) / (static_cast<float>(acc))) > 0.8F) && !m_motion)
    {
        m_motion = !m_motion;
        m_motionSignal->asyncEmit();
    }
    // No movement or movement on few points (ex: an object moving on the video):
    // we can assume that camera is not moving.
    else if((RMS < 1 || ((static_cast<float>(n_move) / (static_cast<float>(acc))) < 0.5F)) && m_motion)
    {
        m_motion = !m_motion;
        m_noMotionSignal->asyncEmit();
    }

    // Keep last image.
    m_lastGrayImg = grayImg;
    // Detect "good" features in the frame. (parameters coming from opencv/samples/cpp/lkdemo.cpp).
    cv::goodFeaturesToTrack(m_lastGrayImg, m_lastCorners, 2000, 0.01, 10);

    m_mainMutex.unlock();
}

// ----------------------------------------------------------------------------

void SOpticalFlow::stopping()
{
    m_initialization = false;
    m_motion         = false;
}

// ----------------------------------------------------------------------------

service::IService::KeyConnectionsMap SOpticalFlow::getAutoConnections() const
{
    KeyConnectionsMap connections;

    connections.push(s_FRAME_TIMELINE_INPUT, data::FrameTL::s_OBJECT_PUSHED_SIG, s_UPDATE_SLOT);

    return connections;
}

} //namespace sight::module::filter::vision
