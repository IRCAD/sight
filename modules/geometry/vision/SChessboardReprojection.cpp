/************************************************************************
 *
 * Copyright (C) 2019-2023 IRCAD France
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

#include "SChessboardReprojection.hpp"

#include <core/com/Signal.hxx>
#include <core/com/Slots.hxx>

#include <data/helper/MedicalImage.hpp>

#include <geometry/vision/helper.hpp>

#include <io/opencv/Camera.hpp>
#include <io/opencv/Image.hpp>
#include <io/opencv/Matrix.hpp>
#include <io/opencv/PointList.hpp>

#include <service/macros.hpp>

#include <ui/base/Preferences.hpp>

#include <opencv2/calib3d.hpp>
#include <opencv2/opencv.hpp>

namespace sight::module::geometry::vision
{

static const core::com::Signals::SignalKeyType s_ERROR_COMPUTED_SIG = "errorComputed";

static const core::com::Slots::SlotKeyType s_TOGGLE_DISTORTION_SLOT      = "toggleDistortion";
static const core::com::Slots::SlotKeyType s_UPDATE_CHESSBOARD_SIZE_SLOT = "updateChessboardSize";

//-----------------------------------------------------------------------------

SChessboardReprojection::SChessboardReprojection()
{
    newSlot(s_TOGGLE_DISTORTION_SLOT, &SChessboardReprojection::toggleDistortion, this);
    newSlot(s_UPDATE_CHESSBOARD_SIZE_SLOT, &SChessboardReprojection::updateChessboardSize, this);

    m_errorComputedSig = newSignal<ErrorComputedSignalType>(s_ERROR_COMPUTED_SIG);
}

//-----------------------------------------------------------------------------

SChessboardReprojection::~SChessboardReprojection()
= default;

//-----------------------------------------------------------------------------

void SChessboardReprojection::configuring()
{
    const ConfigType configTree  = this->getConfiguration();
    const ConfigType boardConfig = configTree.get_child("board");
    const ConfigType config      = configTree.get_child("config.<xmlattr>");

    m_widthKey = boardConfig.get<std::string>("<xmlattr>.width");
    SIGHT_ASSERT("Missing board width preference key.", !m_widthKey.empty());
    m_heightKey = boardConfig.get<std::string>("<xmlattr>.height");
    SIGHT_ASSERT("Missing board height preference key.", !m_heightKey.empty());
    m_squareSizeKey = boardConfig.get<std::string>("<xmlattr>.squareSize");
    SIGHT_ASSERT("Missing board square size preference key.", !m_squareSizeKey.empty());

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
    const auto detectedChessboard = m_detectedChessboard.lock();
    SIGHT_ASSERT("Missing 'detectedChessboard'.", detectedChessboard);

    if(detectedChessboard->getPoints().empty())
    {
        return;
    }

    const auto camera = m_camera.lock();
    SIGHT_ASSERT("Missing 'camera'.", camera);

    cv::Size imgSize;
    cv::Mat cameraMx;
    cv::Mat distortionCoefficients;
    std::tie(cameraMx, imgSize, distortionCoefficients) = io::opencv::Camera::copyToCv(camera.get_shared());

    cv::Mat rvec;
    cv::Mat tvec;

    std::vector<cv::Point2d> detectedPts;
    io::opencv::PointList::copyToCv(detectedChessboard.get_shared(), detectedPts);

    // Cast Point2d to Point2f ...
    std::vector<cv::Point2f> detectedPointsF;
    std::copy(detectedPts.begin(), detectedPts.end(), std::back_inserter(detectedPointsF));

    double rmse = -1.;
    std::vector<cv::Point2f> reprojectedPts;

    if(camera->getIsCalibrated() && !detectedPointsF.empty())
    {
        const auto transform = m_transform.lock();
        SIGHT_ASSERT("Missing 'transform'.", transform);

        io::opencv::Matrix::copyToCv(transform.get_shared(), rvec, tvec);

        std::tie(rmse, reprojectedPts) = sight::geometry::vision::helper::computeReprojectionError(
            m_chessboardModel,
            detectedPointsF,
            rvec,
            tvec,
            cameraMx,
            distortionCoefficients
        );

        m_errorComputedSig->asyncEmit(rmse);
    }

    const auto videoImage = m_videoImage.lock();
    SIGHT_ERROR_IF(
        "Drawing is enabled in the configuration but there is no 'videoImage' to draw onto.",
        !videoImage && (m_drawDetected || m_drawReprojection || m_drawReprojectionError)
    );

    if(videoImage)
    {
        // Reprojected points have a radius equal to 1/3000th of the image's height.
        int reprojectionRadius = static_cast<int>(std::floor(0.003 * imgSize.height));
        reprojectionRadius = std::max(reprojectionRadius, 1);

        if(!data::helper::MedicalImage::checkImageValidity(videoImage.get_shared()))
        {
            return;
        }

        cv::Mat img = io::opencv::Image::moveToCv(videoImage.get_shared());

        const bool drawingEnabled = m_drawDetected || m_drawReprojection || m_drawReprojectionError;
        SIGHT_WARN_IF(
            "An inout 'videoImage' was given to the service but no drawing operation was enabled.",
            !drawingEnabled
        );

        if(m_drawReprojection)
        {
            std::vector<cv::Point2f> drawnDetectedPoints;
            if(!m_distortReprojection && camera->getIsCalibrated())
            {
                cv::undistortPoints(cv::Mat(detectedPointsF), drawnDetectedPoints, cameraMx, distortionCoefficients);
                for(auto& pt : drawnDetectedPoints)
                {
                    const auto pt3d = cv::Matx33f(cameraMx) * pt;
                    pt = cv::Point2f(pt3d.x, pt3d.y);
                }
            }
            else
            {
                drawnDetectedPoints = detectedPointsF;
            }

            const int detectionThickness = reprojectionRadius < 2 ? 1 : 2;
            for(const auto& pt : drawnDetectedPoints)
            {
                cv::circle(img, pt, reprojectionRadius + 3, cv::Scalar(0, 255, 255, 255), detectionThickness);
            }
        }

        if(rmse >= 0.)
        {
            if(m_drawReprojection)
            {
                std::vector<cv::Point2f> drawnReprojectedPts;
                if(m_distortReprojection)
                {
                    drawnReprojectedPts = reprojectedPts;
                }
                else
                {
                    // Project the model but assume the image isn't distorted.
                    cv::projectPoints(
                        cv::Mat(m_chessboardModel),
                        rvec,
                        tvec,
                        cameraMx,
                        cv::Mat(),
                        drawnReprojectedPts
                    );
                }

                for(const auto& pt : drawnReprojectedPts)
                {
                    cv::circle(img, pt, reprojectionRadius, cv::Scalar(255, 255, 0, 255), cv::FILLED);
                }
            }

            if(m_drawReprojectionError)
            {
                const auto fontFace                    = cv::FONT_HERSHEY_SIMPLEX;
                const std::string reprojectionErrorStr = "Reprojection rmse: " + std::to_string(rmse) + " pixels";
                const int leftPadding                  = static_cast<int>(0.05 * imgSize.width);
                const int topPadding                   = static_cast<int>(0.05 * imgSize.height);

                cv::putText(
                    img,
                    reprojectionErrorStr,
                    cv::Point(leftPadding, topPadding),
                    fontFace,
                    1.,
                    cv::Scalar(255, 255, 0, 255),
                    2
                );
            }
        }

        if(drawingEnabled)
        {
            auto sig =
                videoImage->signal<data::Image::BufferModifiedSignalType>(data::Image::s_BUFFER_MODIFIED_SIG);

            sig->asyncEmit();
        }
    }
}

//-----------------------------------------------------------------------------

void SChessboardReprojection::stopping()
{
    m_chessboardModelOut.reset();
}

//-----------------------------------------------------------------------------

void SChessboardReprojection::toggleDistortion()
{
    m_distortReprojection = !m_distortReprojection;
}

//------------------------------------------------------------------------------

void SChessboardReprojection::updateChessboardSize()
{
    std::uint64_t width(1);
    std::uint64_t height(1);
    double squareSize(0.);

    try
    {
        ui::base::Preferences preferences;
        width      = preferences.get(m_widthKey, width);
        height     = preferences.get(m_heightKey, height);
        squareSize = preferences.get(m_squareSizeKey, squareSize);
    }
    catch(const ui::base::PreferencesDisabled&)
    {
        // Nothing to do..
    }

    m_chessboardModel.clear();

    data::PointList::sptr chessboardModelPl = data::PointList::New();

    for(std::uint64_t i = 0 ; i < height - 1 ; ++i)
    {
        const double x = double(i) * squareSize;

        for(std::uint64_t j = 0 ; j < width - 1 ; ++j)
        {
            const double y = double(j) * squareSize;
            m_chessboardModel.push_back(cv::Point3d(x, y, 0.));
            chessboardModelPl->pushBack(data::Point::New(x, y, 0.));
        }
    }

    if(m_hasOutputChessboard)
    {
        m_chessboardModelOut = chessboardModelPl;
    }
}

//-----------------------------------------------------------------------------

service::IService::KeyConnectionsMap SChessboardReprojection::getAutoConnections() const
{
    return {
        {s_TRANSFORM_INPUT, data::Matrix4::s_MODIFIED_SIG, IService::slots::s_UPDATE},
        {s_DETECTED_CHESSBOARD_INPUT, data::PointList::s_MODIFIED_SIG, IService::slots::s_UPDATE},
        {s_CAMERA_INPUT, data::Camera::s_INTRINSIC_CALIBRATED_SIG, IService::slots::s_UPDATE},
        {s_CAMERA_INPUT, data::Camera::s_MODIFIED_SIG, IService::slots::s_UPDATE}
    };
}

//-----------------------------------------------------------------------------

} //namespace sight::module::geometry::vision
