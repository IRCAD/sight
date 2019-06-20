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
/**
 * @brief   Class used to track a cylindrical hybrid marker
 *
 * @see ::arServices::ITracker
 *
 * @subsection Inherited Inherited slots (from ITracker)
 * - \b tracking(timestamp) : Slot to fills the timeline with the new positions of the grid
 * - \b startTracking() : Slot called when the user wants to start tracking
 * - \b stopTracking() : Slot called when the user wants to stop tracking
 *
 * @section XML XML Configuration
 *
 * @code{.xml}
        <service uid="..." type="::hybridMarkerTracker::SHybridMarkerTracker" >
            <inout key="timeline" uid="..." autoConnect="yes"/>
            <inout key="frame" uid="..."/>
        </service>
   @endcode
 * @subsection In-Out In-Out
 * - \b timeline [::arData::FrameTL]: camera used to display video. It is the main timeline used for the tracking.
 * - \b frame [::fwData::Image]: final output video that will be displayed
 */
class HYBRIDMARKERTRACKER_CLASS_API SHybridMarkerTracker : public ::arServices::ITracker
{
public:
    fwCoreServiceClassDefinitionsMacro((SHybridMarkerTracker)(arServices::ITracker))
    /**
     * @brief Constructor.
     */
    HYBRIDMARKERTRACKER_API SHybridMarkerTracker() noexcept;

    /**
     * @brief Destructor.
     */
    HYBRIDMARKERTRACKER_API virtual ~SHybridMarkerTracker() noexcept;

    /// Start process timer
    HYBRIDMARKERTRACKER_API void process(const ::cv::Mat& img, ::cv::Mat& out_img);

    HYBRIDMARKERTRACKER_API void readSettings(std::string filename);

protected:

    HYBRIDMARKERTRACKER_API ::fwServices::IService::KeyConnectionsMap getAutoConnections() const override;

    /**
     * @brief Configuring method : This method does nothing
     */
    HYBRIDMARKERTRACKER_API void configuring() override;

    /**
     * @brief Starting method : This method is used to initialize the service by reading the settings.
     */
    HYBRIDMARKERTRACKER_API void starting() override;

    /**
     * @brief Stopping method : This method is used to stop the service.
     */
    HYBRIDMARKERTRACKER_API void stopping() override;

    /**
     * @brief Updating method : This method does nothing.
     */
    HYBRIDMARKERTRACKER_API void updating() override;

    /// IPPE Pose solver
    IPPE::PoseSolver ippeSolver;

    /// Disambiguate pose by using chess line
    HYBRIDMARKERTRACKER_API void calculateCorrectPose(
        ::cv::InputArray rvec1, ::cv::InputArray tvec1,
        ::cv::InputArray rvec2, ::cv::InputArray tvec2,
        const std::vector< ::cv::Point3f >& pts_3d,
        ::cv::OutputArray rvec, ::cv::OutputArray tvec
        );
    /// Detect marker
    HYBRIDMARKERTRACKER_API virtual void tracking(::fwCore::HiResClock::HiResClockType& timestamp) override;
private:

    /// Downsample Scale
    unsigned int m_imgScale;

    /// pattern tracker
    Tracker* m_tracker;

    ::cv::Mat m_currentcHp;

    ::cv::Mat m_imgTrack;

    /// Camera related parameters
    ::cv::Size m_camImgSize;

//CameraCalibration mCalibration;

    /// function: error to points
    /// pts_d: detection points
    /// pts: points to be compared with 'pts_d'
    /// max_dist: maximum distance b/w a correspondence
    cv::Vec2f errorDistPoints(const std::vector< ::cv::Point2f >& pts_d,
                              const std::vector< ::cv::Point2f >& pts_1,
                              const std::vector< ::cv::Point2f >& pts_2,
                              const double max_dist_sq);

    void drawRect(const ::cv::Mat& cHp, ::cv::Mat& img, ::cv::Scalar color = ::cv::Scalar(255, 0, 0));

    /// Read configuration file
    ::cv::FileStorage m_fs;

    ::cv::Mat m_cameraMatrix;
    ::cv::Mat m_distCoeffs;

    /**
     * @brief The different settings parameters.
     */
    /// HYBRID or CIRCULAR
    std::string m_patternToUse;

    /// The size of the marker used for tracking
    ::cv::Size m_symboardSize;

    /// The size of a square in your defined unit (point, millimeter,etc).
    float m_squareSize;

    float m_asymSquareSize;

    ::cv::Point2f m_symSquareSize;

    /// The radius (millimeter) of cylinder the curved marker is attached on
    float m_radius;

    /// Distance from the center line to chess line
    float m_chessDistCenter;

    /// Interval between chess
    float m_chessInterval;

    /// Size of input image
    ::cv::Size m_imgSize;

    /// Pattern model points
    std::vector< ::cv::Point3f > m_trackMidPatternPoints;
    std::vector< ::cv::Point3f > m_trackTopPatternPoints;
    std::vector< ::cv::Point3f > m_trackBotPatternPoints;
    std::vector< ::cv::Point3f > m_trackChessTopPatternPoint;
    std::vector< ::cv::Point3f > m_trackChessMidPatternPoint;
    std::vector< ::cv::Point3f > m_trackChessBotPatternPoint;

    /// Check if output images are initialized
    bool m_imagesInitialized;
};

} // namespace hybridMarkerTracker