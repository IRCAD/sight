/************************************************************************
 *
 * Copyright (C) 2014-2023 IRCAD France
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

#include <core/com/signal.hxx>
#include <core/com/slots.hxx>

#include <io/opencv/matrix.hpp>

#include <ui/__/preferences.hpp>

#include <opencv2/calib3d.hpp>
#include <opencv2/core.hpp>

namespace sight::module::geometry::vision
{

static const core::com::slots::key_t UPDATE_CHESSBOARD_SIZE_SLOT = "updateChessboardSize";

static const core::com::signals::key_t ERROR_COMPUTED_SIG = "errorComputed";

// ----------------------------------------------------------------------------

open_cv_intrinsic::open_cv_intrinsic() noexcept
{
    new_signal<error_computed_signal_t>(ERROR_COMPUTED_SIG);
    new_slot(UPDATE_CHESSBOARD_SIZE_SLOT, &open_cv_intrinsic::updateChessboardSize, this);
}

// ----------------------------------------------------------------------------

open_cv_intrinsic::~open_cv_intrinsic() noexcept =
    default;

//------------------------------------------------------------------------------

void open_cv_intrinsic::configuring()
{
    const auto config    = this->get_config();
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

void open_cv_intrinsic::starting()
{
    this->updateChessboardSize();
}

// ----------------------------------------------------------------------------

void open_cv_intrinsic::stopping()
{
}

//--------------------------------------------------------------------- ---------

void open_cv_intrinsic::updating()
{
    const auto cal_info = m_calibrationInfo.lock();

    SIGHT_ASSERT("Object with 'calibrationInfo' is not found", cal_info);
    SIGHT_WARN_IF("Calibration info is empty.", cal_info->getPointListContainer().empty());

    if(!cal_info->getPointListContainer().empty())
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

        std::vector<std::vector<cv::Point2f> > image_points;

        for(const data::point_list::csptr& capture : cal_info->getPointListContainer())
        {
            std::vector<cv::Point2f> dst;

            for(data::point::csptr point : capture->getPoints())
            {
                SIGHT_ASSERT("point is null", point);
                dst.emplace_back(
                    static_cast<float>(point->getCoord()[0]),
                    static_cast<float>(point->getCoord()[1])

                );
            }

            image_points.push_back(dst);
            object_points.push_back(points);
        }

        data::image::csptr img = cal_info->getImageContainer().front();

        cv::Mat camera_matrix;
        std::vector<float> dist_coeffs;
        std::vector<cv::Mat> rvecs;
        std::vector<cv::Mat> tvecs;
        cv::Size2i imgsize(static_cast<int>(img->size()[0]), static_cast<int>(img->size()[1]));

        double err =
            cv::calibrateCamera(object_points, image_points, imgsize, camera_matrix, dist_coeffs, rvecs, tvecs);

        this->signal<error_computed_signal_t>(ERROR_COMPUTED_SIG)->async_emit(err);

        const auto pose_camera = m_poseVector.lock();
        if(pose_camera)
        {
            pose_camera->clear();

            for(std::size_t index = 0 ; index < rvecs.size() ; ++index)
            {
                data::matrix4::sptr mat3_d = std::make_shared<data::matrix4>();

                io::opencv::matrix::copy_from_cv(rvecs.at(index), tvecs.at(index), mat3_d);

                pose_camera->push_back(mat3_d);
                auto sig = pose_camera->signal<data::vector::added_signal_t>(
                    data::vector::ADDED_OBJECTS_SIG
                );
                sig->async_emit(pose_camera->get_content());
            }
        }

        SIGHT_DEBUG("Calibration error :" << err);

        const auto cam = m_camera.lock();

        cam->setCx(camera_matrix.at<double>(0, 2));
        cam->setCy(camera_matrix.at<double>(1, 2));
        cam->setFx(camera_matrix.at<double>(0, 0));
        cam->setFy(camera_matrix.at<double>(1, 1));
        cam->setWidth(img->size()[0]);
        cam->setHeight(img->size()[1]);
        cam->setDistortionCoefficient(dist_coeffs[0], dist_coeffs[1], dist_coeffs[2], dist_coeffs[3], dist_coeffs[4]);

        cam->setIsCalibrated(true);

        data::camera::intrinsic_calibrated_signal_t::sptr sig;
        sig = cam->signal<data::camera::intrinsic_calibrated_signal_t>(
            data::camera::INTRINSIC_CALIBRATED_SIG
        );

        sig->async_emit();
    }
}

//------------------------------------------------------------------------------

void open_cv_intrinsic::updateChessboardSize()
{
    try
    {
        ui::preferences preferences;

        if(const auto& saved = preferences.get_optional<decltype(m_width)>(m_widthKey); saved)
        {
            m_width = *saved;
        }

        if(const auto& saved = preferences.get_optional<decltype(m_height)>(m_heightKey); saved)
        {
            m_height = *saved;
        }

        if(const auto& saved = preferences.get_optional<decltype(m_squareSize)>(m_squareSizeKey); saved)
        {
            m_squareSize = *saved;
        }
    }
    catch(const ui::preferences_disabled&)
    {
        // Nothing to do..
    }
}

//------------------------------------------------------------------------------

} // namespace sight::module::geometry::vision
