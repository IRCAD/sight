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
 * - \b tracking(timestamp): Slot called to execute the tracking on a timeline data and returns the results in an
 * image.
 * - \b startTracking(): Slot called when the user wants to start tracking
 * - \b stopTracking(): Slot called when the user wants to stop tracking
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
 * - \b timeline [::arData::FrameTL]: Timeline of frames where pattern detection will be computed.
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

    /**
     * @brief process method takes in an image, detects the patterns, computes the pose,
     * uses the chessboard features to solve ambiguity between 2 possible positions,
     * and finally draws the result on an output image.
     *
     * @param out_img output image
     */
    HYBRIDMARKERTRACKER_API void process(::cv::Mat& out_img);

    /**
     * @brief readSettings method reads a filename and sets the member variables
     *
     * @param filename input file name
     */
    HYBRIDMARKERTRACKER_API void readSettings(std::string filename);

protected:

    HYBRIDMARKERTRACKER_API ::fwServices::IService::KeyConnectionsMap getAutoConnections() const override;

    /**
     * @brief Configuring method: This method does nothing
     */
    HYBRIDMARKERTRACKER_API void configuring() override;

    /**
     * @brief Starting method: This method is used to initialize the service by reading the settings.
     */
    HYBRIDMARKERTRACKER_API void starting() override;

    /**
     * @brief Stopping method: This method is used to stop the service.
     */
    HYBRIDMARKERTRACKER_API void stopping() override;

    /**
     * @brief Updating method: This method does nothing.
     */
    HYBRIDMARKERTRACKER_API void updating() override;

    /// IPPE Pose solver
    IPPE::PoseSolver ippeSolver;

    /**
     * @brief calculateCorrectPose method is used to compute the correct pose between two possible solutions.
     *
     * @param rvec1 rotation vector of the first pose as an input
     * @param tvec1 translation vector of the first pose as an input
     * @param rvec2 rotation vector of the second pose as an input
     * @param tvec2 translation vector of the second pose as an input
     * @param rvec rotation vector of the correct pose as an output
     * @param tvec translation vector of the correct pose as an output
     */
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

    /// Pattern tracker
    TrackerCurvedot* m_tracker;

    /// Current pose
    ::cv::Mat m_currentcHp;

    /// Tracked image
    ::cv::Mat m_imgTrack;

    /// Camera related parameters
    ::cv::Size m_camImgSize;

    /**
     * @brief errorDistPoints method computes the distance error between 2 points
     *
     * @param pts_d detection points
     * @param pts points to be compared with 'pts_d'
     * @param max_dist maximum distance b/w a correspondence
     */
    cv::Vec2f errorDistPoints(const std::vector< ::cv::Point2f >& pts_d,
                              const std::vector< ::cv::Point2f >& pts_1,
                              const std::vector< ::cv::Point2f >& pts_2,
                              const double max_dist_sq);
    // Draws rectangles
    void drawRect(const ::cv::Mat& cHp, ::cv::Mat& img, ::cv::Scalar color = ::cv::Scalar(255, 0, 0));

    /// Camera Matrix
    ::cv::Mat m_cameraMatrix;
    /// Distortion coefficient Matrix
    ::cv::Mat m_distCoeffs;

    /// The size of the marker used for tracking
    ::cv::Size m_symboardSize;

    /// The size of a square in millimeter
    float m_squareSize;

    /// The size of the asymetric pattern in millimeters
    float m_asymSquareSize;
    /// The size of the symetric pattern (width and height) in millimeters
    ::cv::Point2f m_symSquareSize;

    /// The radius (millimeter) of cylinder the curved marker is attached on
    float m_radius;

    /// Distance from the center line to chess line in millimeters
    float m_chessDistCenter;

    /// Interval between chess in millimeters
    float m_chessInterval;

    /// Size of input image
    ::cv::Size m_imgSize;

    /// Middle pattern model points
    std::vector< ::cv::Point3f > m_trackMidPatternPoints;
    /// Top Pattern model points
    std::vector< ::cv::Point3f > m_trackTopPatternPoints;
    /// Bottom Pattern model points
    std::vector< ::cv::Point3f > m_trackBotPatternPoints;
    /// Chessboard Top Pattern model points
    std::vector< ::cv::Point3f > m_trackChessTopPatternPoint;
    /// Chessboard Middle Pattern model points
    std::vector< ::cv::Point3f > m_trackChessMidPatternPoint;
    /// Chessboard Bottom Pattern model points
    std::vector< ::cv::Point3f > m_trackChessBotPatternPoint;

    /// Check if output images are initialized
    bool m_imagesInitialized{ false };
};

} // namespace hybridMarkerTracker