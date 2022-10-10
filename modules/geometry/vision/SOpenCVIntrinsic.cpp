/************************************************************************
 *
 * Copyright (C) 2014-2022 IRCAD France
 * Copyright (C) 2014-2020 IHU Strasbourg
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

#include "SOpenCVIntrinsic.hpp"

#include <core/com/Signal.hxx>
#include <core/com/Slots.hxx>

#include <io/opencv/Matrix.hpp>

#include <ui/base/Preferences.hpp>

#include <opencv2/calib3d.hpp>
#include <opencv2/core.hpp>

namespace sight::module::geometry::vision
{

static const core::com::Slots::SlotKeyType s_UPDATE_CHESSBOARD_SIZE_SLOT = "updateChessboardSize";

static const core::com::Signals::SignalKeyType s_ERROR_COMPUTED_SIG = "errorComputed";

// ----------------------------------------------------------------------------

SOpenCVIntrinsic::SOpenCVIntrinsic() noexcept
{
    newSignal<ErrorComputedSignalType>(s_ERROR_COMPUTED_SIG);
    newSlot(s_UPDATE_CHESSBOARD_SIZE_SLOT, &SOpenCVIntrinsic::updateChessboardSize, this);
}

// ----------------------------------------------------------------------------

SOpenCVIntrinsic::~SOpenCVIntrinsic() noexcept =
    default;

//------------------------------------------------------------------------------

void SOpenCVIntrinsic::configuring()
{
    const auto config   = this->getConfigTree();
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

void SOpenCVIntrinsic::starting()
{
    this->updateChessboardSize();
}

// ----------------------------------------------------------------------------

void SOpenCVIntrinsic::stopping()
{
}

//--------------------------------------------------------------------- ---------

void SOpenCVIntrinsic::updating()
{
    const auto calInfo = m_calibrationInfo.lock();

    SIGHT_ASSERT("Object with 'calibrationInfo' is not found", calInfo);
    SIGHT_WARN_IF("Calibration info is empty.", calInfo->getPointListContainer().empty());

    if(!calInfo->getPointListContainer().empty())
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

        std::vector<std::vector<cv::Point2f> > imagePoints;

        for(const data::PointList::csptr& capture : calInfo->getPointListContainer())
        {
            std::vector<cv::Point2f> dst;

            for(data::Point::csptr point : capture->getPoints())
            {
                SIGHT_ASSERT("point is null", point);
                dst.emplace_back(
                    static_cast<float>(point->getCoord()[0]),
                    static_cast<float>(point->getCoord()[1])

                );
            }

            imagePoints.push_back(dst);
            objectPoints.push_back(points);
        }

        data::Image::csptr img = calInfo->getImageContainer().front();

        cv::Mat cameraMatrix;
        std::vector<float> distCoeffs;
        std::vector<cv::Mat> rvecs;
        std::vector<cv::Mat> tvecs;
        cv::Size2i imgsize(static_cast<int>(img->getSize()[0]), static_cast<int>(img->getSize()[1]));

        double err = cv::calibrateCamera(objectPoints, imagePoints, imgsize, cameraMatrix, distCoeffs, rvecs, tvecs);

        this->signal<ErrorComputedSignalType>(s_ERROR_COMPUTED_SIG)->asyncEmit(err);

        const auto poseCamera = m_poseVector.lock();
        if(poseCamera)
        {
            poseCamera->clear();

            for(std::size_t index = 0 ; index < rvecs.size() ; ++index)
            {
                data::Matrix4::sptr mat3D = data::Matrix4::New();

                io::opencv::Matrix::copyFromCv(rvecs.at(index), tvecs.at(index), mat3D);

                poseCamera->push_back(mat3D);
                auto sig = poseCamera->signal<data::Vector::added_signal_t>(
                    data::Vector::s_ADDED_OBJECTS_SIG
                );
                sig->asyncEmit(poseCamera->get_content());
            }
        }

        SIGHT_DEBUG("Calibration error :" << err);

        const auto cam = m_camera.lock();

        cam->setCx(cameraMatrix.at<double>(0, 2));
        cam->setCy(cameraMatrix.at<double>(1, 2));
        cam->setFx(cameraMatrix.at<double>(0, 0));
        cam->setFy(cameraMatrix.at<double>(1, 1));
        cam->setWidth(img->getSize()[0]);
        cam->setHeight(img->getSize()[1]);
        cam->setDistortionCoefficient(distCoeffs[0], distCoeffs[1], distCoeffs[2], distCoeffs[3], distCoeffs[4]);

        cam->setIsCalibrated(true);

        data::Camera::IntrinsicCalibratedSignalType::sptr sig;
        sig = cam->signal<data::Camera::IntrinsicCalibratedSignalType>(
            data::Camera::s_INTRINSIC_CALIBRATED_SIG
        );

        sig->asyncEmit();
    }
}

//------------------------------------------------------------------------------

void SOpenCVIntrinsic::updateChessboardSize()
{
    try
    {
        ui::base::Preferences preferences;

        if(const auto& saved = preferences.get_optional<decltype(m_width)>(m_widthKey); saved)
        {
            m_width = *saved;
        }

        if(const auto& saved = preferences.get_optional<decltype(m_height)>(m_heightKey); saved)
        {
            m_height = *saved;
        }

        if(const auto& saved = preferences.get_optional<decltype(m_squareSize)>(m_squareSizeKey); saved)
        {
            m_squareSize = *saved;
        }
    }
    catch(const ui::base::PreferencesDisabled&)
    {
        // Nothing to do..
    }
}

//------------------------------------------------------------------------------

} // namespace sight::module::geometry::vision
