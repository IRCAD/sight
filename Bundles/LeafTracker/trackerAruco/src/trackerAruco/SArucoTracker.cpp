/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2014-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "trackerAruco/SArucoTracker.hpp"

#include <arData/Camera.hpp>
#include <arData/FrameTL.hpp>
#include <arData/MarkerTL.hpp>

#include <fwCom/Signal.hxx>
#include <fwCom/Slots.hxx>

#define FW_PROFILING_DISABLED
#include <fwCore/Profiling.hpp>

#include <fwData/Composite.hpp>

#include <aruco/aruco.h>
#include <aruco/cvdrawingutils.h>

#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/path.hpp>
#include <boost/lexical_cast.hpp>

#include <opencv2/core.hpp>
#include <opencv2/opencv.hpp>

// -----------------------------------------------------------------------------

namespace trackerAruco
{
fwServicesRegisterMacro(::tracker::ITracker, ::trackerAruco::SArucoTracker, ::fwData::Composite);
// -----------------------------------------------------------------------------

const ::fwCom::Signals::SignalKeyType SArucoTracker::s_DETECTION_DONE_SIG = "detectionDone";

const ::fwCom::Slots::SlotKeyType SArucoTracker::s_CHANGE_METHOD_SLOT           = "changeMethod";
const ::fwCom::Slots::SlotKeyType SArucoTracker::s_CHANGE_BLOCKSIZE_SLOT        = "changeBlockSize";
const ::fwCom::Slots::SlotKeyType SArucoTracker::s_CHANGE_CONSTANT_SLOT         = "changeConstant";
const ::fwCom::Slots::SlotKeyType SArucoTracker::s_CHANGE_BORDERWIDTH_SLOT      = "changeBorderWidth";
const ::fwCom::Slots::SlotKeyType SArucoTracker::s_CHANGE_PATTERNWIDTH_SLOT     = "changePatternWidth";
const ::fwCom::Slots::SlotKeyType SArucoTracker::s_CHANGE_CORNERREFINEMENT_SLOT = "changeCornerRefinement";
const ::fwCom::Slots::SlotKeyType SArucoTracker::s_CHANGE_SPEED_SLOT            = "changeSpeed";
const ::fwCom::Slots::SlotKeyType SArucoTracker::s_DISPLAY_TAGS_SLOT            = "displayTags";

const ::fwServices::IService::KeyType s_CAMERA_INPUT      = "camera";
const ::fwServices::IService::KeyType s_TAGTL_INOUT_GROUP = "tagTL";

// -----------------------------------------------------------------------------

SArucoTracker::SArucoTracker() throw () :
    m_arUcoTracker(nullptr),
    m_camParameters(nullptr),
    m_threshold("auto"),
    m_borderWidth(0.25),
    m_patternWidth(80),
    m_isInitialized(false),
    m_blockSize(7.),
    m_constant(7.),
    m_debugMarkers(false),
    m_speed(3),
    m_thresholdMethod(::aruco::MarkerDetector::ADPT_THRES),
    m_cornerRefinement(::aruco::MarkerDetector::SUBPIX)
{
    m_sigDetectionDone = newSignal<DetectionDoneSignalType>(s_DETECTION_DONE_SIG);

    newSlot(s_CHANGE_METHOD_SLOT, &SArucoTracker::setMethod, this);
    newSlot(s_CHANGE_CORNERREFINEMENT_SLOT, &SArucoTracker::setCornerRefinement, this);
    newSlot(s_CHANGE_BLOCKSIZE_SLOT, &SArucoTracker::setBlockSize, this);
    newSlot(s_CHANGE_CONSTANT_SLOT, &SArucoTracker::setConstant, this);
    newSlot(s_CHANGE_BORDERWIDTH_SLOT, &SArucoTracker::setBorderWidth, this);
    newSlot(s_CHANGE_PATTERNWIDTH_SLOT, &SArucoTracker::setPatternWidth, this);
    newSlot(s_CHANGE_SPEED_SLOT, &SArucoTracker::setSpeed, this);
    newSlot(s_DISPLAY_TAGS_SLOT, &SArucoTracker::displayTags, this);
}

// -----------------------------------------------------------------------------

SArucoTracker::~SArucoTracker() throw ()
{
}

// -----------------------------------------------------------------------------

void SArucoTracker::configuring() throw (::fwTools::Failed)
{
    this->::tracker::ITracker::configuring();

    ::fwRuntime::ConfigurationElement::sptr cfg = m_configuration->findConfigurationElement("config");
    SLM_ASSERT("Tag 'config' not found.", cfg);

    // gets marker informations
    {
        ::fwRuntime::ConfigurationElement::sptr cfgTrack = cfg->findConfigurationElement("track");
        SLM_ASSERT("Tag 'track' not found.", cfgTrack);
        typedef ::fwRuntime::ConfigurationElementContainer::Container CfgContainer;
        for(const CfgContainer::value_type& elt : cfgTrack->getElements())
        {
            SLM_ASSERT("Missing 'id' attribute.", elt->hasAttribute("id"));
            const std::string markersIDStr = elt->getAttributeValue("id");
            ::boost::tokenizer<> tok(markersIDStr);
            MarkerIDType markersID;
            for( ::boost::tokenizer<>::iterator it = tok.begin(); it != tok.end(); ++it)
            {
                const int id = ::boost::lexical_cast< int >(*it);
                markersID.push_back(id);
            }
            m_markers.push_back(markersID);
        }
    }

    // gets pattern width
    ::fwRuntime::ConfigurationElement::sptr cfgPatternWidth = cfg->findConfigurationElement("patternWidth");
    if (cfgPatternWidth)
    {
        m_patternWidth = ::boost::lexical_cast< double >(cfgPatternWidth->getValue());
    }

    // gets threshold parameters
    {
        ::fwRuntime::ConfigurationElement::sptr cfgThreshold = cfg->findConfigurationElement("threshold");
        if(cfgThreshold)
        {
            ::fwRuntime::ConfigurationElement::sptr cfgMethod    = cfgThreshold->findConfigurationElement("method");
            ::fwRuntime::ConfigurationElement::sptr cfgBlockSize = cfgThreshold->findConfigurationElement("blockSize");
            ::fwRuntime::ConfigurationElement::sptr cfgConstant  = cfgThreshold->findConfigurationElement("constant");

            std::string thresholdMethod = cfgMethod->getValue();
            if(thresholdMethod == "ADPT_THRES")
            {
                m_thresholdMethod = ::aruco::MarkerDetector::ADPT_THRES;
            }
            else if(thresholdMethod == "FIXED_THRES")
            {
                m_thresholdMethod = ::aruco::MarkerDetector::FIXED_THRES;
            }
            else if(thresholdMethod == "CANNY")
            {
                m_thresholdMethod = ::aruco::MarkerDetector::CANNY;
            }
            else
            {
                SLM_FATAL("Threshold method ("+thresholdMethod+") set in parameter is not allowed. "
                          "Allowed values : ADPT_THRES, FIXED_THRES, CANNY");
            }

            std::string blocksize = cfgBlockSize->getValue();
            if(!blocksize.empty())
            {
                m_blockSize = ::boost::lexical_cast<double>(blocksize);
            }

            std::string constant = cfgConstant->getValue();
            if(!constant.empty())
            {
                m_constant = ::boost::lexical_cast<double>(constant);
            }
        }
    }

    // get the debug markers flag
    ::fwRuntime::ConfigurationElement::sptr cfgDebugMarkers = cfg->findConfigurationElement("debugMarkers");
    if (cfgDebugMarkers && cfgDebugMarkers->getValue() == "yes")
    {
        m_debugMarkers = true;
    }
}

// -----------------------------------------------------------------------------

void SArucoTracker::starting() throw (::fwTools::Failed)
{
    // initialized marker timeline matrix (4*2D points)
    const size_t numTagTL = this->getKeyGroupSize(s_TAGTL_INOUT_GROUP);
    for(size_t i = 0; i < numTagTL; ++i)
    {
        ::arData::MarkerTL::sptr markerTL = this->getInOut< ::arData::MarkerTL >(s_TAGTL_INOUT_GROUP, i);
        markerTL->initPoolSize(static_cast<unsigned int>(m_markers[i].size()));
    }
    m_isTracking = true;
}

// -----------------------------------------------------------------------------

void SArucoTracker::stopping() throw (::fwTools::Failed)
{
    delete m_camParameters;
    delete m_arUcoTracker;
    m_isInitialized = false;
    m_isTracking    = false;
}

// -----------------------------------------------------------------------------

void SArucoTracker::updating() throw (::fwTools::Failed)
{
}

// -----------------------------------------------------------------------------

void SArucoTracker::tracking(::fwCore::HiResClock::HiResClockType& timestamp)
{
    ::arData::FrameTL::csptr frameTL = this->getInput< ::arData::FrameTL >(s_TIMELINE_INPUT);

    if(!m_isInitialized)
    {
        m_arUcoTracker = new ::aruco::MarkerDetector();
        m_arUcoTracker->setThresholdMethod(m_thresholdMethod);
        m_arUcoTracker->setThresholdParams(m_blockSize, m_constant);
        m_arUcoTracker->setCornerRefinementMethod(::aruco::MarkerDetector::NONE);
        m_arUcoTracker->setBorderDistance(0.01f);

        ::arData::Camera::csptr arCam = this->getInput< ::arData::Camera >(s_CAMERA_INPUT);
        ::cv::Mat cameraMatrix        = ::cv::Mat::eye(3, 3, CV_64F);
        ::cv::Mat distorsionCoeff     = ::cv::Mat::eye(4, 1, CV_64F);

        // 3x3 matrix (fx 0 cx, 0 fy cy, 0 0 1)
        cameraMatrix.at<double>(0, 0) = arCam->getFx();
        cameraMatrix.at<double>(1, 1) = arCam->getFy();
        cameraMatrix.at<double>(0, 2) = arCam->getCx();
        cameraMatrix.at<double>(1, 2) = arCam->getCy();

        // 4x1 matrix (k1,k2,p1,p2)
        for (unsigned int i = 0; i < 4; ++i)
        {
            distorsionCoeff.at<double>(static_cast<int>(i), 0) = arCam->getDistortionCoefficient()[i];
        }

        // size of the image
        ::cv::Size2i size(static_cast<int>(frameTL->getWidth()), static_cast<int>(frameTL->getHeight()));
        m_camParameters = new ::aruco::CameraParameters(cameraMatrix, distorsionCoeff, size);
        m_isInitialized = true;
    }

    // if parameters changed
    m_arUcoTracker->setThresholdMethod(m_thresholdMethod);
    m_arUcoTracker->setThresholdParams(m_blockSize, m_constant);
    m_arUcoTracker->setCornerRefinementMethod(m_cornerRefinement);
    m_arUcoTracker->setDesiredSpeed(static_cast<int>(m_speed));

    const CSPTR(::arData::FrameTL::BufferType) buffer = frameTL->getClosestBuffer(timestamp);

    OSLM_WARN_IF("Buffer not found with timestamp "<< timestamp, !buffer );
    if(buffer)
    {
        m_lastTimestamp = timestamp;

        const std::uint8_t* frameBuff = &buffer->getElement(0);
        std::vector< ::aruco::Marker > detectedMarkers;

        // read the input image
        const ::cv::Size frameSize(static_cast<int>(frameTL->getWidth()),
                                   static_cast<int>(frameTL->getHeight()));
        ::cv::Mat inImage = ::cv::Mat(frameSize, CV_8UC4, (void*)frameBuff, ::cv::Mat::AUTO_STEP);

        // aruco expects a grey image and make a conversion at the beginning of the detect() method if it receives
        // a RGB image. However we have a RGBA image so we must make the conversion ourselves.
        cv::Mat grey;
        // inImage is BGRA (see constructor of ::cv::Mat)
        cv::cvtColor(inImage, grey, CV_BGRA2GRAY);

        // Ok, let's detect
        m_arUcoTracker->detect(grey, detectedMarkers, *m_camParameters, static_cast<float>(m_patternWidth/1000.));

        // For Debug purpose
        // ::cv::imshow("Threshold", m_arUcoTracker->getThresholdedImage());

        // for each marker, draw info and its boundaries in the image
        unsigned int index = 0;
        size_t tagTLIndex  = 0;
        for(const auto& markersID : m_markers)
        {
            unsigned int color[3] = {0, 0, 0};
            color[index]                      = 255;
            ::arData::MarkerTL::sptr markerTL =
                this->getInOut< ::arData::MarkerTL >(s_TAGTL_INOUT_GROUP, tagTLIndex);
            SPTR(::arData::MarkerTL::BufferType) trackerObject;

            unsigned int markerPosition = 0;
            for (const auto& markerID : markersID)
            {
                for (unsigned int i = 0; i < detectedMarkers.size(); i++)
                {
                    if (detectedMarkers[i].id == markerID)
                    {
                        if(m_debugMarkers)
                        {
                            detectedMarkers[i].draw(inImage, cvScalar(color[0], color[1], color[2], 255), 2);
                        }

                        // Push matrix
                        float markerBuffer[8];
                        for (size_t j = 0; j < 4; ++j)
                        {
                            markerBuffer[j*2]     = detectedMarkers[i][j].x;
                            markerBuffer[j*2 + 1] = detectedMarkers[i][j].y;
                        }
                        if(trackerObject == nullptr)
                        {
                            trackerObject = markerTL->createBuffer(timestamp);
                            markerTL->pushObject(trackerObject);
                        }
                        trackerObject->setElement(markerBuffer, markerPosition);
                    }
                }
                ++markerPosition;
            }

            // Notify
            if(trackerObject != nullptr)
            {
                ::arData::TimeLine::ObjectPushedSignalType::sptr sig;
                sig = markerTL->signal< ::arData::TimeLine::ObjectPushedSignalType >(
                    ::arData::TimeLine::s_OBJECT_PUSHED_SIG );
                sig->asyncEmit(timestamp);
            }

            ++index;
            if(index >= 3)
            {
                index = 0;
            }
            ++tagTLIndex;
        }
        // Emit
        m_sigDetectionDone->asyncEmit(timestamp);
    }
}

// -----------------------------------------------------------------------------

void SArucoTracker::setMethod(unsigned int method)
{
    if(method == 0)
    {
        m_thresholdMethod = ::aruco::MarkerDetector::ADPT_THRES;
    }
    else if(method == 1)
    {
        m_thresholdMethod = ::aruco::MarkerDetector::FIXED_THRES;
    }
    else if(method == 2)
    {
        m_thresholdMethod = ::aruco::MarkerDetector::CANNY;
    }
    else
    {
        SLM_FATAL("Threshold method set in parameter is not allowed. "
                  "Allowed values : ADPT_THRES, FIXED_THRES, CANNY");
    }
}

// -----------------------------------------------------------------------------

void SArucoTracker::setBlockSize(double size)
{
    m_blockSize = size;
}

// -----------------------------------------------------------------------------

void SArucoTracker::setConstant(double constant)
{
    m_constant = constant;
}

// -----------------------------------------------------------------------------

void SArucoTracker::setBorderWidth(double borderWidth)
{
    m_borderWidth = borderWidth;
}

// -----------------------------------------------------------------------------

void SArucoTracker::setPatternWidth(double patternWidth)
{
    m_patternWidth = patternWidth;
}

// -----------------------------------------------------------------------------

void SArucoTracker::setCornerRefinement(unsigned int method)
{
    if(method == ::aruco::MarkerDetector::NONE)
    {
        m_cornerRefinement = ::aruco::MarkerDetector::NONE;
    }
    else if(method == ::aruco::MarkerDetector::HARRIS)
    {
        m_cornerRefinement = ::aruco::MarkerDetector::HARRIS;
    }
    else if(method == ::aruco::MarkerDetector::SUBPIX)
    {
        m_cornerRefinement = ::aruco::MarkerDetector::SUBPIX;
    }
    else if(method == ::aruco::MarkerDetector::LINES)
    {
        m_cornerRefinement = ::aruco::MarkerDetector::LINES;
    }
    else
    {
        // ?
    }
}

// -----------------------------------------------------------------------------

void SArucoTracker::setSpeed(unsigned int value)
{
    if(value <= 3)
    {
        m_speed = value;
    }
}

// -----------------------------------------------------------------------------

void SArucoTracker::displayTags(bool b)
{
    m_debugMarkers = b;
}

} // namespace trackerAruco

