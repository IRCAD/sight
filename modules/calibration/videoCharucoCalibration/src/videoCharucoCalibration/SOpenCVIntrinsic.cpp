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

#include "videoCharucoCalibration/SOpenCVIntrinsic.hpp"

#include <calibration3d/helper.hpp>

#include <core/com/Signal.hxx>
#include <core/com/Slots.hxx>
#include <core/runtime/ConfigurationElement.hpp>
#include <core/tools/fwID.hpp>
#include <core/tools/Object.hpp>

#include <cvIO/Matrix.hpp>

#include <data/CalibrationInfo.hpp>
#include <data/Camera.hpp>
#include <data/mt/ObjectReadLock.hpp>
#include <data/mt/ObjectWriteLock.hpp>
#include <data/PointList.hpp>
#include <data/TransformationMatrix3D.hpp>
#include <data/Vector.hpp>

#include <services/IService.hpp>
#include <services/macros.hpp>

#include <geometry/data/TransformationMatrix3D.hpp>

#include <opencv2/aruco.hpp>
#include <opencv2/aruco/charuco.hpp>
#include <opencv2/calib3d.hpp>
#include <opencv2/core.hpp>
#include <opencv2/opencv.hpp>

#include <ui/base/preferences/helper.hpp>

fwServicesRegisterMacro(services::ICalibration, ::videoCharucoCalibration::SOpenCVIntrinsic, data::Camera)

namespace videoCharucoCalibration
{

static const core::com::Slots::SlotKeyType s_UPDATE_CHARUCOBOARD_SIZE_SLOT = "updateCharucoBoardSize";

static const core::com::Signals::SignalKeyType s_ERROR_COMPUTED_SIG = "errorComputed";

// ----------------------------------------------------------------------------

SOpenCVIntrinsic::SOpenCVIntrinsic() noexcept :
    m_width(11),
    m_height(8),
    m_squareSize(20.0),
    m_markerSizeInBits(6)
{
    newSignal<ErrorComputedSignalType>(s_ERROR_COMPUTED_SIG);
    newSlot(s_UPDATE_CHARUCOBOARD_SIZE_SLOT, &SOpenCVIntrinsic::updateCharucoBoardSize, this);
}

// ----------------------------------------------------------------------------

SOpenCVIntrinsic::~SOpenCVIntrinsic() noexcept
{
}

//------------------------------------------------------------------------------

void SOpenCVIntrinsic::configuring()
{
    const auto configTree = this->getConfigTree();
    const auto cfgBoard   = configTree.get_child("board.<xmlattr>");

    m_widthKey            = cfgBoard.get<std::string>("width", "CHARUCO_WIDTH");
    m_heightKey           = cfgBoard.get<std::string>("height", "CHARUCO_HEIGHT");
    m_squareSizeKey       = cfgBoard.get<std::string>("squareSize", "CHARUCO_SQUARE_SIZE");
    m_markerSizeKey       = cfgBoard.get<std::string>("markerSize", "CHARUCO_MARKER_SIZE");
    m_markerSizeInBitsKey = cfgBoard.get<std::string>("markerSizeInBits", "CHARUCO_MARKER_SIZE_IN_BITS");

    this->updateCharucoBoardSize();
}

// ----------------------------------------------------------------------------

void SOpenCVIntrinsic::starting()
{
    this->updateCharucoBoardSize();
}

// ----------------------------------------------------------------------------

void SOpenCVIntrinsic::stopping()
{
}

//------------------------------------------------------------------------------

void SOpenCVIntrinsic::swapping()
{
    this->stopping();
    this->starting();
}

//--------------------------------------------------------------------- ---------

void SOpenCVIntrinsic::updating()
{
    data::CalibrationInfo::csptr calInfo = this->getInput< data::CalibrationInfo>("calibrationInfo");
    data::Camera::sptr cam               = this->getInOut< data::Camera >("camera");
    data::Vector::sptr poseCamera        = this->getInOut< data::Vector >("poseVector");

    SLM_ASSERT("Object with 'calibrationInfo' is not found", calInfo);
    SLM_ASSERT("'camera' should not be null", cam);
    SLM_WARN_IF("Calibration info is empty.", calInfo->getPointListContainer().empty());

    if(!calInfo->getPointListContainer().empty())
    {
        std::vector<std::vector< ::cv::Point2f > > cornersPoints;
        std::vector<std::vector< int > > ids;

        {
            data::mt::ObjectReadLock calInfoLock(calInfo);
            for(data::PointList::sptr capture : calInfo->getPointListContainer())
            {
                std::vector< ::cv::Point2f > cdst;
                std::vector< int > idst;

                for(data::Point::csptr point : capture->getPoints())
                {
                    SLM_ASSERT("point is null", point);
                    cdst.push_back(::cv::Point2f(static_cast<float>(point->getCoord()[0]),
                                                 static_cast<float>(point->getCoord()[1])));
                    idst.push_back(static_cast<int>(point->getCoord()[2]));
                }
                cornersPoints.push_back(cdst);
                ids.push_back(idst);

            }
        }

        data::Image::sptr img = calInfo->getImageContainer().front();

        ::cv::Mat cameraMatrix;
        std::vector<double> distCoeffs;
        std::vector< ::cv::Mat> rvecs;
        std::vector< ::cv::Mat> tvecs;
        ::cv::Size2i imgsize(static_cast<int>(img->getSize2()[0]), static_cast<int>(img->getSize2()[1]));

        double err = ::cv::aruco::calibrateCameraCharuco(cornersPoints, ids, m_board, imgsize, cameraMatrix, distCoeffs,
                                                         rvecs, tvecs);

        this->signal<ErrorComputedSignalType>(s_ERROR_COMPUTED_SIG)->asyncEmit(err);

        if(poseCamera)
        {
            data::mt::ObjectWriteLock lock(poseCamera);
            poseCamera->getContainer().clear();

            for(size_t index = 0; index < rvecs.size(); ++index)
            {
                data::TransformationMatrix3D::sptr mat3D = data::TransformationMatrix3D::New();

                // Convert from cv to sight.
                ::cvIO::Matrix::copyFromCv(rvecs.at(index), tvecs.at(index), mat3D);

                poseCamera->getContainer().push_back(mat3D);
                auto sig = poseCamera->signal< data::Vector::AddedObjectsSignalType >(
                    data::Vector::s_ADDED_OBJECTS_SIG);
                sig->asyncEmit(poseCamera->getContainer());
            }
        }

        this->signal<ErrorComputedSignalType>(s_ERROR_COMPUTED_SIG)->asyncEmit(err);

        data::mt::ObjectWriteLock camLock(cam);

        cam->setCx(cameraMatrix.at<double>(0, 2));
        cam->setCy(cameraMatrix.at<double>(1, 2));
        cam->setFx(cameraMatrix.at<double>(0, 0));
        cam->setFy(cameraMatrix.at<double>(1, 1));
        cam->setWidth(img->getSize2()[0]);
        cam->setHeight(img->getSize2()[1]);
        cam->setDistortionCoefficient(distCoeffs[0], distCoeffs[1], distCoeffs[2], distCoeffs[3], distCoeffs[4]);

        cam->setIsCalibrated(true);

        data::Camera::IntrinsicCalibratedSignalType::sptr sig;
        sig = cam->signal< data::Camera::IntrinsicCalibratedSignalType >(
            data::Camera::s_INTRINSIC_CALIBRATED_SIG);

        sig->asyncEmit();
    }
}

//------------------------------------------------------------------------------

void SOpenCVIntrinsic::updateCharucoBoardSize()
{
    const std::string widthStr = modules::ui::base::preferences::getPreference(m_widthKey);
    if(!widthStr.empty())
    {
        m_width = std::stoul(widthStr);
    }
    const std::string heightStr = modules::ui::base::preferences::getPreference(m_heightKey);
    if(!heightStr.empty())
    {
        m_height = std::stoul(heightStr);
    }
    const std::string squareSizeStr = modules::ui::base::preferences::getPreference(m_squareSizeKey);
    if(!squareSizeStr.empty())
    {
        m_squareSize = std::stof(squareSizeStr);
    }
    const std::string markerSizeStr = modules::ui::base::preferences::getPreference(m_markerSizeKey);
    if(!markerSizeStr.empty())
    {
        m_markerSize = std::stof(markerSizeStr);
    }
    const std::string markerSizeInBitsStr = modules::ui::base::preferences::getPreference(m_markerSizeInBitsKey);
    if(!markerSizeInBitsStr.empty())
    {
        m_markerSizeInBits = std::stoi(markerSizeInBitsStr);
    }

    try
    {
        m_dictionary = ::calibration3d::helper::generateArucoDictionary(m_width, m_height, m_markerSizeInBits);
    }
    catch (const std::exception& e )
    {
        // Warn user that something went wrong with dictionary generation.
        // We are not using dialog here since SCharucoDetector already displays one.
        SLM_ERROR("Error when generating dictionary: " + std::string(e.what()));

        // Exit the function.
        return;
    }

    m_board = ::cv::aruco::CharucoBoard::create(static_cast<int>(m_width), static_cast<int>(m_height),
                                                m_squareSize, m_markerSize, m_dictionary);
}

//------------------------------------------------------------------------------

} // namespace videoCharucoCalibration
