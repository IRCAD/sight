/************************************************************************
 *
 * Copyright (C) 2018-2021 IRCAD France
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

#include "SSolvePnP.hpp"

#include <core/com/Signal.hxx>

#include <data/Camera.hpp>
#include <data/mt/ObjectReadLock.hpp>
#include <data/mt/ObjectWriteLock.hpp>
#include <data/PointList.hpp>
#include <data/TransformationMatrix3D.hpp>

#include <geometry/vision/helper.hpp>

#include <io/opencv/Camera.hpp>
#include <io/opencv/Matrix.hpp>

namespace sight::modules::geometry::vision
{

const services::IService::KeyType s_CALIBRATION_INPUT = "calibration";
const services::IService::KeyType s_POINTLIST2D_INPUT = "pointList2d";
const services::IService::KeyType s_POINTLIST3D_INPUT = "pointList3d";
const services::IService::KeyType s_MATRIX_INOUT      = "matrix";

//-----------------------------------------------------------------------------

SSolvePnP::SSolvePnP() noexcept
{
}

//-----------------------------------------------------------------------------

void SSolvePnP::computeRegistration(core::HiResClock::HiResClockType)
{
    const auto camera = this->getLockedInput< data::Camera > (s_CALIBRATION_INPUT);

    if(!camera->getIsCalibrated())
    {
        return;
    }

    Camera cvCamera;
    std::tie(cvCamera.intrinsicMat, cvCamera.imageSize, cvCamera.distCoef) =
        io::opencv::Camera::copyToCv(camera.get_shared());

    //get points
    std::vector< ::cv::Point2f > points2d;
    std::vector< ::cv::Point3f > points3d;

    const auto fwPoints2d = this->getLockedInput< data::PointList >(s_POINTLIST2D_INPUT);

    const auto fwPoints3d = this->getLockedInput< data::PointList >(s_POINTLIST3D_INPUT);

    auto fwMatrix = this->getLockedInOut< data::TransformationMatrix3D >(s_MATRIX_INOUT);

    //points list should have same number of points
    if(fwPoints2d->getPoints().size() != fwPoints3d->getPoints().size())
    {
        SLM_ERROR("'" + s_POINTLIST2D_INPUT + "' and '"
                  + s_POINTLIST3D_INPUT + "' should have the same number of points");

        return;
    }

    const size_t numberOfPoints = fwPoints2d->getPoints().size();

    const float cxcyShift[2] = {(static_cast< float >(camera->getWidth()) / 2.f ) - static_cast<float>(camera->getCx()),
                                (static_cast< float >(camera->getHeight()) / 2.f ) -
                                static_cast<float>(camera->getCy())};

    for(size_t i = 0; i < numberOfPoints; ++i)
    {
        // 2d
        data::Point::csptr p2d = fwPoints2d->getPoints()[i];
        ::cv::Point2f cvP2d;

        cvP2d.x = static_cast<float>(p2d->getCoord()[0]) - cxcyShift[0];
        cvP2d.y = static_cast<float>(p2d->getCoord()[1]) - cxcyShift[1];

        points2d.push_back(cvP2d);

        // 3d
        data::Point::csptr p3d = fwPoints3d->getPoints()[i];
        ::cv::Point3f cvP3d;

        cvP3d.x = static_cast<float>(p3d->getCoord()[0]);
        cvP3d.y = static_cast<float>(p3d->getCoord()[1]);
        cvP3d.z = static_cast<float>(p3d->getCoord()[2]);

        points3d.push_back(cvP3d);

    }

    // call solvepnp
    ::cv::Matx44f cvMat = sight::geometry::vision::helper::cameraPoseMonocular(points3d, points2d,
                                                                               cvCamera.intrinsicMat,
                                                                               cvCamera.distCoef);
    // object pose
    if(m_reverseMatrix)
    {
        cvMat = cvMat.inv();
    }

    data::TransformationMatrix3D::sptr matrix = data::TransformationMatrix3D::New();
    io::opencv::Matrix::copyFromCv(cvMat, matrix);

    fwMatrix->deepCopy(matrix);

    const auto sig = fwMatrix->signal< data::TransformationMatrix3D::ModifiedSignalType >
                         ( data::TransformationMatrix3D::s_MODIFIED_SIG);
    sig->asyncEmit();
}

//-----------------------------------------------------------------------------

void SSolvePnP::configuring()
{
    const ConfigType config = this->getConfigTree().get_child("config.<xmlattr>");

    m_reverseMatrix = config.get< bool >("inverse", false);

}

//-----------------------------------------------------------------------------

void SSolvePnP::starting()
{
}

//-----------------------------------------------------------------------------

void SSolvePnP::stopping()
{
}

//-----------------------------------------------------------------------------

void SSolvePnP::updating()
{
    // call computeRegistration slot with fake timestamp (timestamp is not used)
    this->computeRegistration(0.);
}

//-----------------------------------------------------------------------------

} // namespace sight::modules::geometry::vision
