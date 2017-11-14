/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2014-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "videoCalibration/SOpenCVIntrinsic.hpp"

#include <arData/CalibrationInfo.hpp>
#include <arData/Camera.hpp>

#include <fwCom/Slot.hxx>
#include <fwCom/Slots.hxx>

#include <fwData/mt/ObjectReadLock.hpp>
#include <fwData/mt/ObjectWriteLock.hpp>
#include <fwData/PointList.hpp>
#include <fwData/TransformationMatrix3D.hpp>
#include <fwData/Vector.hpp>

#include <fwPreferences/helper.hpp>

#include <fwRuntime/ConfigurationElement.hpp>

#include <fwServices/IService.hpp>
#include <fwServices/macros.hpp>

#include <fwTools/fwID.hpp>
#include <fwTools/Object.hpp>

#include <opencv2/calib3d.hpp>
#include <opencv2/core.hpp>

fwServicesRegisterMacro(::videoCalibration::ICalibration, ::videoCalibration::SOpenCVIntrinsic, ::arData::Camera);

namespace videoCalibration
{

static const ::fwCom::Slots::SlotKeyType s_UPDATE_CHESSBOARD_SIZE_SLOT = "updateChessboardSize";

// ----------------------------------------------------------------------------

SOpenCVIntrinsic::SOpenCVIntrinsic() noexcept :
    m_width(11),
    m_height(8),
    m_squareSize(20.0)
{
    newSlot(s_UPDATE_CHESSBOARD_SIZE_SLOT, &SOpenCVIntrinsic::updateChessboardSize, this);
}

// ----------------------------------------------------------------------------

SOpenCVIntrinsic::~SOpenCVIntrinsic() noexcept
{
}

//------------------------------------------------------------------------------

void SOpenCVIntrinsic::configuring()
{
    ::fwRuntime::ConfigurationElement::sptr cfgBoard = m_configuration->findConfigurationElement("board");
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
    ::arData::CalibrationInfo::csptr calInfo = this->getInput< ::arData::CalibrationInfo>("calibrationInfo");
    ::arData::Camera::sptr cam               = this->getInOut< ::arData::Camera >("camera");
    ::fwData::Vector::sptr poseCamera        = this->getInOut< ::fwData::Vector >("poseVector");

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
                points.push_back(::cv::Point3f(float(y*m_squareSize), float(x*m_squareSize), 0));
            }
        }

        std::vector<std::vector< ::cv::Point2f > > imagePoints;

        {
            ::fwData::mt::ObjectReadLock calInfoLock(calInfo);
            for(::fwData::PointList::sptr capture : calInfo->getPointListContainer())
            {
                std::vector< ::cv::Point2f > dst;

                for(::fwData::Point::csptr point : capture->getCRefPoints())
                {
                    SLM_ASSERT("point is null", point);
                    dst.push_back(::cv::Point2f(static_cast<float>(point->getCRefCoord()[0]),
                                                static_cast<float>(point->getCRefCoord()[1])));
                }
                imagePoints.push_back(dst);
                objectPoints.push_back(points);
            }
        }

        ::fwData::Image::sptr img = calInfo->getImageContainer().front();

        ::cv::Mat cameraMatrix;
        std::vector<float> distCoeffs;
        std::vector<cv::Mat> rvecs;
        std::vector<cv::Mat> tvecs;
        ::cv::Size2i imgsize(static_cast<int>(img->getSize()[0]), static_cast<int>(img->getSize()[1]));

        double err = ::cv::calibrateCamera(objectPoints, imagePoints, imgsize, cameraMatrix, distCoeffs, rvecs, tvecs);

        if(poseCamera)
        {
            poseCamera->getContainer().clear();

            for(size_t index = 0; index < rvecs.size(); ++index)
            {
                ::fwData::TransformationMatrix3D::sptr mat3D = ::fwData::TransformationMatrix3D::New();

                ::cv::Mat rmat;
                ::cv::Rodrigues(rvecs.at(index), rmat);

                ::cv::Mat tmat = tvecs.at(index);

                for(size_t i = 0; i < 3; ++i)
                {
                    for(size_t j = 0; j < 3; ++j)
                    {
                        mat3D->setCoefficient(i, j, rmat.at< double >(static_cast<int>(i),
                                                                      static_cast<int>(j)));
                    }
                    mat3D->setCoefficient(i, 3, tmat.at< double >(static_cast<int>(i)));
                }

                poseCamera->getContainer().push_back(mat3D);
                auto sig = poseCamera->signal< ::fwData::Vector::AddedObjectsSignalType >(
                    ::fwData::Vector::s_ADDED_OBJECTS_SIG);
            }
        }

        OSLM_DEBUG("Calibration error :" << err);

        ::fwData::mt::ObjectWriteLock camLock(cam);

        cam->setCx(cameraMatrix.at<double>(0, 2));
        cam->setCy(cameraMatrix.at<double>(1, 2));
        cam->setFx(cameraMatrix.at<double>(0, 0));
        cam->setFy(cameraMatrix.at<double>(1, 1));
        cam->setWidth(img->getSize()[0]);
        cam->setHeight(img->getSize()[1]);
        cam->setDistortionCoefficient(distCoeffs[0], distCoeffs[1], distCoeffs[2], distCoeffs[3], distCoeffs[4]);

        cam->setIsCalibrated(true);

        ::arData::Camera::IntrinsicCalibratedSignalType::sptr sig;
        sig = cam->signal< ::arData::Camera::IntrinsicCalibratedSignalType >(
            ::arData::Camera::s_INTRINSIC_CALIBRATED_SIG);

        sig->asyncEmit();
    }
}

//------------------------------------------------------------------------------

void SOpenCVIntrinsic::updateChessboardSize()
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
