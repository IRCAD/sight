/************************************************************************
 *
 * Copyright (C) 2018-2023 IRCAD France
 * Copyright (C) 2018-2020 IHU Strasbourg
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

#include "solve_pn_p.hpp"

#include <core/com/signal.hxx>

#include <geometry/vision/helper.hpp>

#include <io/opencv/camera.hpp>
#include <io/opencv/matrix.hpp>

namespace sight::module::geometry::vision
{

//-----------------------------------------------------------------------------

void solve_pn_p::computeRegistration(core::hires_clock::type /*timestamp*/)
{
    const auto camera = m_calibration.lock();

    if(!camera->getIsCalibrated())
    {
        return;
    }

    Camera cv_camera;
    std::tie(cv_camera.intrinsicMat, cv_camera.imageSize, cv_camera.distCoef) =
        io::opencv::camera::copyToCv(camera.get_shared());

    //get points
    std::vector<cv::Point2f> points2d;
    std::vector<cv::Point3f> points3d;

    const auto fw_points2d = m_pointList2d.lock();
    const auto fw_points3d = m_pointList3d.lock();

    auto fw_matrix = m_matrix.lock();

    //points list should have same number of points
    if(fw_points2d->getPoints().size() != fw_points3d->getPoints().size())
    {
        SIGHT_ERROR("The 2d and 3d point lists should have the same number of points");

        return;
    }

    const std::size_t number_of_points = fw_points2d->getPoints().size();

    float shift_x = 0.F;
    float shift_y = 0.F;
    // Shift back 2d points to compensate "shifted" camera in a 3dScene.
    if(m_shiftPoints)
    {
        shift_x = static_cast<float>(camera->getWidth()) / 2.F - static_cast<float>(camera->getCx());
        shift_y = static_cast<float>(camera->getHeight()) / 2.F - static_cast<float>(camera->getCy());
    }

    points2d.resize(number_of_points);
    points3d.resize(number_of_points);

    for(std::size_t i = 0 ; i < number_of_points ; ++i)
    {
        // 2d
        data::point::csptr p2d = fw_points2d->getPoints()[i];
        cv::Point2f cv_p2d;

        cv_p2d.x = static_cast<float>(p2d->getCoord()[0]) - shift_x;
        cv_p2d.y = static_cast<float>(p2d->getCoord()[1]) - shift_y;

        points2d[i] = cv_p2d;

        // 3d
        data::point::csptr p3d = fw_points3d->getPoints()[i];
        cv::Point3f cv_p3d;

        cv_p3d.x = static_cast<float>(p3d->getCoord()[0]);
        cv_p3d.y = static_cast<float>(p3d->getCoord()[1]);
        cv_p3d.z = static_cast<float>(p3d->getCoord()[2]);

        points3d[i] = cv_p3d;
    }

    // call solvepnp
    cv::Matx44f cv_mat = sight::geometry::vision::helper::camera_pose_monocular(
        points3d,
        points2d,
        cv_camera.intrinsicMat,
        cv_camera.distCoef
    );
    // object pose
    if(m_reverseMatrix)
    {
        cv_mat = cv_mat.inv();
    }

    data::matrix4::sptr matrix = std::make_shared<data::matrix4>();
    io::opencv::matrix::copy_from_cv(cv_mat, matrix);

    fw_matrix->deep_copy(matrix);

    const auto sig = fw_matrix->signal<data::matrix4::modified_signal_t>(data::matrix4::MODIFIED_SIG);
    sig->async_emit();
}

//-----------------------------------------------------------------------------

void solve_pn_p::configuring()
{
    const config_t config = this->get_config().get_child("config.<xmlattr>");

    m_reverseMatrix = config.get<bool>("inverse", m_reverseMatrix);
    m_shiftPoints   = config.get<bool>("shift", m_shiftPoints);
}

//-----------------------------------------------------------------------------

void solve_pn_p::starting()
{
}

//-----------------------------------------------------------------------------

void solve_pn_p::stopping()
{
}

//-----------------------------------------------------------------------------

void solve_pn_p::updating()
{
    // call computeRegistration slot with fake timestamp (timestamp is not used)
    this->computeRegistration(0.);
}

//-----------------------------------------------------------------------------

} // namespace sight::module::geometry::vision
