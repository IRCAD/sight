/************************************************************************
 *
 * Copyright (C) 2014-2024 IRCAD France
 * Copyright (C) 2014-2021 IHU Strasbourg
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

#include "open_cv_extrinsic.hpp"

#include <core/com/signal.hxx>
#include <core/com/slot.hxx>
#include <core/com/slots.hxx>

#include <data/calibration_info.hpp>
#include <data/camera.hpp>
#include <data/camera_set.hpp>
#include <data/matrix4.hpp>
#include <data/point_list.hpp>

#include <io/opencv/matrix.hpp>

#include <ui/__/preferences.hpp>

#include <opencv2/calib3d.hpp>
#include <opencv2/core.hpp>

namespace sight::module::geometry::vision
{

static const core::com::signals::key_t ERROR_COMPUTED_SIG = "error_computed";

// ----------------------------------------------------------------------------

open_cv_extrinsic::open_cv_extrinsic() noexcept
{
    new_signal<error_computed_t>(ERROR_COMPUTED_SIG);
}

//------------------------------------------------------------------------------

void open_cv_extrinsic::configuring()
{
    const auto config = this->get_config();
    m_cam_index = config.get<std::size_t>("camIndex");
}

// ----------------------------------------------------------------------------

void open_cv_extrinsic::starting()
{
}

// ----------------------------------------------------------------------------

void open_cv_extrinsic::stopping()
{
}

//------------------------------------------------------------------------------

void open_cv_extrinsic::updating()
{
    const auto cal_info1 = m_calibration_info1.lock();
    const auto cal_info2 = m_calibration_info2.lock();

    SIGHT_ASSERT("Object with 'calibrationInfo1' is not found", cal_info1);
    SIGHT_ASSERT("Object with 'calibrationInfo2' is not found", cal_info2);

    SIGHT_WARN_IF("Calibration info is empty.", cal_info1->get_point_list_container().empty());
    if(!cal_info1->get_point_list_container().empty())
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

        std::vector<std::vector<cv::Point2f> > image_points1;
        std::vector<std::vector<cv::Point2f> > image_points2;
        {
            const auto pt_lists1 = cal_info1->get_point_list_container();
            const auto pt_lists2 = cal_info2->get_point_list_container();

            SIGHT_ERROR_IF("The two calibrationInfo do not have the same size", pt_lists1.size() != pt_lists2.size());

            auto itr1     = pt_lists1.begin();
            auto itr2     = pt_lists2.begin();
            auto itr_end1 = pt_lists1.end();
            auto itr_end2 = pt_lists2.end();

            for( ; itr1 != itr_end1 && itr2 != itr_end2 ; ++itr1, ++itr2)
            {
                const data::point_list::csptr& pt_list1 = *itr1;
                const data::point_list::csptr& pt_list2 = *itr2;
                std::vector<cv::Point2f> img_point1;
                std::vector<cv::Point2f> img_point2;

                for(data::point::csptr point : pt_list1->get_points())
                {
                    SIGHT_ASSERT("point is null", point);
                    img_point1.emplace_back(
                        static_cast<float>(point->get_coord()[0]),
                        static_cast<float>(point->get_coord()[1])

                    );
                }

                for(data::point::csptr point : pt_list2->get_points())
                {
                    SIGHT_ASSERT("point is null", point);
                    img_point2.emplace_back(
                        static_cast<float>(point->get_coord()[0]),
                        static_cast<float>(point->get_coord()[1])

                    );
                }

                image_points1.push_back(img_point1);
                image_points2.push_back(img_point2);
                object_points.push_back(points);
            }
        }

        // Set the cameras
        cv::Mat camera_matrix1 = cv::Mat::eye(3, 3, CV_64F);
        cv::Mat camera_matrix2 = cv::Mat::eye(3, 3, CV_64F);

        std::vector<float> distortion_coefficients1(5);
        std::vector<float> distortion_coefficients2(5);
        cv::Mat rotation_matrix;
        cv::Mat translation_vector;
        cv::Mat essential_matrix;
        cv::Mat fundamental_matrix;

        const auto cam_series = m_camera_set.lock();

        SIGHT_ASSERT(
            "camera index must be > 0 and < camSeries->size()",
            m_cam_index > 0 && m_cam_index < cam_series->size()
        );

        data::image::csptr img = cal_info1->get_image_container().front();
        cv::Size2i imgsize(static_cast<int>(img->size()[0]), static_cast<int>(img->size()[1]));
        {
            data::camera::csptr cam1 = cam_series->get_camera(0);
            data::camera::csptr cam2 = cam_series->get_camera(m_cam_index);

            data::mt::locked_ptr cam1_lock(cam1);
            data::mt::locked_ptr cam2_lock(cam2);

            camera_matrix1.at<double>(0, 0) = cam1->get_fx();
            camera_matrix1.at<double>(1, 1) = cam1->get_fy();
            camera_matrix1.at<double>(0, 2) = cam1->get_cx();
            camera_matrix1.at<double>(1, 2) = cam1->get_cy();

            camera_matrix2.at<double>(0, 0) = cam2->get_fx();
            camera_matrix2.at<double>(1, 1) = cam2->get_fy();
            camera_matrix2.at<double>(0, 2) = cam2->get_cx();
            camera_matrix2.at<double>(1, 2) = cam2->get_cy();
            for(std::size_t i = 0 ; i < 5 ; ++i)
            {
                distortion_coefficients1[i] = static_cast<float>(cam1->get_distortion_coefficient()[i]);
                distortion_coefficients2[i] = static_cast<float>(cam2->get_distortion_coefficient()[i]);
            }
        }
        double err = cv::stereoCalibrate(
            object_points,
            image_points1,
            image_points2,
            camera_matrix1,
            distortion_coefficients1,
            camera_matrix2,
            distortion_coefficients2,
            imgsize,
            rotation_matrix,
            translation_vector,
            essential_matrix,
            fundamental_matrix,
            cv::CALIB_FIX_INTRINSIC,
            cv::TermCriteria(
                cv::TermCriteria::MAX_ITER + cv::TermCriteria::EPS,
                100,
                1e-5
            )
        );

        this->signal<error_computed_t>(ERROR_COMPUTED_SIG)->async_emit(err);

        data::matrix4::sptr matrix = std::make_shared<data::matrix4>();
        cv::Mat cv4x4              = cv::Mat::eye(4, 4, CV_64F);
        rotation_matrix.copyTo(cv4x4(cv::Rect(0, 0, 3, 3)));
        translation_vector.copyTo(cv4x4(cv::Rect(3, 0, 1, 3)));

        io::opencv::matrix::copy_from_cv(cv4x4, matrix);

        {
            cam_series->set_extrinsic_matrix(m_cam_index, matrix);
        }

        auto sig = cam_series->signal<data::camera_set::extrinsic_calibrated_signal_t>(
            data::camera_set::EXTRINSIC_CALIBRATED_SIG
        );
        sig->async_emit();

        // Export matrix if needed.
        m_matrix = matrix;
    }
}

//------------------------------------------------------------------------------

} // namespace sight::module::geometry::vision
