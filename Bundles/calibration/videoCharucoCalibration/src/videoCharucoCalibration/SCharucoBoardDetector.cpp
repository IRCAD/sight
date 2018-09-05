/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2018.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "videoCharucoCalibration/SCharucoBoardDetector.hpp"

#include <arData/CalibrationInfo.hpp>

#include <calibration3d/helper.hpp>

#include <cvIO/FrameTL.hpp>
#include <cvIO/Image.hpp>

#include <fwCom/Signal.hxx>
#include <fwCom/Slots.hxx>

#include <fwData/Array.hpp>
#include <fwData/Composite.hpp>
#include <fwData/mt/ObjectWriteLock.hpp>

#include <fwDataTools/helper/Array.hpp>

#include <fwGui/dialog/MessageDialog.hpp>

#include <fwPreferences/helper.hpp>

#include <fwServices/IService.hpp>
#include <fwServices/macros.hpp>

#include <opencv2/aruco.hpp>
#include <opencv2/aruco/charuco.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/opencv.hpp>

namespace videoCharucoCalibration
{

const ::fwCom::Slots::SlotKeyType SCharucoBoardDetector::s_CHECK_POINTS_SLOT             = "checkPoints";
const ::fwCom::Slots::SlotKeyType SCharucoBoardDetector::s_DETECT_POINTS_SLOT            = "detectPoints";
const ::fwCom::Slots::SlotKeyType SCharucoBoardDetector::s_UPDATE_CHARUCOBOARD_SIZE_SLOT = "updateCharucoBoardSize";

const ::fwCom::Signals::SignalKeyType SCharucoBoardDetector::s_CHARUCOBOARD_DETECTED_SIG = "charucoBoardDetected";
const ::fwCom::Signals::SignalKeyType SCharucoBoardDetector::s_CHARUCOBOARD_NOT_DETECTED_SIG
    = "charucoBoardNotDetected";

static const ::fwServices::IService::KeyType s_TIMELINE_INPUT    = "timeline";
static const ::fwServices::IService::KeyType s_CALIBRATION_INOUT = "calInfo";
static const ::fwServices::IService::KeyType s_DETECTION_INOUT   = "detection";

// ----------------------------------------------------------------------------

SCharucoBoardDetector::SCharucoBoardDetector() noexcept :
    m_width(11),
    m_height(8),
    m_isDetected(false),
    m_lastTimestamp(0),
    m_markerSizeInBits(6)
{
    m_sigCharucoBoardDetected    = newSignal< CharucoBoardDetectedSignalType >( s_CHARUCOBOARD_DETECTED_SIG );
    m_sigCharucoBoardNotDetected = newSignal< CharucoBoardNotDetectedSignalType >( s_CHARUCOBOARD_NOT_DETECTED_SIG );

    newSlot( s_CHECK_POINTS_SLOT, &SCharucoBoardDetector::checkPoints, this );
    newSlot( s_DETECT_POINTS_SLOT, &SCharucoBoardDetector::detectPoints, this );
    newSlot( s_UPDATE_CHARUCOBOARD_SIZE_SLOT, &SCharucoBoardDetector::updateCharucoBoardSize, this );
}

// ----------------------------------------------------------------------------

SCharucoBoardDetector::~SCharucoBoardDetector() noexcept
{
}

// ----------------------------------------------------------------------------

void SCharucoBoardDetector::configuring()
{
    SLM_ASSERT("You must have the same number of 'timeline' keys and 'calInfo' keys",
               this->getKeyGroupSize(s_TIMELINE_INPUT) == this->getKeyGroupSize(s_CALIBRATION_INOUT));

    const auto configTree = this->getConfigTree();
    const auto cfgBoard   = configTree.get_child("board.<xmlattr>");

    m_widthKey            = cfgBoard.get<std::string>("width", "CHARUCO_WIDTH");
    m_heightKey           = cfgBoard.get<std::string>("height", "CHARUCO_HEIGHT");
    m_squareSizeKey       = cfgBoard.get<std::string>("squareSize", "CHARUCO_SQUARE_SIZE");
    m_markerSizeKey       = cfgBoard.get<std::string>("markerSize", "CHARUCO_MARKER_SIZE");
    m_markerSizeInBitsKey = cfgBoard.get<std::string>("markerSizeInBits", "CHARUCO_MARKER_SIZE_IN_BITS");

    this->updateCharucoBoardSize();
}

// ----------------------------------------------------------------------------

void SCharucoBoardDetector::starting()
{
    m_cornerAndIdLists.resize( this->getKeyGroupSize(s_TIMELINE_INPUT) );
}

// ----------------------------------------------------------------------------

void SCharucoBoardDetector::updating()
{
}

// ----------------------------------------------------------------------------

void SCharucoBoardDetector::stopping()
{
}

// ----------------------------------------------------------------------------

void SCharucoBoardDetector::checkPoints( ::fwCore::HiResClock::HiResClockType timestamp)
{
    if (timestamp > m_lastTimestamp)
    {
        ::fwCore::HiResClock::HiResClockType lastTimestamp;
        lastTimestamp = std::numeric_limits< ::fwCore::HiResClock::HiResClockType >::max();

        const size_t numTimeline  = this->getKeyGroupSize(s_TIMELINE_INPUT);
        const size_t numDetection = this->getKeyGroupSize(s_DETECTION_INOUT);

        OSLM_ERROR_IF("Different number of input timelines and detected point lists.",
                      numDetection > 0 && numTimeline != numDetection);

        const bool detection = (numDetection > 0) && (numTimeline == numDetection);

        // Grab timeline objects
        for(size_t i = 0; i < numTimeline; ++i)
        {
            auto frameTL = this->getInput< ::arData::FrameTL >(s_TIMELINE_INPUT, i);
            lastTimestamp = std::min(lastTimestamp, frameTL->getNewerTimestamp());
        }

        m_isDetected = true;

        for(size_t i = 0; i < numTimeline; ++i)
        {
            auto tl = this->getInput< ::arData::FrameTL >(s_TIMELINE_INPUT, i);

            ::fwData::PointList::sptr charucoBoardPoints;
            if(detection)
            {
                auto tlDetection = this->getInOut< ::arData::FrameTL >(s_DETECTION_INOUT, i);
                if(!tlDetection->isAllocated())
                {
                    tlDetection->initPoolSize(tl->getWidth(), tl->getHeight(), ::fwTools::Type::s_UINT8, 4);
                }
                charucoBoardPoints = this->detectCharucoBoard(tl, lastTimestamp, tlDetection);
            }
            else
            {
                charucoBoardPoints = this->detectCharucoBoard(tl, lastTimestamp );

            }

            if((!charucoBoardPoints)||((charucoBoardPoints->getPoints().size()) < 4))
            {
                m_isDetected = false;
                break;

            }
            m_cornerAndIdLists[i] = charucoBoardPoints;
        }

        if(m_isDetected)
        {
            m_sigCharucoBoardDetected->asyncEmit();
        }
        else
        {
            m_sigCharucoBoardNotDetected->asyncEmit();
        }

        m_lastTimestamp = lastTimestamp;
    }
}

// ----------------------------------------------------------------------------

void SCharucoBoardDetector::detectPoints()
{
    if(m_isDetected)
    {
        const size_t numInfo = this->getKeyGroupSize(s_CALIBRATION_INOUT);
        for(size_t i = 0; i < numInfo; ++i)
        {
            auto calInfo                      = this->getInOut< ::arData::CalibrationInfo >(s_CALIBRATION_INOUT, i);
            const auto frameTL                = this->getInput< ::arData::FrameTL >(s_TIMELINE_INPUT, i);
            const ::fwData::Image::sptr image = this->createImage( frameTL, m_lastTimestamp);

            ::fwData::mt::ObjectWriteLock lock(calInfo);
            calInfo->addRecord(image, m_cornerAndIdLists[i]);

            // Notify
            const ::arData::CalibrationInfo::AddedRecordSignalType::sptr sig
                = calInfo->signal< ::arData::CalibrationInfo::AddedRecordSignalType >
                      (::arData::CalibrationInfo::s_ADDED_RECORD_SIG);

            sig->asyncEmit();
        }
    }
}

// ----------------------------------------------------------------------------

void SCharucoBoardDetector::updateCharucoBoardSize()
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
    const std::string markerSizeInBitsStr = ::fwPreferences::getPreference(m_markerSizeInBitsKey);
    if(!markerSizeInBitsStr.empty())
    {
        m_markerSizeInBits = std::stoi(markerSizeInBitsStr);
    }

    try
    {
        m_dictionary = ::calibration3d::helper::generateArucoDictionary(m_width, m_height, m_markerSizeInBits);
    }
    catch (const std::exception& e )
    {
        // Warn user that something went wrong with dictionary generation.
        ::fwGui::dialog::MessageDialog::sptr errorDialog = ::fwGui::dialog::MessageDialog::New();
        errorDialog->setTitle("Error in dictionary generation");
        errorDialog->setIcon(::fwGui::dialog::IMessageDialog::Icons::CRITICAL);
        errorDialog->setMessage("Error when generating dictionary: " + std::string(e.what()));
        errorDialog->show();

        // Exit the function.
        return;
    }

    m_board = ::cv::aruco::CharucoBoard::create(static_cast<int>(m_width), static_cast<int>(m_height),
                                                m_squareSize, m_markerSize, m_dictionary);

}

// ----------------------------------------------------------------------------

::fwData::Image::sptr SCharucoBoardDetector::createImage( ::arData::FrameTL::csptr tl,
                                                          ::fwCore::HiResClock::HiResClockType timestamp)
{
    ::fwData::Image::sptr image;

    const CSPTR(::arData::FrameTL::BufferType) buffer = tl->getClosestBuffer(timestamp);
    if (buffer)
    {
        image = ::fwData::Image::New();

        ::fwData::Image::SizeType size(3);
        size[0] = tl->getWidth();
        size[1] = tl->getHeight();
        size[2] = 1;
        const ::fwData::Image::SpacingType::value_type voxelSize = 1;
        image->allocate(size, tl->getType(), tl->getNumberOfComponents());
        ::fwData::Image::OriginType origin(3, 0);

        image->setOrigin(origin);
        ::fwData::Image::SpacingType spacing(3, voxelSize);
        image->setSpacing(spacing);
        image->setWindowWidth(1);
        image->setWindowCenter(0);

        ::fwData::Array::sptr array = image->getDataArray();

        ::fwDataTools::helper::Array arrayHelper(array);

        const std::uint8_t*  frameBuff = &buffer->getElement(0);
        std::uint8_t* index            = arrayHelper.begin< std::uint8_t >();

        std::copy( frameBuff, frameBuff+buffer->getSize(), index);
    }

    return image;
}

// ----------------------------------------------------------------------------

::fwData::PointList::sptr SCharucoBoardDetector::detectCharucoBoard(const ::arData::FrameTL::csptr tl,
                                                                    const ::fwCore::HiResClock::HiResClockType timestamp,
                                                                    ::arData::FrameTL::sptr tlDetection)
{

    ::fwData::PointList::sptr pointlist;

    const CSPTR(::arData::FrameTL::BufferType) buffer = tl->getClosestBuffer(timestamp);

    if(buffer)
    {
        const auto pixType = tl->getType();
        OSLM_ASSERT("Expected 8bit pixel components, have " << 8 * pixType.sizeOf(), pixType.sizeOf() == 1);

        std::uint8_t* frameBuff = const_cast< std::uint8_t*>( &buffer->getElement(0) );

        ::cv::Mat grayImg;
        const ::cv::Mat img = ::cvIO::FrameTL::moveToCv(tl, frameBuff);
        if (tl->getNumberOfComponents() == 1)
        {
            grayImg = img;
        }
        else if (tl->getNumberOfComponents() == 3)
        {
            ::cv::cvtColor(img, grayImg, CV_RGB2GRAY);
        }
        else if(tl->getNumberOfComponents() == 4)
        {
            ::cv::cvtColor(img, grayImg, CV_RGBA2GRAY);
        }
        else
        {
            OSLM_FATAL("Wrong type of image (nb of components = "<<tl->getNumberOfComponents()<<").");
        }

        std::vector<std::vector< ::cv::Point2f> > arucoCorners;
        std::vector<int> arucoIds;

        ::cv::aruco::detectMarkers( grayImg, m_dictionary, arucoCorners, arucoIds);

        if(arucoIds.size())
        {
            ::cv::Mat chessBoardCorners, chessBoardIds;
            ::cv::aruco::interpolateCornersCharuco(arucoCorners, arucoIds, grayImg, m_board, chessBoardCorners,
                                                   chessBoardIds);

            pointlist                                       = ::fwData::PointList::New();
            ::fwData::PointList::PointListContainer& points = pointlist->getPoints();
            points.reserve(static_cast< size_t >(chessBoardCorners.size[0]));

            for(int i = 0; i < chessBoardCorners.size[0]; ++i)
            {

                ::fwData::Point::sptr point =
                    ::fwData::Point::New((chessBoardCorners.at< ::cv::Point2f>( ::cv::Point(0, i) )).x,
                                         (chessBoardCorners.at< ::cv::Point2f>( ::cv::Point(0, i) )).y,
                                         (static_cast<float>(chessBoardIds.at<int>( ::cv::Point(0, i)))));
                points.push_back(point);
            }

            if(tlDetection)
            {
                SPTR(::arData::FrameTL::BufferType) detectionBuffer = tlDetection->createBuffer(timestamp);
                std::uint8_t* frameDetectionBuffer = detectionBuffer->addElement(0);
                ::cv::Mat frameDetectionCV = ::cvIO::FrameTL::moveToCv(tlDetection, frameDetectionBuffer);

                ::cv::Mat imgCpy;
                img.copyTo(imgCpy);

                ::cv::aruco::drawDetectedCornersCharuco(imgCpy, chessBoardCorners, chessBoardIds);

                imgCpy.copyTo(frameDetectionCV);

                tlDetection->pushObject(detectionBuffer);
                auto sig = tlDetection->signal< ::arData::TimeLine::ObjectPushedSignalType >(
                    ::arData::TimeLine::s_OBJECT_PUSHED_SIG);
                sig->asyncEmit(timestamp);
            }
        }
    }

    return pointlist;

}

// ----------------------------------------------------------------------------

} //namespace videoCharucoCalibration
