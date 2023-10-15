/************************************************************************
 *
 * Copyright (C) 2020-2023 IRCAD France
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

#include "depth_image_masking.hpp"

#include <core/com/signal.hxx>
#include <core/com/slots.hxx>

#include <io/opencv/image.hpp>

#include <service/macros.hpp>

namespace sight::module::filter::vision
{

static const core::com::slots::key_t SET_BACKGROUND_SLOT = "setBackground";
static const core::com::slots::key_t SET_THRESHOLD_SLOT  = "setThreshold";

// ------------------------------------------------------------------------------

depth_image_masking::depth_image_masking() noexcept
{
    new_slot(SET_BACKGROUND_SLOT, &depth_image_masking::setBackground, this);
    new_slot(SET_THRESHOLD_SLOT, &depth_image_masking::setThreshold, this);
}

// ------------------------------------------------------------------------------

depth_image_masking::~depth_image_masking() noexcept =
    default;

// ------------------------------------------------------------------------------

void depth_image_masking::configuring()
{
}

// ------------------------------------------------------------------------------

void depth_image_masking::starting()
{
}

// ------------------------------------------------------------------------------

void depth_image_masking::stopping()
{
}

//-----------------------------------------------------------------------------

service::connections_t depth_image_masking::auto_connections() const
{
    connections_t connections;

    connections.push(s_DEPTH_IMAGE_KEY, data::image::BUFFER_MODIFIED_SIG, service::slots::UPDATE);

    return connections;
}

// ------------------------------------------------------------------------------

void depth_image_masking::updating()
{
    if(!m_cvDepthMaskImage.empty())
    {
        const auto videoImage = m_videoImage.lock();
        SIGHT_ASSERT("No '" << s_VIDEO_IMAGE_KEY << "' found.", videoImage);
        const auto depthImage = m_depthImage.lock();
        SIGHT_ASSERT("No '" << s_DEPTH_IMAGE_KEY << "' found.", depthImage);

        const cv::Mat cvVideoImage = io::opencv::image::moveToCv(videoImage.get_shared());
        const cv::Mat cvDepthImage = io::opencv::image::moveToCv(depthImage.get_shared());

        cv::Mat cvMaskedDepth;
        cvDepthImage.copyTo(cvMaskedDepth, m_cvMaskImage);

        cv::Mat cvForegroundImage = (cvMaskedDepth < (m_cvDepthMaskImage - m_threshold));

        cv::Mat morphElem = cv::getStructuringElement(cv::MORPH_ELLIPSE, cv::Size(7, 7));
        cv::dilate(cvForegroundImage, cvForegroundImage, morphElem);
        cv::erode(cvForegroundImage, cvForegroundImage, morphElem);

        cv::Mat cvMaskedVideo = cv::Mat::zeros(cvVideoImage.rows, cvVideoImage.cols, cvVideoImage.type());
        cvVideoImage.copyTo(cvMaskedVideo, cvForegroundImage);

        auto foregroundImage = m_foregroundImage.lock();
        SIGHT_ASSERT("No '" << s_FOREGROUND_IMAGE_KEY << "' found.", foregroundImage);

        io::opencv::image::copyFromCv(*foregroundImage, cvMaskedVideo);

        const auto sig = foregroundImage->signal<data::image::BufferModifiedSignalType>(
            data::image::BUFFER_MODIFIED_SIG
        );
        sig->async_emit();

        m_sigComputed->async_emit();
    }
}

// ------------------------------------------------------------------------------

void depth_image_masking::setBackground()
{
    const auto maskImage = m_maskImage.lock();
    SIGHT_ASSERT("No '" << s_MASK_IMAGE_KEY << "' found.", maskImage);
    const auto depthImage = m_depthImage.lock();
    SIGHT_ASSERT("No '" << s_DEPTH_IMAGE_KEY << "' found.", depthImage);

    if(maskImage && depthImage && (maskImage->getType() != core::type::NONE)
       && (depthImage->getType() != core::type::NONE))
    {
        const cv::Mat cvDepthImage = io::opencv::image::moveToCv(depthImage.get_shared());
        m_cvMaskImage = io::opencv::image::moveToCv(maskImage.get_shared());
        if(m_cvMaskImage.channels() == 4)
        {
            cv::cvtColor(m_cvMaskImage, m_cvMaskImage, cv::COLOR_BGRA2GRAY);
        }
        else if(m_cvMaskImage.channels() == 3)
        {
            cv::cvtColor(m_cvMaskImage, m_cvMaskImage, cv::COLOR_BGR2GRAY);
        }

        m_cvMaskImage = (m_cvMaskImage > 0);

        if(m_cvDepthMaskImage.empty())
        {
            m_cvDepthMaskImage = cv::Mat::zeros(cvDepthImage.rows, cvDepthImage.cols, cvDepthImage.type());
        }

        cvDepthImage.copyTo(m_cvDepthMaskImage, m_cvMaskImage);
    }
}

// ------------------------------------------------------------------------------

void depth_image_masking::setThreshold(int _threshold)
{
    m_threshold = _threshold;
}

// ------------------------------------------------------------------------------

} // namespace sight::module::filter::vision
