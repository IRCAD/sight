/************************************************************************
 *
 * Copyright (C) 2014-2023 IRCAD France
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

static const core::com::slots::key_t UPDATE_CHESSBOARD_SIZE_SLOT = "updateChessboardSize";
static const core::com::signals::key_t ERROR_COMPUTED_SIG        = "errorComputed";

// ----------------------------------------------------------------------------

open_cv_extrinsic::open_cv_extrinsic() noexcept
{
    new_signal<error_computed_signal_t>(ERROR_COMPUTED_SIG);
    new_slot(UPDATE_CHESSBOARD_SIZE_SLOT, &open_cv_extrinsic::updateChessboardSize, this);
}

// ----------------------------------------------------------------------------

open_cv_extrinsic::~open_cv_extrinsic() noexcept =
    default;

//------------------------------------------------------------------------------

void open_cv_extrinsic::configuring()
{
    const auto config = this->get_config();
    m_camIndex = config.get<std::size_t>("camIndex");

    const auto cfg_board = config.get_child("board.<xmlattr>");

    m_widthKey = cfg_board.get<std::string>("width");
    SIGHT_ASSERT("Attribute 'width' is empty", !m_widthKey.empty());

    m_heightKey = cfg_board.get<std::string>("height");
    SIGHT_ASSERT("Attribute 'height' is empty", !m_heightKey.empty());

    if(const auto square_size_key = cfg_board.get_optional<std::string>("squareSize"); square_size_key.has_value())
    {
        m_squareSizeKey = square_size_key.value();
        SIGHT_ASSERT("Attribute 'squareSize' is empty", !m_squareSizeKey.empty());
    }
}

// ----------------------------------------------------------------------------

void open_cv_extrinsic::starting()
{
    this->updateChessboardSize();
}

// ----------------------------------------------------------------------------

void open_cv_extrinsic::stopping()
{
}

//------------------------------------------------------------------------------

void open_cv_extrinsic::updating()
{
    const auto cal_info1 = m_calibrationInfo1.lock();
    const auto cal_info2 = m_calibrationInfo2.lock();

    SIGHT_ASSERT("Object with 'calibrationInfo1' is not found", cal_info1);
    SIGHT_ASSERT("Object with 'calibrationInfo2' is not found", cal_info2);

    SIGHT_WARN_IF("Calibration info is empty.", cal_info1->getPointListContainer().empty());
    if(!cal_info1->getPointListContainer().empty())
    {
        std::vector<std::vector<cv::Point3f> > object_points;

        std::vector<cv::Point3f> points;
        for(unsigned int y = 0 ; y < m_height - 1 ; ++y)
        {
            for(unsigned int x = 0 ; x < m_width - 1 ; ++x)
            {
                points.emplace_back(
                    static_cast<float>(x) * m_squareSize,
                    static_cast<float>(y) * m_squareSize,
                    0.F

                );
            }
        }

        std::vector<std::vector<cv::Point2f> > image_points1;
        std::vector<std::vector<cv::Point2f> > image_points2;
        {
            const auto pt_lists1 = cal_info1->getPointListContainer();
            const auto pt_lists2 = cal_info2->getPointListContainer();

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

                for(data::point::csptr point : pt_list1->getPoints())
                {
                    SIGHT_ASSERT("point is null", point);
                    img_point1.emplace_back(
                        static_cast<float>(point->getCoord()[0]),
                        static_cast<float>(point->getCoord()[1])

                    );
                }

                for(data::point::csptr point : pt_list2->getPoints())
                {
                    SIGHT_ASSERT("point is null", point);
                    img_point2.emplace_back(
                        static_cast<float>(point->getCoord()[0]),
                        static_cast<float>(point->getCoord()[1])

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
            m_camIndex > 0 && m_camIndex < cam_series->size()
        );

        data::image::csptr img = cal_info1->getImageContainer().front();
        cv::Size2i imgsize(static_cast<int>(img->size()[0]), static_cast<int>(img->size()[1]));
        {
            data::camera::csptr cam1 = cam_series->get_camera(0);
            data::camera::csptr cam2 = cam_series->get_camera(m_camIndex);

            data::mt::locked_ptr cam1_lock(cam1);
            data::mt::locked_ptr cam2_lock(cam2);

            camera_matrix1.at<double>(0, 0) = cam1->getFx();
            camera_matrix1.at<double>(1, 1) = cam1->getFy();
            camera_matrix1.at<double>(0, 2) = cam1->getCx();
            camera_matrix1.at<double>(1, 2) = cam1->getCy();

            camera_matrix2.at<double>(0, 0) = cam2->getFx();
            camera_matrix2.at<double>(1, 1) = cam2->getFy();
            camera_matrix2.at<double>(0, 2) = cam2->getCx();
            camera_matrix2.at<double>(1, 2) = cam2->getCy();
            for(std::size_t i = 0 ; i < 5 ; ++i)
            {
                distortion_coefficients1[i] = static_cast<float>(cam1->getDistortionCoefficient()[i]);
                distortion_coefficients2[i] = static_cast<float>(cam2->getDistortionCoefficient()[i]);
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

        this->signal<error_computed_signal_t>(ERROR_COMPUTED_SIG)->async_emit(err);

        data::matrix4::sptr matrix = std::make_shared<data::matrix4>();
        cv::Mat cv4x4              = cv::Mat::eye(4, 4, CV_64F);
        rotation_matrix.copyTo(cv4x4(cv::Rect(0, 0, 3, 3)));
        translation_vector.copyTo(cv4x4(cv::Rect(3, 0, 1, 3)));

        io::opencv::matrix::copy_from_cv(cv4x4, matrix);

        {
            cam_series->set_extrinsic_matrix(m_camIndex, matrix);
        }

        data::camera_set::extrinsic_calibrated_signal_t::sptr sig;
        sig = cam_series->signal<data::camera_set::extrinsic_calibrated_signal_t>(
            data::camera_set::EXTRINSIC_CALIBRATED_SIG
        );

        sig->async_emit();

        // Export matrix if needed.
        m_matrix = matrix;
    }
}

//------------------------------------------------------------------------------

void open_cv_extrinsic::updateChessboardSize()
{
    try
    {
        ui::preferences preferences;
        m_width      = preferences.get(m_widthKey, m_width);
        m_height     = preferences.get(m_heightKey, m_height);
        m_squareSize = preferences.get(m_squareSizeKey, m_squareSize);
    }
    catch(const ui::preferences_disabled&)
    {
        // Nothing to do..
    }
}

//------------------------------------------------------------------------------

} // namespace sight::module::geometry::vision
