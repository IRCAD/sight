/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2018.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "videoCharucoCalibration/SOpenCVExtrinsic.hpp"

#include <arData/CalibrationInfo.hpp>
#include <arData/Camera.hpp>
#include <arData/CameraSeries.hpp>

#include <calibration3d/helper.hpp>

#include <fwCom/Signal.hxx>
#include <fwCom/Slot.hxx>
#include <fwCom/Slots.hxx>

#include <fwData/mt/ObjectReadLock.hpp>
#include <fwData/mt/ObjectWriteLock.hpp>
#include <fwData/PointList.hpp>
#include <fwData/TransformationMatrix3D.hpp>

#include <fwGui/dialog/MessageDialog.hpp>

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

static const ::fwCom::Signals::SignalKeyType s_ERROR_COMPUTED_SIG = "errorComputed";

// ----------------------------------------------------------------------------

SOpenCVExtrinsic::SOpenCVExtrinsic() noexcept :
    m_width(11),
    m_height(8),
    m_squareSize(20.0),
    m_camIndex(1)
{
    newSignal<ErrorComputedSignalType>(s_ERROR_COMPUTED_SIG);
    newSlot(s_UPDATE_CHARUCOBOARD_SIZE_SLOT, &SOpenCVExtrinsic::updateCharucoBoardSize, this);
}

// ----------------------------------------------------------------------------

SOpenCVExtrinsic::~SOpenCVExtrinsic() noexcept
{
}

//------------------------------------------------------------------------------

void SOpenCVExtrinsic::configuring()
{
    const auto configTree = this->getConfigTree();
    m_camIndex = configTree.get<size_t>("camIndex", m_camIndex);

    const auto cfgBoard = configTree.get_child("board.<xmlattr>");

    m_widthKey      = cfgBoard.get<std::string>("width", "CHARUCO_WIDTH");
    m_heightKey     = cfgBoard.get<std::string>("height", "CHARUCO_HEIGHT");
    m_squareSizeKey = cfgBoard.get<std::string>("squareSize", "CHARUCO_SQUARE_SIZE");
    m_markerSizeKey = cfgBoard.get<std::string>("markerSize", "CHARUCO_MARKER_SIZE");

    this->updateCharucoBoardSize();
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
        for (size_t y = 0; y < m_height - 1; ++y)
        {
            for (size_t x = 0; x < m_width - 1; ++x)
            {
                points.push_back(::cv::Point3f(static_cast<float>(y)*m_squareSize, static_cast<float>(x)*m_squareSize,
                                               0));
            }
        }

        std::vector<std::vector< ::cv::Point2f > > imagePoints1;
        std::vector<std::vector< ::cv::Point2f > > imagePoints2;
        std::vector<std::vector<int> > ids1;
        std::vector<std::vector<int> > ids2;

        std::vector< size_t > degeneratedImagesCam1, degeneratedImagesCam2;

        {
            const ::fwData::mt::ObjectReadLock calInfo1Lock(calInfo1);
            const ::fwData::mt::ObjectReadLock calInfo2Lock(calInfo2);

            ::arData::CalibrationInfo::PointListContainerType ptlists1 = calInfo1->getPointListContainer();
            ::arData::CalibrationInfo::PointListContainerType ptlists2 = calInfo2->getPointListContainer();

            SLM_ASSERT("The two calibrationInfo have not the same size", ptlists1.size() == ptlists2.size());

            ::arData::CalibrationInfo::PointListContainerType::iterator itr1    = ptlists1.begin();
            ::arData::CalibrationInfo::PointListContainerType::iterator itr2    = ptlists2.begin();
            ::arData::CalibrationInfo::PointListContainerType::iterator itr1End = ptlists1.end();

            imagePoints1.reserve(ptlists1.size());
            ids1.reserve(ptlists1.size());
            imagePoints2.reserve(ptlists2.size());
            ids2.reserve(ptlists2.size());

            for(; itr1 != itr1End; ++itr1, ++itr2)
            {

                ::fwData::PointList::sptr ptList1 = *itr1;
                ::fwData::PointList::sptr ptList2 = *itr2;
                std::vector< ::cv::Point2f > imgPoint1;
                std::vector< ::cv::Point2f > imgPoint2;
                std::vector<int> tempIds1;
                std::vector<int> tempIds2;

                imgPoint1.reserve(ptList1->getPoints().size());
                tempIds1.reserve(imgPoint1.size());
                imgPoint2.reserve(ptList2->getPoints().size());
                tempIds2.reserve(imgPoint2.size());

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
        const ::cv::Mat rotationMatrix    = ::cv::Mat::zeros(3, 3, CV_64F);
        const ::cv::Mat translationVector = ::cv::Mat::zeros(3, 1, CV_64F);
        const ::cv::Mat essentialMatrix   = ::cv::Mat::zeros(3, 3, CV_64F);
        const ::cv::Mat fundamentalMatrix = ::cv::Mat::zeros(3, 3, CV_64F);

        ::fwData::Image::sptr img = calInfo1->getImageContainer().front();
        ::cv::Size2i imgsize(static_cast<int>(img->getSize()[0]), static_cast<int>(img->getSize()[1]));
        {

            const ::fwData::mt::ObjectReadLock camSeriesLock(camSeries);
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

        ::cv::Size boardSize(static_cast<int>(m_width), static_cast<int>(m_height));

        ::cv::Mat allBoardCoord = ::cv::Mat::ones(3, (boardSize.width-1)*(boardSize.height-1), CV_64F);
        std::vector<int> allIds;
        allIds.reserve(static_cast<size_t>((boardSize.width-1)*(boardSize.height-1)));

        //We create a list of the charuco board's points coordinates
        for(int i = 0; i < (boardSize.width-1)*(boardSize.height-1); i++)
        {
            allBoardCoord.at<double>(0, i) =
                static_cast< double > (static_cast< float >(i%(boardSize.width-1)+1) * m_squareSize );
            allBoardCoord.at<double>(1, i) =
                static_cast< double > (static_cast< float >(i/(boardSize.width-1)+1) * m_squareSize );
            allIds.push_back(i);
        }

        std::vector<std::vector< ::cv::Point2f > > allPoints1;
        std::vector<std::vector< ::cv::Point2f > > allPoints2;

        allPoints1.reserve(imagePoints1.size());
        allPoints2.reserve(imagePoints1.size());

        for(size_t i = 0; i < imagePoints1.size(); i++)
        {
            std::vector< ::cv::Point2f > tempBoardCoords1;
            std::vector< ::cv::Point2f > boardCoords1;
            std::vector< ::cv::Point2f > imagePointsUndistored1;

            boardCoords1.reserve(ids1[i].size());
            imagePointsUndistored1.reserve(ids1[i].size());

            //Create the list of points present in the image with theirs corresponding coordinates in the board
            for(size_t j = 0; j < ids1[i].size(); j++)
            {
                const float x = static_cast<float>(ids1[i][j]%(boardSize.width-1)+1) * m_squareSize;
                const float y = static_cast<float>((ids1[i][j]/(boardSize.width-1))+1 ) * m_squareSize;

                ::cv::Point2f temp(x, y);
                boardCoords1.push_back(temp);
            }

            //Undistort the image points
            ::cv::undistortPoints(imagePoints1[i], imagePointsUndistored1, cameraMatrix1, distortionCoefficients1);

            // verify if points are not a degenerated configuration
            if(this->checkDegeneratedConfiguration(imagePointsUndistored1, boardCoords1, boardSize))
            {
                degeneratedImagesCam1.push_back(i+1);
            }

            //We do the same with the image from the second camera
            std::vector< ::cv::Point2f > tempBoardCoords2;
            std::vector< ::cv::Point2f > boardCoords2;
            std::vector< ::cv::Point2f > imagePointsUndistored2;

            boardCoords2.reserve(ids2[i].size());
            imagePointsUndistored2.reserve(ids2[i].size());

            for(size_t j = 0; j < ids2[i].size(); j++)
            {
                ::cv::Point2f temp(static_cast<float>(ids2[i][j]%(boardSize.width-1)+1)*m_squareSize,
                                   static_cast<float>((ids2[i][j]/(boardSize.width-1))+1)*m_squareSize);
                boardCoords2.push_back(temp);
            }
            ::cv::undistortPoints(imagePoints2[i], imagePointsUndistored2, cameraMatrix2, distortionCoefficients2);

            // verify if points are not a degenerated configuration
            if(this->checkDegeneratedConfiguration(imagePointsUndistored2, boardCoords2, boardSize))
            {
                degeneratedImagesCam2.push_back(i+1);
            }

            //Find the corresponding homography between the board and the image plan
            ::cv::Mat H1             = ::cv::findHomography(boardCoords1, imagePointsUndistored1);
            ::cv::Mat allBoardCoord1 = H1*allBoardCoord;

            tempBoardCoords1.reserve(static_cast<size_t>((boardSize.width-1)*(boardSize.height-1)));

            //Homogenize the new coordinates
            for(int j = 0; j < (boardSize.width-1)*(boardSize.height-1); j++)
            {
                ::cv::Point2f temp(static_cast<float>(allBoardCoord1.at<double>(0, j)/allBoardCoord1.at<double>(2, j)),
                                   static_cast<float>(allBoardCoord1.at<double>(1, j)/allBoardCoord1.at<double>(2, j)));
                tempBoardCoords1.push_back(temp);

            }
            allPoints1.push_back(tempBoardCoords1);

            ::cv::Mat H2             = ::cv::findHomography(boardCoords2, imagePointsUndistored2);
            ::cv::Mat allBoardCoord2 = H2*allBoardCoord;

            tempBoardCoords2.reserve(static_cast<size_t>((boardSize.width-1)*(boardSize.height-1)));

            for(int j = 0; j < (boardSize.width-1)*(boardSize.height-1); j++)
            {
                ::cv::Point2f temp(static_cast<float>(allBoardCoord2.at<double>(0, j)/allBoardCoord2.at<double>(2, j)),
                                   static_cast<float>(allBoardCoord2.at<double>(1, j)/allBoardCoord2.at<double>(2, j)));
                tempBoardCoords2.push_back(temp);
            }
            allPoints2.push_back(tempBoardCoords2);
        }

        // check if we have some degenerated configuration
        // display the list of problematic images
        std::stringstream messageIm1, messageIm2;
        if(!degeneratedImagesCam1.empty())
        {
            messageIm1<<"please check image(s): " + std::to_string(degeneratedImagesCam1[0]);
            for(size_t i = 1; i < degeneratedImagesCam1.size(); ++i)
            {
                messageIm1 <<", "<< std::to_string(i);
            }
            messageIm1 << " of camera 1 ";
        }

        if(!degeneratedImagesCam2.empty())
        {
            messageIm1 << " & ";
            messageIm2<<"please check image(s): " + std::to_string(degeneratedImagesCam2[0]);
            for(size_t i = 1; i < degeneratedImagesCam2.size(); ++i)
            {
                messageIm2 << ", " << std::to_string(i);
            }
            messageIm2 << " of camera 2 ";
        }

        // if one of those stringstream are not empty we should display the popup and not perform calibration.
        if(!messageIm1.str().empty() || !messageIm2.str().empty())
        {
            ::fwGui::dialog::MessageDialog::sptr dialog = ::fwGui::dialog::MessageDialog::New();
            dialog->setTitle("Calibration Error");
            dialog->setIcon(::fwGui::dialog::IMessageDialog::Icons::WARNING);
            dialog->setMessage("Extrinsic Calibration cannot be performed due to degenerate configuration."
                               + messageIm1.str() + messageIm2.str());
            dialog->show();

            return;
        }

        // compute stereo calibration
        ::cv::Mat identity = ::cv::Mat::eye(3, 3, CV_64F);
        ::cv::Mat nullVec  = ::cv::Mat::zeros(1, 5, CV_32F);
        double err = ::cv::stereoCalibrate(objectPoints, allPoints1, allPoints2,
                                           identity, nullVec,
                                           identity, nullVec,
                                           imgsize, rotationMatrix, translationVector, essentialMatrix,
                                           fundamentalMatrix,
                                           CV_CALIB_FIX_INTRINSIC,
                                           ::cv::TermCriteria(::cv::TermCriteria::MAX_ITER + ::cv::TermCriteria::EPS,
                                                              100, 1e-5));

        this->signal<ErrorComputedSignalType>(s_ERROR_COMPUTED_SIG)->asyncEmit(err);

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
        m_width = std::stoul(widthStr);
    }
    const std::string heightStr = ::fwPreferences::getPreference(m_heightKey);
    if(!heightStr.empty())
    {
        m_height = std::stoul(heightStr);
    }
    const std::string squareSizeStr = ::fwPreferences::getPreference(m_squareSizeKey);
    if(!squareSizeStr.empty())
    {
        m_squareSize = std::stof(squareSizeStr);
    }
    const std::string markerSizeStr = ::fwPreferences::getPreference(m_markerSizeKey);
    if(!markerSizeStr.empty())
    {
        m_markerSize = std::stof(markerSizeStr);
    }

}

//------------------------------------------------------------------------------

bool SOpenCVExtrinsic::checkDegeneratedConfiguration(const std::vector<cv::Point2f>& _undistortedPoints,
                                                     const std::vector< ::cv::Point2f >& _boardCoords,
                                                     const cv::Size& _boardSize)
{
    ::cv::Mat M = ::cv::Mat::zeros(2* static_cast<int>(_undistortedPoints.size()), 9, CV_32F), u, w, vt;

    //Verify that this is not a degenerate configuration
    if(_undistortedPoints.size() <
       std::max(static_cast<size_t>(_boardSize.width), static_cast<size_t>(_boardSize.height))+2)
    {
        for(int i = 0; i < static_cast<int>(_undistortedPoints.size()); i++)
        {
            //avoid conversion warning between opencv (int) and std::vector (size_t)
            const size_t index = static_cast<size_t>(i);

            M.at<float>(i*2, 3) = -_boardCoords[index].x;
            M.at<float>(i*2, 4) = -_boardCoords[index].y;
            M.at<float>(i*2, 5) = -1;

            M.at<float>(i*2, 6) = _undistortedPoints[index].y*_boardCoords[index].x;
            M.at<float>(i*2, 7) = _undistortedPoints[index].y*_boardCoords[index].y;
            M.at<float>(i*2, 8) = _undistortedPoints[index].y;

            M.at<float>(i*2+1, 0) = _boardCoords[index].x;
            M.at<float>(i*2+1, 1) = _boardCoords[index].y;
            M.at<float>(i*2+1, 2) = 1;

            M.at<float>(i*2+1, 6) = -_undistortedPoints[index].x*_boardCoords[index].x;
            M.at<float>(i*2+1, 7) = -_undistortedPoints[index].x*_boardCoords[index].y;
            M.at<float>(i*2+1, 8) = -_undistortedPoints[index].x;
        }
        ::cv::SVDecomp(M, w, u, vt);
        if(w.at<float>(w.size().height-1) < 0.001f)
        {
            return true;
        }
    }

    return false;
}

//------------------------------------------------------------------------------

} // namespace videoCharucoCalibration
