/************************************************************************
 *
 * Copyright (C) 2019 IRCAD France
 * Copyright (C) 2019 IHU Strasbourg
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

#pragma once

#include "hybridMarkerTracker/config.hpp"

#include <arServices/ITracker.hpp>

#include <fwServices/macros.hpp>

#include <IPPE/ippe.h>

#include <opencv2/highgui.hpp>

#include <tracker_curvedot.h>
#include <tracker_keydot.h>
namespace hybridMarkerTracker
{
class HYBRIDMARKERTRACKER_CLASS_API SHybridMarkerTracker : public ::arServices::ITracker
{
public:
    fwCoreServiceClassDefinitionsMacro((SHybridMarkerTracker)(arServices::ITracker))

//    constructor
    HYBRIDMARKERTRACKER_API SHybridMarkerTracker() noexcept;
//    destructor
    HYBRIDMARKERTRACKER_API virtual ~SHybridMarkerTracker() noexcept;

//    Start process timer
    HYBRIDMARKERTRACKER_API void process(const ::cv::Mat& img, ::cv::Mat& out_img);

    ::cv::Mat current_cHp;
    HYBRIDMARKERTRACKER_API void readSettings(std::string filename);

    // Downsample Scale
    unsigned int img_scale;
//    pattern tracker
    Tracker* tracker;

protected:

    HYBRIDMARKERTRACKER_API ::fwServices::IService::KeyConnectionsMap getAutoConnections() const override;

//    to configure the service
    HYBRIDMARKERTRACKER_API void configuring() override;

//    to start the service
    HYBRIDMARKERTRACKER_API void starting() override;

//    to stop the service
    HYBRIDMARKERTRACKER_API void stopping() override;

//    to update the service
    HYBRIDMARKERTRACKER_API void updating() override;

//    IPPE Pose solver
    IPPE::PoseSolver ippe_solver;

//    Disambiguate pose by using chess line
    HYBRIDMARKERTRACKER_API void calculate_correct_pose(
        ::cv::InputArray rvec1, ::cv::InputArray tvec1,
        ::cv::InputArray rvec2, ::cv::InputArray tvec2,
        const std::vector< ::cv::Point3f >& pts_3d,
        ::cv::OutputArray rvec, ::cv::OutputArray tvec
        );
//    Detect marker
    HYBRIDMARKERTRACKER_API virtual void tracking(::fwCore::HiResClock::HiResClockType& timestamp) override;
private:

    ::cv::Mat m_img_track;

    // --- Camera related parameters ---
    ::cv::Size cam_img_size;

    //CameraCalibration mCalibration;

    // function: error to points
    // pts_d: detection points
    // pts: points to be compared with 'pts_d'
    // max_dist: maximum distance b/w a correspondence
    cv::Vec2f error_dist_points(const std::vector< ::cv::Point2f >& pts_d,
                                const std::vector< ::cv::Point2f >& pts_1,
                                const std::vector< ::cv::Point2f >& pts_2,
                                const double max_dist_sq);

    void draw_rect(const ::cv::Mat& cHp, ::cv::Mat& img, ::cv::Scalar color = ::cv::Scalar(255, 0, 0));

    // Read configuration file
    ::cv::FileStorage fs;

    ::cv::Mat cameraMatrix;
    ::cv::Mat distCoeffs;

    // Settings
    std::string patternToUse;   // HYBRID or CIRCULAR

    ::cv::Size cirboardSize;        // The size of the board -> Number of items by width and height

    float cirSize;              // The size of the circular board in mm

    ::cv::Size symboardSize;        // The size of the marker used for tracking

    float squareSize;           // The size of a square in your defined unit (point, millimeter,etc).

    float asymSquareSize;

    ::cv::Point2f symSquareSize;

    float radius;               // The radius (millimeter) of cylinder the curved marker is attached on

    float Chess_Dist_Center;    // Distance from the center line to chess line

    float Chess_Interval;       // Interval between chess

    ::cv::Size img_size;            // Size of input image

    // Pattern model points
    std::vector< ::cv::Point3f > trackMidPatternPoints;
    std::vector< ::cv::Point3f > trackTopPatternPoints;
    std::vector< ::cv::Point3f > trackBotPatternPoints;
    std::vector< ::cv::Point3f > trackChessTopPatternPoint;
    std::vector< ::cv::Point3f > trackChessMidPatternPoint;
    std::vector< ::cv::Point3f > trackChessBotPatternPoint;

    std::vector< ::cv::Point3f > trackCirPatternPoint;  // Circular-dot pattern

    /// Check if output images are initialized
    bool m_imagesInitialized;
};

} // namespace hybridMarkerTracker