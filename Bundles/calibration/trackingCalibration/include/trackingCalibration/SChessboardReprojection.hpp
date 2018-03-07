/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2018.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#pragma once

#include "trackingCalibration/config.hpp"

#include <fwServices/IService.hpp>

#include <opencv2/core.hpp>

namespace trackingCalibration
{
/**
 * @brief
 *
 * @section Signals Signals
 * - \b reprojectionComputed(double): sends the reprojection error.
 *
 * @section Slots Slots
 * - \b updateChessboardSize(): updates the chessboard model from the preferences.
 * - \b setMovingCamera(bool): configure the service to compute a hand-eye calibration for a moving or fixed camera
 *
 * @section XML XML Configuration
 *
 * @code{.xml}
    <service type="::trackingCalibration::SChessboardReprojection">
        <in key="camera" uid="..." />
        <in key="detectedChessboard" uid="..." />
        <in key="handEyeX" uid="..." />
        <in key="handEyeZ" uid="..." />
        <in key="trackerMatrix" uid="..." />
        <inout key="reprojectedChessboard" uid="..." />
        <board width="CHESSBOARD_WIDTH" height="CHESSBOARD_HEIGHT" squareSize="CHESSBOARD_SQUARE_SIZE" />
        <config movingCamera="false" />
    </service>
 * @endcode
 *
 * @subsection Input Input
 * - \b camera [::arData::Camera]: calibrated camera filming the chessboard
 * - \b detectedChessboard [::fwData::PointList]: chessboard detected in the current camera image
 * - \b handEyeX [::fwData::TransformationMatrix3D]: hand-eye X matrix
 * - \b handEyeZ [::fwData::TransformationMatrix3D]: hand-eye Z matrix
 * - \b trackerMatrix [::fwData::TransformationMatrix3D]: tracked camera position, gives the actual camera position
 *  when concatenated to hand-eye X matrix
 *
 * @subsection InOut InOut
 * - \b reprojectedChessboard [::fwData::PointList]: chessboard reprojection in image space
 *
 * @subsection Configuration Configuration
 * - \b board (mandatory): chessboard preferences (width, height and square size)
 * - \b config (optional)
 *     - \b movingCamera (optional, values=true|false, default: false): defines if the camera is a moving element of our
 *          hand eye system (i.e. fixed chessboard)
 */
class TRACKINGCALIBRATION_CLASS_API SChessboardReprojection : public ::fwServices::IService
{
public:

    fwCoreServiceClassDefinitionsMacro((SChessboardReprojection)(::fwServices::IService));

    /// Constructor.
    TRACKINGCALIBRATION_API SChessboardReprojection() noexcept;

    /// Destructor.
    TRACKINGCALIBRATION_API virtual ~SChessboardReprojection() noexcept;

protected:

    /// Configures chessboard preferences.
    TRACKINGCALIBRATION_API void configuring() override;

    /// Updates the model using the initial preferences.
    TRACKINGCALIBRATION_API void starting() override;

    /// Reprojects the chessboard models and computes the mean reprojection error.
    TRACKINGCALIBRATION_API void updating() override;

    /// Does nothing.
    TRACKINGCALIBRATION_API void stopping() override;

    /// Connect all the inputs' modified signals to this service's update slot.
    TRACKINGCALIBRATION_API virtual KeyConnectionsMap getAutoConnections() const override;

private:

    /// SLOT: Fetches the chessboard dimension from the preferences and computes the model.
    void updateChessboardSize();

    /// SLOT: sets moving or fixed camera mode
    void setMovingCamera(bool movingCamera);

    /// Computes the mean pointwise distance between the detected and the reprojected chessboards.
    static double meanDistance(const std::vector< ::cv::Point2d >& _detected,
                               const std::vector< ::cv::Point2d >& _reprojected);

    /// Type of signal emitted when the reprojection error is computed.
    typedef ::fwCom::Signal< void (double) > ErrorComputedSignalType;

    /// Chessboard width preference.
    std::string m_boardWidthKey;

    /// Chessboard height preference.
    std::string m_boardHeightKey;

    /// Chessboard square size preference.
    std::string m_boardSquareSizeKey;

    /// Chessboard model based on the preferences.
    std::vector< ::cv::Point3d > m_chessboardModel;

    bool m_movingCamera;

};

} // namespace trackingCalibration
