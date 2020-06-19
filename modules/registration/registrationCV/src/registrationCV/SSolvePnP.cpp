/************************************************************************
 *
 * Copyright (C) 2018-2019 IRCAD France
 * Copyright (C) 2018-2019 IHU Strasbourg
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

#include "registrationCV/SSolvePnP.hpp"

#include <arData/Camera.hpp>

#include <calibration3d/helper.hpp>

#include <cvIO/Camera.hpp>
#include <cvIO/Matrix.hpp>

#include <fwCom/Signal.hxx>

#include <fwData/mt/ObjectReadLock.hpp>
#include <fwData/mt/ObjectWriteLock.hpp>
#include <fwData/PointList.hpp>
#include <fwData/TransformationMatrix3D.hpp>

namespace registrationCV
{

const ::fwServices::IService::KeyType s_CALIBRATION_INPUT = "calibration";
const ::fwServices::IService::KeyType s_POINTLIST2D_INPUT = "pointList2d";
const ::fwServices::IService::KeyType s_POINTLIST3D_INPUT = "pointList3d";
const ::fwServices::IService::KeyType s_MATRIX_INOUT      = "matrix";

//-----------------------------------------------------------------------------

SSolvePnP::SSolvePnP() noexcept :
    m_videoRef(TOP_LEFT)
{
    m_videoRefMap.insert( std::make_pair("top_left", TOP_LEFT));
    m_videoRefMap.insert( std::make_pair("center", CENTER));

    m_offset = {{0.f, 0.f}};
}

//-----------------------------------------------------------------------------

void SSolvePnP::computeRegistration(::fwCore::HiResClock::HiResClockType)
{
    const auto camera = this->getInput< ::arData::Camera > (s_CALIBRATION_INPUT);
    SLM_FATAL_IF("Camera '" + s_CALIBRATION_INPUT + "' not found", !camera);

    ::fwData::mt::ObjectReadLock cameraLock(camera);

    if(!camera->getIsCalibrated())
    {
        return;
    }

    Camera cvCamera;
    std::tie(cvCamera.intrinsicMat, cvCamera.imageSize, cvCamera.distCoef) = ::cvIO::Camera::copyToCv(camera);

    // if coordinate system is not the same as OpenCV's (TOP_LEFT), compute corresponding offset
    if(m_videoRef == CENTER)
    {
        m_offset[0] = static_cast<float>(cvCamera.imageSize.width) / 2.f;
        m_offset[1] = static_cast<float>(cvCamera.imageSize.height) / 2.f;
    }

    //get points
    std::vector< ::cv::Point2f > points2d;
    std::vector< ::cv::Point3f > points3d;

    const auto fwPoints2d = this->getInput< ::fwData::PointList >(s_POINTLIST2D_INPUT);
    SLM_ASSERT("'" + s_POINTLIST2D_INPUT + "' should not be null", fwPoints2d);
    const auto fwPoints3d = this->getInput< ::fwData::PointList >(s_POINTLIST3D_INPUT);
    SLM_ASSERT("'" + s_POINTLIST3D_INPUT + "' should not be null", fwPoints3d);

    auto fwMatrix = this->getInOut< ::fwData::TransformationMatrix3D >(s_MATRIX_INOUT);
    SLM_ASSERT("'" + s_MATRIX_INOUT + "' should not be null", fwMatrix);

    ::fwData::mt::ObjectReadLock readLock2d(fwPoints2d);
    ::fwData::mt::ObjectReadLock readLock3d(fwPoints3d);

    //points list should have same number of points
    if(fwPoints2d->getPoints().size() != fwPoints3d->getPoints().size())
    {
        SLM_ERROR("'" + s_POINTLIST2D_INPUT + "' and '"
                  + s_POINTLIST3D_INPUT + "' should have the same number of points");

        return;
    }

    const size_t numberOfPoints = fwPoints2d->getPoints().size();

    for(size_t i = 0; i < numberOfPoints; ++i)
    {
        // 2d
        ::fwData::Point::csptr p2d = fwPoints2d->getPoints()[i];
        ::fwData::mt::ObjectReadLock readLockp2d(p2d);
        ::cv::Point2f cvP2d;

        cvP2d.x = static_cast<float>(p2d->getCoord()[0]) + m_offset[0];
        cvP2d.y = static_cast<float>(p2d->getCoord()[1]) + m_offset[1];

        points2d.push_back(cvP2d);

        // 3d
        ::fwData::Point::csptr p3d = fwPoints3d->getPoints()[i];
        ::fwData::mt::ObjectReadLock readLockp3d(p3d);
        ::cv::Point3f cvP3d;
        cvP3d.x = static_cast<float>(p3d->getCoord()[0]);
        cvP3d.y = static_cast<float>(p3d->getCoord()[1]);
        cvP3d.z = static_cast<float>(p3d->getCoord()[2]);
        points3d.push_back(cvP3d);

    }

    // call solvepnp
    ::cv::Matx44f cvMat = ::calibration3d::helper::cameraPoseMonocular(points3d, points2d,
                                                                       cvCamera.intrinsicMat, cvCamera.distCoef);
    // object pose
    if(m_reverseMatrix)
    {
        cvMat = cvMat.inv();
    }

    ::fwData::mt::ObjectWriteLock writeLock(fwMatrix);
    ::cvIO::Matrix::copyFromCv(cvMat, fwMatrix);

    const auto sig = fwMatrix->signal< ::fwData::TransformationMatrix3D::ModifiedSignalType >
                         ( ::fwData::TransformationMatrix3D::s_MODIFIED_SIG);
    sig->asyncEmit();
}

//-----------------------------------------------------------------------------

void SSolvePnP::configuring()
{
    const ConfigType config = this->getConfigTree().get_child("config.<xmlattr>");

    const std::string videoRef = config.get< std::string >("videoReference", "top_left");

    m_reverseMatrix = config.get< bool >("inverse", false);

    auto it = m_videoRefMap.find(videoRef);

    if(it == m_videoRefMap.end())
    {
        SLM_WARN("'videoReference' of value '" + videoRef + "' is not handled.")
    }
    else
    {
        m_videoRef = it->second;
    }
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

} // namespace registrationCV
