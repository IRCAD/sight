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
        <service uid="..." type="::hybridMarkerTracker::SHybridMarkerTracker">
            <in key="frameIn" uid="..." />
            <inout key="frame" uid="..." />
            <inout key="pose" uid="..." />
        </service>
   @endcode
 * @subsection In In
 * - \b frameIn [::fwData::Image]: input image to process tracking on
 * @subsection In-Out In-Out
 * - \b frame [::fwData::Image]: final output image with tracking information that will be displayed
 * - \b pose [::fwData::TransformationMatrix3D]: transformation matrix from the tag to the camera
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
     * @brief Updating method: call tracking with a current timestamp.
     */
    HYBRIDMARKERTRACKER_API void updating() override;

    /**
     * @brief calculateCorrectPose method is used to compute the correct pose between two possible solutions.
     *
     * @param rvec1 rotation vector of the first pose as an input
     * @param tvec1 translation vector of the first pose as an input
     * @param rvec2 rotation vector of the second pose as an input
     * @param tvec2 translation vector of the second pose as an input
     * @param pts3d 3 dimension vector
     * @param rvec rotation vector of the correct pose as an output
     * @param tvec translation vector of the correct pose as an output
     */
    HYBRIDMARKERTRACKER_API void calculateCorrectPose(
        ::cv::InputArray rvec1, ::cv::InputArray tvec1,
        ::cv::InputArray rvec2, ::cv::InputArray tvec2,
        const std::vector< ::cv::Point3f >& pts3d,
        ::cv::OutputArray rvec, ::cv::OutputArray tvec
        );

    /// Detect marker
    HYBRIDMARKERTRACKER_API virtual void tracking(::fwCore::HiResClock::HiResClockType& timestamp) override;

private:

    /**
     * @brief process method detects the patterns on m_imgTrack image, computes the pose,
     * uses the chessboard features to solve ambiguity between 2 possible positions,
     * and finally draws the tracking results.
     */
    void process();

    /**
     * @brief readSettings method reads a filename and sets the member variables
     *
     * @param filename input file name
     */
    void readSettings(const std::string& filename);

    /**
     * @brief errorDistPoints method computes the distance error between 2 points
     *
     * @param ptsDect detection points
     * @param pts1 point 1 to be compared with 'ptsDect'
     * @param pts2 point 2 to be compared with 'ptsDect'
     * @param maxDistSq maximum distance b/w a correspondence
     */
    ::cv::Vec2f errorDistPoints(const std::vector< ::cv::Point2f >& ptsDect,
                                const std::vector< ::cv::Point2f >& pts1,
                                const std::vector< ::cv::Point2f >& pts2,
                                const double maxDistSq);

    /**
     * @brief drawRect Draws rectangles
     *
     * @param cHp the estimated pose
     * @param img the image to draw on
     * @param color the color of the rectangle
     */
    void drawRect(const ::cv::Mat& cHp, ::cv::Mat& img, ::cv::Scalar color = ::cv::Scalar(255, 0, 0));

    /// IPPE Pose solver
    IPPE::PoseSolver m_ippeSolver;

    /// Pattern tracker
    TrackerCurvedot* m_tracker { nullptr };

    /// Current pose
    ::cv::Mat m_currentcHp;

    /// Tracked image
    ::cv::Mat m_imgTrack;

    /// Camera Matrix
    ::cv::Mat m_cameraMatrix;

    /// Distortion coefficient Matrix
    ::cv::Mat m_distCoeffs;

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
};

} // namespace hybridMarkerTracker