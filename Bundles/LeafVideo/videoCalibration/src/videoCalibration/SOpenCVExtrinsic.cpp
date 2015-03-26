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
#include <fwData/TransformationMatrix3D.hpp>

#include <arData/CameraSeries.hpp>
#include <arData/Camera.hpp>
#include <arData/CalibrationInfo.hpp>

#include "videoCalibration/SOpenCVExtrinsic.hpp"

fwServicesRegisterMacro(::videoCalibration::ICalibration, ::videoCalibration::SOpenCVExtrinsic, ::arData::CameraSeries);

namespace videoCalibration
{

// ----------------------------------------------------------------------------

SOpenCVExtrinsic::SOpenCVExtrinsic() throw () : m_width(0),
                                                m_height(0),
                                                m_camIndex(1)
{

}

// ----------------------------------------------------------------------------

SOpenCVExtrinsic::~SOpenCVExtrinsic() throw ()
{

}

//------------------------------------------------------------------------------

void SOpenCVExtrinsic::configuring() throw (fwTools::Failed)
{
    ::fwRuntime::ConfigurationElement::sptr cfg1 = m_configuration->findConfigurationElement("calibrationInfo1ID");
    SLM_ASSERT("Tag 'calibrationInfo1ID' not found.", cfg1);

    m_calibrationInfo1ID = cfg1->getValue();
    SLM_ASSERT("'calibrationInfo1ID' is empty.", !m_calibrationInfo1ID.empty());

    ::fwRuntime::ConfigurationElement::sptr cfg2 = m_configuration->findConfigurationElement("calibrationInfo2ID");
    SLM_ASSERT("Tag 'calibrationInfo2ID' not found.", cfg2);

    m_calibrationInfo2ID = cfg2->getValue();
    SLM_ASSERT("'calibrationInfo2ID' is empty.", !m_calibrationInfo2ID.empty());

    ::fwRuntime::ConfigurationElement::sptr cfgIdx = m_configuration->findConfigurationElement("camIndex");
    if(cfgIdx)
    {
        std::string idxStr = cfgIdx->getValue();
        SLM_ASSERT("'camIndex' is empty.", !idxStr.empty());
        m_camIndex = ::boost::lexical_cast<size_t>(idxStr);
    }

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

void SOpenCVExtrinsic::starting() throw (fwTools::Failed)
{
}

// ----------------------------------------------------------------------------

void SOpenCVExtrinsic::stopping() throw (fwTools::Failed)
{
}

//------------------------------------------------------------------------------

void SOpenCVExtrinsic::swapping() throw (fwTools::Failed)
{
    this->stopping();
    this->starting();
}

//--------------------------------------------------------------------- ---------

void SOpenCVExtrinsic::updating() throw (fwTools::Failed)
{
    ::arData::CameraSeries::sptr camSeries = this->getObject< ::arData::CameraSeries >();

    ::fwTools::Object::sptr obj1             = ::fwTools::fwID::getObject(m_calibrationInfo1ID);
    ::arData::CalibrationInfo::sptr calInfo1 = ::arData::CalibrationInfo::dynamicCast(obj1);
    SLM_ASSERT("Object with id '" + m_calibrationInfo1ID + "' is not an 'arData::CalibrationInfo'", calInfo1);

    ::fwTools::Object::sptr obj2             = ::fwTools::fwID::getObject(m_calibrationInfo2ID);
    ::arData::CalibrationInfo::sptr calInfo2 = ::arData::CalibrationInfo::dynamicCast(obj2);
    SLM_ASSERT("Object with id '" + m_calibrationInfo2ID + "' is not an 'arData::CalibrationInfo'", calInfo2);

    SLM_ASSERT("camera index must be > 0 and < camSeries->getNumberOfCameras()",
               m_camIndex > 0 && m_camIndex < camSeries->getNumberOfCameras());

    SLM_WARN_IF("Calibration info is empty.", calInfo1->getPointListContainer().empty());
    if(!calInfo1->getPointListContainer().empty())
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

        std::vector<std::vector< ::cv::Point2f > > imagePoints1;
        std::vector<std::vector< ::cv::Point2f > > imagePoints2;
        {
            ::fwData::mt::ObjectReadLock calInfo1Lock(calInfo1);
            ::fwData::mt::ObjectReadLock calInfo2Lock(calInfo2);

            ::arData::CalibrationInfo::PointListContainerType ptlists1 = calInfo1->getPointListContainer();
            ::arData::CalibrationInfo::PointListContainerType ptlists2 = calInfo2->getPointListContainer();

            SLM_ASSERT("The two calibrationInfo have not the same size", ptlists1.size() == ptlists2.size());

            ::arData::CalibrationInfo::PointListContainerType::iterator itr1   = ptlists1.begin();
            ::arData::CalibrationInfo::PointListContainerType::iterator itr2   = ptlists2.begin();
            ::arData::CalibrationInfo::PointListContainerType::iterator itrEnd = ptlists1.end();


            for(; itr1 != itrEnd; ++itr1, ++itr2)
            {
                ::fwData::PointList::sptr ptList1 = *itr1;
                ::fwData::PointList::sptr ptList2 = *itr2;
                std::vector< ::cv::Point2f > imgPoint1;
                std::vector< ::cv::Point2f > imgPoint2;

                for(fwData::Point::csptr point : ptList1->getCRefPoints())
                {
                    SLM_ASSERT("point is null", point);
                    imgPoint1.push_back(::cv::Point2f(
                                            static_cast<float>(point->getCRefCoord()[0]),
                                            static_cast<float>(point->getCRefCoord()[1])));
                }

                for(fwData::Point::csptr point : ptList2->getCRefPoints())
                {
                    SLM_ASSERT("point is null", point);
                    imgPoint2.push_back(::cv::Point2f(
                                            static_cast<float>(point->getCRefCoord()[0]),
                                            static_cast<float>(point->getCRefCoord()[1])));
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

        ::fwData::Image::sptr img = calInfo1->getImageContainer().front();
        ::cv::Size2i imgsize(static_cast<int>(img->getSize()[0]), static_cast<int>(img->getSize()[1]));
        {

            ::fwData::mt::ObjectReadLock camSeriesLock(camSeries);
            ::arData::Camera::sptr cam1 = camSeries->getCamera(0);
            ::arData::Camera::sptr cam2 = camSeries->getCamera(m_camIndex);

            ::fwData::mt::ObjectReadLock cam1Lock(cam1);
            ::fwData::mt::ObjectReadLock cam2Lock(cam2);

            cameraMatrix1.at<double>(0,0) = cam1->getFx();
            cameraMatrix1.at<double>(1,1) = cam1->getFy();
            cameraMatrix1.at<double>(0,2) = cam1->getCx();
            cameraMatrix1.at<double>(1,2) = cam1->getCy();

            cameraMatrix2.at<double>(0,0) = cam2->getFx();
            cameraMatrix2.at<double>(1,1) = cam2->getFy();
            cameraMatrix2.at<double>(0,2) = cam2->getCx();
            cameraMatrix2.at<double>(1,2) = cam2->getCy();
            for (int i = 0; i < 5; ++i)
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
                                           ::cv::TermCriteria(CV_TERMCRIT_ITER+CV_TERMCRIT_EPS, 100, 1e-5),
                                           CV_CALIB_FIX_INTRINSIC);

        OSLM_DEBUG("Calibration error :" << err);

        ::fwData::TransformationMatrix3D::sptr matrix = ::fwData::TransformationMatrix3D::New();
        for (int i = 0; i<3; ++i)
        {
            for (int j = 0; j<3; ++j)
            {
                matrix->setCoefficient(i,j, rotationMatrix.at<double>(i,j));
            }
        }
        matrix->setCoefficient(0,3, translationVector.at<double>(0,0));
        matrix->setCoefficient(1,3, translationVector.at<double>(1,0));
        matrix->setCoefficient(2,3, translationVector.at<double>(2,0));

        {
            ::fwData::mt::ObjectWriteLock camSeriesLock(camSeries);
            camSeries->setExtrinsicMatrix(m_camIndex, matrix);
        }

        ::arData::CameraSeries::ExtrinsicCalibratedSignalType::sptr sig;
        sig = camSeries->signal< ::arData::CameraSeries::ExtrinsicCalibratedSignalType > (
            ::arData::CameraSeries::s_EXTRINSIC_CALIBRATED_SIG);

        sig->asyncEmit();
    }
}

//------------------------------------------------------------------------------

} // namespace videoCalibration
