/************************************************************************
 *
 * Copyright (C) 2019-2024 IRCAD France
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

#include <core/com/signal.hxx>
#include <core/com/slots.hxx>

#include <data/helper/medical_image.hpp>

#include <geometry/vision/helper.hpp>

#include <io/opencv/camera.hpp>
#include <io/opencv/image.hpp>
#include <io/opencv/matrix.hpp>
#include <io/opencv/point_list.hpp>

#include <service/macros.hpp>

#include <ui/__/preferences.hpp>

#include <opencv2/calib3d.hpp>
#include <opencv2/opencv.hpp>

namespace sight::module::geometry::vision
{

static const core::com::signals::key_t ERROR_COMPUTED_SIG = "error_computed";

static const core::com::slots::key_t TOGGLE_DISTORTION_SLOT      = "toggle_distortion";
static const core::com::slots::key_t UPDATE_CHESSBOARD_SIZE_SLOT = "update_chessboard_size";

//-----------------------------------------------------------------------------

chessboard_reprojection::chessboard_reprojection()
{
    new_slot(TOGGLE_DISTORTION_SLOT, &chessboard_reprojection::toggle_distortion, this);
    new_slot(UPDATE_CHESSBOARD_SIZE_SLOT, &chessboard_reprojection::update_chessboard_size, this);

    m_error_computed_sig = new_signal<error_computed_t>(ERROR_COMPUTED_SIG);
}

//-----------------------------------------------------------------------------

chessboard_reprojection::~chessboard_reprojection()
= default;

//-----------------------------------------------------------------------------

void chessboard_reprojection::configuring()
{
    const config_t config_tree  = this->get_config();
    const config_t board_config = config_tree.get_child("board");
    const config_t config       = config_tree.get_child("config.<xmlattr>");

    m_width_key = board_config.get<std::string>("<xmlattr>.width");
    SIGHT_ASSERT("Missing board width preference key.", !m_width_key.empty());
    m_height_key = board_config.get<std::string>("<xmlattr>.height");
    SIGHT_ASSERT("Missing board height preference key.", !m_height_key.empty());
    m_square_size_key = board_config.get<std::string>("<xmlattr>.squareSize");
    SIGHT_ASSERT("Missing board square size preference key.", !m_square_size_key.empty());

    const std::string output_key = config_tree.get_optional<std::string>("out.<xmlattr>.key").get_value_or("");
    if(output_key == CHESSBOARD_MODEL_OUTPUT)
    {
        m_has_output_chessboard = true;
    }

    m_distort_reprojection = config.get<bool>("distortReprojection", true);
    m_draw_reprojection    = config.get<bool>("drawReprojection", true);
    m_draw_detected        = config.get<bool>("drawDetected", true);
}

//-----------------------------------------------------------------------------

void chessboard_reprojection::starting()
{
    this->update_chessboard_size();
}

//-----------------------------------------------------------------------------

void chessboard_reprojection::updating()
{
    const auto detected_chessboard = m_detected_chessboard.lock();
    SIGHT_ASSERT("Missing 'detectedChessboard'.", detected_chessboard);

    if(detected_chessboard->get_points().empty())
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
    std::copy(detected_pts.begin(), detected_pts.end(), std::back_inserter(detected_points_f));

    double rmse = -1.;
    std::vector<cv::Point2f> reprojected_pts;

    if(camera->get_is_calibrated() && !detected_points_f.empty())
    {
        const auto transform = m_transform.lock();
        SIGHT_ASSERT("Missing 'transform'.", transform);

        io::opencv::matrix::copy_to_cv(transform.get_shared(), rvec, tvec);

        std::tie(rmse, reprojected_pts) = sight::geometry::vision::helper::compute_reprojection_error(
            m_chessboard_model,
            detected_points_f,
            rvec,
            tvec,
            camera_mx,
            distortion_coefficients
        );

        m_error_computed_sig->async_emit(rmse);
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
                        cv::Mat(m_chessboard_model),
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
            auto sig =
                video_image->signal<data::image::buffer_modified_signal_t>(data::image::BUFFER_MODIFIED_SIG);

            sig->async_emit();
        }
    }
}

//-----------------------------------------------------------------------------

void chessboard_reprojection::stopping()
{
    m_chessboard_model_out.reset();
}

//-----------------------------------------------------------------------------

void chessboard_reprojection::toggle_distortion()
{
    m_distort_reprojection = !m_distort_reprojection;
}

//------------------------------------------------------------------------------

void chessboard_reprojection::update_chessboard_size()
{
    std::uint64_t width(1);
    std::uint64_t height(1);
    double square_size(0.);

    try
    {
        ui::preferences preferences;
        width       = preferences.get(m_width_key, width);
        height      = preferences.get(m_height_key, height);
        square_size = preferences.get(m_square_size_key, square_size);
    }
    catch(const ui::preferences_disabled&)
    {
        // Nothing to do..
    }

    m_chessboard_model.clear();

    data::point_list::sptr chessboard_model_pl = std::make_shared<data::point_list>();

    for(std::uint64_t i = 0 ; i < height - 1 ; ++i)
    {
        const double x = double(i) * square_size;

        for(std::uint64_t j = 0 ; j < width - 1 ; ++j)
        {
            const double y = double(j) * square_size;
            m_chessboard_model.push_back(cv::Point3d(x, y, 0.));
            chessboard_model_pl->push_back(std::make_shared<data::point>(x, y, 0.));
        }
    }

    if(m_has_output_chessboard)
    {
        m_chessboard_model_out = chessboard_model_pl;
    }
}

//-----------------------------------------------------------------------------

service::connections_t chessboard_reprojection::auto_connections() const
{
    return {
        {TRANSFORM_INPUT, data::matrix4::MODIFIED_SIG, service::slots::UPDATE},
        {DETECTED_CHESSBOARD_INPUT, data::point_list::MODIFIED_SIG, service::slots::UPDATE},
        {CAMERA_INPUT, data::camera::INTRINSIC_CALIBRATED_SIG, service::slots::UPDATE},
        {CAMERA_INPUT, data::camera::MODIFIED_SIG, service::slots::UPDATE}
    };
}

//-----------------------------------------------------------------------------

} //namespace sight::module::geometry::vision
