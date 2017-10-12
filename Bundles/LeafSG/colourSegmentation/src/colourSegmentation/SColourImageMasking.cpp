/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2016-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "colourSegmentation/SColourImageMasking.hpp"

#include <arData/FrameTL.hpp>

#include <fwData/mt/ObjectReadLock.hpp>

#include <fwDataTools/helper/ArrayGetter.hpp>

#include <fwServices/macros.hpp>

#include <boost/make_unique.hpp>

namespace colourSegmentation
{

fwServicesRegisterMacro( ::fwServices::IOperator, ::colourSegmentation::SColourImageMasking);

const ::fwCom::Slots::SlotKeyType SColourImageMasking::s_SET_BACKGROUND_SLOT = "setBackground";
const ::fwCom::Slots::SlotKeyType SColourImageMasking::s_SET_FOREGROUND_SLOT = "setForeground";

const ::fwServices::IService::KeyType s_MASK_KEY          = "mask";
const ::fwServices::IService::KeyType s_VIDEO_TL_KEY      = "videoTL";
const ::fwServices::IService::KeyType s_VIDEO_MASK_TL_KEY = "videoMaskTL";

// ------------------------------------------------------------------------------

SColourImageMasking::SColourImageMasking() noexcept
{
    newSlot( s_SET_BACKGROUND_SLOT, &SColourImageMasking::setBackground, this );
    newSlot( s_SET_FOREGROUND_SLOT, &SColourImageMasking::setForeground, this );
}

// ------------------------------------------------------------------------------

SColourImageMasking::~SColourImageMasking() noexcept
{
}

// ------------------------------------------------------------------------------

void SColourImageMasking::configuring()
{
    const ::fwServices::IService::ConfigType config = this->getConfigTree().get_child("config.<xmlattr>");

    m_scaleFactor = config.get<float>("scaleFactor", 1.0);
    m_noise       = config.get<double>("noise", 0.0);
}

// ------------------------------------------------------------------------------

void SColourImageMasking::starting()
{
    m_masker = ::boost::make_unique< ::colourImageMasking::Masker >(::colourImageMasking::HSv,
                                                                    ::colourImageMasking::LLRatio);
    m_masker->setThreshold(1.);

    m_lastVideoTimestamp = ::fwCore::HiResClock::getTimeInMilliSec();
}

// ------------------------------------------------------------------------------

void SColourImageMasking::stopping()
{
}

//-----------------------------------------------------------------------------

::fwServices::IService::KeyConnectionsMap SColourImageMasking::getAutoConnections() const
{
    KeyConnectionsMap connections;

    connections.push( s_VIDEO_TL_KEY, ::arData::FrameTL::s_OBJECT_PUSHED_SIG, s_UPDATE_SLOT );

    return connections;
}

// ------------------------------------------------------------------------------

void SColourImageMasking::updating()
{
    if(m_masker->isModelLearned())
    {
        auto mask        = this->getInput< ::fwData::Image >(s_MASK_KEY);
        auto videoTL     = this->getInput< ::arData::FrameTL >(s_VIDEO_TL_KEY);
        auto videoMaskTL = this->getInOut< ::arData::FrameTL >(s_VIDEO_MASK_TL_KEY);

        // Get current timestamp
        fwCore::HiResClock::HiResClockType currentTimestamp = ::fwCore::HiResClock::getTimeInMilliSec();

        // Get image from the video timeline
        CSPTR(::arData::FrameTL::BufferType) videoBuffer = videoTL->getClosestBuffer(currentTimestamp);

        if(!videoBuffer)
        {
            OSLM_INFO("Buffer not found with timestamp "<< timestamp);
            return;
        }

        const std::uint8_t* frameBuffOutVideo = &videoBuffer->getElement(0);

        fwCore::HiResClock::HiResClockType videoTimestamp = videoBuffer->getTimestamp();
        if(videoTimestamp <= m_lastVideoTimestamp)
        {
            return;
        }

        m_lastVideoTimestamp = videoTimestamp;

        // Get Mask image
        ::fwData::mt::ObjectReadLock imageLock(mask);
        const ::fwData::Array::sptr arrayMask = mask->getDataArray();
        const ::fwDataTools::helper::ArrayGetter arrayMaskHelper(arrayMask);
        SLM_ASSERT("Empty image buffer", arrayMaskHelper.getBuffer());

        // convert mask to an OpenCV image:
        ::cv::Mat maskCV;

        maskCV = ::cv::Mat(
            ::cv::Size(static_cast<int>(mask->getSize()[0]),
                       static_cast<int>(mask->getSize()[1])),
            CV_8UC4, (void*)arrayMaskHelper.getBuffer(), ::cv::Mat::AUTO_STEP);

        ::cv::cvtColor(maskCV, maskCV, cv::COLOR_BGR2GRAY);

        //convert mask to an OpenCV image:
        ::cv::Mat videoCV;
        videoCV = ::cv::Mat(
            ::cv::Size(static_cast<int>(videoTL->getWidth()),
                       static_cast<int>(videoTL->getHeight())),
            CV_8UC4, (void*)frameBuffOutVideo, ::cv::Mat::AUTO_STEP);

        // Create image mask to put inside the timeline
        SPTR(::arData::FrameTL::BufferType) maskBuffer = videoMaskTL->createBuffer(currentTimestamp);
        std::uint8_t* frameBuffOutMask = maskBuffer->addElement(0);

        // Get the foreground mask
        ::cv::Mat foregroundMask = m_masker->makeMask(videoCV, m_maskDownsize, maskCV);

        // Create an openCV mat that aliases the buffer created from the output timeline
        ::cv::Mat videoMaskCV(videoCV.size(), videoCV.type(), (void*)frameBuffOutMask, ::cv::Mat::AUTO_STEP);
        videoCV.copyTo(videoMaskCV, foregroundMask);

        // Push the mask object in the timeline
        videoMaskTL->pushObject(maskBuffer);

        auto sig = videoMaskTL->signal< ::arData::TimeLine::ObjectPushedSignalType >(
            ::arData::TimeLine::s_OBJECT_PUSHED_SIG);
        sig->asyncEmit(currentTimestamp);
    }
}

// ------------------------------------------------------------------------------

void SColourImageMasking::setBackground()
{
    auto mask    = this->getInput< ::fwData::Image >(s_MASK_KEY);
    auto videoTL = this->getInput< ::arData::FrameTL >(s_VIDEO_TL_KEY);

    ::fwCore::HiResClock::HiResClockType currentTimestamp = ::fwCore::HiResClock::getTimeInMilliSec();
    CSPTR(::arData::FrameTL::BufferType) videoBuffer      = videoTL->getClosestBuffer(currentTimestamp);
    const std::uint8_t* frameBuffOutVideo = &videoBuffer->getElement(0);

    if(!videoBuffer)
    {
        OSLM_INFO("Buffer not found with timestamp "<< timestamp);
        return;
    }

    //convert mask to an OpenCV image:
    ::cv::Mat videoCV;
    videoCV = ::cv::Mat(
        ::cv::Size(static_cast<int>(videoTL->getWidth()),
                   static_cast<int>(videoTL->getHeight())),
        CV_8UC4, (void*)frameBuffOutVideo, ::cv::Mat::AUTO_STEP);

    //convert mask to an OpenCV image:
    ::cv::Mat maskCV;

    ::fwData::mt::ObjectReadLock imageLock(mask);
    const ::fwData::Array::sptr arrayMask = mask->getDataArray();
    const ::fwDataTools::helper::ArrayGetter arrayMaskHelper(arrayMask);
    SLM_ASSERT("Empty image buffer", arrayMaskHelper.getBuffer());

    maskCV = ::cv::Mat(
        ::cv::Size(static_cast<int>(mask->getSize()[0]),
                   static_cast<int>(mask->getSize()[1])),
        CV_8UC4, (void*)arrayMaskHelper.getBuffer(), ::cv::Mat::AUTO_STEP);

    // Save size to downscale the image (speed up the process but decrease segmentation quality)
    m_maskDownsize = ::cv::Size(static_cast<int>(static_cast<float>(maskCV.size[0])*m_scaleFactor),
                                static_cast<int>(static_cast<float>(maskCV.size[1])*m_scaleFactor));

    // Flip mask image (Y axis) because of the offscreen render
    ::cv::flip(maskCV, maskCV, 0);

    // Convert color mask to grayscale value
    ::cv::cvtColor(maskCV, maskCV, cv::COLOR_RGB2BGR);
    ::cv::cvtColor(maskCV, maskCV, cv::COLOR_BGR2GRAY);

    // Erode a little bit the mask to avoid the borders
    // Construct element type
    int elementType = ::cv::MORPH_ELLIPSE;
    // Choose element size
    int elementErodeSize(1);
    // Construct erosion element
    ::cv::Mat elementErode = ::cv::getStructuringElement(elementType,
                                                         ::cv::Size(2*elementErodeSize+1, 2*elementErodeSize+1),
                                                         ::cv::Point(elementErodeSize, elementErodeSize));
    // Perform erosion
    ::cv::erode(maskCV, maskCV, elementErode);

    // Learn background color model
    m_masker->trainBackgroundModel(videoCV, maskCV, 5);

    // Initialize the mask timeline
    auto videoMaskTL = this->getInOut< ::arData::FrameTL >(s_VIDEO_MASK_TL_KEY);
    videoMaskTL->initPoolSize(videoTL->getWidth(), videoTL->getHeight(), ::fwTools::Type::s_UINT8, 4);
}

// ------------------------------------------------------------------------------

void SColourImageMasking::setForeground()
{
    auto videoTL = this->getInput< ::arData::FrameTL >(s_VIDEO_TL_KEY);

    fwCore::HiResClock::HiResClockType currentTimestamp = ::fwCore::HiResClock::getTimeInMilliSec();
    CSPTR(::arData::FrameTL::BufferType) videoBuffer = videoTL->getClosestBuffer(currentTimestamp);
    const std::uint8_t* frameBuffOutVideo = &videoBuffer->getElement(0);

    if(!videoBuffer)
    {
        OSLM_INFO("Buffer not found with timestamp "<< timestamp);
        return;
    }

    //convert mask to an OpenCV image:
    ::cv::Mat videoCV;
    videoCV = ::cv::Mat(
        ::cv::Size(static_cast<int>(videoTL->getWidth()),
                   static_cast<int>(videoTL->getHeight())),
        CV_8UC4, (void*)frameBuffOutVideo, ::cv::Mat::AUTO_STEP);

    // Convert RGB to HSV
    ::cv::Mat videoBGR, videoHSV;
    ::cv::cvtColor(videoCV, videoBGR, CV_RGBA2BGR);
    ::cv::cvtColor(videoBGR, videoHSV, CV_BGR2HSV);

    // Define a range of color in HSV
    ::cv::Scalar lowerColor(35, 0, 0);
    ::cv::Scalar upperColor(360, 255, 255);

    // Get the mask to learn the foreground model
    ::cv::Mat foregroundMask;
    ::cv::inRange(videoHSV, lowerColor, upperColor, foregroundMask);

    // Remove small objects by performing an opening
    ::cv::Mat openForegroundMask;
    // Construct element type
    int elementType = ::cv::MORPH_ELLIPSE;
    // Choose element size
    int elementErodeSize(2);
    // Construct erosion element
    ::cv::Mat elementErode = ::cv::getStructuringElement(elementType,
                                                         ::cv::Size(2*elementErodeSize+1, 2*elementErodeSize+1),
                                                         ::cv::Point(elementErodeSize, elementErodeSize));
    // Perform erosion
    ::cv::erode(foregroundMask, openForegroundMask, elementErode);

    // Learn foreground color model
    m_masker->trainForegroundModel(videoCV, openForegroundMask, 5, m_noise);
}

// ------------------------------------------------------------------------------

} // namespace colourSegmentation
