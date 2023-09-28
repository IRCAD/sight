/************************************************************************
 *
 * Copyright (C) 2014-2023 IRCAD France
 * Copyright (C) 2014-2021 IHU Strasbourg
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

#include "SOpenCVExtrinsic.hpp"

#include <core/com/signal.hxx>
#include <core/com/slot.hxx>
#include <core/com/slots.hxx>

#include <data/CalibrationInfo.hpp>
#include <data/Camera.hpp>
#include <data/CameraSet.hpp>
#include <data/Matrix4.hpp>
#include <data/PointList.hpp>

#include <io/opencv/Matrix.hpp>

#include <ui/__/Preferences.hpp>

#include <opencv2/calib3d.hpp>
#include <opencv2/core.hpp>

namespace sight::module::geometry::vision
{

static const core::com::slots::key_t UPDATE_CHESSBOARD_SIZE_SLOT = "updateChessboardSize";
static const core::com::signals::key_t ERROR_COMPUTED_SIG        = "errorComputed";

// ----------------------------------------------------------------------------

SOpenCVExtrinsic::SOpenCVExtrinsic() noexcept
{
    new_signal<ErrorComputedSignalType>(ERROR_COMPUTED_SIG);
    new_slot(UPDATE_CHESSBOARD_SIZE_SLOT, &SOpenCVExtrinsic::updateChessboardSize, this);
}

// ----------------------------------------------------------------------------

SOpenCVExtrinsic::~SOpenCVExtrinsic() noexcept =
    default;

//------------------------------------------------------------------------------

void SOpenCVExtrinsic::configuring()
{
    const auto config = this->getConfiguration();
    m_camIndex = config.get<std::size_t>("camIndex");

    const auto cfgBoard = config.get_child("board.<xmlattr>");

    m_widthKey = cfgBoard.get<std::string>("width");
    SIGHT_ASSERT("Attribute 'width' is empty", !m_widthKey.empty());

    m_heightKey = cfgBoard.get<std::string>("height");
    SIGHT_ASSERT("Attribute 'height' is empty", !m_heightKey.empty());

    if(const auto squareSizeKey = cfgBoard.get_optional<std::string>("squareSize"); squareSizeKey.has_value())
    {
        m_squareSizeKey = squareSizeKey.value();
        SIGHT_ASSERT("Attribute 'squareSize' is empty", !m_squareSizeKey.empty());
    }
}

// ----------------------------------------------------------------------------

void SOpenCVExtrinsic::starting()
{
    this->updateChessboardSize();
}

// ----------------------------------------------------------------------------

void SOpenCVExtrinsic::stopping()
{
}

//------------------------------------------------------------------------------

void SOpenCVExtrinsic::updating()
{
    const auto calInfo1 = m_calibrationInfo1.lock();
    const auto calInfo2 = m_calibrationInfo2.lock();

    SIGHT_ASSERT("Object with 'calibrationInfo1' is not found", calInfo1);
    SIGHT_ASSERT("Object with 'calibrationInfo2' is not found", calInfo2);

    SIGHT_WARN_IF("Calibration info is empty.", calInfo1->getPointListContainer().empty());
    if(!calInfo1->getPointListContainer().empty())
    {
        std::vector<std::vector<cv::Point3f> > objectPoints;

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

        std::vector<std::vector<cv::Point2f> > imagePoints1;
        std::vector<std::vector<cv::Point2f> > imagePoints2;
        {
            const auto pt_lists1 = calInfo1->getPointListContainer();
            const auto pt_lists2 = calInfo2->getPointListContainer();

            SIGHT_ERROR_IF("The two calibrationInfo do not have the same size", pt_lists1.size() != pt_lists2.size());

            auto itr1    = pt_lists1.begin();
            auto itr2    = pt_lists2.begin();
            auto itrEnd1 = pt_lists1.end();
            auto itrEnd2 = pt_lists2.end();

            for( ; itr1 != itrEnd1 && itr2 != itrEnd2 ; ++itr1, ++itr2)
            {
                const data::PointList::csptr& ptList1 = *itr1;
                const data::PointList::csptr& ptList2 = *itr2;
                std::vector<cv::Point2f> imgPoint1;
                std::vector<cv::Point2f> imgPoint2;

                for(data::Point::csptr point : ptList1->getPoints())
                {
                    SIGHT_ASSERT("point is null", point);
                    imgPoint1.emplace_back(
                        static_cast<float>(point->getCoord()[0]),
                        static_cast<float>(point->getCoord()[1])

                    );
                }

                for(data::Point::csptr point : ptList2->getPoints())
                {
                    SIGHT_ASSERT("point is null", point);
                    imgPoint2.emplace_back(
                        static_cast<float>(point->getCoord()[0]),
                        static_cast<float>(point->getCoord()[1])

                    );
                }

                imagePoints1.push_back(imgPoint1);
                imagePoints2.push_back(imgPoint2);
                objectPoints.push_back(points);
            }
        }

        // Set the cameras
        cv::Mat cameraMatrix1 = cv::Mat::eye(3, 3, CV_64F);
        cv::Mat cameraMatrix2 = cv::Mat::eye(3, 3, CV_64F);

        std::vector<float> distortionCoefficients1(5);
        std::vector<float> distortionCoefficients2(5);
        cv::Mat rotationMatrix;
        cv::Mat translationVector;
        cv::Mat essentialMatrix;
        cv::Mat fundamentalMatrix;

        const auto camSeries = m_camera_set.lock();

        SIGHT_ASSERT(
            "camera index must be > 0 and < camSeries->size()",
            m_camIndex > 0 && m_camIndex < camSeries->size()
        );

        data::Image::csptr img = calInfo1->getImageContainer().front();
        cv::Size2i imgsize(static_cast<int>(img->size()[0]), static_cast<int>(img->size()[1]));
        {
            data::Camera::csptr cam1 = camSeries->get_camera(0);
            data::Camera::csptr cam2 = camSeries->get_camera(m_camIndex);

            data::mt::locked_ptr cam1Lock(cam1);
            data::mt::locked_ptr cam2Lock(cam2);

            cameraMatrix1.at<double>(0, 0) = cam1->getFx();
            cameraMatrix1.at<double>(1, 1) = cam1->getFy();
            cameraMatrix1.at<double>(0, 2) = cam1->getCx();
            cameraMatrix1.at<double>(1, 2) = cam1->getCy();

            cameraMatrix2.at<double>(0, 0) = cam2->getFx();
            cameraMatrix2.at<double>(1, 1) = cam2->getFy();
            cameraMatrix2.at<double>(0, 2) = cam2->getCx();
            cameraMatrix2.at<double>(1, 2) = cam2->getCy();
            for(std::size_t i = 0 ; i < 5 ; ++i)
            {
                distortionCoefficients1[i] = static_cast<float>(cam1->getDistortionCoefficient()[i]);
                distortionCoefficients2[i] = static_cast<float>(cam2->getDistortionCoefficient()[i]);
            }
        }
        double err = cv::stereoCalibrate(
            objectPoints,
            imagePoints1,
            imagePoints2,
            cameraMatrix1,
            distortionCoefficients1,
            cameraMatrix2,
            distortionCoefficients2,
            imgsize,
            rotationMatrix,
            translationVector,
            essentialMatrix,
            fundamentalMatrix,
            cv::CALIB_FIX_INTRINSIC,
            cv::TermCriteria(
                cv::TermCriteria::MAX_ITER + cv::TermCriteria::EPS,
                100,
                1e-5
            )
        );

        this->signal<ErrorComputedSignalType>(ERROR_COMPUTED_SIG)->async_emit(err);

        data::Matrix4::sptr matrix = std::make_shared<data::Matrix4>();
        cv::Mat cv4x4              = cv::Mat::eye(4, 4, CV_64F);
        rotationMatrix.copyTo(cv4x4(cv::Rect(0, 0, 3, 3)));
        translationVector.copyTo(cv4x4(cv::Rect(3, 0, 1, 3)));

        io::opencv::Matrix::copyFromCv(cv4x4, matrix);

        {
            camSeries->set_extrinsic_matrix(m_camIndex, matrix);
        }

        data::CameraSet::extrinsic_calibrated_signal_t::sptr sig;
        sig = camSeries->signal<data::CameraSet::extrinsic_calibrated_signal_t>(
            data::CameraSet::EXTRINSIC_CALIBRATED_SIG
        );

        sig->async_emit();

        // Export matrix if needed.
        m_matrix = matrix;
    }
}

//------------------------------------------------------------------------------

void SOpenCVExtrinsic::updateChessboardSize()
{
    try
    {
        ui::Preferences preferences;
        m_width      = preferences.get(m_widthKey, m_width);
        m_height     = preferences.get(m_heightKey, m_height);
        m_squareSize = preferences.get(m_squareSizeKey, m_squareSize);
    }
    catch(const ui::PreferencesDisabled&)
    {
        // Nothing to do..
    }
}

//------------------------------------------------------------------------------

} // namespace sight::module::geometry::vision
