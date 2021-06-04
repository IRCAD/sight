/************************************************************************
 *
 * Copyright (C) 2019-2021 IRCAD France
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

#include "modules/geometry/vision/config.hpp"

#include <data/MarkerMap.hpp>

#include <service/IController.hpp>

#include <opencv2/core.hpp>

namespace sight::module::geometry::vision
{

/**
 * @brief Reprojects the chessboard model onto the detected points using the estimated camera pose and calibration.
 *
 * Computes the reprojection error and sends it through a signal.
 *
 * @section Slots Slots
 *  - \b toggleDistortion(): toggles distortion for the reprojected points.
 *  - \b updateChessboardSize(): updates the chessboard model from the preferences.
 *
 * @section Signal Signal
 *  -\b errorComputed(double): sends the computed RMSE in pixels.
 *
 * @section XML XML Configuration
 *
 * @code{.xml}
     <service type="sight::module::geometry::vision::SChessboardReprojection">
        <in key="transform" uid="..."/>
        <in key="camera" uid="..."/>
        <in key="detectedChessboard" uid="..." />
        <inout key="videoImage" uid="..." />
        <out key="chessboardModel" uid="..." />
        <board width="CHESSBOARD_WIDTH" height="CHESSBOARD_HEIGHT" squareSize="CHESSBOARD_SQUARE_SIZE" />
        <config drawDetected="true" drawReprojection="true" distortReprojection="true" drawReprojectionError="true" />
     </service>
   @endcode
 *
 * @subsection Input Input
 * - \b tranform [sight::data::Matrix4] (mandatory): transform between the chessboard and the camera.
 * - \b camera [sight::data::Camera] (mandatory): camera filming the chessboard.
 * - \b detectedChessboard [sight::data::PointList] (mandatory): detected chessboard corners.
 *
 * @subsection In-Out In-Out
 * - \b videoImage [sight::data::Image] (optional): image of the chessboard, can be used to display the detected and/or
 * reprojected points.
 *
 * @subsection Output Output
 * - \b chessboardModel [sight::data::PointList] (optional): 3d chessboard model as described by the `board`
 * preferences.
 *
 * @subsection Configuration Configuration:
 * - \b board (mandatory): preference keys to retrieve the number of squares of the board in
 *                         width and height as well as square's size.
 * - \b drawDetected (optional, default=true): draw the detected points onto the image.
 * - \b drawReprojection (optional, default="true"): draw the reprojected points onto the image.
 * - \b drawReprojectionError (optional, default="true"): draw the reprojection error on the image.
 * - \b distortReprojection (optional, default="true"): whether the 'videoImage' is undistorted, in which case the
 *                                                      reprojected points should not be distorted.
 */
class MODULE_GEOMETRY_VISION_CLASS_API SChessboardReprojection : public service::IService
{
public:

    SIGHT_DECLARE_SERVICE(SChessboardReprojection, service::IService);

    ///Constructor
    MODULE_GEOMETRY_VISION_API SChessboardReprojection();

    ///Destructor
    MODULE_GEOMETRY_VISION_API virtual ~SChessboardReprojection() final;

protected:

    /// Configures the service.
    MODULE_GEOMETRY_VISION_API virtual void configuring() final;

    /// Does nothing.
    MODULE_GEOMETRY_VISION_API virtual void starting() final;

    /// Reprojects the detected using the camera's intrinsics and the tranform. Computes the reprojection's RMSE.
    /// Writes the detected and reprojected points on the video image if there is one.
    MODULE_GEOMETRY_VISION_API virtual void updating() final;

    /// Does nothing.
    MODULE_GEOMETRY_VISION_API virtual void stopping() final;

    /// Connects camera, transform and detected points modification to the update slot.
    virtual service::IService::KeyConnectionsMap getAutoConnections() const final;

private:

    using ErrorComputedSignalType = core::com::Signal<void (double)>;

    /// Fetches the chessboard dimension from the preferences and computes the model.
    void updateChessboardSize();

    /// Enables/disabled distorting the reprojected points.
    void toggleDistortion();

    /// Preference key to retrieve the chessboard width.
    std::string m_widthKey;

    /// Preference key to retrieve the chessboard height.
    std::string m_heightKey;

    /// Preference key to retrieve the size of the chessboard's squares.
    std::string m_squareSizeKey;

    /// Width of the chessboard we're looking for.
    size_t m_width {11};

    /// Height of the chessboard we're looking for.
    size_t m_height {8};

    /// Apply distortion to the reprojected points if true. Undistort the detected points otherwise.
    bool m_distortReprojection {true};

    /// Draw reprojection points on the image if true.
    bool m_drawReprojection {true};

    /// Draw detected points on the image if true.
    bool m_drawDetected {true};

    /// Draw the reprojection error on the image if true.
    bool m_drawReprojectionError {true};

    /// True if this service outputs the chessboard model.
    bool m_hasOutputChessboard {false};

    /// Chessboard model.
    std::vector< ::cv::Point3f> m_chessboardModel;

    /// Signal sent when the reprojection error is computed.
    ErrorComputedSignalType::sptr m_errorComputedSig;
};

} //namespace sight::module::geometry::vision
