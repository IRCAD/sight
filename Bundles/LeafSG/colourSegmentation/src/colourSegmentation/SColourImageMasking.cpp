/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "colourSegmentation/SColourImageMasking.hpp"

#include <arData/FrameTL.hpp>

#include <cvIO/FrameTL.hpp>
#include <cvIO/Image.hpp>

#include <fwCom/Signal.hxx>
#include <fwCom/Slots.hxx>

#include <fwData/mt/ObjectReadLock.hpp>

#include <fwDataTools/helper/ArrayGetter.hpp>

#include <fwServices/macros.hpp>

#include <boost/make_unique.hpp>
#include <boost/tokenizer.hpp>

namespace colourSegmentation
{

fwServicesRegisterMacro( ::fwServices::IOperator, ::colourSegmentation::SColourImageMasking);

const ::fwCom::Slots::SlotKeyType SColourImageMasking::s_SET_BACKGROUND_SLOT            = "setBackground";
const ::fwCom::Slots::SlotKeyType SColourImageMasking::s_SET_FOREGROUND_SLOT            = "setForeground";
const ::fwCom::Slots::SlotKeyType SColourImageMasking::s_SET_THRESHOLD_SLOT             = "setThreshold";
const ::fwCom::Slots::SlotKeyType SColourImageMasking::s_SET_NOISE_LEVEL_SLOT           = "setNoiseLevel";
const ::fwCom::Slots::SlotKeyType SColourImageMasking::s_SET_BACKGROUND_COMPONENTS_SLOT = "setBackgroundComponents";
const ::fwCom::Slots::SlotKeyType SColourImageMasking::s_SET_FOREGROUND_COMPONENTS_SLOT = "setForegroundComponents";

const ::fwServices::IService::KeyType s_MASK_KEY          = "mask";
const ::fwServices::IService::KeyType s_VIDEO_TL_KEY      = "videoTL";
const ::fwServices::IService::KeyType s_VIDEO_MASK_TL_KEY = "videoMaskTL";

// ------------------------------------------------------------------------------

SColourImageMasking::SColourImageMasking() noexcept :
    m_lastVideoTimestamp(::fwCore::HiResClock::getTimeInMilliSec()),
    m_scaleFactor(1.),
    m_maskDownsize(::cv::Size(0, 0)),
    m_lowerColor(::cv::Scalar(0, 0, 0)),
    m_upperColor(::cv::Scalar(255, 255, 255)),
    m_noise(0.),
    m_backgroundComponents(5),
    m_foregroundComponents(5)
{
    newSlot( s_SET_BACKGROUND_SLOT, &SColourImageMasking::setBackground, this );
    newSlot( s_SET_FOREGROUND_SLOT, &SColourImageMasking::setForeground, this );
    newSlot( s_SET_THRESHOLD_SLOT, &SColourImageMasking::setThreshold, this );
    newSlot( s_SET_NOISE_LEVEL_SLOT, &SColourImageMasking::setNoiseLevel, this );
    newSlot( s_SET_BACKGROUND_COMPONENTS_SLOT, &SColourImageMasking::setBackgroundComponents, this );
    newSlot( s_SET_FOREGROUND_COMPONENTS_SLOT, &SColourImageMasking::setForegroundComponents, this );
}

// ------------------------------------------------------------------------------

SColourImageMasking::~SColourImageMasking() noexcept
{
}

// ------------------------------------------------------------------------------

void SColourImageMasking::configuring()
{
    const ::fwServices::IService::ConfigType config = this->getConfigTree().get_child("config.<xmlattr>");

    m_scaleFactor          = config.get<float>("scaleFactor", 1.0);
    m_noise                = config.get<double>("noise", 0.0);
    m_foregroundComponents = config.get<int>("foregroundComponents", 5);
    m_backgroundComponents = config.get<int>("backgroundComponents", 5);

    OSLM_ASSERT("Scale factor must be between 0 and 1. Current value: " << m_scaleFactor,
                (m_scaleFactor > 0 && m_scaleFactor <= 1));
    OSLM_ASSERT("The number of background components must be greater than 0. Current value: " << m_backgroundComponents,
                m_backgroundComponents > 0);
    OSLM_ASSERT("Noise value must be >= 0. Current value:" << m_noise, m_noise >= 0);
    OSLM_ASSERT("The number of foreground components must be greater than 0. Current value: " << m_foregroundComponents,
                m_foregroundComponents > 0);

    m_lowerColor = ::cv::Scalar(0, 0, 0);
    m_upperColor = ::cv::Scalar(255, 255, 255);

    const ::fwServices::IService::ConfigType hsvConfig = this->getConfigTree().get_child("HSV");
    std::string s_lowerValue                           = hsvConfig.get<std::string>("lower", "");
    std::string s_upperValue                           = hsvConfig.get<std::string>("upper", "");

    const ::boost::char_separator<char> sep {",", ";"};

    if(!s_lowerValue.empty())
    {
        const ::boost::tokenizer<boost::char_separator<char> > tokLower {s_lowerValue, sep};
        int i = 0;
        for(const auto& it : tokLower)
        {
            SLM_ASSERT("Only 3 integers needed to define lower HSV value", i < 3);
            m_lowerColor[i++] = ::boost::lexical_cast< double >(it);
        }
    }

    if(!s_upperValue.empty())
    {
        const ::boost::tokenizer<boost::char_separator<char> > tokUpper {s_upperValue, sep};
        int i = 0;
        for(const auto& it : tokUpper)
        {
            SLM_ASSERT("Only 3 integers needed to define upper HSV value", i < 3);
            m_upperColor[i++] = ::boost::lexical_cast< double >(it);
        }
    }
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
        ::fwCore::HiResClock::HiResClockType currentTimestamp = ::fwCore::HiResClock::getTimeInMilliSec();

        // Get image from the video timeline
        CSPTR(::arData::FrameTL::BufferType) videoBuffer = videoTL->getClosestBuffer(currentTimestamp);

        if(!videoBuffer)
        {
            OSLM_INFO("Buffer not found with timestamp "<< currentTimestamp);
            return;
        }

        const std::uint8_t* frameBuffOutVideo = &videoBuffer->getElement(0);

        ::fwCore::HiResClock::HiResClockType videoTimestamp = videoBuffer->getTimestamp();
        if(videoTimestamp <= m_lastVideoTimestamp)
        {
            return;
        }

        m_lastVideoTimestamp = videoTimestamp;

        // convert the ::fw::Data::Image mask to an OpenCV image
        ::cv::Mat maskCV = ::cvIO::Image::copyToCv(mask);

        ::cv::cvtColor(maskCV, maskCV, cv::COLOR_BGR2GRAY);

	maskCV = (maskCV > 0);

        //convert the ::arData::FrameTL videoTL to an OpenCV image
        const ::cv::Mat videoCV = ::cvIO::FrameTL::moveToCv(videoTL, frameBuffOutVideo);

        // Create image mask to put inside the timeline
        SPTR(::arData::FrameTL::BufferType) maskBuffer = videoMaskTL->createBuffer(currentTimestamp);
        std::uint8_t* frameBuffOutMask = maskBuffer->addElement(0);

        ::cv::Mat videoMaskCV = ::cvIO::FrameTL::moveToCv(videoMaskTL, frameBuffOutMask);

        // Get the foreground mask
        ::cv::Mat foregroundMask = m_masker->makeMask(videoCV, m_maskDownsize, maskCV);

        // Create an openCV mat that aliases the buffer created from the output timeline
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
    if(!videoBuffer)
    {
        OSLM_INFO("Buffer not found with timestamp " << currentTimestamp);
        return;
    }
    const std::uint8_t* frameBuffOutVideo = &videoBuffer->getElement(0);

    //convert the ::arData::FrameTL videoTL to an OpenCV image
    const ::cv::Mat videoCV = ::cvIO::FrameTL::moveToCv(videoTL, frameBuffOutVideo);

    // convert the ::fwData::Image mask to an OpenCV image
    ::cv::Mat maskCV = ::cvIO::Image::copyToCv(mask);

    // Convert color mask to grayscale value
    ::cv::cvtColor(maskCV, maskCV, cv::COLOR_RGB2GRAY);

    maskCV = (maskCV > 0);

    // Save size to downscale the image (speed up the process but decrease segmentation quality)
    m_maskDownsize = ::cv::Size(static_cast<int>(static_cast<float>(maskCV.size[1])*m_scaleFactor),
                                static_cast<int>(static_cast<float>(maskCV.size[0])*m_scaleFactor));

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
    m_masker->trainBackgroundModel(videoCV, maskCV, m_backgroundComponents);

    // Initialize the mask timeline
    auto videoMaskTL = this->getInOut< ::arData::FrameTL >(s_VIDEO_MASK_TL_KEY);
    videoMaskTL->initPoolSize(videoTL->getWidth(), videoTL->getHeight(), ::fwTools::Type::s_UINT8, 4);
}

// ------------------------------------------------------------------------------

void SColourImageMasking::setForeground()
{
    auto videoTL = this->getInput< ::arData::FrameTL >(s_VIDEO_TL_KEY);

    ::fwCore::HiResClock::HiResClockType currentTimestamp = ::fwCore::HiResClock::getTimeInMilliSec();
    CSPTR(::arData::FrameTL::BufferType) videoBuffer      = videoTL->getClosestBuffer(currentTimestamp);
    if(!videoBuffer)
    {
        OSLM_INFO("Buffer not found with timestamp "<< currentTimestamp);
        return;
    }
    const std::uint8_t* frameBuffOutVideo = &videoBuffer->getElement(0);

    //convert mask to an OpenCV image:
    ::cv::Mat videoCV = ::cvIO::FrameTL::moveToCv(videoTL, frameBuffOutVideo);

    // Convert RGB to HSV
    ::cv::Mat videoBGR, videoHSV;
    ::cv::cvtColor(videoCV, videoBGR, CV_RGBA2BGR);
    ::cv::cvtColor(videoBGR, videoHSV, CV_BGR2HSV);

    // Get the mask to learn the foreground model
    ::cv::Mat foregroundMask;
    ::cv::inRange(videoHSV, m_lowerColor, m_upperColor, foregroundMask);

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
    m_masker->trainForegroundModel(videoCV, openForegroundMask, m_foregroundComponents, m_noise);
}

// ------------------------------------------------------------------------------

void SColourImageMasking::setThreshold(int threshold)
{
    if(m_masker)
    {
        m_masker->setThreshold(threshold);
    }
}

// ------------------------------------------------------------------------------

void SColourImageMasking::setNoiseLevel(double noiseLevel)
{
    m_noise = noiseLevel;
}

// ------------------------------------------------------------------------------

void SColourImageMasking::setBackgroundComponents(int bgComponents)
{
    m_backgroundComponents = bgComponents;
}

// ------------------------------------------------------------------------------

void SColourImageMasking::setForegroundComponents(int fgComponents)
{
    m_foregroundComponents = fgComponents;
}

// ------------------------------------------------------------------------------

} // namespace colourSegmentation
