/************************************************************************
 *
 * Copyright (C) 2014-2021 IRCAD France
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

#include "videoCalibration/SOpenCVExtrinsic.hpp"

#include <core/com/Signal.hxx>
#include <core/com/Slot.hxx>
#include <core/com/Slots.hxx>
#include <core/runtime/ConfigurationElement.hpp>
#include <core/tools/fwID.hpp>
#include <core/tools/Object.hpp>

#include <cvIO/Matrix.hpp>

#include <data/CalibrationInfo.hpp>
#include <data/Camera.hpp>
#include <data/CameraSeries.hpp>
#include <data/mt/ObjectReadLock.hpp>
#include <data/mt/ObjectWriteLock.hpp>
#include <data/PointList.hpp>
#include <data/TransformationMatrix3D.hpp>

#include <fwPreferences/helper.hpp>

#include <services/IService.hpp>
#include <services/macros.hpp>

#include <opencv2/calib3d.hpp>
#include <opencv2/core.hpp>

fwServicesRegisterMacro(services::ICalibration, ::videoCalibration::SOpenCVExtrinsic, data::CameraSeries)

namespace videoCalibration
{

static const core::com::Slots::SlotKeyType s_UPDATE_CHESSBOARD_SIZE_SLOT = "updateChessboardSize";
static const core::com::Signals::SignalKeyType s_ERROR_COMPUTED_SIG      = "errorComputed";

static const services::IService::KeyType s_CAMERASERIES_INOUT = "cameraSeries";
static const services::IService::KeyType s_MATRIX_OUTPUT      = "matrix";
static const services::IService::KeyType s_CALIBINFO1_INPUT   = "calibrationInfo1";
static const services::IService::KeyType s_CALIBINFO2_INPUT   = "calibrationInfo2";

// ----------------------------------------------------------------------------

SOpenCVExtrinsic::SOpenCVExtrinsic() noexcept :
    m_width(11),
    m_height(8),
    m_squareSize(20.0),
    m_camIndex(1)
{
    newSignal<ErrorComputedSignalType>(s_ERROR_COMPUTED_SIG);
    newSlot(s_UPDATE_CHESSBOARD_SIZE_SLOT, &SOpenCVExtrinsic::updateChessboardSize, this);
}

// ----------------------------------------------------------------------------

SOpenCVExtrinsic::~SOpenCVExtrinsic() noexcept
{
}

//------------------------------------------------------------------------------

void SOpenCVExtrinsic::configuring()
{
    core::runtime::ConfigurationElement::sptr cfgIdx = m_configuration->findConfigurationElement("camIndex");
    if(cfgIdx)
    {
        std::string idxStr = cfgIdx->getValue();
        SLM_ASSERT("'camIndex' is empty.", !idxStr.empty());
        m_camIndex = ::boost::lexical_cast<size_t>(idxStr);
    }

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

void SOpenCVExtrinsic::starting()
{
    this->updateChessboardSize();
}

// ----------------------------------------------------------------------------

void SOpenCVExtrinsic::stopping()
{
}

//------------------------------------------------------------------------------

void SOpenCVExtrinsic::swapping()
{
    this->stopping();
    this->starting();
}

//--------------------------------------------------------------------- ---------

void SOpenCVExtrinsic::updating()
{
    data::CameraSeries::sptr camSeries = this->getInOut< data::CameraSeries >(s_CAMERASERIES_INOUT);

    SLM_ASSERT("camera index must be > 0 and < camSeries->getNumberOfCameras()",
               m_camIndex > 0 && m_camIndex < camSeries->getNumberOfCameras());

    data::CalibrationInfo::csptr calInfo1 = this->getInput< data::CalibrationInfo>(s_CALIBINFO1_INPUT);
    data::CalibrationInfo::csptr calInfo2 = this->getInput< data::CalibrationInfo>(s_CALIBINFO2_INPUT);

    SLM_ASSERT("Object with 'calibrationInfo1' is not found", calInfo1);
    SLM_ASSERT("Object with 'calibrationInfo2' is not found", calInfo2);

    SLM_WARN_IF("Calibration info is empty.", calInfo1->getPointListContainer().empty());
    if(!calInfo1->getPointListContainer().empty())
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

        std::vector<std::vector< ::cv::Point2f > > imagePoints1;
        std::vector<std::vector< ::cv::Point2f > > imagePoints2;
        {
            data::mt::ObjectReadLock calInfo1Lock(calInfo1);
            data::mt::ObjectReadLock calInfo2Lock(calInfo2);

            data::CalibrationInfo::PointListContainerType ptlists1 = calInfo1->getPointListContainer();
            data::CalibrationInfo::PointListContainerType ptlists2 = calInfo2->getPointListContainer();

            SLM_ERROR_IF("The two calibrationInfo do not have the same size", ptlists1.size() != ptlists2.size());

            data::CalibrationInfo::PointListContainerType::iterator itr1    = ptlists1.begin();
            data::CalibrationInfo::PointListContainerType::iterator itr2    = ptlists2.begin();
            data::CalibrationInfo::PointListContainerType::iterator itrEnd1 = ptlists1.end();
            data::CalibrationInfo::PointListContainerType::iterator itrEnd2 = ptlists2.end();

            for(; itr1 != itrEnd1 && itr2 != itrEnd2; ++itr1, ++itr2)
            {
                data::PointList::sptr ptList1 = *itr1;
                data::PointList::sptr ptList2 = *itr2;
                std::vector< ::cv::Point2f > imgPoint1;
                std::vector< ::cv::Point2f > imgPoint2;

                for(data::Point::csptr point : ptList1->getPoints())
                {
                    SLM_ASSERT("point is null", point);
                    imgPoint1.push_back(::cv::Point2f(
                                            static_cast<float>(point->getCoord()[0]),
                                            static_cast<float>(point->getCoord()[1])));
                }

                for(data::Point::csptr point : ptList2->getPoints())
                {
                    SLM_ASSERT("point is null", point);
                    imgPoint2.push_back(::cv::Point2f(
                                            static_cast<float>(point->getCoord()[0]),
                                            static_cast<float>(point->getCoord()[1])));
                }

                imagePoints1.push_back(imgPoint1);
                imagePoints2.push_back(imgPoint2);
                objectPoints.push_back(points);
            }
        }

        // Set the cameras
        ::cv::Mat cameraMatrix1 = ::cv::Mat::eye(3, 3, CV_64F);
        ::cv::Mat cameraMatrix2 = ::cv::Mat::eye(3, 3, CV_64F);

        std::vector<float> distortionCoefficients1(5);
        std::vector<float> distortionCoefficients2(5);
        ::cv::Mat rotationMatrix;
        ::cv::Mat translationVector;
        ::cv::Mat essentialMatrix;
        ::cv::Mat fundamentalMatrix;

        data::Image::sptr img = calInfo1->getImageContainer().front();
        ::cv::Size2i imgsize(static_cast<int>(img->getSize2()[0]), static_cast<int>(img->getSize2()[1]));
        {

            data::mt::ObjectReadLock camSeriesLock(camSeries);
            data::Camera::sptr cam1 = camSeries->getCamera(0);
            data::Camera::sptr cam2 = camSeries->getCamera(m_camIndex);

            data::mt::ObjectReadLock cam1Lock(cam1);
            data::mt::ObjectReadLock cam2Lock(cam2);

            cameraMatrix1.at<double>(0, 0) = cam1->getFx();
            cameraMatrix1.at<double>(1, 1) = cam1->getFy();
            cameraMatrix1.at<double>(0, 2) = cam1->getCx();
            cameraMatrix1.at<double>(1, 2) = cam1->getCy();

            cameraMatrix2.at<double>(0, 0) = cam2->getFx();
            cameraMatrix2.at<double>(1, 1) = cam2->getFy();
            cameraMatrix2.at<double>(0, 2) = cam2->getCx();
            cameraMatrix2.at<double>(1, 2) = cam2->getCy();
            for (size_t i = 0; i < 5; ++i)
            {
                distortionCoefficients1[i] = static_cast<float>(cam1->getDistortionCoefficient()[i]);
                distortionCoefficients2[i] = static_cast<float>(cam2->getDistortionCoefficient()[i]);
            }
        }
        double err = ::cv::stereoCalibrate(objectPoints, imagePoints1, imagePoints2,
                                           cameraMatrix1, distortionCoefficients1,
                                           cameraMatrix2, distortionCoefficients2,
                                           imgsize, rotationMatrix, translationVector, essentialMatrix,
                                           fundamentalMatrix,
                                           ::cv::CALIB_FIX_INTRINSIC,
                                           ::cv::TermCriteria(::cv::TermCriteria::MAX_ITER + ::cv::TermCriteria::EPS,
                                                              100, 1e-5));

        this->signal<ErrorComputedSignalType>(s_ERROR_COMPUTED_SIG)->asyncEmit(err);

        data::TransformationMatrix3D::sptr matrix = data::TransformationMatrix3D::New();
        ::cv::Mat cv4x4 = ::cv::Mat::eye(4, 4, CV_64F);
        rotationMatrix.copyTo(cv4x4(::cv::Rect(0, 0, 3, 3)));
        translationVector.copyTo(cv4x4(::cv::Rect(3, 0, 1, 3)));

        ::cvIO::Matrix::copyFromCv(cv4x4, matrix);

        {
            data::mt::ObjectWriteLock camSeriesLock(camSeries);
            camSeries->setExtrinsicMatrix(m_camIndex, matrix);
        }

        data::CameraSeries::ExtrinsicCalibratedSignalType::sptr sig;
        sig = camSeries->signal< data::CameraSeries::ExtrinsicCalibratedSignalType > (
            data::CameraSeries::s_EXTRINSIC_CALIBRATED_SIG);

        sig->asyncEmit();

        // Export matrix if needed.
        this->setOutput(s_MATRIX_OUTPUT, matrix);

    }
}

//------------------------------------------------------------------------------

void SOpenCVExtrinsic::updateChessboardSize()
{
    const std::string widthStr = ::fwPreferences::getPreference(m_widthKey);
    if(!widthStr.empty())
    {
        m_width = std::stoi(widthStr);
    }
    const std::string heightStr = ::fwPreferences::getPreference(m_heightKey);
    if(!heightStr.empty())
    {
        m_height = std::stoi(heightStr);
    }
    const std::string squareSizeStr = ::fwPreferences::getPreference(m_squareSizeKey);
    if(!squareSizeStr.empty())
    {
        m_squareSize = std::stof(squareSizeStr);
    }
}

//------------------------------------------------------------------------------

} // namespace videoCalibration
