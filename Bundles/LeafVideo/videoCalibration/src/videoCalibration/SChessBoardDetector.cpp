/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2014-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "videoCalibration/SChessBoardDetector.hpp"

#include <arData/CalibrationInfo.hpp>

#include <fwCom/Signal.hxx>
#include <fwCom/Slots.hxx>

#include <fwData/Array.hpp>
#include <fwData/Composite.hpp>

#include <fwDataTools/helper/Array.hpp>

#include <fwPreferences/helper.hpp>

#include <fwServices/IService.hpp>
#include <fwServices/macros.hpp>

#include <opencv2/calib3d.hpp>
#include <opencv2/imgproc.hpp>

namespace videoCalibration
{
fwServicesRegisterMacro(::fwServices::IController, ::videoCalibration::SChessBoardDetector, ::fwData::Composite);

const ::fwCom::Slots::SlotKeyType SChessBoardDetector::s_CHECK_POINTS_SLOT           = "checkPoints";
const ::fwCom::Slots::SlotKeyType SChessBoardDetector::s_DETECT_POINTS_SLOT          = "detectPoints";
const ::fwCom::Slots::SlotKeyType SChessBoardDetector::s_UPDATE_CHESSBOARD_SIZE_SLOT = "updateChessboardSize";

const ::fwCom::Signals::SignalKeyType SChessBoardDetector::s_CHESSBOARD_DETECTED_SIG     = "chessboardDetected";
const ::fwCom::Signals::SignalKeyType SChessBoardDetector::s_CHESSBOARD_NOT_DETECTED_SIG = "chessboardNotDetected";

const ::fwServices::IService::KeyType s_TIMELINE_INPUT    = "timeline";
const ::fwServices::IService::KeyType s_CALIBRATION_INOUT = "calInfo";

// ----------------------------------------------------------------------------

SChessBoardDetector::SChessBoardDetector() noexcept :
    m_width(11),
    m_height(8),
    m_isDetected(false),
    m_lastTimestamp(0)
{
    m_sigChessboardDetected    = newSignal< ChessboardDetectedSignalType >( s_CHESSBOARD_DETECTED_SIG );
    m_sigChessboardNotDetected = newSignal< ChessboardNotDetectedSignalType >( s_CHESSBOARD_NOT_DETECTED_SIG );

    newSlot( s_CHECK_POINTS_SLOT, &SChessBoardDetector::checkPoints, this );
    newSlot( s_DETECT_POINTS_SLOT, &SChessBoardDetector::detectPoints, this );
    newSlot( s_UPDATE_CHESSBOARD_SIZE_SLOT, &SChessBoardDetector::updateChessboardSize, this );
}

// ----------------------------------------------------------------------------

SChessBoardDetector::~SChessBoardDetector() noexcept
{
}

// ----------------------------------------------------------------------------

void SChessBoardDetector::configuring()
{
    SLM_ASSERT("You must have the same number of 'timeline' keys and 'calInfo' keys",
               this->getKeyGroupSize(s_TIMELINE_INPUT) == this->getKeyGroupSize(s_CALIBRATION_INOUT));

    ::fwRuntime::ConfigurationElement::sptr cfgBoard = m_configuration->findConfigurationElement("board");
    SLM_ASSERT("Tag 'board' not found.", cfgBoard);

    SLM_ASSERT("Attribute 'width' is missing.", cfgBoard->hasAttribute("width"));
    m_widthKey = cfgBoard->getAttributeValue("width");
    SLM_ASSERT("Attribute 'width' is empty", !m_widthKey.empty());

    SLM_ASSERT("Attribute 'height' is missing.", cfgBoard->hasAttribute("height"));
    m_heightKey = cfgBoard->getAttributeValue("height");
    SLM_ASSERT("Attribute 'height' is empty", !m_heightKey.empty());
}

// ----------------------------------------------------------------------------

void SChessBoardDetector::starting()
{
    m_pointsLists.resize( this->getKeyGroupSize(s_TIMELINE_INPUT) );
    this->updateChessboardSize();
}

// ----------------------------------------------------------------------------

void SChessBoardDetector::updating()
{
}

// ----------------------------------------------------------------------------

void SChessBoardDetector::stopping()
{
}

// ----------------------------------------------------------------------------

void SChessBoardDetector::checkPoints( ::fwCore::HiResClock::HiResClockType timestamp)
{
    if (timestamp > m_lastTimestamp)
    {
        ::fwCore::HiResClock::HiResClockType lastTimestamp;
        lastTimestamp = std::numeric_limits< ::fwCore::HiResClock::HiResClockType >::max();

        size_t numTimeline = this->getKeyGroupSize(s_TIMELINE_INPUT);
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
            ::fwData::PointList::sptr chessBoardPoints = this->detectChessboard(tl, lastTimestamp, m_width, m_height);

            if(!chessBoardPoints)
            {
                m_isDetected = false;
                break;

            }
            m_pointsLists[i] = chessBoardPoints;
        }

        if(m_isDetected)
        {
            m_sigChessboardDetected->asyncEmit();
        }
        else
        {
            m_sigChessboardNotDetected->asyncEmit();
        }

        m_lastTimestamp = lastTimestamp;
    }
}

// ----------------------------------------------------------------------------

void SChessBoardDetector::detectPoints()
{
    if(m_isDetected)
    {
        size_t numInfo = this->getKeyGroupSize(s_CALIBRATION_INOUT);
        for(size_t i = 0; i < numInfo; ++i)
        {
            auto calInfo = this->getInOut< ::arData::CalibrationInfo >(s_CALIBRATION_INOUT, i);
            auto frameTL = this->getInput< ::arData::FrameTL >(s_TIMELINE_INPUT, i);
            ::fwData::Image::sptr image = this->createImage( frameTL, m_lastTimestamp);

            calInfo->addRecord(image, m_pointsLists[i]);

            // Notify
            ::arData::CalibrationInfo::AddedRecordSignalType::sptr sig;
            sig = calInfo->signal< ::arData::CalibrationInfo::AddedRecordSignalType >
                      (::arData::CalibrationInfo::s_ADDED_RECORD_SIG);

            sig->asyncEmit();
        }
    }
}

// ----------------------------------------------------------------------------

void SChessBoardDetector::updateChessboardSize()
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
}

// ----------------------------------------------------------------------------

::fwData::Image::sptr SChessBoardDetector::createImage( ::arData::FrameTL::csptr tl,
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

SPTR(::fwData::PointList) SChessBoardDetector::detectChessboard(::arData::FrameTL::csptr tl,
                                                                ::fwCore::HiResClock::HiResClockType timestamp,
                                                                size_t xDim, size_t yDim)
{
    ::fwData::PointList::sptr pointlist;

    const CSPTR(::arData::FrameTL::BufferType) buffer = tl->getClosestBuffer(timestamp);

    if(buffer)
    {
        int height = static_cast<int>(tl->getHeight());
        int width  = static_cast<int>(tl->getWidth());

        std::uint8_t* frameBuff = const_cast< std::uint8_t*>( &buffer->getElement(0) );

        cv::Mat grayImg;
        if (tl->getNumberOfComponents() == 1)
        {
            grayImg = cv::Mat(height, width, CV_8UC1, frameBuff);
        }
        else
        {
            cv::Mat img(height, width, CV_8UC4, frameBuff);
            cv::cvtColor(img, grayImg, CV_RGBA2GRAY);
        }

        cv::Size boardSize(static_cast<int>(xDim) - 1, static_cast<int>(yDim) - 1);
        std::vector< cv::Point2f > corners;

        int flags = CV_CALIB_CB_ADAPTIVE_THRESH | CV_CALIB_CB_NORMALIZE_IMAGE | CV_CALIB_CB_FILTER_QUADS
                    | CV_CALIB_CB_FAST_CHECK;

        if (cv::findChessboardCorners(grayImg, boardSize, corners, flags))
        {
            cv::TermCriteria term(cv::TermCriteria::MAX_ITER + cv::TermCriteria::EPS, 30, 0.1);
            cv::cornerSubPix(grayImg, corners, cv::Size(5, 5), cv::Size(-1, -1), term);

            pointlist                                       = ::fwData::PointList::New();
            ::fwData::PointList::PointListContainer& points = pointlist->getRefPoints();
            points.reserve(corners.size());

            for(cv::Point2f& p : corners)
            {
                ::fwData::Point::sptr point = ::fwData::Point::New(p.x, p.y);
                points.push_back(point);
            }
        }
    }

    return pointlist;
}

// ----------------------------------------------------------------------------

} //namespace videoCalibration

