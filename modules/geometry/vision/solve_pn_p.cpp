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

    Camera cvCamera;
    std::tie(cvCamera.intrinsicMat, cvCamera.imageSize, cvCamera.distCoef) =
        io::opencv::camera::copyToCv(camera.get_shared());

    //get points
    std::vector<cv::Point2f> points2d;
    std::vector<cv::Point3f> points3d;

    const auto fwPoints2d = m_pointList2d.lock();
    const auto fwPoints3d = m_pointList3d.lock();

    auto fwMatrix = m_matrix.lock();

    //points list should have same number of points
    if(fwPoints2d->getPoints().size() != fwPoints3d->getPoints().size())
    {
        SIGHT_ERROR("The 2d and 3d point lists should have the same number of points");

        return;
    }

    const std::size_t numberOfPoints = fwPoints2d->getPoints().size();

    float shiftX = 0.F;
    float shiftY = 0.F;
    // Shift back 2d points to compensate "shifted" camera in a 3dScene.
    if(m_shiftPoints)
    {
        shiftX = static_cast<float>(camera->getWidth()) / 2.F - static_cast<float>(camera->getCx());
        shiftY = static_cast<float>(camera->getHeight()) / 2.F - static_cast<float>(camera->getCy());
    }

    points2d.resize(numberOfPoints);
    points3d.resize(numberOfPoints);

    for(std::size_t i = 0 ; i < numberOfPoints ; ++i)
    {
        // 2d
        data::point::csptr p2d = fwPoints2d->getPoints()[i];
        cv::Point2f cvP2d;

        cvP2d.x = static_cast<float>(p2d->getCoord()[0]) - shiftX;
        cvP2d.y = static_cast<float>(p2d->getCoord()[1]) - shiftY;

        points2d[i] = cvP2d;

        // 3d
        data::point::csptr p3d = fwPoints3d->getPoints()[i];
        cv::Point3f cvP3d;

        cvP3d.x = static_cast<float>(p3d->getCoord()[0]);
        cvP3d.y = static_cast<float>(p3d->getCoord()[1]);
        cvP3d.z = static_cast<float>(p3d->getCoord()[2]);

        points3d[i] = cvP3d;
    }

    // call solvepnp
    cv::Matx44f cvMat = sight::geometry::vision::helper::cameraPoseMonocular(
        points3d,
        points2d,
        cvCamera.intrinsicMat,
        cvCamera.distCoef
    );
    // object pose
    if(m_reverseMatrix)
    {
        cvMat = cvMat.inv();
    }

    data::matrix4::sptr matrix = std::make_shared<data::matrix4>();
    io::opencv::matrix::copyFromCv(cvMat, matrix);

    fwMatrix->deep_copy(matrix);

    const auto sig = fwMatrix->signal<data::matrix4::ModifiedSignalType>(data::matrix4::MODIFIED_SIG);
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
