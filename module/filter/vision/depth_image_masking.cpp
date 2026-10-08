/************************************************************************
 *
 * Copyright (C) 2020-2026 IRCAD France
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

#include <io/opencv/image.hpp>

namespace sight::module::filter::vision
{

// ------------------------------------------------------------------------------

depth_image_masking::depth_image_masking() noexcept :
    filter(has_signals::signals())
{
    new_slot(slots::SET_BACKGROUND, &depth_image_masking::set_background, this);
    new_slot(slots::SET_THRESHOLD, &depth_image_masking::set_threshold, this);
}

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

    connections.push(DEPTH_IMAGE_KEY, data::image::signals::BUFFER_MODIFIED, service::slots::UPDATE);

    return connections;
}

// ------------------------------------------------------------------------------

void depth_image_masking::updating()
{
    if(!m_cv_depth_mask_image.empty())
    {
        const auto video_image = m_video_image.lock();
        SIGHT_ASSERT("No '" << VIDEO_IMAGE_KEY << "' found.", video_image);
        const auto depth_image = m_depth_image.lock();
        SIGHT_ASSERT("No '" << DEPTH_IMAGE_KEY << "' found.", depth_image);

        const cv::Mat cv_video_image = io::opencv::image::move_to_cv(video_image.get_shared());
        const cv::Mat cv_depth_image = io::opencv::image::move_to_cv(depth_image.get_shared());

        cv::Mat cv_masked_depth;
        cv_depth_image.copyTo(cv_masked_depth, m_cv_mask_image);

        cv::Mat cv_foreground_image = (cv_masked_depth < (m_cv_depth_mask_image - m_threshold));

        cv::Mat morph_elem = cv::getStructuringElement(cv::MORPH_ELLIPSE, cv::Size(7, 7));
        cv::dilate(cv_foreground_image, cv_foreground_image, morph_elem);
        cv::erode(cv_foreground_image, cv_foreground_image, morph_elem);

        cv::Mat cv_masked_video = cv::Mat::zeros(cv_video_image.rows, cv_video_image.cols, cv_video_image.type());
        cv_video_image.copyTo(cv_masked_video, cv_foreground_image);

        auto foreground_image = m_foreground_image.lock();
        SIGHT_ASSERT("No '" << FOREGROUND_IMAGE_KEY << "' found.", foreground_image);

        io::opencv::image::copy_from_cv(*foreground_image, cv_masked_video);

        foreground_image->async_emit(data::image::signals::BUFFER_MODIFIED);

        this->async_emit(signals::SUCCEEDED);
    }
}

// ------------------------------------------------------------------------------

void depth_image_masking::set_background()
{
    const auto mask_image = m_mask_image.lock();
    SIGHT_ASSERT("No '" << MASK_IMAGE_KEY << "' found.", mask_image);
    const auto depth_image = m_depth_image.lock();
    SIGHT_ASSERT("No '" << DEPTH_IMAGE_KEY << "' found.", depth_image);

    if(mask_image && depth_image && (mask_image->type() != core::type::NONE)
       && (depth_image->type() != core::type::NONE))
    {
        const cv::Mat cv_depth_image = io::opencv::image::move_to_cv(depth_image.get_shared());
        m_cv_mask_image = io::opencv::image::move_to_cv(mask_image.get_shared());
        if(m_cv_mask_image.channels() == 4)
        {
            cv::cvtColor(m_cv_mask_image, m_cv_mask_image, cv::COLOR_BGRA2GRAY);
        }
        else if(m_cv_mask_image.channels() == 3)
        {
            cv::cvtColor(m_cv_mask_image, m_cv_mask_image, cv::COLOR_BGR2GRAY);
        }

        m_cv_mask_image = (m_cv_mask_image > 0);

        if(m_cv_depth_mask_image.empty())
        {
            m_cv_depth_mask_image = cv::Mat::zeros(cv_depth_image.rows, cv_depth_image.cols, cv_depth_image.type());
        }

        cv_depth_image.copyTo(m_cv_depth_mask_image, m_cv_mask_image);
    }
}

// ------------------------------------------------------------------------------

void depth_image_masking::set_threshold(int _threshold)
{
    m_threshold = _threshold;
}

// ------------------------------------------------------------------------------

} // namespace sight::module::filter::vision
