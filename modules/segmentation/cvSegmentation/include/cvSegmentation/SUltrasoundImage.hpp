/************************************************************************
 *
 * Copyright (C) 2018-2020 IRCAD France
 * Copyright (C) 2018-2020 IHU Strasbourg
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

#include "cvSegmentation/config.hpp"

#include <fwData/Image.hpp>

#include <fwMath/IntrasecTypes.hpp>
#include <fwMath/VectorFunctions.hpp>

#include <fwServices/IOperator.hpp>

#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>

namespace cvSegmentation
{

/**
 * @brief Extract the ultrasound plane and its associated information from an ultrasound video.
 *
 * @section Signals Signals
 * - \b intChanged(int, std::string): Emitted when an integer parameter changes.
 *
 * @section Slots Slots
 * - \b updateIntExtractionParameter(int , std::string): Update int parameters used for the ultrasound plane extraction.
 *   - threshold.min: Sets the threshold.
 * - \b updateDoubleExtractionParameter(double , std::string): Update double parameters used for the ultrasound plane
 * extraction.
 *   - us.depth: Sets the depth of the US image.
 * - \b changeDebugEchoPlane(bool , std::string): Update debug mode for the ultrasound plane extraction.
 *   - debugUSExtraction: Sets the debug mode.
 * - \b changeEchoShape(bool, key): Change the ultrasound shape.
 *   - shape: Sets the ultrasound shape.
 * - \b resetEchoPlane(): Reset the ultrasound plane extraction.
 *
 * @section XML XML Configuration
 *
 * @code{.xml}
   <service type="::cvSegmentation::SUltrasoundImage">
       <in key="ultrasoundImage" uid="..." />
       <inout key="extractedUltrasoundBeam" uid="..." />
   </service>
   @endcode
 *
 * @subsection In In
 * - \b ultrasoundImage [::fwData::Image](mandatory): image outputted by the ultrasound machine.
 *
 * @subsection In-Out In-Out
 * - \b extractedUltrasoundBeam [::fwData::Image](mandatory): ultrasound beam image extracted from the input.
 *
 * @subsection Configuration Configuration:
 * - \b thresholdMin (optional, default=4) : Minimum threshold used for ultrasound plane extraction.
 * - \b USDepth (optional, default=19.) : Value of depth of the US probe.
 * - \b debug (optional, default=no) : Debug mode.
 * - \b convex (optional, default=yes) : Convex shape of extracted image.
 */
class CVSEGMENTATION_CLASS_API SUltrasoundImage : public ::fwServices::IOperator
{

public:

    fwCoreServiceMacro(SUltrasoundImage, ::fwServices::IOperator)

    typedef ::fwCom::Signal< void (int, std::string) > IntegerChangedSignalType;

    /// Initialize members.
    CVSEGMENTATION_API SUltrasoundImage() noexcept;

    /// Does nothing.
    CVSEGMENTATION_API virtual ~SUltrasoundImage() noexcept;

    struct ProbeSimulationSettings
    {
        int depth          = 120;
        int width          = 90;
        int angle          = 60;
        int deltaDepth     = 50;
        size_t matrixWidth = 310;
        size_t matrixDepth = 290;
        bool simuEchoOn    = false;
        bool echoShapeOn   = false;

        fwVec3d centerPosition{0., 0., 0.};
        fwVec3d direction{1., 0., 0.};
        fwVec3d normal{0., 1., 0.};
    };
private:

    /**
     * @brief Proposals to connect service slots to associated object signals.
     * @return A map of each proposed connection.
     */
    ::fwServices::IService::KeyConnectionsMap getAutoConnections() const final;

    /// Does nothing.
    void starting() final;

    /// Does nothing.
    void stopping() final;

    /// Processes the input to detect the ultrasound's beam shape and remaps it into the output.
    void updating() final;

    /// Configures the service.
    void configuring() final;

    /**
     * @brief compute the equation of a line (ax+b).
     * @param p1 coordinates of the first point of the line.
     * @param p2 coordinates of the second point of the line.
     * @return the line equation passing through the 2 points p1 and p2.
     */
    ::cv::Vec2d computeLineEquation(const ::cv::Vec2d& p1, const ::cv::Vec2d& p2) const;

    /**
     * @brief compute intersection between two lines.
     * @param eql1 first line equation (eql1[0]x + eql1[1]).
     * @param eql2 second line equation (eql2[0]x + eql2[1]).
     * @return the coordinates of the intersection point between the two lines eql1 and eql2.
     */
    ::cv::Vec2d computeLinesIntersection(const ::cv::Vec2d& eql1, const ::cv::Vec2d& eql2) const;

    /**
     * @brief compute intersections between a circle and a line.
     * @param eql line equation (eql1[0]x + eql1[1]).
     * @param center circle center coordinates.
     * @param R circle radius.
     * @return the list of intersection points between a circle defined by a center (center) and a radius (R), and a
     * line (eql).
     */
    std::vector< ::cv::Vec2d > computeLineCircleIntersection(const ::cv::Vec2d& eql, const ::cv::Vec2d& center,
                                                             const double R) const;

    /**
     * @brief compute angle between vertical line and a line equation.
     * @param center coordinates of a point on the vertical line.
     * @param arcPoint line equation of a side of an arc.
     * @return angle (degrees) between vertical line and a line equation.
     */
    double computeArcAngle(const ::cv::Vec2d& center, const std::vector< ::cv::Vec2d>& arcPoint) const;

    /**
     * @brief check if there is data (pixel value !=0) under the arc equation define by a set of 2D points.
     * @param input image to process.
     * @param points arc points.
     * @return true if there is data under the arc equation, false otherwise.
     */
    bool isDataUnderArc(const ::cv::Mat& input, const std::vector< ::cv::Point2d>& points) const;

    /**
     * @brief extract the data from the ultrasound plane as a texture.
     * @param input image to process.
     */
    void process(const cv::Mat& input);

    /**
     * @brief extract the data from the ultrasound plane as a texture with an ultrasound shape.
     * @param input image to process.
     * @param lines equations of sides of the convex US shape to extract inside the input image.
     * @param thresh rough binary image after a threshold applied on the input image.
     * @param binaryConvex rough binary ultrasound shape of the input image.
     * @return convex probe simulation settings.
     */
    ProbeSimulationSettings processConvexShape(::cv::Mat input,
                                               std::vector< ::cv::Vec2d> lines,
                                               ::cv::Mat thresh,
                                               ::cv::Mat binaryConvex);

    /**
     * @brief extract the data from the ultrasound plane as a texture with a rectangular shape.
     * @param lines equations of the lines corresponding to rectangular sides.
     * @return rectangular probe simulation settings.
     */
    ProbeSimulationSettings processRectangularShape(std::vector< ::cv::Vec2d> lines);

    /**
     * @brief SLOT: Update integer extraction parameters used to extract the ultrasound plane.
     * @param val value to set.
     * @param key the key used to select the member to change.
     */
    void updateIntExtractionParameter(int val, std::string key);

    /**
     * @brief SLOT: Update double extraction parameters used to extract the ultrasound plane.
     * @param val value to set.
     * @param key the key used to select the member to change.
     */
    void updateDoubleExtractionParameter(double val, std::string key);

    /// SLOT: Reset ultrasound plane extraction and compute new "palmito" extraction parameters.
    void resetEchoPlane();

    /**
     * @brief SLOT: change debug mode.
     * @param val value to set.
     * @param key the key used to select the member to change.
     */
    void changeDebugEchoPlane(bool debug, std::string key);

    /**
     * @brief SLOT: change echo shape.
     * @param val value to set.
     * @param key the key used to select the member to change.
     */
    void changeEchoShape(bool echoShape, std::string key);

    /// Updates the beam extraction map using the current probe settings.
    void updateBeamExtractionMap();

    /// Handle if the probe settings are initialized.
    bool m_probeSettingsInitialized {false};

    /// Probe settings.
    ProbeSimulationSettings m_probeSettings;

    /// Remember the four corners of the ultrasound plane to prevent cases
    /// when the points can not be computed (use the previous points in those cases).
    /// Order is: top-left, top-right, bottom-left and bottom-right.
    std::array< fwVec3d, 4> m_echoRefPoints;

    /// Store the minimum threshold used for ultrasound plane extraction.
    int m_thresholdMin {4};

    /// Store the current value of depth of the US probe (must be read from the monitor).
    double m_usDepth {19.};

    /// Debug mode.
    bool m_debug {false};

    /// Convex shape of extracted image.
    bool m_convexShape {true};

    /// Map used to extract the ultrasound beam from the video.
    ::cv::Mat m_extractionMap { 1, 1, CV_32FC2 };
};

} // cvSegmentation
