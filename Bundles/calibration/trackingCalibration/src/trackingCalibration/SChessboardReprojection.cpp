/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2018.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "trackingCalibration/SChessboardReprojection.hpp"

#include <arData/Camera.hpp>

#include <cvIO/Camera.hpp>
#include <cvIO/Matrix.hpp>
#include <cvIO/PointList.hpp>

#include <fwCom/Slots.hxx>

#include <fwData/PointList.hpp>
#include <fwData/TransformationMatrix3D.hpp>

#include <fwPreferences/helper.hpp>

#include <opencv2/calib3d.hpp>

namespace trackingCalibration
{

const ::fwServices::IService::KeyType s_CAMERA_INPUT              = "camera";
const ::fwServices::IService::KeyType s_DETECTED_CHESSBOARD_INPUT = "detectedChessboard";
const ::fwServices::IService::KeyType s_HAND_EYE_X_INPUT          = "handEyeX";
const ::fwServices::IService::KeyType s_HAND_EYE_Z_INPUT          = "handEyeZ";
const ::fwServices::IService::KeyType s_TRACKER_MATRIX_INPUT      = "trackerMatrix";

const ::fwServices::IService::KeyType s_REPROJECTED_CHESSBOARD_INOUT = "reprojectedChessboard";

const ::fwCom::Slots::SlotKeyType s_UPDATE_CHESSBOARD_SIZE_SLOT = "updateChessboardSize";

//------------------------------------------------------------------------------

SChessboardReprojection::SChessboardReprojection() noexcept
{
    newSlot(s_UPDATE_CHESSBOARD_SIZE_SLOT, &SChessboardReprojection::updateChessboardSize, this);
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

    ::cv::Matx44d cvX, cvZ, cvTracker;
    ::cvIO::Matrix::copyToCv(handEyeX, cvX);
    ::cvIO::Matrix::copyToCv(handEyeZ, cvZ);
    ::cvIO::Matrix::copyToCv(trackerMatrix, cvTracker);

    // TODO: handle fixed camera hand-eye reprojections
    ::cv::Matx44d modelToCamera = cvX.inv() * cvTracker.inv() * cvZ;

    ::cv::Mat intrinsic, distortionCoefficients;
    ::cv::Size imageSize;
    ::cvIO::Camera::copyToCv(camera, intrinsic, imageSize, distortionCoefficients);

    std::vector< ::cv::Point2d > cvChessboard;
    ::cvIO::PointList::copyToCv(detectedPointList, cvChessboard);

    ::cv::Matx33d rotMat = modelToCamera.get_minor<3, 3>(0, 0);
//    ::cv::Matx33d rotMat = ::cv::Mat(modelToCamera)(::cv::Range(0,0), ::cv::Range(2,2));
    ::cv::Mat rvec, tvec;
    ::cv::Rodrigues(rotMat, rvec);
    tvec.push_back(modelToCamera(0, 3));
    tvec.push_back(modelToCamera(1, 3));
    tvec.push_back(modelToCamera(2, 3));

    std::cerr << "mToC : " << modelToCamera << std::endl;
    std::cerr << "Rvec : " << rvec << "      Tvec : " << tvec << std::endl;

    std::vector< ::cv::Point2d > projectedPoints;
    ::cv::projectPoints(m_chessboardModel, rvec, tvec, intrinsic, distortionCoefficients, projectedPoints);

    const auto outputPl = this->getInOut< ::fwData::PointList >(s_REPROJECTED_CHESSBOARD_INOUT);
    SLM_ASSERT("Missing projected points list.", outputPl);

    ::cvIO::PointList::copyFromCv(projectedPoints, outputPl);

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

    return connections;
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

    for(unsigned long i = 0; i < height - 1; ++i)
    {
        const double x = i * squareSize;

        for(unsigned long j = 0; j < width - 1; ++j)
        {
            const double y = j * squareSize;
            m_chessboardModel.push_back(::cv::Point3d(x, y, 0.));
        }
    }
}

//------------------------------------------------------------------------------

} // namespace trackingCalibration
