/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2014-2018.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "trackerAruco/SArucoTracker.hpp"

#include <arData/Camera.hpp>
#include <arData/FrameTL.hpp>
#include <arData/MarkerTL.hpp>

#include <fwCom/Signal.hxx>
#include <fwCom/Slots.hxx>

#include <boost/foreach.hpp>
#include <boost/tokenizer.hpp>

#include <opencv2/core.hpp>
#include <opencv2/opencv.hpp>

//-----------------------------------------------------------------------------

namespace trackerAruco
{
fwServicesRegisterMacro(::arServices::ITracker, ::trackerAruco::SArucoTracker);
//-----------------------------------------------------------------------------

const ::fwCom::Signals::SignalKeyType SArucoTracker::s_DETECTION_DONE_SIG  = "detectionDone";
const ::fwCom::Signals::SignalKeyType SArucoTracker::s_MARKER_DETECTED_SIG = "markerDetected";

const ::fwCom::Slots::SlotKeyType SArucoTracker::s_SET_DOUBLE_PARAMETER_SLOT = "setDoubleParameter";
const ::fwCom::Slots::SlotKeyType SArucoTracker::s_SET_INT_PARAMETER_SLOT    = "setIntParameter";
const ::fwCom::Slots::SlotKeyType SArucoTracker::s_SET_BOOL_PARAMETER_SLOT   = "setBoolParameter";
const ::fwCom::Slots::SlotKeyType SArucoTracker::s_SET_ENUM_PARAMETER_SLOT   = "setEnumParameter";

const ::fwServices::IService::KeyType s_CAMERA_INPUT      = "camera";
const ::fwServices::IService::KeyType s_TAGTL_INOUT_GROUP = "tagTL";

//-----------------------------------------------------------------------------

SArucoTracker::SArucoTracker() noexcept :
    m_isInitialized(false),
    m_debugMarkers(false)
{
    m_sigDetectionDone = newSignal<DetectionDoneSignalType>(s_DETECTION_DONE_SIG);

    newSignal<MarkerDetectedSignalType>(s_MARKER_DETECTED_SIG);

    newSlot(s_SET_DOUBLE_PARAMETER_SLOT, &SArucoTracker::setDoubleParameter, this);
    newSlot(s_SET_INT_PARAMETER_SLOT, &SArucoTracker::setIntParameter, this);
    newSlot(s_SET_BOOL_PARAMETER_SLOT, &SArucoTracker::setBoolParameter, this);
    newSlot(s_SET_ENUM_PARAMETER_SLOT, &SArucoTracker::setEnumParameter, this);

    // initialize detector parameters
    m_detectorParams = ::cv::aruco::DetectorParameters::create();

    // We need to tweak some parameters to adjust detection in our cases.
    // minimum distance between corners for detected markers relative to its perimeter (default 0.05)
    m_detectorParams->minCornerDistanceRate = 0.01;
    // minimum mean distance beetween two marker corners to be considered
    // similar, so that the smaller one is removed.
    // The rate is relative to the smaller perimeter of the two markers (default 0.05).
    m_detectorParams->minMarkerDistanceRate = 0.01;
    //minimum distance of any corner to the image border for detected markers (in pixels) (default 3)
    m_detectorParams->minDistanceToBorder = 1;

    // corner refinement method. (CORNER_REFINE_NONE, no refinement. CORNER_REFINE_SUBPIX,
    // do subpixel refinement. CORNER_REFINE_CONTOUR use contour-Points)
    m_detectorParams->cornerRefinementMethod = ::cv::aruco::CornerRefineMethod::CORNER_REFINE_SUBPIX;

    // For now only original aruco markers are used
    m_dictionary = ::cv::aruco::Dictionary::get(::cv::aruco::DICT_ARUCO_ORIGINAL);

}
//-----------------------------------------------------------------------------

SArucoTracker::~SArucoTracker() noexcept
{
}

//-----------------------------------------------------------------------------

void SArucoTracker::configuring()
{
    this->::arServices::ITracker::configuring();

    const ::fwServices::IService::ConfigType config = this->getConfigTree();

    const ::fwServices::IService::ConfigType& cfgElement = config.get_child("config");

    const ::fwServices::IService::ConfigType& trackCfg = cfgElement.get_child("track");

    BOOST_FOREACH(const auto& elt,  trackCfg.equal_range("marker"))
    {
        const ::fwServices::IService::ConfigType& cfg = elt.second;
        const std::string markersIDStr                = cfg.get< std::string >("<xmlattr>.id");
        ::boost::tokenizer<> tok(markersIDStr);
        MarkerIDType markersID;
        for( const auto& it: tok)
        {
            const int id = ::boost::lexical_cast< int >(it);
            markersID.push_back(id);
        }
        m_markers.push_back(markersID);

    }

    // get the debug markers flag
    m_debugMarkers = config.get<bool>("debugMarkers", false);

}

//-----------------------------------------------------------------------------

void SArucoTracker::starting()
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

//-----------------------------------------------------------------------------

void SArucoTracker::stopping()
{
    m_isInitialized = false;
    m_isTracking    = false;
}

//-----------------------------------------------------------------------------

void SArucoTracker::updating()
{
}

//-----------------------------------------------------------------------------

void SArucoTracker::tracking(::fwCore::HiResClock::HiResClockType& timestamp)
{
    ::arData::FrameTL::csptr frameTL = this->getInput< ::arData::FrameTL >(s_TIMELINE_INPUT);

    if(!m_isInitialized)
    {
        ::arData::Camera::csptr arCam = this->getInput< ::arData::Camera >(s_CAMERA_INPUT);

        m_cameraParams.intrinsic  = ::cv::Mat::eye(3, 3, CV_64F);
        m_cameraParams.distorsion = ::cv::Mat::eye(4, 1, CV_64F);

        // 3x3 matrix (fx 0 cx, 0 fy cy, 0 0 1)
        m_cameraParams.intrinsic.at<double>(0, 0) = arCam->getFx();
        m_cameraParams.intrinsic.at<double>(1, 1) = arCam->getFy();
        m_cameraParams.intrinsic.at<double>(0, 2) = arCam->getCx();
        m_cameraParams.intrinsic.at<double>(1, 2) = arCam->getCy();

        // 4x1 matrix (k1,k2,p1,p2)
        for (std::uint8_t i = 0; i < 4; ++i)
        {
            m_cameraParams.distorsion.at<double>(i, 0) = arCam->getDistortionCoefficient()[i];
        }

        // size of the image
        m_cameraParams.size.width  = static_cast<int>(frameTL->getWidth());
        m_cameraParams.size.height = static_cast<int>(frameTL->getHeight());
        m_isInitialized            = true;
    }

    const CSPTR(::arData::FrameTL::BufferType) buffer = frameTL->getClosestBuffer(timestamp);

    OSLM_WARN_IF("Buffer not found with timestamp "<< timestamp, !buffer );
    if(buffer)
    {
        bool foundMarker = false;
        m_lastTimestamp = timestamp;

        const std::uint8_t* frameBuff = &buffer->getElement(0);

        std::vector<std::vector< ::cv::Point2f> > detectedMarkers;
        std::vector< int > detectedMarkersIds;

        // read the input image
        const ::cv::Size frameSize(static_cast<int>(frameTL->getWidth()),
                                   static_cast<int>(frameTL->getHeight()));

        ::cv::Mat inImage = ::cv::Mat(frameSize, CV_8UC4, (void*)frameBuff, ::cv::Mat::AUTO_STEP);

        // aruco expects a grey image and make a conversion at the beginning of the detect() method if it receives
        // a RGB image. However we have a RGBA image so we must make the conversion ourselves.
        cv::Mat grey, bgr;
        // inImage is BGRA (see constructor of ::cv::Mat)
        cv::cvtColor(inImage, grey, CV_BGRA2GRAY);

        // Ok, let's detect
        ::cv::aruco::detectMarkers(grey, m_dictionary, detectedMarkers, detectedMarkersIds, m_detectorParams,
                                   ::cv::noArray(),
                                   m_cameraParams.intrinsic, m_cameraParams.distorsion);

        //Note: This draws all detected markers
        if(m_debugMarkers)
        {
            // since drawDetectedMarkers does not handle 4 channels ::cv::mat
            ::cv::cvtColor(inImage, bgr, CV_BGRA2BGR);
            ::cv::aruco::drawDetectedMarkers(bgr, detectedMarkers, detectedMarkersIds);
            ::cv::cvtColor(bgr, inImage, CV_BGR2BGRA);
        }

        size_t tagTLIndex = 0;
        for(const auto& markersID : m_markers)
        {
            ::arData::MarkerTL::sptr markerTL =
                this->getInOut< ::arData::MarkerTL >(s_TAGTL_INOUT_GROUP, tagTLIndex);
            SPTR(::arData::MarkerTL::BufferType) trackerObject;

            unsigned int markerPosition = 0;
            for (const auto& markerID : markersID)
            {
                for (unsigned int i = 0; i < detectedMarkersIds.size(); i++)
                {
                    if (detectedMarkersIds[i] == markerID)
                    {
                        foundMarker = true;

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

            this->signal<MarkerDetectedSignalType>(s_MARKER_DETECTED_SIG)->asyncEmit(foundMarker);

            ++tagTLIndex;
        }
        // Emit
        m_sigDetectionDone->asyncEmit(timestamp);
    }
}

//-----------------------------------------------------------------------------

void SArucoTracker::setIntParameter(int _val, std::string _key)
{

    if(_key == "adaptiveThreshWinSizeMin")
    {
        m_detectorParams->adaptiveThreshWinSizeMin = _val;
    }
    else if(_key == "adaptiveThreshWinSizeMax")
    {
        m_detectorParams->adaptiveThreshWinSizeMax = _val;
    }
    else if(_key == "adaptiveThreshWinSizeStep")
    {
        m_detectorParams->adaptiveThreshWinSizeStep = _val;
    }
    else if(_key == "minDistanceToBorder")
    {
        m_detectorParams->minDistanceToBorder = _val;
    }
    else if(_key == "cornerRefinementWinSize")
    {
        m_detectorParams->cornerRefinementWinSize = _val;
    }
    else if(_key == "cornerRefinementMaxIterations")
    {
        m_detectorParams->cornerRefinementMaxIterations = _val;
    }
    else if(_key == "markerBorderBits")
    {
        m_detectorParams->markerBorderBits = _val;
    }
    else if(_key == "perspectiveRemovePixelPerCell")
    {
        m_detectorParams->perspectiveRemovePixelPerCell = _val;
    }
    else
    {
        SLM_ERROR("The slot key : '"+ _key + "' is not handled");
    }
}

//-----------------------------------------------------------------------------

void SArucoTracker::setDoubleParameter(double _val, std::string _key)
{
    if(_key == "adaptiveThreshConstant")
    {
        m_detectorParams->adaptiveThreshConstant = _val;
    }
    else if(_key == "minMarkerPerimeterRate")
    {
        m_detectorParams->minMarkerPerimeterRate = _val;
    }
    else if(_key == "maxMarkerPerimeterRate")
    {
        m_detectorParams->maxMarkerPerimeterRate = _val;
    }
    else if(_key == "polygonalApproxAccuracyRate")
    {
        m_detectorParams->polygonalApproxAccuracyRate = _val;
    }
    else if(_key == "minCornerDistanceRate")
    {
        m_detectorParams->minCornerDistanceRate = _val;
    }
    else if(_key == "minMarkerDistanceRate")
    {
        m_detectorParams->minMarkerDistanceRate = _val;
    }
    else if(_key == "cornerRefinementMinAccuracy")
    {
        m_detectorParams->cornerRefinementMinAccuracy = _val;
    }
    else if(_key == "perspectiveRemoveIgnoredMarginPerCell")
    {
        m_detectorParams->perspectiveRemoveIgnoredMarginPerCell = _val;
    }
    else if(_key == "maxErroneousBitsInBorderRate")
    {
        m_detectorParams->maxErroneousBitsInBorderRate = _val;
    }
    else if(_key == "minOtsuStdDev")
    {
        m_detectorParams->minOtsuStdDev = _val;
    }
    else if(_key == "errorCorrectionRate")
    {
        m_detectorParams->errorCorrectionRate = _val;
    }
    else
    {
        SLM_ERROR("The slot key : '"+ _key + "' is not handled");
    }
}

//-----------------------------------------------------------------------------

void SArucoTracker::setBoolParameter(bool _val, std::string _key)
{
    if(_key == "debugMode")
    {
        m_debugMarkers = _val;
    }
    else
    {
        SLM_ERROR("The slot key : '"+ _key + "' is not handled");
    }
}

//-----------------------------------------------------------------------------

void SArucoTracker::setEnumParameter(std::string _val, std::string _key)
{
    if(_key == "corner")
    {
        if(_val == "NONE" )
        {
            m_detectorParams->cornerRefinementMethod = ::cv::aruco::CornerRefineMethod::CORNER_REFINE_NONE;
        }
        else if(_val == "SUBPIX")
        {
            m_detectorParams->cornerRefinementMethod = ::cv::aruco::CornerRefineMethod::CORNER_REFINE_SUBPIX;
        }
        else if(_val == "CONTOUR")
        {
            m_detectorParams->cornerRefinementMethod = ::cv::aruco::CornerRefineMethod::CORNER_REFINE_CONTOUR;
        }
        else
        {
            SLM_ERROR("Value : '"+ _val + "' is not supported");
        }
    }
    else
    {
        SLM_ERROR("The slot key : '"+ _key + "' is not handled");
    }

}

//-----------------------------------------------------------------------------

} // namespace trackerAruco
