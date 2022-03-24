/************************************************************************
 *
 * Copyright (C) 2018-2022 IRCAD France
 * Copyright (C) 2018-2021 IHU Strasbourg
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

#include "SCharucoBoardDetector.hpp"

#include <core/com/Signal.hxx>
#include <core/com/Slots.hxx>

#include <geometry/vision/helper.hpp>

#include <io/opencv/FrameTL.hpp>
#include <io/opencv/Image.hpp>

#include <ui/base/dialog/MessageDialog.hpp>
#include <ui/base/Preferences.hpp>

#include <opencv2/aruco.hpp>
#include <opencv2/aruco/charuco.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/opencv.hpp>

namespace sight::module::geometry::vision::charuco
{

const core::com::Slots::SlotKeyType SCharucoBoardDetector::s_CHECK_POINTS_SLOT              = "checkPoints";
const core::com::Slots::SlotKeyType SCharucoBoardDetector::s_DETECT_POINTS_SLOT             = "detectPoints";
const core::com::Slots::SlotKeyType SCharucoBoardDetector::s_UPDATE_CHARUCO_BOARD_SIZE_SLOT = "updateCharucoBoardSize";

const core::com::Signals::SignalKeyType SCharucoBoardDetector::s_CHARUCO_BOARD_DETECTED_SIG =
    "charucoBoardDetected";
const core::com::Signals::SignalKeyType SCharucoBoardDetector::s_CHARUCO_BOARD_NOT_DETECTED_SIG =
    "charucoBoardNotDetected";

// ----------------------------------------------------------------------------

SCharucoBoardDetector::SCharucoBoardDetector() noexcept :
    m_width(11),
    m_height(8),
    m_isDetected(false),
    m_lastTimestamp(0),
    m_markerSizeInBits(6)
{
    m_sigCharucoBoardDetected    = newSignal<CharucoBoardDetectedSignalType>(s_CHARUCO_BOARD_DETECTED_SIG);
    m_sigCharucoBoardNotDetected = newSignal<CharucoBoardNotDetectedSignalType>(s_CHARUCO_BOARD_NOT_DETECTED_SIG);

    newSlot(s_CHECK_POINTS_SLOT, &SCharucoBoardDetector::checkPoints, this);
    newSlot(s_DETECT_POINTS_SLOT, &SCharucoBoardDetector::detectPoints, this);
    newSlot(s_UPDATE_CHARUCO_BOARD_SIZE_SLOT, &SCharucoBoardDetector::updateCharucoBoardSize, this);
}

// ----------------------------------------------------------------------------

SCharucoBoardDetector::~SCharucoBoardDetector() noexcept
{
}

// ----------------------------------------------------------------------------

void SCharucoBoardDetector::configuring()
{
    SIGHT_ASSERT(
        "You must have the same number of 'timeline' keys and 'calInfo' keys",
        m_timeline.size() == m_calInfo.size()
    );

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
    m_cornerAndIdLists.resize(m_timeline.size());
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

void SCharucoBoardDetector::checkPoints(core::HiResClock::HiResClockType timestamp)
{
    if(timestamp > m_lastTimestamp)
    {
        core::HiResClock::HiResClockType lastTimestamp;
        lastTimestamp = std::numeric_limits<core::HiResClock::HiResClockType>::max();

        const std::size_t numTimeline  = m_timeline.size();
        const std::size_t numDetection = m_detection.size();

        SIGHT_ERROR_IF(
            "Different number of input timelines and detected point lists.",
            numDetection > 0 && numTimeline != numDetection
        );

        const bool detection = (numDetection > 0) && (numTimeline == numDetection);

        // Grab timeline objects
        for(std::size_t i = 0 ; i < numTimeline ; ++i)
        {
            auto frameTL = m_timeline[i].lock();
            lastTimestamp = std::min(lastTimestamp, frameTL->getNewerTimestamp());
        }

        m_isDetected = true;

        for(std::size_t i = 0 ; i < numTimeline ; ++i)
        {
            auto tl = m_timeline[i].lock();

            data::PointList::sptr charucoBoardPoints;
            if(detection)
            {
                auto tlDetection = m_detection[i].lock();
                if(!tlDetection->isAllocated())
                {
                    tlDetection->initPoolSize(
                        tl->getWidth(),
                        tl->getHeight(),
                        core::Type::UINT8,
                        data::FrameTL::PixelFormat::RGBA
                    );
                }

                charucoBoardPoints = this->detectCharucoBoard(tl.get_shared(), lastTimestamp, tlDetection.get_shared());
            }
            else
            {
                charucoBoardPoints = this->detectCharucoBoard(tl.get_shared(), lastTimestamp);
            }

            if((!charucoBoardPoints) || ((charucoBoardPoints->getPoints().size()) < 4))
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
        const std::size_t numInfo = m_calInfo.size();
        for(std::size_t i = 0 ; i < numInfo ; ++i)
        {
            auto calInfo                  = m_calInfo[i].lock();
            const auto frameTL            = m_timeline[i].lock();
            const data::Image::sptr image = this->createImage(*frameTL, m_lastTimestamp);

            calInfo->addRecord(image, m_cornerAndIdLists[i]);

            // Notify
            const data::CalibrationInfo::AddedRecordSignalType::sptr sig =
                calInfo->signal<data::CalibrationInfo::AddedRecordSignalType>
                    (data::CalibrationInfo::s_ADDED_RECORD_SIG);

            sig->asyncEmit();
        }
    }
}

// ----------------------------------------------------------------------------

void SCharucoBoardDetector::updateCharucoBoardSize()
{
    try
    {
        ui::base::Preferences preferences;
        m_width            = preferences.get(m_widthKey, m_width);
        m_height           = preferences.get(m_heightKey, m_height);
        m_squareSize       = preferences.get(m_squareSizeKey, m_squareSize);
        m_markerSize       = preferences.get(m_markerSizeKey, m_markerSize);
        m_markerSizeInBits = preferences.get(m_markerSizeInBitsKey, m_markerSizeInBits);
    }
    catch(const ui::base::PreferencesDisabled&)
    {
        // Nothing to do..
    }

    try
    {
        m_dictionary = sight::geometry::vision::helper::generateArucoDictionary(m_width, m_height, m_markerSizeInBits);
    }
    catch(const std::exception& e)
    {
        // Warn user that something went wrong with dictionary generation.
        sight::ui::base::dialog::MessageDialog::sptr errorDialog = sight::ui::base::dialog::MessageDialog::New();
        errorDialog->setTitle("Error in dictionary generation");
        errorDialog->setIcon(ui::base::dialog::IMessageDialog::Icons::CRITICAL);
        errorDialog->setMessage("Error when generating dictionary: " + std::string(e.what()));
        errorDialog->show();

        // Exit the function.
        return;
    }

    m_board = cv::aruco::CharucoBoard::create(
        static_cast<int>(m_width),
        static_cast<int>(m_height),
        m_squareSize,
        m_markerSize,
        m_dictionary
    );
}

// ----------------------------------------------------------------------------

data::Image::sptr SCharucoBoardDetector::createImage(
    const data::FrameTL& tl,
    core::HiResClock::HiResClockType timestamp
)
{
    data::Image::sptr image;

    const CSPTR(data::FrameTL::BufferType) buffer = tl.getClosestBuffer(timestamp);
    if(buffer)
    {
        image = data::Image::New();

        data::Image::PixelFormat format = data::Image::PixelFormat::UNDEFINED;

        // FIXME: Currently, FrameTL does not contain Pixel format, so we assume that format is GrayScale, RGB or RGBA.
        switch(tl.numComponents())
        {
            case 1:
                format = data::Image::PixelFormat::GRAY_SCALE;
                break;

            case 3:
                format = data::Image::PixelFormat::RGB;
                break;

            case 4:
                format = data::Image::PixelFormat::RGBA;
                break;

            default:
                format = data::Image::PixelFormat::UNDEFINED;
        }

        const data::Image::Size size = {tl.getWidth(), tl.getHeight(), 1};
        image->resize(size, tl.getType(), format);
        const data::Image::Origin origin = {0., 0., 0.};
        image->setOrigin(origin);
        const data::Image::Spacing spacing = {1., 1., 1.};
        image->setSpacing(spacing);
        image->setWindowWidth(1);
        image->setWindowCenter(0);

        const auto dumpLock = image->dump_lock();

        const std::uint8_t* frameBuff = &buffer->getElement(0);
        auto itr                      = image->begin<std::uint8_t>();

        std::copy(frameBuff, frameBuff + buffer->getSize(), itr);
    }

    return image;
}

// ----------------------------------------------------------------------------

data::PointList::sptr SCharucoBoardDetector::detectCharucoBoard(
    const data::FrameTL::csptr tl,
    const core::HiResClock::HiResClockType timestamp,
    data::FrameTL::sptr tlDetection
)
{
    data::PointList::sptr pointlist;

    const CSPTR(data::FrameTL::BufferType) buffer = tl->getClosestBuffer(timestamp);

    if(buffer)
    {
        const auto pixType = tl->getType();
        SIGHT_ASSERT(
            "Expected 8bit pixel components, have " << 8 * pixType.size(),
            pixType.size() == 1
        );

        std::uint8_t* frameBuff = const_cast<std::uint8_t*>(&buffer->getElement(0));

        cv::Mat grayImg;
        const cv::Mat img = io::opencv::FrameTL::moveToCv(tl, frameBuff);
        if(tl->numComponents() == 1)
        {
            grayImg = img;
        }
        else if(tl->numComponents() == 3)
        {
            cv::cvtColor(img, grayImg, cv::COLOR_RGB2GRAY);
        }
        else if(tl->numComponents() == 4)
        {
            cv::cvtColor(img, grayImg, cv::COLOR_RGBA2GRAY);
        }
        else
        {
            SIGHT_FATAL("Wrong type of image (nb of components = " << tl->numComponents() << ").");
        }

        std::vector<std::vector<cv::Point2f> > arucoCorners;
        std::vector<int> arucoIds;

        cv::aruco::detectMarkers(grayImg, m_dictionary, arucoCorners, arucoIds);

        if(arucoIds.size())
        {
            cv::Mat chessBoardCorners, chessBoardIds;
            cv::aruco::interpolateCornersCharuco(
                arucoCorners,
                arucoIds,
                grayImg,
                m_board,
                chessBoardCorners,
                chessBoardIds
            );

            pointlist = data::PointList::New();
            data::PointList::PointListContainer& points = pointlist->getPoints();
            points.reserve(static_cast<std::size_t>(chessBoardCorners.size[0]));

            for(int i = 0 ; i < chessBoardCorners.size[0] ; ++i)
            {
                data::Point::sptr point =
                    data::Point::New(
                        (chessBoardCorners.at<cv::Point2f>(cv::Point(0, i))).x,
                        (chessBoardCorners.at<cv::Point2f>(cv::Point(0, i))).y,
                        (static_cast<float>(chessBoardIds.at<int>(cv::Point(0, i))))
                    );
                points.push_back(point);
            }

            if(tlDetection)
            {
                SPTR(data::FrameTL::BufferType) detectionBuffer = tlDetection->createBuffer(timestamp);
                std::uint8_t* frameDetectionBuffer = detectionBuffer->addElement(0);
                cv::Mat frameDetectionCV           = io::opencv::FrameTL::moveToCv(tlDetection, frameDetectionBuffer);

                cv::Mat imgCpy;
                img.copyTo(imgCpy);

                cv::aruco::drawDetectedCornersCharuco(imgCpy, chessBoardCorners, chessBoardIds);

                imgCpy.copyTo(frameDetectionCV);

                tlDetection->pushObject(detectionBuffer);
                auto sig = tlDetection->signal<data::TimeLine::ObjectPushedSignalType>(
                    data::TimeLine::s_OBJECT_PUSHED_SIG
                );
                sig->asyncEmit(timestamp);
            }
        }
    }

    return pointlist;
}

// ----------------------------------------------------------------------------

} //namespace sight::module::geometry::vision::charuco
