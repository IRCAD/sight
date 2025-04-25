/************************************************************************
 *
 * Copyright (C) 2014-2025 IRCAD France
 * Copyright (C) 2014-2020 IHU Strasbourg
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

#include "open_cv_intrinsic.hpp"

#include <core/com/slots.hxx>

#include <io/opencv/matrix.hpp>

#include <ui/__/preferences.hpp>

#include <opencv2/calib3d.hpp>
#include <opencv2/core.hpp>

namespace sight::module::geometry::vision
{

static const core::com::slots::key_t UPDATE_CHESSBOARD_SIZE_SLOT = "update_chessboard_size";

//------------------------------------------------------------------------------

void open_cv_intrinsic::configuring()
{
}

// ----------------------------------------------------------------------------

void open_cv_intrinsic::starting()
{
}

// ----------------------------------------------------------------------------

void open_cv_intrinsic::stopping()
{
}

//--------------------------------------------------------------------- ---------

void open_cv_intrinsic::updating()
{
    const auto cal_info = m_calibration_info.lock();

    SIGHT_ASSERT("Object with 'calibrationInfo' is not found", cal_info);
    SIGHT_WARN_IF("Calibration info is empty.", cal_info->get_point_list_container().empty());

    if(!cal_info->get_point_list_container().empty())
    {
        std::vector<std::vector<cv::Point3f> > object_points;

        std::vector<cv::Point3f> points;
        for(unsigned int y = 0 ; y < static_cast<unsigned int>(*m_height - 1) ; ++y)
        {
            for(unsigned int x = 0 ; x < static_cast<unsigned int>(*m_width - 1) ; ++x)
            {
                points.emplace_back(
                    static_cast<float>(x * m_square_size.value()),
                    static_cast<float>(y * m_square_size.value()),
                    0.F
                );
            }
        }

        std::vector<std::vector<cv::Point2f> > image_points;

        for(const data::point_list::csptr& capture : cal_info->get_point_list_container())
        {
            std::vector<cv::Point2f> dst;

            for(data::point::csptr point : capture->get_points())
            {
                SIGHT_ASSERT("point is null", point);
                dst.emplace_back(
                    static_cast<float>((*point)[0]),
                    static_cast<float>((*point)[1])

                );
            }

            image_points.push_back(dst);
            object_points.push_back(points);
        }

        data::image::csptr img = cal_info->get_image_container().front();

        cv::Mat camera_matrix;
        std::vector<float> dist_coeffs;
        std::vector<cv::Mat> rvecs;
        std::vector<cv::Mat> tvecs;
        cv::Size2i imgsize(static_cast<int>(img->size()[0]), static_cast<int>(img->size()[1]));

        const auto pose_camera = m_pose_vector.lock();
        if(pose_camera)
        {
            pose_camera->clear();

            for(std::size_t index = 0 ; index < rvecs.size() ; ++index)
            {
                data::matrix4::sptr mat_3d = std::make_shared<data::matrix4>();

                io::opencv::matrix::copy_from_cv(rvecs.at(index), tvecs.at(index), mat_3d);

                pose_camera->push_back(mat_3d);
                auto sig = pose_camera->signal<data::vector::added_signal_t>(
                    data::vector::ADDED_OBJECTS_SIG
                );
                sig->async_emit(pose_camera->get_content());
            }
        }

        const double err =
            cv::calibrateCamera(object_points, image_points, imgsize, camera_matrix, dist_coeffs, rvecs, tvecs);
        SIGHT_DEBUG("Calibration error :" << err);

        const auto cam = m_camera.lock();

        cam->set_cx(camera_matrix.at<double>(0, 2));
        cam->set_cy(camera_matrix.at<double>(1, 2));
        cam->set_fx(camera_matrix.at<double>(0, 0));
        cam->set_fy(camera_matrix.at<double>(1, 1));
        cam->set_width(img->size()[0]);
        cam->set_height(img->size()[1]);
        cam->set_distortion_coefficient(dist_coeffs[0], dist_coeffs[1], dist_coeffs[2], dist_coeffs[3], dist_coeffs[4]);
        cam->set_calibration_error(err);

        cam->set_is_calibrated(true);

        auto sig = cam->signal<data::camera::intrinsic_calibrated_signal_t>(data::camera::INTRINSIC_CALIBRATED_SIG);
        sig->async_emit();
    }
}

//------------------------------------------------------------------------------

} // namespace sight::module::geometry::vision
