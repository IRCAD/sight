/************************************************************************
 *
 * Copyright (C) 2014-2023 IRCAD France
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

#include "aruco_tracker.hpp"

#include <core/com/signal.hxx>
#include <core/com/slots.hxx>

#include <io/opencv/camera.hpp>
#include <io/opencv/frame_tl.hpp>
#include <io/opencv/image.hpp>

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

const core::com::signals::key_t aruco_tracker::DETECTION_DONE_SIG  = "detectionDone";
const core::com::signals::key_t aruco_tracker::MARKER_DETECTED_SIG = "markerDetected";

const core::com::slots::key_t aruco_tracker::SET_PARAMETER_SLOT = "setParameter";

//-----------------------------------------------------------------------------

aruco_tracker::aruco_tracker() noexcept :
    m_sigDetectionDone(new_signal<detection_done_signal_t>(DETECTION_DONE_SIG))
{
    new_signal<marker_detected_signal_t>(MARKER_DETECTED_SIG);

    new_slot(SET_PARAMETER_SLOT, &aruco_tracker::setParameter, this);

    // Initialize detector parameters
    m_detectorParams = cv::makePtr<cv::aruco::DetectorParameters>();

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
    m_dictionary =
        cv::makePtr<cv::aruco::Dictionary>(cv::aruco::getPredefinedDictionary(cv::aruco::DICT_ARUCO_ORIGINAL));
}

//-----------------------------------------------------------------------------

aruco_tracker::~aruco_tracker() noexcept =
    default;

//-----------------------------------------------------------------------------

service::connections_t aruco_tracker::auto_connections() const
{
    connections_t connections;

    connections.push(s_FRAME_INOUT, data::object::MODIFIED_SIG, service::slots::UPDATE);
    connections.push(s_FRAME_INOUT, data::image::BUFFER_MODIFIED_SIG, service::slots::UPDATE);

    return connections;
}

//-----------------------------------------------------------------------------

void aruco_tracker::configuring()
{
    this->service::tracker::configuring();

    const auto config = this->get_config();

    const auto& track_cfg = config.get_child("track");

    // NOLINTNEXTLINE(bugprone-branch-clone)
    BOOST_FOREACH(const auto& elt, track_cfg.equal_range("marker"))
    {
        const auto& cfg           = elt.second;
        const auto markers_id_str = cfg.get<std::string>("<xmlattr>.id");
        boost::tokenizer<> tok(markers_id_str);
        marker_id_t markers_id;
        for(const auto& it : tok)
        {
            const int id = boost::lexical_cast<int>(it);
            markers_id.push_back(id);
        }

        m_markers.push_back(markers_id);
    }

    // Get the debug markers flag
    m_debugMarkers = config.get<bool>("debugMarkers", false);

    // Do corner refinement ?
    const bool do_corner_refinement = config.get<bool>("cornerRefinement", true);
    m_detectorParams->cornerRefinementMethod = (do_corner_refinement
                                                ? cv::aruco::CornerRefineMethod::CORNER_REFINE_NONE
                                                : cv::aruco::CornerRefineMethod::CORNER_REFINE_SUBPIX);
}

//-----------------------------------------------------------------------------

void aruco_tracker::starting()
{
    m_isTracking = true;
}

//-----------------------------------------------------------------------------

void aruco_tracker::stopping()
{
    m_isInitialized = false;
    m_isTracking    = false;
}

//-----------------------------------------------------------------------------

void aruco_tracker::updating()
{
    // When working with a frame (newest design), we do not rely on the timestamp
    // So we can just send the current one.
    // When removing timelines from the service then we could get rid of it
    auto timestamp = core::hires_clock::get_time_in_milli_sec();
    this->tracking(timestamp);
}

//-----------------------------------------------------------------------------

void aruco_tracker::tracking(core::hires_clock::type& _timestamp)
{
    if(!m_isInitialized)
    {
        const auto ar_cam = m_camera.lock();

        std::tie(m_cameraParams.intrinsic, m_cameraParams.size, m_cameraParams.distorsion) =
            io::opencv::camera::copyToCv(ar_cam.get_shared());

        m_isInitialized = true;
    }

    cv::Mat in_image;
    auto frame = m_frame.lock();
    if(frame)
    {
        in_image = io::opencv::image::move_to_cv(frame.get_shared());
    }

    if(!in_image.empty())
    {
        // Check number of components of image.
        const auto nb_of_components = in_image.channels();

        cv::Mat grey;
        cv::Mat bgr;

        if(nb_of_components == 4) // RGBA or BGRA.
        {
            cv::cvtColor(in_image, grey, cv::COLOR_BGRA2GRAY);
        }
        else if(nb_of_components == 3) // RGB or BGR.
        {
            cv::cvtColor(in_image, grey, cv::COLOR_BGR2GRAY);
        }
        else if(nb_of_components == 1) // Grey level.
        {
            grey = in_image;
        }
        // Discard "exotic" values of components (0, 2, > 4).
        else
        {
            SIGHT_ERROR(
                "Invalid number of components ( " + std::to_string(nb_of_components) + " ) for : '"
                << s_FRAME_INOUT << "' (accepted values are 1, 3 or 4). "
            );

            return;
        }

        bool found_marker = false;
        std::vector<std::vector<cv::Point2f> > detected_markers;
        std::vector<int> detected_markers_ids;

        cv::Mat undistort_grey;
        {
            const auto ar_cam = m_camera.lock();
            if(ar_cam->getIsCalibrated())
            {
                cv::undistort(grey, undistort_grey, m_cameraParams.intrinsic, m_cameraParams.distorsion);
            }
            else
            {
                undistort_grey = grey;
            }
        }

        // Ok, let's detect
        cv::aruco::detectMarkers(
            undistort_grey,
            m_dictionary,
            detected_markers,
            detected_markers_ids,
            m_detectorParams,
            cv::noArray()
        );

        //Note: This draws all detected markers
        if(m_debugMarkers)
        {
            if(nb_of_components == 4) // RGBA or BGRA.
            {
                // since drawDetectedMarkers does not handle 4 channels cv::mat
                cv::cvtColor(in_image, bgr, cv::COLOR_BGRA2BGR);
                cv::aruco::drawDetectedMarkers(bgr, detected_markers, detected_markers_ids);
                cv::cvtColor(bgr, in_image, cv::COLOR_BGR2BGRA);
            }
            // If nbOfComponents == 1 or == 3 it's ok.
            // It is useless to test other values since "wrong" number of components has previously been discarded.
            else
            {
                cv::aruco::drawDetectedMarkers(in_image, detected_markers, detected_markers_ids);
            }
        }

        std::size_t tag_tl_index = 0;
        for(const auto& markers_id : m_markers)
        {
            for(const auto& marker_id : markers_id)
            {
                for(unsigned int i = 0 ; i < detected_markers_ids.size() ; i++)
                {
                    if(detected_markers_ids[i] == marker_id)
                    {
                        found_marker = true;

                        // Push matrix
                        auto marker_map = m_markerMap[tag_tl_index].lock();
                        SIGHT_ASSERT("Marker map not found", marker_map);

                        data::marker_map::marker_t marker;
                        marker.resize(4);
                        for(std::size_t j = 0 ; j < 4 ; ++j)
                        {
                            marker[j][0] = detected_markers[i][j].x;
                            marker[j][1] = detected_markers[i][j].y;
                        }

                        marker_map->setMarker(std::to_string(marker_id), marker);
                    }
                }
            }

            auto marker_map = m_markerMap[tag_tl_index].lock();
            // Always send the signal even if we did not find anything.
            // This allows to keep updating the whole processing pipeline.
            auto sig = marker_map->signal<data::object::modified_signal_t>(data::object::MODIFIED_SIG);
            sig->async_emit();

            this->signal<marker_detected_signal_t>(MARKER_DETECTED_SIG)->async_emit(found_marker);

            ++tag_tl_index;
        }

        // Emit
        m_sigDetectionDone->async_emit(_timestamp);
    }
}

//-----------------------------------------------------------------------------

void aruco_tracker::setParameter(sight::ui::parameter_t _val, std::string _key)
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
