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
 * @subsection Signals Signals
 * - \b reprojectionComputed(double): sends the reprojection error.
 *
 * @subsection Slots Slots
 * - \b updateChessboardSize(): updates the chessboard model from the preferences
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
 */
class TRACKINGCALIBRATION_CLASS_API SChessboardReprojection : public ::fwServices::IService
{
public:

    fwCoreServiceClassDefinitionsMacro((SChessboardReprojection)(::fwServices::IService));

    /**
     * @brief Constructor.
     */
    TRACKINGCALIBRATION_API SChessboardReprojection() noexcept;

    /**
     * @brief Destructor.
     */
    TRACKINGCALIBRATION_API virtual ~SChessboardReprojection() noexcept;

protected:

    TRACKINGCALIBRATION_API void configuring() override;
    TRACKINGCALIBRATION_API void starting() override;
    TRACKINGCALIBRATION_API void updating() override;
    TRACKINGCALIBRATION_API void stopping() override;

    TRACKINGCALIBRATION_API virtual KeyConnectionsMap getAutoConnections() const override;

private:

    void updateChessboardSize();

    std::string m_boardWidthKey;

    std::string m_boardHeightKey;

    std::string m_boardSquareSizeKey;

    std::vector< ::cv::Point3d > m_chessboardModel;

};

} // namespace trackingCalibration
