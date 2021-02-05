/************************************************************************
 *
 * Copyright (C) 2014-2021 IRCAD France
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

#include "videoCalibration/SOpenCVIntrinsic.hpp"

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

#include <opencv2/calib3d.hpp>
#include <opencv2/core.hpp>

#include <ui/base/preferences/helper.hpp>

fwServicesRegisterMacro(::sight::services::ICalibration, ::videoCalibration::SOpenCVIntrinsic, ::sight::data::Camera)

namespace videoCalibration
{

static const core::com::Slots::SlotKeyType s_UPDATE_CHESSBOARD_SIZE_SLOT = "updateChessboardSize";

static const core::com::Signals::SignalKeyType s_ERROR_COMPUTED_SIG = "errorComputed";

// ----------------------------------------------------------------------------

SOpenCVIntrinsic::SOpenCVIntrinsic() noexcept :
    m_width(11),
    m_height(8),
    m_squareSize(20.0)
{
    newSignal<ErrorComputedSignalType>(s_ERROR_COMPUTED_SIG);
    newSlot(s_UPDATE_CHESSBOARD_SIZE_SLOT, &SOpenCVIntrinsic::updateChessboardSize, this);
}

// ----------------------------------------------------------------------------

SOpenCVIntrinsic::~SOpenCVIntrinsic() noexcept
{
}

//------------------------------------------------------------------------------

void SOpenCVIntrinsic::configuring()
{
    core::runtime::ConfigurationElement::sptr cfgBoard = m_configuration->findConfigurationElement("board");
    SLM_ASSERT("Tag 'board' not found.", cfgBoard);

    SLM_ASSERT("Attribute 'width' is missing.", cfgBoard->hasAttribute("width"));
    m_widthKey = cfgBoard->getAttributeValue("width");
    SLM_ASSERT("Attribute 'width' is empty", !m_widthKey.empty());

    SLM_ASSERT("Attribute 'height' is missing.", cfgBoard->hasAttribute("height"));
    m_heightKey = cfgBoard->getAttributeValue("height");
    SLM_ASSERT("Attribute 'height' is empty", !m_heightKey.empty());

    if( cfgBoard->hasAttribute("squareSize"))
    {
        m_squareSizeKey = cfgBoard->getAttributeValue("squareSize");
        SLM_ASSERT("Attribute 'squareSize' is empty", !m_squareSizeKey.empty());
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
    SLM_WARN_IF("Calibration info is empty.", calInfo->getPointListContainer().empty());

    if(!calInfo->getPointListContainer().empty())
    {
        std::vector<std::vector< ::cv::Point3f > > objectPoints;

        std::vector< ::cv::Point3f > points;
        for (unsigned int y = 0; y < m_height - 1; ++y)
        {
            for (unsigned int x = 0; x < m_width - 1; ++x)
            {
                points.push_back(::cv::Point3f(static_cast<float>(x)*m_squareSize,
                                               static_cast<float>(y)*m_squareSize,
                                               0));
            }
        }

        std::vector<std::vector< ::cv::Point2f > > imagePoints;

        {
            data::mt::ObjectReadLock calInfoLock(calInfo);
            for(data::PointList::sptr capture : calInfo->getPointListContainer())
            {
                std::vector< ::cv::Point2f > dst;

                for(data::Point::csptr point : capture->getPoints())
                {
                    SLM_ASSERT("point is null", point);
                    dst.push_back(::cv::Point2f(static_cast<float>(point->getCoord()[0]),
                                                static_cast<float>(point->getCoord()[1])));
                }
                imagePoints.push_back(dst);
                objectPoints.push_back(points);
            }
        }

        data::Image::sptr img = calInfo->getImageContainer().front();

        ::cv::Mat cameraMatrix;
        std::vector<float> distCoeffs;
        std::vector<cv::Mat> rvecs;
        std::vector<cv::Mat> tvecs;
        ::cv::Size2i imgsize(static_cast<int>(img->getSize2()[0]), static_cast<int>(img->getSize2()[1]));

        double err = ::cv::calibrateCamera(objectPoints, imagePoints, imgsize, cameraMatrix, distCoeffs, rvecs, tvecs);

        this->signal<ErrorComputedSignalType>(s_ERROR_COMPUTED_SIG)->asyncEmit(err);

        if(poseCamera)
        {
            poseCamera->getContainer().clear();

            for(size_t index = 0; index < rvecs.size(); ++index)
            {
                data::TransformationMatrix3D::sptr mat3D = data::TransformationMatrix3D::New();

                ::cvIO::Matrix::copyFromCv(rvecs.at(index), tvecs.at(index), mat3D);

                poseCamera->getContainer().push_back(mat3D);
                auto sig = poseCamera->signal< data::Vector::AddedObjectsSignalType >(
                    data::Vector::s_ADDED_OBJECTS_SIG);
                sig->asyncEmit(poseCamera->getContainer());
            }
        }

        SLM_DEBUG("Calibration error :" << err);

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

void SOpenCVIntrinsic::updateChessboardSize()
{
    const std::string widthStr = modules::ui::base::preferences::getPreference(m_widthKey);
    if(!widthStr.empty())
    {
        m_width = std::stoi(widthStr);
    }
    const std::string heightStr = modules::ui::base::preferences::getPreference(m_heightKey);
    if(!heightStr.empty())
    {
        m_height = std::stoi(heightStr);
    }
    const std::string squareSizeStr = modules::ui::base::preferences::getPreference(m_squareSizeKey);
    if(!squareSizeStr.empty())
    {
        m_squareSize = std::stof(squareSizeStr);
    }
}

//------------------------------------------------------------------------------

} // namespace videoCalibration
