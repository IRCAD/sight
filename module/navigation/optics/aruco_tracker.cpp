/************************************************************************
 *
 * Copyright (C) 2014-2024 IRCAD France
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
const core::com::signals::key_t aruco_tracker::MARKER_DETECTED_SIG = "marker_detected";

const core::com::slots::key_t aruco_tracker::SET_PARAMETER_SLOT = "set_parameter";

//-----------------------------------------------------------------------------

aruco_tracker::aruco_tracker() noexcept :
    m_sig_detection_done(new_signal<detection_done_signal_t>(DETECTION_DONE_SIG))
{
    new_signal<marker_detected_signal_t>(MARKER_DETECTED_SIG);

    // Initialize detector parameters
    m_detector_params = cv::makePtr<cv::aruco::DetectorParameters>();

    // We need to tweak some parameters to adjust detection in our cases.
    //minimum distance of any corner to the image border for detected markers (in pixels) (default 3)
    m_detector_params->minDistanceToBorder = 1;

    // minimum mean distance beetween two marker corners to be considered
    // similar, so that the smaller one is removed.
    // The rate is relative to the smaller perimeter of the two markers (default 0.05).
    m_detector_params->minMarkerDistanceRate = 0.01;

    m_detector_params->cornerRefinementMethod = cv::aruco::CornerRefineMethod::CORNER_REFINE_SUBPIX;

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
    return {
        {FRAME_INOUT, data::object::MODIFIED_SIG, service::slots::UPDATE},
        {FRAME_INOUT, data::image::BUFFER_MODIFIED_SIG, service::slots::UPDATE}
    };
}

//-----------------------------------------------------------------------------

void aruco_tracker::configuring(const config_t& _config)
{
    this->service::tracker::configuring(_config);

    const auto& track_cfg = _config.get_child("track");

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

    // Do corner refinement ?
    m_detector_params->cornerRefinementMethod = (*m_corner_refinement
                                                 ? cv::aruco::CornerRefineMethod::CORNER_REFINE_NONE
                                                 : cv::aruco::CornerRefineMethod::CORNER_REFINE_SUBPIX);
}

//-----------------------------------------------------------------------------

void aruco_tracker::starting()
{
    m_is_tracking = true;
}

//-----------------------------------------------------------------------------

void aruco_tracker::stopping()
{
    m_is_initialized = false;
    m_is_tracking    = false;
}

//-----------------------------------------------------------------------------

void aruco_tracker::updating()
{
    // When working with a frame (newest design), we do not rely on the timestamp
    // So we can just send the current one.
    // When removing timelines from the service then we could get rid of it
    auto timestamp = core::clock::get_time_in_milli_sec();
    this->tracking(timestamp);
}

//-----------------------------------------------------------------------------

void aruco_tracker::tracking(core::clock::type& _timestamp)
{
    if(!m_is_initialized)
    {
        const auto ar_cam = m_camera.lock();

        std::tie(m_camera_params.intrinsic, m_camera_params.size, m_camera_params.distorsion) =
            io::opencv::camera::copy_to_cv(ar_cam.get_shared());

        m_is_initialized = true;
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
                << FRAME_INOUT << "' (accepted values are 1, 3 or 4). "
            );

            return;
        }

        bool found_marker = false;
        std::vector<std::vector<cv::Point2f> > detected_markers;
        std::vector<int> detected_markers_ids;

        cv::Mat undistort_grey;
        {
            const auto ar_cam = m_camera.lock();
            if(ar_cam->get_is_calibrated())
            {
                cv::undistort(grey, undistort_grey, m_camera_params.intrinsic, m_camera_params.distorsion);
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
            m_detector_params,
            cv::noArray()
        );

        //Note: This draws all detected markers
        if(*m_debug_mode)
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
                        auto marker_map = m_marker_map[tag_tl_index].lock();
                        SIGHT_ASSERT("Marker map not found", marker_map);

                        data::marker_map::marker_t marker;
                        marker.resize(4);
                        for(std::size_t j = 0 ; j < 4 ; ++j)
                        {
                            marker[j][0] = detected_markers[i][j].x;
                            marker[j][1] = detected_markers[i][j].y;
                        }

                        marker_map->set_marker(std::to_string(marker_id), marker);
                    }
                }
            }

            auto marker_map = m_marker_map[tag_tl_index].lock();
            // Always send the signal even if we did not find anything.
            // This allows to keep updating the whole processing pipeline.
            auto sig = marker_map->signal<data::object::modified_signal_t>(data::object::MODIFIED_SIG);
            sig->async_emit();

            this->signal<marker_detected_signal_t>(MARKER_DETECTED_SIG)->async_emit(found_marker);

            ++tag_tl_index;
        }

        // Emit
        m_sig_detection_done->async_emit(_timestamp);
    }
}

//-----------------------------------------------------------------------------

void aruco_tracker::on_property_set(std::string_view _key)
{
    if(_key == "adaptive_th_win_size_min")
    {
        static const int s_ADAPTIVE_THRESH_WIN_SIZE_MIN_VALUE = 3;
        int val                                               = static_cast<int>(*m_adaptive_th_win_size_min);
        if(m_detector_params->adaptiveThreshWinSizeMin < s_ADAPTIVE_THRESH_WIN_SIZE_MIN_VALUE)
        {
            SIGHT_ERROR("Tried to set adaptive_thresh_win_size_min < 3, let it set to 3");
            val = s_ADAPTIVE_THRESH_WIN_SIZE_MIN_VALUE;
        }

        if(val >= m_detector_params->adaptiveThreshWinSizeMax)
        {
            val = m_detector_params->adaptiveThreshWinSizeMax - 1;
            SIGHT_ERROR("Tried to set adaptive_thresh_win_size_min > adaptive_th_win_size_max, let it set to " << val);
        }

        m_detector_params->adaptiveThreshWinSizeMin = val;
    }
    else if(_key == "adaptive_th_win_size_max")
    {
        int val = static_cast<int>(*m_adaptive_th_win_size_max);
        if(m_detector_params->adaptiveThreshWinSizeMin >= val)
        {
            val = m_detector_params->adaptiveThreshWinSizeMin + 1;
            SIGHT_ERROR(
                "Tried to set adaptive_th_win_size_max < adaptive_thresh_win_size_min, let it set to "
                << val
            );
        }

        m_detector_params->adaptiveThreshWinSizeMax = val;
    }
    else if(_key == "adaptive_th_win_size_step")
    {
        m_detector_params->adaptiveThreshWinSizeStep = static_cast<int>(*m_adaptive_th_win_size_step);
    }
    else if(_key == "min_distance_to_border")
    {
        m_detector_params->minDistanceToBorder = static_cast<int>(*m_min_distance_to_border);
    }
    else if(_key == "corner_refinement_win_size")
    {
        m_detector_params->cornerRefinementWinSize = static_cast<int>(*m_corner_refinement_win_size);
    }
    else if(_key == "corner_refinement_max_iterations")
    {
        int val = static_cast<int>(*m_corner_refinement_max_iterations);
        if(val <= 0)
        {
            val = 1;
            SIGHT_ERROR("Tried to set cornerRefinementMaxIterations <=0, let it set to " << val);
        }

        m_detector_params->cornerRefinementMaxIterations = val;
    }
    else if(_key == "marker_border_bits")
    {
        m_detector_params->markerBorderBits = static_cast<int>(*m_marker_border_bits);
    }
    else if(_key == "perspective_remove_pixel_per_cell")
    {
        m_detector_params->perspectiveRemovePixelPerCell = static_cast<int>(*m_perspective_remove_pixel_per_cell);
    }
    else if(_key == "adaptive_th_constant")
    {
        m_detector_params->adaptiveThreshConstant = *m_adaptive_th_constant;
    }
    else if(_key == "min_marker_perimeter_rate")
    {
        m_detector_params->minMarkerPerimeterRate = *m_min_marker_perimeter_rate;
    }
    else if(_key == "max_marker_perimeter_rate")
    {
        m_detector_params->maxMarkerPerimeterRate = *m_max_marker_perimeter_rate;
    }
    else if(_key == "polygonal_approx_accuracy_rate")
    {
        m_detector_params->polygonalApproxAccuracyRate = *m_polygonal_approx_accuracy_rate;
    }
    else if(_key == "min_corner_distance_rate")
    {
        m_detector_params->minCornerDistanceRate = *m_min_corner_distance_rate;
    }
    else if(_key == "min_marker_distance_rate")
    {
        m_detector_params->minMarkerDistanceRate = *m_min_marker_distance_rate;
    }
    else if(_key == "corner_refinement_min_accuracy")
    {
        double val = *m_corner_refinement_min_accuracy;
        if(val <= 0.)
        {
            val = 0.01;
            SIGHT_ERROR("Tried to set cornerRefinementMinAccuracy <=0, let it set to " << val);
        }

        m_detector_params->cornerRefinementMinAccuracy = val;
    }
    else if(_key == "perspective_remove_ignored_margin_per_cell")
    {
        m_detector_params->perspectiveRemoveIgnoredMarginPerCell = *m_perspective_remove_ignored_margin_per_cell;
    }
    else if(_key == "max_erroneous_bits_in_border_rate")
    {
        m_detector_params->maxErroneousBitsInBorderRate = *m_max_erroneous_bits_in_border_rate;
    }
    else if(_key == "min_otsu_std_dev")
    {
        m_detector_params->minOtsuStdDev = *m_min_otsu_std_dev;
    }
    else if(_key == "error_correction_rate")
    {
        m_detector_params->errorCorrectionRate = *m_error_correction_rate;
    }
    else if(_key == "corner_refinement")
    {
        if(*m_corner_refinement)
        {
            m_detector_params->cornerRefinementMethod = cv::aruco::CornerRefineMethod::CORNER_REFINE_SUBPIX;
        }
        else
        {
            m_detector_params->cornerRefinementMethod = cv::aruco::CornerRefineMethod::CORNER_REFINE_NONE;
        }
    }
    else if(_key != "debug_mode")
    {
        SIGHT_ERROR("The slot key : '" << _key << "' is not handled");
    }
}

//-----------------------------------------------------------------------------

} // namespace sight::module::navigation::optics
