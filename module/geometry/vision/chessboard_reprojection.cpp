/************************************************************************
 *
 * Copyright (C) 2019-2026 IRCAD France
 * Copyright (C) 2019-2021 IHU Strasbourg
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

#include "chessboard_reprojection.hpp"

#include "core/spy_log.hpp"

#include <data/helper/medical_image.hpp>

#include <geometry/vision/helper.hpp>

#include <io/opencv/camera.hpp>
#include <io/opencv/image.hpp>
#include <io/opencv/matrix.hpp>
#include <io/opencv/point_list.hpp>

#include <opencv2/calib3d.hpp>
#include <opencv2/core/types.hpp>

#include <cstddef>
#include <opencv2/imgproc.hpp>

namespace sight::module::geometry::vision
{

//-----------------------------------------------------------------------------

chessboard_reprojection::chessboard_reprojection()
{
    new_slot(slots::TOGGLE_DISTORTION, &chessboard_reprojection::toggle_distortion, this);
    new_slot(slots::UPDATE_CHESSBOARD_MODEL, &chessboard_reprojection::update_chessboard_model, this);

    new_signal<signals::error_computed_t>(signals::ERROR_COMPUTED);
}

//-----------------------------------------------------------------------------

void chessboard_reprojection::configuring()
{
    const config_t config_tree = this->get_config();
    const config_t config      = config_tree.get_child("config.<xmlattr>");

    m_distort_reprojection = config.get<bool>("distortReprojection", true);
    m_draw_reprojection    = config.get<bool>("drawReprojection", true);
    m_draw_detected        = config.get<bool>("drawDetected", true);
}

//-----------------------------------------------------------------------------

void chessboard_reprojection::starting()
{
}

//-----------------------------------------------------------------------------

void chessboard_reprojection::updating()
{
    if(m_chessboard_model_3d.empty())
    {
        update_chessboard_model();
    }

    const auto detected_chessboard = m_detected_chessboard.lock();
    SIGHT_ASSERT("Missing 'detectedChessboard'.", detected_chessboard);

    if(detected_chessboard->empty())
    {
        return;
    }

    const auto camera = m_camera.lock();
    SIGHT_ASSERT("Missing 'camera'.", camera);

    cv::Size img_size;
    cv::Mat camera_mx;
    cv::Mat distortion_coefficients;
    std::tie(camera_mx, img_size, distortion_coefficients) = io::opencv::camera::copy_to_cv(camera.get_shared());

    cv::Mat rvec;
    cv::Mat tvec;

    std::vector<cv::Point2d> detected_pts;
    io::opencv::point_list::copy_to_cv(detected_chessboard.get_shared(), detected_pts);

    // Cast Point2d to Point2f ...
    std::vector<cv::Point2f> detected_points_f;
    std::ranges::copy(detected_pts, std::back_inserter(detected_points_f));

    double rmse = -1.;
    std::vector<cv::Point2f> reprojected_pts;

    if(camera->get_is_calibrated() && !detected_points_f.empty())
    {
        const auto transform = m_transform.lock();
        SIGHT_ASSERT("Missing 'transform'.", transform);

        io::opencv::matrix::copy_to_cv(*transform, rvec, tvec);

        std::tie(rmse, reprojected_pts) = sight::geometry::vision::helper::compute_reprojection_error(
            m_chessboard_model_3d,
            detected_points_f,
            rvec,
            tvec,
            camera_mx,
            distortion_coefficients
        );

        async_emit(signals::ERROR_COMPUTED, rmse);
    }

    const auto video_image = m_video_image.lock();
    SIGHT_ERROR_IF(
        "Drawing is enabled in the configuration but there is no 'videoImage' to draw onto.",
        !video_image && (m_draw_detected || m_draw_reprojection || m_draw_reprojection_error)
    );

    if(video_image)
    {
        // Reprojected points have a radius equal to 1/3000th of the image's height.
        int reprojection_radius = static_cast<int>(std::floor(0.003 * img_size.height));
        reprojection_radius = std::max(reprojection_radius, 1);

        if(!data::helper::medical_image::check_image_validity(video_image.get_shared()))
        {
            return;
        }

        cv::Mat img = io::opencv::image::move_to_cv(video_image.get_shared());

        const bool drawing_enabled = m_draw_detected || m_draw_reprojection || m_draw_reprojection_error;
        SIGHT_WARN_IF(
            "An inout 'videoImage' was given to the service but no drawing operation was enabled.",
            !drawing_enabled
        );

        if(m_draw_reprojection)
        {
            std::vector<cv::Point2f> drawn_detected_points;
            if(!m_distort_reprojection && camera->get_is_calibrated())
            {
                cv::undistortPoints(
                    cv::Mat(detected_points_f),
                    drawn_detected_points,
                    camera_mx,
                    distortion_coefficients
                );
                for(auto& pt : drawn_detected_points)
                {
                    const auto pt3d = cv::Matx33f(camera_mx) * pt;
                    pt = cv::Point2f(pt3d.x, pt3d.y);
                }
            }
            else
            {
                drawn_detected_points = detected_points_f;
            }

            const int detection_thickness = reprojection_radius < 2 ? 1 : 2;
            for(const auto& pt : drawn_detected_points)
            {
                cv::circle(img, pt, reprojection_radius + 3, cv::Scalar(0, 255, 255, 255), detection_thickness);
            }
        }

        if(rmse >= 0.)
        {
            if(m_draw_reprojection)
            {
                std::vector<cv::Point2f> drawn_reprojected_pts;
                if(m_distort_reprojection)
                {
                    drawn_reprojected_pts = reprojected_pts;
                }
                else
                {
                    // Project the model but assume the image isn't distorted.
                    cv::projectPoints(
                        cv::Mat(m_chessboard_model_3d),
                        rvec,
                        tvec,
                        camera_mx,
                        cv::Mat(),
                        drawn_reprojected_pts
                    );
                }

                for(const auto& pt : drawn_reprojected_pts)
                {
                    cv::circle(img, pt, reprojection_radius, cv::Scalar(255, 255, 0, 255), cv::FILLED);
                }
            }

            if(m_draw_reprojection_error)
            {
                const auto font_face                     = cv::FONT_HERSHEY_SIMPLEX;
                const std::string reprojection_error_str = "Reprojection rmse: " + std::to_string(rmse) + " pixels";
                const int left_padding                   = static_cast<int>(0.05 * img_size.width);
                const int top_padding                    = static_cast<int>(0.05 * img_size.height);

                cv::putText(
                    img,
                    reprojection_error_str,
                    cv::Point(left_padding, top_padding),
                    font_face,
                    1.,
                    cv::Scalar(255, 255, 0, 255),
                    2
                );
            }
        }

        if(drawing_enabled)
        {
            video_image->async_emit(data::image::signals::BUFFER_MODIFIED);
        }
    }
}

//-----------------------------------------------------------------------------

void chessboard_reprojection::stopping()
{
}

//-----------------------------------------------------------------------------

void chessboard_reprojection::toggle_distortion()
{
    m_distort_reprojection = !m_distort_reprojection;
}

//-----------------------------------------------------------------------------

void chessboard_reprojection::update_chessboard_model()
{
    const auto chessboard_model = m_chessboard_model.lock();
    SIGHT_ASSERT("Missing 'chessboard_model'.", chessboard_model);
    SIGHT_ASSERT("chessboard_model doesn't contain any points", !chessboard_model->empty());
    m_chessboard_model_3d.clear();
    m_chessboard_model_3d.resize(chessboard_model->size());
    std::size_t i = 0;
    for(const auto& pt : *chessboard_model)
    {
        cv::Point3f pt3d;
        pt3d.x                   = static_cast<float>((*pt)[0]);
        pt3d.y                   = static_cast<float>((*pt)[1]);
        pt3d.z                   = static_cast<float>((*pt)[2]);
        m_chessboard_model_3d[i] = pt3d;
        ++i;
    }
}

//-----------------------------------------------------------------------------

service::connections_t chessboard_reprojection::auto_connections() const
{
    return {
        {TRANSFORM_INPUT, data::signals::MODIFIED, service::slots::UPDATE},
        {DETECTED_CHESSBOARD_INPUT, data::signals::MODIFIED, service::slots::UPDATE},
        {CAMERA_INPUT, data::camera::signals::INTRINSIC_CALIBRATED, service::slots::UPDATE},
        {CAMERA_INPUT, data::signals::MODIFIED, service::slots::UPDATE},
        {CHESSBOARD_MODEL, data::signals::MODIFIED, slots::UPDATE_CHESSBOARD_MODEL}
    };
}

//-----------------------------------------------------------------------------

} //namespace sight::module::geometry::vision
