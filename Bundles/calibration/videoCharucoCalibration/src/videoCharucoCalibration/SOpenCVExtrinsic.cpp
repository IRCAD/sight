/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2018.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "videoCharucoCalibration/SOpenCVExtrinsic.hpp"

#include <arData/CalibrationInfo.hpp>
#include <arData/Camera.hpp>
#include <arData/CameraSeries.hpp>

#include <fwCom/Slot.hxx>
#include <fwCom/Slots.hxx>

#include <fwData/mt/ObjectReadLock.hpp>
#include <fwData/mt/ObjectWriteLock.hpp>
#include <fwData/PointList.hpp>
#include <fwData/TransformationMatrix3D.hpp>

#include <fwPreferences/helper.hpp>

#include <fwRuntime/ConfigurationElement.hpp>

#include <fwServices/IService.hpp>
#include <fwServices/macros.hpp>

#include <fwTools/fwID.hpp>
#include <fwTools/Object.hpp>

#include <opencv2/aruco.hpp>
#include <opencv2/aruco/charuco.hpp>
#include <opencv2/calib3d.hpp>
#include <opencv2/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/opencv.hpp>

#include <fstream>
#include <iostream>

fwServicesRegisterMacro(::arServices::ICalibration, ::videoCharucoCalibration::SOpenCVExtrinsic,
                        ::arData::CameraSeries);

namespace videoCharucoCalibration
{

static const ::fwCom::Slots::SlotKeyType s_UPDATE_CHARUCOBOARD_SIZE_SLOT = "updateCharucoBoardSize";

// ----------------------------------------------------------------------------

SOpenCVExtrinsic::SOpenCVExtrinsic() noexcept :
    m_width(11),
    m_height(8),
    m_squareSize(20.0),
    m_camIndex(1)
{
    newSlot(s_UPDATE_CHARUCOBOARD_SIZE_SLOT, &SOpenCVExtrinsic::updateCharucoBoardSize, this);
}

// ----------------------------------------------------------------------------

SOpenCVExtrinsic::~SOpenCVExtrinsic() noexcept
{
}

//------------------------------------------------------------------------------

void SOpenCVExtrinsic::configuring()
{
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
    m_widthKey = cfgBoard->getAttributeValue("width");
    SLM_ASSERT("Attribute 'width' is empty", !m_widthKey.empty());

    SLM_ASSERT("Attribute 'height' is missing.", cfgBoard->hasAttribute("height"));
    m_heightKey = cfgBoard->getAttributeValue("height");
    SLM_ASSERT("Attribute 'height' is empty", !m_heightKey.empty());

    SLM_ASSERT("Attribute 'squareSize' is missing.", cfgBoard->hasAttribute("squareSize"));
    m_squareSizeKey = cfgBoard->getAttributeValue("squareSize");
    SLM_ASSERT("Attribute 'squareSize' is empty", !m_squareSizeKey.empty());

    SLM_ASSERT("Attribute 'markerSize' is missing.", cfgBoard->hasAttribute("markerSize"));
    m_markerSizeKey = cfgBoard->getAttributeValue("markerSize");
    SLM_ASSERT("Attribute 'markerSize' is empty", !m_markerSizeKey.empty());
}

// ----------------------------------------------------------------------------

void SOpenCVExtrinsic::starting()
{
    this->updateCharucoBoardSize();
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
    ::arData::CameraSeries::sptr camSeries = this->getInOut< ::arData::CameraSeries >("cameraSeries");

    SLM_ASSERT("camera index must be > 0 and < camSeries->getNumberOfCameras()",
               m_camIndex > 0 && m_camIndex < camSeries->getNumberOfCameras());

    ::arData::CalibrationInfo::csptr calInfo1 = this->getInput< ::arData::CalibrationInfo>("calibrationInfo1");
    ::arData::CalibrationInfo::csptr calInfo2 = this->getInput< ::arData::CalibrationInfo>("calibrationInfo2");

    SLM_ASSERT("Object with 'calibrationInfo1' is not found", calInfo1);
    SLM_ASSERT("Object with 'calibrationInfo2' is not found", calInfo2);

    SLM_WARN_IF("Calibration info is empty.", calInfo1->getPointListContainer().empty());
    if(!(calInfo1->getPointListContainer().empty()))
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

        std::vector<std::vector< ::cv::Point2f > > imagePoints1;
        std::vector<std::vector< ::cv::Point2f > > imagePoints2;
        std::vector<std::vector<int> > ids1;
        std::vector<std::vector<int> > ids2;
        {
            ::fwData::mt::ObjectReadLock calInfo1Lock(calInfo1);
            ::fwData::mt::ObjectReadLock calInfo2Lock(calInfo2);

            ::arData::CalibrationInfo::PointListContainerType ptlists1 = calInfo1->getPointListContainer();
            ::arData::CalibrationInfo::PointListContainerType ptlists2 = calInfo2->getPointListContainer();

            SLM_ASSERT("The two calibrationInfo have not the same size", ptlists1.size() == ptlists2.size());

            ::arData::CalibrationInfo::PointListContainerType::iterator itr1    = ptlists1.begin();
            ::arData::CalibrationInfo::PointListContainerType::iterator itr2    = ptlists2.begin();
            ::arData::CalibrationInfo::PointListContainerType::iterator itr1End = ptlists1.end();

            for(; itr1 != itr1End; ++itr1, ++itr2)
            {

                ::fwData::PointList::sptr ptList1 = *itr1;
                ::fwData::PointList::sptr ptList2 = *itr2;
                std::vector< ::cv::Point2f > imgPoint1;
                std::vector< ::cv::Point2f > imgPoint2;
                std::vector<int> tempIds1;
                std::vector<int> tempIds2;

                for(fwData::Point::csptr point : ptList1->getPoints())
                {
                    SLM_ASSERT("point is null", point);
                    imgPoint1.push_back(::cv::Point2f(
                                            static_cast<float>(point->getCoord()[0]),
                                            static_cast<float>(point->getCoord()[1])));
                    tempIds1.push_back(static_cast<int>(point->getCoord()[2]));
                }
                imagePoints1.push_back(imgPoint1);
                ids1.push_back(tempIds1);

                for(fwData::Point::csptr point : ptList2->getPoints())
                {
                    SLM_ASSERT("point is null", point);
                    imgPoint2.push_back(::cv::Point2f(
                                            static_cast<float>(point->getCoord()[0]),
                                            static_cast<float>(point->getCoord()[1])));
                    tempIds2.push_back(static_cast<int>(point->getCoord()[2]));
                }
                imagePoints2.push_back(imgPoint2);
                ids2.push_back(tempIds2);

                objectPoints.push_back(points);

            }
        }

        // Set the cameras
        ::cv::Mat cameraMatrix1 = ::cv::Mat::eye(3, 3, CV_64F);
        ::cv::Mat cameraMatrix2 = ::cv::Mat::eye(3, 3, CV_64F);

        std::vector<float> distortionCoefficients1(5);
        std::vector<float> distortionCoefficients2(5);
        ::cv::Mat rotationMatrix    = cv::Mat::zeros(3, 3, CV_64F);
        ::cv::Mat rotationVector    = ::cv::Mat::zeros(3, 1, CV_64F);
        ::cv::Mat translationVector = ::cv::Mat::zeros(3, 1, CV_64F);
        ::cv::Mat essentialMatrix   = cv::Mat::zeros(3, 3, CV_64F);
        ::cv::Mat fundamentalMatrix = cv::Mat::zeros(3, 3, CV_64F);

        ::fwData::Image::sptr img = calInfo1->getImageContainer().front();
        ::cv::Size2i imgsize(static_cast<int>(img->getSize()[0]), static_cast<int>(img->getSize()[1]));
        {

            ::fwData::mt::ObjectReadLock camSeriesLock(camSeries);
            ::arData::Camera::sptr cam1 = camSeries->getCamera(0);
            ::arData::Camera::sptr cam2 = camSeries->getCamera(m_camIndex);

            ::fwData::mt::ObjectReadLock cam1Lock(cam1);
            ::fwData::mt::ObjectReadLock cam2Lock(cam2);

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

        ::cv::Size boardSize(m_width, m_height);
        ::cv::Ptr< ::cv::aruco::Dictionary > dictionary =
            ::cv::aruco::generateCustomDictionary((int) (boardSize.height*boardSize.width)/2.0, 6,
                                                  ::cv::aruco::Dictionary::get(
                                                      ::cv::aruco::DICT_6X6_1000));

        ::cv::Ptr< ::cv::aruco::CharucoBoard > board = ::cv::aruco::CharucoBoard::create(boardSize.width,
                                                                                         boardSize.height, m_squareSize,
                                                                                         m_markerSize, dictionary);

        ::cv::Mat allBoardCoord = ::cv::Mat::ones(3, (boardSize.width-1)*(boardSize.height-1), CV_64F);
        std::vector<int> allIds;

        //We create a list of the charuco board's points coordinates
        for(size_t i = 0; i < (boardSize.width-1)*(boardSize.height-1); i++)
        {
            allBoardCoord.at<double>(0, i) = (i%(boardSize.width-1)+1)*m_squareSize;
            allBoardCoord.at<double>(1, i) = ((int) (i/(boardSize.width-1))+1)*m_squareSize;
            allIds.push_back(i);
        }

        std::vector<std::vector< ::cv::Point2f > > allPoints1;
        std::vector<std::vector< ::cv::Point2f > > allPoints2;
        for(size_t i = 0; i < imagePoints1.size(); i++)
        {
            std::vector< ::cv::Point2f > tempBoardCoords1;
            std::vector< ::cv::Point2f > boardCoords1;
            std::vector< ::cv::Point2f > imagePointsUndistored1;

            //Create the list of points present in the image with theire corresponding coordinates in the board
            for(size_t j = 0; j < ids1[i].size(); j++)
            {
                ::cv::Point2f temp((ids1[i][j]%(boardSize.width-1)+1)*m_squareSize,
                                   ((int) (ids1[i][j]/(boardSize.width-1))+1)*m_squareSize);
                boardCoords1.push_back(temp);
            }

            //Undistort the image points
            ::cv::undistortPoints(imagePoints1[i], imagePointsUndistored1, cameraMatrix1, distortionCoefficients1);

            //Find the corresponding homography between the board and the image plan
            ::cv::Mat H1             = ::cv::findHomography(boardCoords1, imagePoints1[i]);
            ::cv::Mat allBoardCoord1 = H1*allBoardCoord;

            //Homogenize the new coordinates
            for(size_t j = 0; j < (boardSize.width-1)*(boardSize.height-1); j++)
            {
                ::cv::Point2f temp(allBoardCoord1.at<double>(0, j)/allBoardCoord1.at<double>(2, j),
                                   allBoardCoord1.at<double>(1, j)/allBoardCoord1.at<double>(2, j));
                tempBoardCoords1.push_back(temp);

            }
            allPoints1.push_back(tempBoardCoords1);

            //We do the same with the image from the second camera
            std::vector< ::cv::Point2f > tempBoardCoords2;
            std::vector< ::cv::Point2f > boardCoords2;
            std::vector< ::cv::Point2f > imagePointsUndistored2;
            for(size_t j = 0; j < ids2[i].size(); j++)
            {
                ::cv::Point2f temp((ids2[i][j]%(boardSize.width-1)+1)*m_squareSize,
                                   (int) ((ids2[i][j]/(boardSize.width-1))+1)*m_squareSize);
                boardCoords2.push_back(temp);
            }
            ::cv::Mat H2             = ::cv::findHomography(boardCoords2, imagePoints2[i]);
            ::cv::Mat allBoardCoord2 = H2*allBoardCoord;
            ::cv::undistortPoints(imagePoints2[i], imagePointsUndistored2, cameraMatrix2, distortionCoefficients2);
            for(size_t j = 0; j < (boardSize.width-1)*(boardSize.height-1); j++)
            {
                ::cv::Point2f temp(allBoardCoord2.at<double>(0, j)/allBoardCoord2.at<double>(2, j),
                                   allBoardCoord2.at<double>(1, j)/allBoardCoord2.at<double>(2, j));
                tempBoardCoords2.push_back(temp);
            }
            allPoints2.push_back(tempBoardCoords2);
        }

        ::cv::Mat identity = ::cv::Mat::eye(3, 3, CV_64F);
        ::cv::Mat nullVec;
        double err = ::cv::stereoCalibrate(objectPoints, allPoints1, allPoints2,
                                           identity, nullVec,
                                           identity, nullVec,
                                           imgsize, rotationMatrix, translationVector, essentialMatrix,
                                           fundamentalMatrix,
                                           CV_CALIB_FIX_INTRINSIC,
                                           ::cv::TermCriteria(::cv::TermCriteria::MAX_ITER + ::cv::TermCriteria::EPS,
                                                              100, 1e-5));

        ::fwData::TransformationMatrix3D::sptr matrix = ::fwData::TransformationMatrix3D::New();
        for (size_t i = 0; i < 3; ++i)
        {
            for (size_t j = 0; j < 3; ++j)
            {
                matrix->setCoefficient(i, j, rotationMatrix.at<double>(static_cast<int>(i), static_cast<int>(j)));
            }
        }
        matrix->setCoefficient(0, 3, translationVector.at<double>(0, 0));
        matrix->setCoefficient(1, 3, translationVector.at<double>(1, 0));
        matrix->setCoefficient(2, 3, translationVector.at<double>(2, 0));

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

void SOpenCVExtrinsic::updateCharucoBoardSize()
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
        m_squareSize = std::stod(squareSizeStr);
    }
    const std::string markerSizeStr = ::fwPreferences::getPreference(m_markerSizeKey);
    if(!markerSizeStr.empty())
    {
        m_markerSize = std::stod(markerSizeStr);
    }
}

//------------------------------------------------------------------------------

} // namespace videoCharucoCalibration
