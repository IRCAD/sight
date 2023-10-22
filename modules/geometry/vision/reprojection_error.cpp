/************************************************************************
 *
 * Copyright (C) 2017-2023 IRCAD France
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

#include "reprojection_error.hpp"

#include <core/com/signal.hxx>
#include <core/com/slots.hxx>

#include <geometry/vision/helper.hpp>

#include <io/opencv/camera.hpp>
#include <io/opencv/image.hpp>

#include <opencv2/calib3d.hpp>
#include <opencv2/opencv.hpp>

namespace sight::module::geometry::vision
{

const core::com::slots::key_t reprojection_error::COMPUTE_SLOT       = "compute";
const core::com::slots::key_t reprojection_error::SET_PARAMETER_SLOT = "setParameter";

static const core::com::signals::key_t ERROR_COMPUTED_SIG = "errorComputed";

//-----------------------------------------------------------------------------

reprojection_error::reprojection_error() :

    m_cvColor(cv::Scalar(255, 255, 255, 255))
{
    new_signal<error_computed_signal_t>(ERROR_COMPUTED_SIG);

    new_slot(COMPUTE_SLOT, &reprojection_error::compute, this);
    new_slot(SET_PARAMETER_SLOT, &reprojection_error::setParameter, this);
}

//-----------------------------------------------------------------------------

void reprojection_error::configuring()
{
    service::config_t config = this->get_config();
    m_patternWidth = config.get<double>("patternWidth", m_patternWidth);
    SIGHT_ASSERT("patternWidth setting is set to " << m_patternWidth << " but should be > 0.", m_patternWidth > 0);

    auto in_cfg = config.equal_range("in");
    for(auto it_cfg = in_cfg.first ; it_cfg != in_cfg.second ; ++it_cfg)
    {
        const auto group = it_cfg->second.get<std::string>("<xmlattr>.group", "");
        if(group == s_MATRIX_INPUT)
        {
            auto key_cfg = it_cfg->second.equal_range("key");
            for(auto it_key_cfg = key_cfg.first ; it_key_cfg != key_cfg.second ; ++it_key_cfg)
            {
                const auto key = it_key_cfg->second.get<std::string>("<xmlattr>.id");
                m_matricesTag.push_back(key);
            }

            break;
        }
    }
}

//-----------------------------------------------------------------------------

void reprojection_error::starting()
{
    //3D Points
    const float half_width = static_cast<float>(m_patternWidth) * .5F;

    m_objectPoints.emplace_back(-half_width, half_width, 0.F);
    m_objectPoints.emplace_back(half_width, half_width, 0.F);
    m_objectPoints.emplace_back(half_width, -half_width, 0.F);
    m_objectPoints.emplace_back(-half_width, -half_width, 0.F);

    //TODO: Add an option to use a chessboard instead of a marker
    // --> configure height, width and square size(in mm)

    auto camera = m_camera.lock();
    SIGHT_ASSERT("Camera is not found", camera);

    cv::Size img_size;
    std::tie(m_cameraMatrix, img_size, m_distorsionCoef) = io::opencv::camera::copyToCv(camera.get_shared());

    m_cvExtrinsic = cv::Mat::eye(4, 4, CV_64F);

    auto extrinsic = m_extrinsic.lock();
    if(extrinsic)
    {
        for(std::uint8_t i = 0 ; i < 4 ; ++i)
        {
            for(std::uint8_t j = 0 ; j < 4 ; ++j)
            {
                m_cvExtrinsic.at<double>(i, j) = (*extrinsic)(i, j);
            }
        }
    }
}

//-----------------------------------------------------------------------------

void reprojection_error::stopping()
{
    m_objectPoints.clear();
}

//-----------------------------------------------------------------------------

void reprojection_error::compute(core::hires_clock::type _timestamp)
{
    if(!this->started())
    {
        return;
    }

    if(_timestamp > m_lastTimestamp)
    {
        std::vector<sight::geometry::vision::helper::error_and_points_t> errors;

        {
            auto marker_map = m_markerMap.lock();

            // For each matrix
            unsigned int i = 0;
            for(const auto& marker_key : m_matricesTag)
            {
                const auto* marker = marker_map->getMarker(marker_key);

                if(marker != nullptr)
                {
                    std::vector<cv::Point2f> points2_d;

                    cv::Mat mat = cv::Mat::eye(4, 4, CV_64F);
                    {
                        auto matrix = m_matrix[i].lock();
                        for(std::uint8_t r = 0 ; r < 4 ; ++r)
                        {
                            for(std::uint8_t c = 0 ; c < 4 ; ++c)
                            {
                                mat.at<double>(r, c) = static_cast<double>((*matrix)(r, c));
                            }
                        }
                    }

                    const cv::Mat pose = m_cvExtrinsic * mat;

                    cv::Mat rot = pose(cv::Rect(0, 0, 3, 3));

                    cv::Mat tvec = cv::Mat(3, 1, CV_64F);
                    tvec.at<double>(0) = pose.at<double>(0, 3);
                    tvec.at<double>(1) = pose.at<double>(1, 3);
                    tvec.at<double>(2) = pose.at<double>(2, 3);

                    cv::Mat rvec;

                    cv::Rodrigues(rot, rvec);

                    for(const auto& p : *marker)
                    {
                        points2_d.emplace_back(p[0], p[1]);
                    }

                    sight::geometry::vision::helper::error_and_points_t err_p =
                        sight::geometry::vision::helper::compute_reprojection_error(
                            m_objectPoints,
                            points2_d,
                            rvec,
                            tvec,
                            m_cameraMatrix,
                            m_distorsionCoef
                        );

                    this->signal<error_computed_signal_t>(ERROR_COMPUTED_SIG)->async_emit(err_p.first);

                    errors.push_back(err_p);
                }

                ++i;
            }
        }

        // draw reprojected points
        if(m_display)
        {
            for(const auto& err : errors)
            {
                auto frame = m_frame.lock();
                SIGHT_ASSERT("The input " << s_FRAME_INOUT << " is not valid.", frame);

                if(frame->getSizeInBytes() > 0)
                {
                    cv::Mat cv_image = io::opencv::image::move_to_cv(frame.get_shared());

                    std::vector<cv::Point2f> reprojected_p = err.second;

                    for(auto& j : reprojected_p)
                    {
                        cv::circle(cv_image, j, 7, m_cvColor, 1, cv::LINE_8);
                    }
                }
            }
        }
    }
}

//-----------------------------------------------------------------------------

void reprojection_error::setParameter(sight::ui::parameter_t _val, std::string _key)
{
    if(_key == "display")
    {
        m_display = std::get<bool>(_val);
    }
    else if(_key == "color")
    {
        const auto color = std::get<sight::ui::color_t>(_val);
        m_cvColor = cv::Scalar(color[0], color[1], color[2], 255);
    }
    else
    {
        SIGHT_ERROR("the key '" + _key + "' is not handled");
    }
}

//-----------------------------------------------------------------------------

void reprojection_error::updating()
{
    // When working with a frame (newest design), we do not rely on the timetamp
    // So we can just send the current one.
    // When removing timelines from the service then we could get rid of it
    auto timestamp = core::hires_clock::get_time_in_milli_sec();
    this->compute(timestamp);
}

//-----------------------------------------------------------------------------

service::connections_t reprojection_error::auto_connections() const
{
    connections_t connections;
    connections.push(s_MATRIX_INPUT, data::object::MODIFIED_SIG, service::slots::UPDATE);
    return connections;
}

//-----------------------------------------------------------------------------

} //namespace sight::module::geometry::vision
