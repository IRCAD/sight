/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2014-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */


#include "trackerAruco/SArucoTracker.hpp"

#include <fwCore/Profiling.hpp>

#include <arData/Camera.hpp>
#include <extData/FrameTL.hpp>
#include <arData/MarkerTL.hpp>

#include <fwCom/Signal.hxx>
#include <fwCom/Slots.hxx>
#include <fwData/Composite.hpp>

#include <aruco/aruco.h>
#include <aruco/cvdrawingutils.h>

#include <boost/filesystem/path.hpp>
#include <boost/filesystem/operations.hpp>
#include <boost/lexical_cast.hpp>

#include <opencv2/core/core.hpp>
#include <opencv2/opencv.hpp>

//-----------------------------------------------------------------------------

namespace trackerAruco
{
fwServicesRegisterMacro(::tracker::ITracker, ::trackerAruco::SArucoTracker, ::fwData::Composite);
//-----------------------------------------------------------------------------

const ::fwCom::Signals::SignalKeyType SArucoTracker::s_DETECTION_DONE_SIG = "detectionDone";

const ::fwCom::Slots::SlotKeyType SArucoTracker::s_CHANGE_METHOD_SLOT           = "changeMethod";
const ::fwCom::Slots::SlotKeyType SArucoTracker::s_CHANGE_BLOCKSIZE_SLOT        = "changeBlockSize";
const ::fwCom::Slots::SlotKeyType SArucoTracker::s_CHANGE_CONSTANT_SLOT         = "changeConstant";
const ::fwCom::Slots::SlotKeyType SArucoTracker::s_CHANGE_BORDERWIDTH_SLOT      = "changeBorderWidth";
const ::fwCom::Slots::SlotKeyType SArucoTracker::s_CHANGE_PATTERNWIDTH_SLOT     = "changePatternWidth";
const ::fwCom::Slots::SlotKeyType SArucoTracker::s_CHANGE_CORNERREFINEMENT_SLOT = "changeCornerRefinement";
const ::fwCom::Slots::SlotKeyType SArucoTracker::s_CHANGE_SPEED_SLOT            = "changeSpeed";
const ::fwCom::Slots::SlotKeyType SArucoTracker::s_DISPLAY_TAGS_SLOT            = "displayTags";

const ::fwCom::Slots::SlotKeyType SArucoTracker::s_DETECT_MARKER_SLOT = "detectMarker";

//-----------------------------------------------------------------------------

SArucoTracker::SArucoTracker() throw () :
    m_arUcoTracker(NULL),
    m_camParameters(NULL),
    m_threshold("auto"),
    m_borderWidth(0.25),
    m_patternWidth(80),
    m_lastTimestamp(0),
    m_isInitialized(false),
    m_blockSize(7.),
    m_constant(7.),
    m_debugMarkers(false),
    m_speed(3),
    m_thresholdMethod(::aruco::MarkerDetector::ADPT_THRES),
    m_cornerRefinement(::aruco::MarkerDetector::NONE)
{
    m_sigDetectionDone = DetectionDoneSignalType::New();
    m_signals( s_DETECTION_DONE_SIG,  m_sigDetectionDone);



    m_slotChangeMethod           = ::fwCom::newSlot(&SArucoTracker::setMethod, this);
    m_slotChangeBlockSize        = ::fwCom::newSlot(&SArucoTracker::setBlockSize, this);
    m_slotChangeConstant         = ::fwCom::newSlot(&SArucoTracker::setConstant, this);
    m_slotChangeBorderWidth      = ::fwCom::newSlot(&SArucoTracker::setBorderWidth, this);
    m_slotChangePatternWidth     = ::fwCom::newSlot(&SArucoTracker::setPatternWidth, this);
    m_slotChangeCornerRefinement = ::fwCom::newSlot(&SArucoTracker::setCornerRefinement,this);
    m_slotChangeSpeed            = ::fwCom::newSlot(&SArucoTracker::setSpeed, this);
    m_slotDisplayTags            = ::fwCom::newSlot(&SArucoTracker::displayTags, this);
    m_slotDetectMarker           = ::fwCom::newSlot(&SArucoTracker::detectMarker,this);


    ::fwCom::HasSlots::m_slots(s_CHANGE_METHOD_SLOT, m_slotChangeMethod);
    ::fwCom::HasSlots::m_slots(s_CHANGE_CORNERREFINEMENT_SLOT, m_slotChangeCornerRefinement);
    ::fwCom::HasSlots::m_slots(s_CHANGE_BLOCKSIZE_SLOT, m_slotChangeBlockSize);
    ::fwCom::HasSlots::m_slots(s_CHANGE_CONSTANT_SLOT, m_slotChangeConstant);
    ::fwCom::HasSlots::m_slots(s_CHANGE_BORDERWIDTH_SLOT, m_slotChangeBorderWidth);
    ::fwCom::HasSlots::m_slots(s_CHANGE_PATTERNWIDTH_SLOT, m_slotChangePatternWidth);
    ::fwCom::HasSlots::m_slots(s_DETECT_MARKER_SLOT, m_slotDetectMarker);
    ::fwCom::HasSlots::m_slots(s_CHANGE_SPEED_SLOT, m_slotChangeSpeed);
    ::fwCom::HasSlots::m_slots(s_DISPLAY_TAGS_SLOT, m_slotDisplayTags);

    ::fwCom::HasSlots::m_slots.setWorker( m_associatedWorker );

    m_connections = ::fwServices::helper::SigSlotConnection::New();
}

//-----------------------------------------------------------------------------

SArucoTracker::~SArucoTracker() throw ()
{
}

//-----------------------------------------------------------------------------

void SArucoTracker::configuring() throw (::fwTools::Failed)
{
    ::fwRuntime::ConfigurationElement::sptr cfg = m_configuration->findConfigurationElement("config");
    SLM_ASSERT("Tag 'config' not found.", cfg);

    // gets video timeline
    {
        ::fwRuntime::ConfigurationElement::sptr cfgTimelineVideo = cfg->findConfigurationElement("timelineVideo");
        SLM_ASSERT("Tag 'timelineVideo' not found.", cfgTimelineVideo);
        m_timelineVideoKey = cfgTimelineVideo->getValue();
        SLM_ASSERT("'timelineVideo' is empty", !m_timelineVideoKey.empty());
    }

    // gets marker informations
    {
        ::fwRuntime::ConfigurationElement::sptr cfgTrack = cfg->findConfigurationElement("track");
        SLM_ASSERT("Tag 'track' not found.", cfgTrack);
        typedef ::fwRuntime::ConfigurationElementContainer::Container CfgContainer;
        for(const CfgContainer::value_type& elt : cfgTrack->getElements())
        {
            SLM_ASSERT("Missing 'id' attribute.", elt->hasAttribute("id"));
            SLM_ASSERT("Missing 'timeline' attribute.", elt->hasAttribute("timeline"));

            const std::string currentTimeline = elt->getAttributeValue("timeline");
            const std::string markersID       = elt->getAttributeValue("id");

            m_timelineVector.push_back(MarkerPairType());
            MarkerPairType& markerPair = m_timelineVector.back();
            markerPair.first = currentTimeline;

            ::boost::tokenizer<> tok(markersID);
            for( ::boost::tokenizer<>::iterator it = tok.begin(); it!=tok.end(); ++it)
            {
                const int id = ::boost::lexical_cast< int >(*it);
                m_markers[id] = currentTimeline;
                markerPair.second.push_back(id);
            }
        }
    }

    // gets camera
    {
        ::fwRuntime::ConfigurationElement::sptr cfgCamera = cfg->findConfigurationElement("camera");
        SLM_ASSERT("Tag 'camera' not found.", cfgCamera);
        m_cameraKey = cfgCamera->getValue();
        SLM_ASSERT("'camera' is empty", !m_cameraKey.empty());
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

//-----------------------------------------------------------------------------

void SArucoTracker::starting() throw (::fwTools::Failed)
{
    ::fwData::Composite::sptr comp = this->getObject< ::fwData::Composite >();

    // initialized marker timeline matrix (4*2D points)
    for(const MarkerPairType &elt : m_timelineVector)
    {
        ::arData::MarkerTL::sptr markerTL = comp->at< ::arData::MarkerTL >(elt.first);
        markerTL->initPoolSize(static_cast<unsigned int>(elt.second.size()));
    }

    ::extData::FrameTL::sptr frameTL = comp->at< ::extData::FrameTL >(m_timelineVideoKey);

    m_connections->connect(frameTL, ::extData::TimeLine::s_OBJECT_PUSHED_SIG, this->getSptr(),
                           ::trackerAruco::SArucoTracker::s_DETECT_MARKER_SLOT);
}

//-----------------------------------------------------------------------------

void SArucoTracker::stopping() throw (::fwTools::Failed)
{
    m_connections->disconnect();
    delete m_camParameters;
    delete m_arUcoTracker;
    m_isInitialized = false;
}

//-----------------------------------------------------------------------------

void SArucoTracker::updating() throw (::fwTools::Failed)
{
}

//-----------------------------------------------------------------------------

void SArucoTracker::detectMarker(::fwCore::HiResClock::HiResClockType timestamp)
{
    if (timestamp > m_lastTimestamp)
    {
        ::fwData::Composite::sptr comp = this->getObject< ::fwData::Composite >();

        ::extData::FrameTL::sptr frameTL = comp->at< ::extData::FrameTL >(m_timelineVideoKey);
        if(!m_isInitialized)
        {
            m_arUcoTracker = new ::aruco::MarkerDetector();
            m_arUcoTracker->setThresholdMethod(m_thresholdMethod);
            m_arUcoTracker->setThresholdParams(m_blockSize, m_constant);
            m_arUcoTracker->setCornerRefinementMethod(::aruco::MarkerDetector::NONE);

            ::arData::Camera::sptr arCam = comp->at< ::arData::Camera >(m_cameraKey);
            ::cv::Mat cameraMatrix       = ::cv::Mat::eye(3, 3, CV_64F);
            cv::Mat distorsionCoeff = ::cv::Mat::eye(4, 1, CV_64F);


            // 3x3 matrix (fx 0 cx, 0 fy cy, 0 0 1)

            cameraMatrix.at<double>(0,0) = arCam->getFx();
            cameraMatrix.at<double>(1,1) = arCam->getFy();
            cameraMatrix.at<double>(0,2) = arCam->getCx();
            cameraMatrix.at<double>(1,2) = arCam->getCy();

            //4x1 matrix (k1,k2,p1,p2)

            for (unsigned int i = 0; i < 4; ++i)
            {
                distorsionCoeff.at<double>(static_cast<int>(i),0) = arCam->getDistortionCoefficient()[i];
            }

            //size of the image
            ::cv::Size2i size(static_cast<int>(frameTL->getWidth()), static_cast<int>(frameTL->getHeight()));
            m_camParameters = new ::aruco::CameraParameters(cameraMatrix, distorsionCoeff, size);

            m_isInitialized = true;
        }

        //if parameters changed
        m_arUcoTracker->setThresholdMethod(m_thresholdMethod);
        m_arUcoTracker->setThresholdParams(m_blockSize, m_constant);
        m_arUcoTracker->setCornerRefinementMethod(m_cornerRefinement);
        m_arUcoTracker->setDesiredSpeed(m_speed);

        ::fwCore::HiResClock::HiResClockType timestamp     = frameTL->getNewerTimestamp();
        const CSPTR(::extData::FrameTL::BufferType) buffer = frameTL->getClosestBuffer(timestamp);

        OSLM_WARN_IF("Buffer not found with timestamp "<< timestamp, !buffer );
        if(buffer)
        {
            m_lastTimestamp = timestamp;

            const ::boost::uint8_t* frameBuff = &buffer->getElement(0);
            std::vector< ::aruco::Marker > detectedMarkers;

            // read the input image
            ::cv::Mat inImage =
                ::cv::Mat (::cv::Size(static_cast<int>(frameTL->getWidth()), static_cast<int>(frameTL->getHeight())),
                           CV_8UC4, (void*)frameBuff, ::cv::Mat::AUTO_STEP);


            // aruco expects a grey image and make a conversion at the beginning of the detect() method if it receives
            // a RGB image. However we have a RGBA image so we must make the conversion ourselves.
            cv::Mat grey;
            cv::cvtColor(inImage, grey, CV_RGBA2GRAY);

            //Ok, let's detect
            m_arUcoTracker->detect(grey, detectedMarkers, *m_camParameters, static_cast<float>(m_patternWidth/1000.));

            //For Debug purpose
            //::cv::imshow("Threshold", m_arUcoTracker->getThresholdedImage());


            //for each marker, draw info and its boundaries in the image
            unsigned int index = 0;
            for(const MarkerPairType &tlPair : m_timelineVector)
            {
                unsigned int color[3] = {0,0,0};
                color[index]                      = 255;
                ::arData::MarkerTL::sptr markerTL = comp->at< ::arData::MarkerTL >(tlPair.first);
                SPTR(::arData::MarkerTL::BufferType) trackerObject;

                for(unsigned int markerPosition = 0; markerPosition < tlPair.second.size(); ++markerPosition)
                {
                    for (unsigned int i = 0; i < detectedMarkers.size(); i++)
                    {
                        if (detectedMarkers[i].id == tlPair.second[markerPosition])
                        {
                            if(m_debugMarkers)
                            {
                                detectedMarkers[i].draw(inImage,cvScalar(color[0],color[1],color[2],255),2);
                            }

                            //Push matrix
                            float markerBuffer[8];
                            for (size_t j = 0; j < 4; ++j)
                            {
                                markerBuffer[j*2]     = detectedMarkers[i][j].x;
                                markerBuffer[j*2 + 1] = detectedMarkers[i][j].y;
                            }

                            if(trackerObject == NULL)
                            {
                                trackerObject = markerTL->createBuffer(timestamp);
                                markerTL->pushObject(trackerObject);
                            }

                            trackerObject->setElement(markerBuffer, markerPosition);
                        }

                    }
                }

                //Notify
                if(trackerObject != NULL)
                {
                    ::extData::TimeLine::ObjectPushedSignalType::sptr sig;
                    sig = markerTL->signal< ::extData::TimeLine::ObjectPushedSignalType >(
                        ::extData::TimeLine::s_OBJECT_PUSHED_SIG );
                    sig->asyncEmit(timestamp);
                }

                ++index;
                if(index >= 3)
                {
                    index = 0;
                }
            }
            //Emit
            m_sigDetectionDone->asyncEmit(timestamp);
        }
    }
}

//-----------------------------------------------------------------------------

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

//-----------------------------------------------------------------------------

void SArucoTracker::setBlockSize(double size)
{
    m_blockSize = size;
}

//-----------------------------------------------------------------------------

void SArucoTracker::setConstant(double constant)
{
    m_constant = constant;
}

//-----------------------------------------------------------------------------

void SArucoTracker::setBorderWidth(double borderWidth)
{
    m_borderWidth = borderWidth;
}

//-----------------------------------------------------------------------------

void SArucoTracker::setPatternWidth(double patternWidth)
{
    m_patternWidth = patternWidth;
}

//-----------------------------------------------------------------------------

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
        //?
    }
}

//-----------------------------------------------------------------------------

void SArucoTracker::setSpeed(unsigned int value)
{
    if(value <= 3)
    {
        m_speed = value;
    }

}

//-----------------------------------------------------------------------------

void SArucoTracker::displayTags(bool b)
{
    if(b)
    {
        m_debugMarkers = true;
    }
    else
    {
        m_debugMarkers = false;
    }
}
} // namespace trackerAruco

