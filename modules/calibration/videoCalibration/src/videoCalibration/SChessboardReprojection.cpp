/************************************************************************
 *
 * Copyright (C) 2019-2021 IRCAD France
 * Copyright (C) 2019-2021 IHU Strasbourg
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

#include "videoCalibration/SChessboardReprojection.hpp"

#include <calibration3d/helper.hpp>

#include <core/com/Signal.hxx>
#include <core/com/Slots.hxx>

#include <cvIO/Camera.hpp>
#include <cvIO/Image.hpp>
#include <cvIO/Matrix.hpp>
#include <cvIO/PointList.hpp>

#include <data/Camera.hpp>
#include <data/Image.hpp>
#include <data/mt/ObjectReadLock.hpp>
#include <data/mt/ObjectWriteLock.hpp>
#include <data/PointList.hpp>
#include <data/tools/fieldHelper/MedicalImageHelpers.hpp>
#include <data/TransformationMatrix3D.hpp>

#include <services/macros.hpp>

#include <opencv2/calib3d.hpp>
#include <opencv2/opencv.hpp>

#include <ui/base/preferences/helper.hpp>

fwServicesRegisterMacro( ::sight::services::IController, ::videoCalibration::SChessboardReprojection)

namespace videoCalibration
{

static const core::com::Signals::SignalKeyType s_ERROR_COMPUTED_SIG = "errorComputed";

static const services::IService::KeyType s_TRANSFORM_INPUT           = "transform";
static const services::IService::KeyType s_DETECTED_CHESSBOARD_INPUT = "detectedChessboard";
static const services::IService::KeyType s_CAMERA_INPUT              = "camera";

static const services::IService::KeyType s_VIDEO_IMAGE_INOUT = "videoImage";

static const services::IService::KeyType s_CHESSBOARD_MODEL_OUTPUT = "chessboardModel";

static const core::com::Slots::SlotKeyType s_TOGGLE_DISTORTION_SLOT      = "toggleDistortion";
static const core::com::Slots::SlotKeyType s_UPDATE_CHESSBOARD_SIZE_SLOT = "updateChessboardSize";

//-----------------------------------------------------------------------------

SChessboardReprojection::SChessboardReprojection()
{
    newSlot(s_TOGGLE_DISTORTION_SLOT, &SChessboardReprojection::toggleDistortion, this);
    newSlot(s_UPDATE_CHESSBOARD_SIZE_SLOT, &SChessboardReprojection::updateChessboardSize, this);

    m_errorComputedSig = newSignal< ErrorComputedSignalType >(s_ERROR_COMPUTED_SIG);
}

//-----------------------------------------------------------------------------

SChessboardReprojection::~SChessboardReprojection()
{
}

//-----------------------------------------------------------------------------

void SChessboardReprojection::configuring()
{
    const ConfigType configTree  = this->getConfigTree();
    const ConfigType boardConfig = configTree.get_child("board");
    const ConfigType config      = configTree.get_child("config.<xmlattr>");

    m_widthKey = boardConfig.get<std::string>("<xmlattr>.width");
    SLM_ASSERT("Missing board width preference key.", !m_widthKey.empty());
    m_heightKey = boardConfig.get<std::string>("<xmlattr>.height");
    SLM_ASSERT("Missing board height preference key.", !m_heightKey.empty());
    m_squareSizeKey = boardConfig.get<std::string>("<xmlattr>.squareSize");
    SLM_ASSERT("Missing board square size preference key.", !m_squareSizeKey.empty());

    const std::string outputKey = configTree.get_optional<std::string>("out.<xmlattr>.key").get_value_or("");
    if(outputKey == s_CHESSBOARD_MODEL_OUTPUT)
    {
        m_hasOutputChessboard = true;
    }

    m_distortReprojection = config.get<bool>("distortReprojection", true);
    m_drawReprojection    = config.get<bool>("drawReprojection", true);
    m_drawDetected        = config.get<bool>("drawDetected", true);
}

//-----------------------------------------------------------------------------

void SChessboardReprojection::starting()
{
    this->updateChessboardSize();
}

//-----------------------------------------------------------------------------

void SChessboardReprojection::updating()
{
    data::PointList::csptr detectedChessboard = this->getInput< data::PointList >(s_DETECTED_CHESSBOARD_INPUT);
    SLM_ASSERT("Missing 'detectedChessboard'.", detectedChessboard);
    data::mt::ObjectReadLock detectedPtsLock(detectedChessboard);

    if(detectedChessboard->getPoints().empty())
    {
        return;
    }

    data::Camera::csptr camera = this->getInput< data::Camera >(s_CAMERA_INPUT);
    SLM_ASSERT("Missing 'camera'.", camera);
    data::mt::ObjectReadLock cameraLock(camera);

    ::cv::Size imgSize;
    ::cv::Mat cameraMx, distortionCoefficents;
    std::tie(cameraMx, imgSize, distortionCoefficents) = ::cvIO::Camera::copyToCv(camera);

    ::cv::Mat rvec, tvec;

    std::vector< ::cv::Point2d > detectedPts;
    ::cvIO::PointList::copyToCv(detectedChessboard, detectedPts);

    // Cast Point2d to Point2f ...
    std::vector< ::cv::Point2f > detectedPointsF;
    std::copy(detectedPts.begin(), detectedPts.end(), std::back_inserter(detectedPointsF));

    double rmse = -1.;
    std::vector< ::cv::Point2f > reprojectedPts;

    if (camera->getIsCalibrated() && !detectedPointsF.empty())
    {
        data::TransformationMatrix3D::csptr transform =
            this->getInput< data::TransformationMatrix3D >(s_TRANSFORM_INPUT);
        SLM_ASSERT("Missing 'transform'.", transform);
        data::mt::ObjectReadLock trfLock(transform);

        ::cvIO::Matrix::copyToCv(transform, rvec, tvec);

        std::tie(rmse, reprojectedPts) = ::calibration3d::helper::computeReprojectionError(m_chessboardModel,
                                                                                           detectedPointsF,
                                                                                           rvec, tvec, cameraMx,
                                                                                           distortionCoefficents);

        m_errorComputedSig->asyncEmit(rmse);
    }

    data::Image::sptr videoImage = this->getInOut< data::Image >(s_VIDEO_IMAGE_INOUT);
    SLM_ERROR_IF("Drawing is enabled in the configuration but there is no 'videoImage' to draw onto.",
                 !videoImage && (m_drawDetected || m_drawReprojection || m_drawReprojectionError));

    if(videoImage)
    {
        // Reprojected points have a radius equal to 1/3000th of the image's height.
        int reprojRadius = static_cast<int>(std::floor(0.003 * imgSize.height));
        reprojRadius = std::max(reprojRadius, 1);

        data::mt::ObjectWriteLock imgLock(videoImage);
        if(!::data::tools::fieldHelper::MedicalImageHelpers::checkImageValidity(videoImage))
        {
            return;
        }

        ::cv::Mat img = ::cvIO::Image::moveToCv(videoImage);

        const bool drawingEnabled = m_drawReprojection || m_drawReprojection || m_drawReprojectionError;
        SLM_WARN_IF("An inout 'videoImage' was given to the service but no drawing operation was enabled.",
                    !drawingEnabled);

        if (m_drawReprojection)
        {
            std::vector< ::cv::Point2f > drawnDetectedPoints;
            if (!m_distortReprojection && camera->getIsCalibrated())
            {
                ::cv::undistortPoints(::cv::Mat(detectedPointsF), drawnDetectedPoints, cameraMx, distortionCoefficents);
                for (auto& pt : drawnDetectedPoints)
                {
                    const auto pt3d = ::cv::Matx33f(cameraMx) * pt;
                    pt = ::cv::Point2f(pt3d.x, pt3d.y);
                }
            }
            else
            {
                drawnDetectedPoints = detectedPointsF;
            }

            const int detectionThickness = reprojRadius < 2 ? 1 : 2;
            for (const auto& pt : drawnDetectedPoints)
            {
                ::cv::circle(img, pt, reprojRadius + 3, ::cv::Scalar(0, 255, 255, 255), detectionThickness);
            }
        }

        if(rmse >= 0.)
        {
            if (m_drawReprojection)
            {
                std::vector< ::cv::Point2f > drawnReprojPts;
                if (m_distortReprojection)
                {
                    drawnReprojPts = reprojectedPts;
                }
                else
                {
                    // Project the model but assume the image isn't distorted.
                    ::cv::projectPoints(::cv::Mat(m_chessboardModel), rvec, tvec, cameraMx, ::cv::Mat(),
                                        drawnReprojPts);
                }

                for (const auto& pt : drawnReprojPts)
                {
                    ::cv::circle(img, pt, reprojRadius, ::cv::Scalar(255, 255, 0, 255), ::cv::FILLED);
                }
            }

            if (m_drawReprojectionError)
            {
                const auto fontFace              = ::cv::FONT_HERSHEY_SIMPLEX;
                const std::string reprojErrorStr = "Reprojection rmse: " + std::to_string(rmse) + " pixels";
                const int leftPadding            = static_cast<int>(0.05 * imgSize.width);
                const int topPadding             = static_cast<int>(0.05 * imgSize.height);

                ::cv::putText(img, reprojErrorStr, ::cv::Point(leftPadding, topPadding), fontFace, 1.,
                              ::cv::Scalar(255, 255, 0, 255), 2);
            }
        }

        if(drawingEnabled)
        {
            auto sig =
                videoImage->signal< data::Image::BufferModifiedSignalType >(data::Image::s_BUFFER_MODIFIED_SIG);

            sig->asyncEmit();
        }
    }
}

//-----------------------------------------------------------------------------

void SChessboardReprojection::stopping()
{
}

//-----------------------------------------------------------------------------

void SChessboardReprojection::toggleDistortion()
{
    m_distortReprojection = !m_distortReprojection;
}

//------------------------------------------------------------------------------

void SChessboardReprojection::updateChessboardSize()
{
    unsigned long width(0), height(0);
    const std::string widthStr = modules::ui::base::preferences::getPreference(m_widthKey);
    if(!widthStr.empty())
    {
        width = std::stoul(widthStr);
    }
    const std::string heightStr = modules::ui::base::preferences::getPreference(m_heightKey);
    if(!heightStr.empty())
    {
        height = std::stoul(heightStr);
    }

    double squareSize(0.);
    const std::string squareSizeStr = modules::ui::base::preferences::getPreference(m_squareSizeKey);
    if(!squareSizeStr.empty())
    {
        squareSize = std::stod(squareSizeStr);
    }

    m_chessboardModel.clear();

    data::PointList::sptr chessboardModelPl = data::PointList::New();

    for(unsigned long i = 0; i < height - 1; ++i)
    {
        const double x = i * squareSize;

        for(unsigned long j = 0; j < width - 1; ++j)
        {
            const double y = j * squareSize;
            m_chessboardModel.push_back(::cv::Point3d(x, y, 0.));
            chessboardModelPl->pushBack(data::Point::New(x, y, 0.));
        }
    }

    if(m_hasOutputChessboard)
    {
        this->setOutput(s_CHESSBOARD_MODEL_OUTPUT, chessboardModelPl);
    }
}

//-----------------------------------------------------------------------------

::services::IService::KeyConnectionsMap SChessboardReprojection::getAutoConnections() const
{
    KeyConnectionsMap connections;
    connections.push(s_TRANSFORM_INPUT, data::TransformationMatrix3D::s_MODIFIED_SIG, s_UPDATE_SLOT);
    connections.push(s_DETECTED_CHESSBOARD_INPUT, data::PointList::s_MODIFIED_SIG, s_UPDATE_SLOT);
    connections.push(s_CAMERA_INPUT, data::Camera::s_INTRINSIC_CALIBRATED_SIG, s_UPDATE_SLOT);
    connections.push(s_CAMERA_INPUT, data::Camera::s_MODIFIED_SIG, s_UPDATE_SLOT);

    return connections;
}

//-----------------------------------------------------------------------------

} //namespace videoCalibration
