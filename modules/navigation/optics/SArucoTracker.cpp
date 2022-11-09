/************************************************************************
 *
 * Copyright (C) 2014-2022 IRCAD France
 * Copyright (C) 2014-2021 IHU Strasbourg
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

// cspell:ignore NOLINTNEXTLINE

#include "SArucoTracker.hpp"

#include <core/com/Signal.hxx>
#include <core/com/Slots.hxx>

#include <io/opencv/Camera.hpp>
#include <io/opencv/FrameTL.hpp>
#include <io/opencv/Image.hpp>

#include <boost/foreach.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/tokenizer.hpp>

#include <opencv2/calib3d.hpp>
#include <opencv2/core.hpp>
#include <opencv2/opencv.hpp>

//-----------------------------------------------------------------------------

namespace sight::module::navigation::optics
{

//-----------------------------------------------------------------------------

const core::com::Signals::SignalKeyType SArucoTracker::s_DETECTION_DONE_SIG  = "detectionDone";
const core::com::Signals::SignalKeyType SArucoTracker::s_MARKER_DETECTED_SIG = "markerDetected";

const core::com::Slots::SlotKeyType SArucoTracker::s_SET_PARAMETER_SLOT = "setParameter";

//-----------------------------------------------------------------------------

SArucoTracker::SArucoTracker() noexcept :
    m_sigDetectionDone(newSignal<DetectionDoneSignalType>(s_DETECTION_DONE_SIG))
{
    newSignal<MarkerDetectedSignalType>(s_MARKER_DETECTED_SIG);

    newSlot(s_SET_PARAMETER_SLOT, &SArucoTracker::setParameter, this);

    // Initialize detector parameters
    m_detectorParams = cv::aruco::DetectorParameters::create();

    // We need to tweak some parameters to adjust detection in our cases.
    //minimum distance of any corner to the image border for detected markers (in pixels) (default 3)
    m_detectorParams->minDistanceToBorder = 1;

    // minimum mean distance beetween two marker corners to be considered
    // similar, so that the smaller one is removed.
    // The rate is relative to the smaller perimeter of the two markers (default 0.05).
    m_detectorParams->minMarkerDistanceRate = 0.01;

    // corner refinement method. (CORNER_REFINE_NONE, no refinement. CORNER_REFINE_SUBPIX,
    // do subpixel refinement.)
    m_detectorParams->cornerRefinementMethod = cv::aruco::CornerRefineMethod::CORNER_REFINE_SUBPIX;

    // For now only original aruco markers are used
    m_dictionary = cv::aruco::Dictionary::get(cv::aruco::DICT_ARUCO_ORIGINAL);
}

//-----------------------------------------------------------------------------

SArucoTracker::~SArucoTracker() noexcept =
    default;

//-----------------------------------------------------------------------------

service::IService::KeyConnectionsMap SArucoTracker::getAutoConnections() const
{
    KeyConnectionsMap connections;

    connections.push(s_FRAME_INOUT, data::Object::s_MODIFIED_SIG, s_UPDATE_SLOT);
    connections.push(s_FRAME_INOUT, data::Image::s_BUFFER_MODIFIED_SIG, s_UPDATE_SLOT);

    return connections;
}

//-----------------------------------------------------------------------------

void SArucoTracker::configuring()
{
    this->service::ITracker::configuring();

    const auto config = this->getConfiguration();

    const auto& trackCfg = config.get_child("track");

    // NOLINTNEXTLINE(bugprone-branch-clone)
    BOOST_FOREACH(const auto& elt, trackCfg.equal_range("marker"))
    {
        const auto& cfg         = elt.second;
        const auto markersIDStr = cfg.get<std::string>("<xmlattr>.id");
        boost::tokenizer<> tok(markersIDStr);
        MarkerIDType markersID;
        for(const auto& it : tok)
        {
            const int id = boost::lexical_cast<int>(it);
            markersID.push_back(id);
        }

        m_markers.push_back(markersID);
    }

    // Get the debug markers flag
    m_debugMarkers = config.get<bool>("debugMarkers", false);

    // Do corner refinement ?
    const bool doCornerRefinement = config.get<bool>("cornerRefinement", true);
    m_detectorParams->cornerRefinementMethod = (doCornerRefinement
                                                ? cv::aruco::CornerRefineMethod::CORNER_REFINE_NONE
                                                : cv::aruco::CornerRefineMethod::CORNER_REFINE_SUBPIX);
}

//-----------------------------------------------------------------------------

void SArucoTracker::starting()
{
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
    // When working with a frame (newest design), we do not rely on the timestamp
    // So we can just send the current one.
    // When removing timelines from the service then we could get rid of it
    auto timestamp = core::HiResClock::getTimeInMilliSec();
    this->tracking(timestamp);
}

//-----------------------------------------------------------------------------

void SArucoTracker::tracking(core::HiResClock::HiResClockType& timestamp)
{
    if(!m_isInitialized)
    {
        const auto arCam = m_camera.lock();

        std::tie(m_cameraParams.intrinsic, m_cameraParams.size, m_cameraParams.distorsion) =
            io::opencv::Camera::copyToCv(arCam.get_shared());

        m_isInitialized = true;
    }

    cv::Mat inImage;
    auto frame = m_frame.lock();
    if(frame)
    {
        inImage = io::opencv::Image::moveToCv(frame.get_shared());
    }

    if(!inImage.empty())
    {
        // Check number of components of image.
        const auto nbOfComponents = inImage.channels();

        cv::Mat grey;
        cv::Mat bgr;

        if(nbOfComponents == 4) // RGBA or BGRA.
        {
            cv::cvtColor(inImage, grey, cv::COLOR_BGRA2GRAY);
        }
        else if(nbOfComponents == 3) // RGB or BGR.
        {
            cv::cvtColor(inImage, grey, cv::COLOR_BGR2GRAY);
        }
        else if(nbOfComponents == 1) // Grey level.
        {
            grey = inImage;
        }
        // Discard "exotic" values of components (0, 2, > 4).
        else
        {
            SIGHT_ERROR(
                "Invalid number of components ( " + std::to_string(nbOfComponents) + " ) for : '"
                << s_FRAME_INOUT << "' (accepted values are 1, 3 or 4). "
            );

            return;
        }

        bool foundMarker = false;
        std::vector<std::vector<cv::Point2f> > detectedMarkers;
        std::vector<int> detectedMarkersIds;

        cv::Mat undistortGrey;
        {
            const auto arCam = m_camera.lock();
            if(arCam->getIsCalibrated())
            {
                cv::undistort(grey, undistortGrey, m_cameraParams.intrinsic, m_cameraParams.distorsion);
            }
            else
            {
                undistortGrey = grey;
            }
        }

        // Ok, let's detect
        cv::aruco::detectMarkers(
            undistortGrey,
            m_dictionary,
            detectedMarkers,
            detectedMarkersIds,
            m_detectorParams,
            cv::noArray()
        );

        //Note: This draws all detected markers
        if(m_debugMarkers)
        {
            if(nbOfComponents == 4) // RGBA or BGRA.
            {
                // since drawDetectedMarkers does not handle 4 channels cv::mat
                cv::cvtColor(inImage, bgr, cv::COLOR_BGRA2BGR);
                cv::aruco::drawDetectedMarkers(bgr, detectedMarkers, detectedMarkersIds);
                cv::cvtColor(bgr, inImage, cv::COLOR_BGR2BGRA);
            }
            // If nbOfComponents == 1 or == 3 it's ok.
            // It is useless to test other values since "wrong" number of components has previously been discarded.
            else
            {
                cv::aruco::drawDetectedMarkers(inImage, detectedMarkers, detectedMarkersIds);
            }
        }

        std::size_t tagTLIndex = 0;
        for(const auto& markersID : m_markers)
        {
            unsigned int markerPosition = 0;
            for(const auto& markerID : markersID)
            {
                for(unsigned int i = 0 ; i < detectedMarkersIds.size() ; i++)
                {
                    if(detectedMarkersIds[i] == markerID)
                    {
                        foundMarker = true;

                        // Push matrix
                        auto markerMap = m_markerMap[tagTLIndex].lock();
                        SIGHT_ASSERT("Marker map not found", markerMap);

                        data::MarkerMap::MarkerType marker;
                        marker.resize(4);
                        for(std::size_t j = 0 ; j < 4 ; ++j)
                        {
                            marker[j][0] = detectedMarkers[i][j].x;
                            marker[j][1] = detectedMarkers[i][j].y;
                        }

                        markerMap->setMarker(std::to_string(markerID), marker);
                    }
                }

                ++markerPosition;
            }

            auto markerMap = m_markerMap[tagTLIndex].lock();
            // Always send the signal even if we did not find anything.
            // This allows to keep updating the whole processing pipeline.
            auto sig = markerMap->signal<data::Object::ModifiedSignalType>(data::Object::s_MODIFIED_SIG);
            sig->asyncEmit();

            this->signal<MarkerDetectedSignalType>(s_MARKER_DETECTED_SIG)->asyncEmit(foundMarker);

            ++tagTLIndex;
        }

        // Emit
        m_sigDetectionDone->asyncEmit(timestamp);
    }
}

//-----------------------------------------------------------------------------

void SArucoTracker::setParameter(sight::ui::base::parameter_t _val, std::string _key)
{
    if(_key == "adaptiveThreshWinSizeMin")
    {
        static const int s_ADAPTIVE_THRESH_WIN_SIZE_MIN_VALUE = 3;
        int val                                               = std::get<int>(_val);
        if(m_detectorParams->adaptiveThreshWinSizeMin < s_ADAPTIVE_THRESH_WIN_SIZE_MIN_VALUE)
        {
            SIGHT_ERROR("Tried to set adaptiveThreshWinSizeMin < 3, let it set to 3");
            val = s_ADAPTIVE_THRESH_WIN_SIZE_MIN_VALUE;
        }

        if(val >= m_detectorParams->adaptiveThreshWinSizeMax)
        {
            val = m_detectorParams->adaptiveThreshWinSizeMax - 1;
            SIGHT_ERROR("Tried to set adaptiveThreshWinSizeMin > adaptiveThreshWinSizeMax, let it set to " << val);
        }

        m_detectorParams->adaptiveThreshWinSizeMin = val;
    }
    else if(_key == "adaptiveThreshWinSizeMax")
    {
        int val = std::get<int>(_val);
        if(m_detectorParams->adaptiveThreshWinSizeMin >= val)
        {
            val = m_detectorParams->adaptiveThreshWinSizeMin + 1;
            SIGHT_ERROR("Tried to set adaptiveThreshWinSizeMax < adaptiveThreshWinSizeMin, let it set to " << val);
        }

        m_detectorParams->adaptiveThreshWinSizeMax = val;
    }
    else if(_key == "adaptiveThreshWinSizeStep")
    {
        m_detectorParams->adaptiveThreshWinSizeStep = std::get<int>(_val);
    }
    else if(_key == "minDistanceToBorder")
    {
        m_detectorParams->minDistanceToBorder = std::get<int>(_val);
    }
    else if(_key == "cornerRefinementWinSize")
    {
        m_detectorParams->cornerRefinementWinSize = std::get<int>(_val);
    }
    else if(_key == "cornerRefinementMaxIterations")
    {
        int val = std::get<int>(_val);
        if(val <= 0)
        {
            val = 1;
            SIGHT_ERROR("Tried to set cornerRefinementMaxIterations <=0, let it set to " << val);
        }

        m_detectorParams->cornerRefinementMaxIterations = val;
    }
    else if(_key == "markerBorderBits")
    {
        m_detectorParams->markerBorderBits = std::get<int>(_val);
    }
    else if(_key == "perspectiveRemovePixelPerCell")
    {
        m_detectorParams->perspectiveRemovePixelPerCell = std::get<int>(_val);
    }
    else if(_key == "adaptiveThreshConstant")
    {
        m_detectorParams->adaptiveThreshConstant = std::get<double>(_val);
    }
    else if(_key == "minMarkerPerimeterRate")
    {
        m_detectorParams->minMarkerPerimeterRate = std::get<double>(_val);
    }
    else if(_key == "maxMarkerPerimeterRate")
    {
        m_detectorParams->maxMarkerPerimeterRate = std::get<double>(_val);
    }
    else if(_key == "polygonalApproxAccuracyRate")
    {
        m_detectorParams->polygonalApproxAccuracyRate = std::get<double>(_val);
    }
    else if(_key == "minCornerDistanceRate")
    {
        m_detectorParams->minCornerDistanceRate = std::get<double>(_val);
    }
    else if(_key == "minMarkerDistanceRate")
    {
        m_detectorParams->minMarkerDistanceRate = std::get<double>(_val);
    }
    else if(_key == "cornerRefinementMinAccuracy")
    {
        double val = std::get<double>(_val);
        if(val <= 0.)
        {
            val = 0.01;
            SIGHT_ERROR("Tried to set cornerRefinementMinAccuracy <=0, let it set to " << val);
        }

        m_detectorParams->cornerRefinementMinAccuracy = val;
    }
    else if(_key == "perspectiveRemoveIgnoredMarginPerCell")
    {
        m_detectorParams->perspectiveRemoveIgnoredMarginPerCell = std::get<double>(_val);
    }
    else if(_key == "maxErroneousBitsInBorderRate")
    {
        m_detectorParams->maxErroneousBitsInBorderRate = std::get<double>(_val);
    }
    else if(_key == "minOtsuStdDev")
    {
        m_detectorParams->minOtsuStdDev = std::get<double>(_val);
    }
    else if(_key == "errorCorrectionRate")
    {
        m_detectorParams->errorCorrectionRate = std::get<double>(_val);
    }
    else if(_key == "debugMode")
    {
        m_debugMarkers = std::get<bool>(_val);
    }
    else if(_key == "corner")
    {
        if(std::get<bool>(_val))
        {
            m_detectorParams->cornerRefinementMethod = cv::aruco::CornerRefineMethod::CORNER_REFINE_SUBPIX;
        }
        else
        {
            m_detectorParams->cornerRefinementMethod = cv::aruco::CornerRefineMethod::CORNER_REFINE_NONE;
        }
    }
    else
    {
        SIGHT_ERROR("The slot key : '" + _key + "' is not handled");
    }
}

//-----------------------------------------------------------------------------

} // namespace sight::module::navigation::optics
