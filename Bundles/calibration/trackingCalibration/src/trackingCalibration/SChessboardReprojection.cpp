/************************************************************************
 *
 * Copyright (C) 2018 IRCAD France
 * Copyright (C) 2018 IHU Strasbourg
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

#include "trackingCalibration/SChessboardReprojection.hpp"

#include <arData/Camera.hpp>

#include <cvIO/Camera.hpp>
#include <cvIO/Matrix.hpp>
#include <cvIO/PointList.hpp>

#include <fwCom/Signal.hxx>
#include <fwCom/Slots.hxx>

#include <fwData/mt/ObjectReadLock.hpp>
#include <fwData/PointList.hpp>
#include <fwData/TransformationMatrix3D.hpp>

#include <fwPreferences/helper.hpp>

#include <opencv2/calib3d.hpp>

namespace trackingCalibration
{

static const ::fwServices::IService::KeyType s_CAMERA_INPUT              = "camera";
static const ::fwServices::IService::KeyType s_DETECTED_CHESSBOARD_INPUT = "detectedChessboard";
static const ::fwServices::IService::KeyType s_HAND_EYE_X_INPUT          = "handEyeX";
static const ::fwServices::IService::KeyType s_HAND_EYE_Z_INPUT          = "handEyeZ";
static const ::fwServices::IService::KeyType s_TRACKER_MATRIX_INPUT      = "trackerMatrix";

static const ::fwServices::IService::KeyType s_REPROJECTED_CHESSBOARD_INOUT = "reprojectedChessboard";

static const ::fwServices::IService::KeyType s_CHESSBOARD_MODEL_OUTPUT = "chessboardModel";

static const ::fwCom::Slots::SlotKeyType s_UPDATE_CHESSBOARD_SIZE_SLOT = "updateChessboardSize";
static const ::fwCom::Slots::SlotKeyType s_SET_MOVING_CAMERA_SLOT      = "setMovingCamera";

static const ::fwCom::Signals::SignalKeyType s_ERROR_COMPUTED_SIGNAL = "errorComputed";

//------------------------------------------------------------------------------

SChessboardReprojection::SChessboardReprojection() noexcept
{
    newSlot(s_UPDATE_CHESSBOARD_SIZE_SLOT, &SChessboardReprojection::updateChessboardSize, this);
    newSlot(s_SET_MOVING_CAMERA_SLOT, &SChessboardReprojection::setMovingCamera, this);

    newSignal< ErrorComputedSignalType >(s_ERROR_COMPUTED_SIGNAL);
}

//------------------------------------------------------------------------------

SChessboardReprojection::~SChessboardReprojection() noexcept
{

}

//------------------------------------------------------------------------------

void SChessboardReprojection::configuring()
{
    const auto configTree  = this->getConfigTree();
    const auto boardConfig = configTree.get_child("board.<xmlattr>");

    m_boardWidthKey      = boardConfig.get<std::string>("width");
    m_boardHeightKey     = boardConfig.get<std::string>("height");
    m_boardSquareSizeKey = boardConfig.get<std::string>("squareSize");

    m_movingCamera = configTree.get<bool>("config.<xmlattr>.movingCamera", false);

    const std::string outputKey = configTree.get_optional<std::string>("out.<xmlattr>.key").get_value_or("");
    if(outputKey == s_CHESSBOARD_MODEL_OUTPUT)
    {
        m_hasOutputChessboard = true;
    }

}

//------------------------------------------------------------------------------

void SChessboardReprojection::starting()
{
    this->updateChessboardSize();
}

//------------------------------------------------------------------------------

void SChessboardReprojection::updating()
{
    const auto camera            = this->getInput< ::arData::Camera >(s_CAMERA_INPUT);
    const auto detectedPointList = this->getInput< ::fwData::PointList >(s_DETECTED_CHESSBOARD_INPUT);
    const auto handEyeX          = this->getInput< ::fwData::TransformationMatrix3D >(s_HAND_EYE_X_INPUT);
    const auto handEyeZ          = this->getInput< ::fwData::TransformationMatrix3D >(s_HAND_EYE_Z_INPUT);
    const auto trackerMatrix     = this->getInput< ::fwData::TransformationMatrix3D >(s_TRACKER_MATRIX_INPUT);

    SLM_ASSERT("Missing camera.", camera);
    SLM_ASSERT("Missing detected chessboard.", detectedPointList);
    SLM_ASSERT("Missing hand eye X.", handEyeX);
    SLM_ASSERT("Missing hand eye Z.", handEyeZ);
    SLM_ASSERT("Missing tracker matrix.", trackerMatrix);

    const auto outputPl = this->getInOut< ::fwData::PointList >(s_REPROJECTED_CHESSBOARD_INOUT);
    SLM_ASSERT("Missing projected points list.", outputPl);

    if(!detectedPointList->getPoints().empty())
    {
        ::cv::Matx44d cvX, cvZ, cvTracker;
        ::cvIO::Matrix::copyToCv(handEyeX, cvX);
        ::cvIO::Matrix::copyToCv(handEyeZ, cvZ);
        ::cvIO::Matrix::copyToCv(trackerMatrix, cvTracker);

        ::cv::Matx44d modelToCamera;
        if(m_movingCamera)
        {
            modelToCamera = cvX.inv() * cvTracker.inv() * cvZ;
        }
        else
        {
            modelToCamera = cvZ.inv() * cvTracker * cvX;
        }

        ::cv::Mat intrinsic, distortionCoefficients;
        ::cv::Size imageSize;
        std::tie(intrinsic, imageSize, distortionCoefficients) = ::cvIO::Camera::copyToCv(camera);

        std::vector< ::cv::Point2d > cvDetected;

        ::fwData::mt::ObjectReadLock lock(detectedPointList);

        ::cvIO::PointList::copyToCv(detectedPointList, cvDetected);

        ::cv::Matx33d rotMat = modelToCamera.get_minor<3, 3>(0, 0);
        ::cv::Mat rvec, tvec;
        ::cv::Rodrigues(rotMat, rvec);
        tvec.push_back(modelToCamera(0, 3));
        tvec.push_back(modelToCamera(1, 3));
        tvec.push_back(modelToCamera(2, 3));

        std::vector< ::cv::Point2d > projectedPoints;
        ::cv::projectPoints(m_chessboardModel, rvec, tvec, intrinsic, distortionCoefficients, projectedPoints);

        ::cvIO::PointList::copyFromCv(projectedPoints, outputPl);

        const double error = meanDistance(cvDetected, projectedPoints);
        this->signal<ErrorComputedSignalType>(s_ERROR_COMPUTED_SIGNAL)->asyncEmit(error);
    }
    else
    {
        // No detected points = no reprojected points.
        outputPl->getPoints().clear();
    }

    auto sig = outputPl->signal< ::fwData::Object::ModifiedSignalType >(::fwData::Object::s_MODIFIED_SIG);
    sig->asyncEmit();
}

//------------------------------------------------------------------------------

void SChessboardReprojection::stopping()
{

}

//------------------------------------------------------------------------------

fwServices::IService::KeyConnectionsMap SChessboardReprojection::getAutoConnections() const
{
    ::fwServices::IService::KeyConnectionsMap connections;
    connections.push(s_DETECTED_CHESSBOARD_INPUT, ::fwData::PointList::s_MODIFIED_SIG, s_UPDATE_SLOT);
    connections.push(s_TRACKER_MATRIX_INPUT, ::fwData::TransformationMatrix3D::s_MODIFIED_SIG, s_UPDATE_SLOT);
    connections.push(s_HAND_EYE_X_INPUT, ::fwData::TransformationMatrix3D::s_MODIFIED_SIG, s_UPDATE_SLOT);
    connections.push(s_HAND_EYE_Z_INPUT, ::fwData::TransformationMatrix3D::s_MODIFIED_SIG, s_UPDATE_SLOT);
    connections.push(s_CAMERA_INPUT, ::arData::Camera::s_MODIFIED_SIG, s_UPDATE_SLOT);
    connections.push(s_CAMERA_INPUT, ::arData::Camera::s_INTRINSIC_CALIBRATED_SIG, s_UPDATE_SLOT);

    return connections;
}

//------------------------------------------------------------------------------

double SChessboardReprojection::meanDistance(const std::vector< ::cv::Point2d >& _detected,
                                             const std::vector< ::cv::Point2d >& _reprojected)
{
    SLM_ASSERT("Detected and reprojected point lists don't have the same size.",
               _detected.size() == _reprojected.size());

    double res = 0.;

    // Sum of distances.
    for(size_t i = 0; i < _detected.size(); ++i)
    {
        const ::cv::Vec2d v = ::cv::Vec2d(_detected[i]) - ::cv::Vec2d(_reprojected[i]);
        res += ::cv::norm(v, ::cv::NORM_L2);
    }

    // Return the mean distance difference.
    return res / _detected.size();
}

//------------------------------------------------------------------------------

void SChessboardReprojection::updateChessboardSize()
{
    unsigned long width(0), height(0);
    const std::string widthStr = ::fwPreferences::getPreference(m_boardWidthKey);
    if(!widthStr.empty())
    {
        width = std::stoul(widthStr);
    }
    const std::string heightStr = ::fwPreferences::getPreference(m_boardHeightKey);
    if(!heightStr.empty())
    {
        height = std::stoul(heightStr);
    }

    double squareSize;
    const std::string squareSizeStr = ::fwPreferences::getPreference(m_boardSquareSizeKey);
    if(!squareSizeStr.empty())
    {
        squareSize = std::stod(squareSizeStr);
    }

    m_chessboardModel.clear();

    ::fwData::PointList::sptr pl_chessboardModel = ::fwData::PointList::New();

    for(unsigned long i = 0; i < height - 1; ++i)
    {
        const double x = i * squareSize;

        for(unsigned long j = 0; j < width - 1; ++j)
        {
            const double y = j * squareSize;
            m_chessboardModel.push_back(::cv::Point3d(x, y, 0.));
            pl_chessboardModel->pushBack(::fwData::Point::New(x, y, 0.));
        }
    }

    if(m_hasOutputChessboard)
    {
        this->setOutput(s_CHESSBOARD_MODEL_OUTPUT, pl_chessboardModel);
    }
}

//------------------------------------------------------------------------------

void SChessboardReprojection::setMovingCamera(bool _movingCamera)
{
    m_movingCamera = _movingCamera;
}

//------------------------------------------------------------------------------

} // namespace trackingCalibration
