/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2014-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <opencv2/core/core.hpp>
#include <opencv2/calib3d/calib3d.hpp>

#include <fwTools/fwID.hpp>
#include <fwTools/Object.hpp>

#include <fwRuntime/ConfigurationElement.hpp>

#include <fwServices/registry/ObjectService.hpp>
#include <fwServices/IService.hpp>
#include <fwServices/Base.hpp>

#include <fwData/mt/ObjectWriteLock.hpp>
#include <fwData/mt/ObjectReadLock.hpp>
#include <fwData/PointList.hpp>

#include <arData/Camera.hpp>
#include <arData/CalibrationInfo.hpp>

#include "videoCalibration/SOpenCVIntrinsic.hpp"

fwServicesRegisterMacro(::videoCalibration::ICalibration, ::videoCalibration::SOpenCVIntrinsic, ::arData::Camera);

namespace videoCalibration
{

// ----------------------------------------------------------------------------

SOpenCVIntrinsic::SOpenCVIntrinsic() throw () :
    m_width(0),
    m_height(0)
{

}

// ----------------------------------------------------------------------------

SOpenCVIntrinsic::~SOpenCVIntrinsic() throw ()
{

}

//------------------------------------------------------------------------------

void SOpenCVIntrinsic::configuring() throw (fwTools::Failed)
{
    ::fwRuntime::ConfigurationElement::sptr cfg = m_configuration->findConfigurationElement("calibrationInfoID");
    SLM_ASSERT("Tag 'calibrationInfoID' not found.", cfg);

    m_calibrationInfoID = cfg->getValue();
    SLM_ASSERT("'calibrationInfoID' is empty.", !m_calibrationInfoID.empty());

    ::fwRuntime::ConfigurationElement::sptr cfgBoard = m_configuration->findConfigurationElement("board");
    SLM_ASSERT("Tag 'board' not found.", cfgBoard);

    SLM_ASSERT("Attribute 'width' is missing.", cfgBoard->hasAttribute("width"));
    std::string width = cfgBoard->getAttributeValue("width");
    SLM_ASSERT("Attribute 'width' is empty", !width.empty());
    m_width = ::boost::lexical_cast< unsigned int> (width);

    SLM_ASSERT("Attribute 'height' is missing.", cfgBoard->hasAttribute("height"));
    std::string height = cfgBoard->getAttributeValue("height");
    SLM_ASSERT("Attribute 'height' is empty", !height.empty());
    m_height = ::boost::lexical_cast< unsigned int> (height);
}

// ----------------------------------------------------------------------------

void SOpenCVIntrinsic::starting() throw (fwTools::Failed)
{
}

// ----------------------------------------------------------------------------

void SOpenCVIntrinsic::stopping() throw (fwTools::Failed)
{
}

//------------------------------------------------------------------------------

void SOpenCVIntrinsic::swapping() throw (fwTools::Failed)
{
    this->stopping();
    this->starting();
}

//--------------------------------------------------------------------- ---------

void SOpenCVIntrinsic::updating() throw (fwTools::Failed)
{
    ::arData::Camera::sptr cam = this->getObject< ::arData::Camera >();

    ::fwTools::Object::sptr obj             = ::fwTools::fwID::getObject(m_calibrationInfoID);
    ::arData::CalibrationInfo::sptr calInfo = ::arData::CalibrationInfo::dynamicCast(obj);
    SLM_ASSERT("Object with id '" + m_calibrationInfoID + "' is not an 'arData::CalibrationInfo'", calInfo);

    SLM_WARN_IF("Calibration info is empty.", calInfo->getPointListContainer().empty());
    if(!calInfo->getPointListContainer().empty())
    {
        std::vector<std::vector< ::cv::Point3f > > objectPoints;

        std::vector< ::cv::Point3f > points;
        const float squareSize = 20;
        for (unsigned int y = 0; y < m_height - 1; ++y)
        {
            for (unsigned int x = 0; x < m_width - 1; ++x)
            {
                points.push_back(::cv::Point3f(float(y*squareSize), float(x*squareSize), 0));
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
        OSLM_DEBUG("Calibration error :" << err);

        ::fwData::mt::ObjectWriteLock camLock(cam);

        cam->setCx(cameraMatrix.at<double>(0,2));
        cam->setCy(cameraMatrix.at<double>(1,2));
        cam->setFx(cameraMatrix.at<double>(0,0));
        cam->setFy(cameraMatrix.at<double>(1,1));
        cam->setWidth(static_cast<double>(img->getSize()[0]));
        cam->setHeight(static_cast<double>(img->getSize()[1]));
        cam->setDistortionCoefficient(distCoeffs[0], distCoeffs[1], distCoeffs[2], distCoeffs[3], distCoeffs[4]);

        cam->setIsCalibrated(true);

        ::arData::Camera::IntrinsicCalibratedSignalType::sptr sig;
        sig = cam->signal< ::arData::Camera::IntrinsicCalibratedSignalType >(
            ::arData::Camera::s_INTRINSIC_CALIBRATED_SIG);

        sig->asyncEmit ();
    }
}

//------------------------------------------------------------------------------

} // namespace videoCalibration
