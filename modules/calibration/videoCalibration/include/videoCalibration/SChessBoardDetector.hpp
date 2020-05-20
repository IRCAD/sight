/************************************************************************
 *
 * Copyright (C) 2014-2019 IRCAD France
 * Copyright (C) 2014-2019 IHU Strasbourg
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

#include "videoCalibration/config.hpp"

#include <fwData/Image.hpp>
#include <fwData/PointList.hpp>

#include <fwServices/IController.hpp>

namespace videoCalibration
{

/**
 * @brief Detects chessboards in images and pushes detected points to CalibrationInfo objects.
 *
 * Every update triggers detection on the current input images. The 'recordPoints' slot must be called to store
 * the chessboard positions in the CalibrationInfo structure after a successful detection.
 *
 * @section Signals Signals
 * - \b chessboardDetected(bool): Emitted after trying to detect a chessboard. Sends whether it was detected or not.
 * - \b chessboardFound(): Emitted if a chessboard pattern was recognized in the image.
 *
 * @section Slots Slots
 * - \b recordPoints(): Request to store the current image in the calibration data, if the chessboard is detected.
 * - \b updateChessboardSize(): update the parameters of the chessboard from preferences.
 *
 * @section XML XML Configuration
 *
 * @code{.xml}
        <service uid="..." type="::videoCalibration::SChessBoardDetector" >
            <in group="image">
                <key uid="..." />
                <key uid="..." />
            </in>
            <inout group="calInfo">
                <key uid="..." />
                <key uid="..." />
            </inout>
            <inout group="detection">
                <key uid="..." />
                <key uid="..." />
            </inout>
            <board width="CHESSBOARD_WIDTH" height="CHESSBOARD_HEIGHT" scale="CHESSBOARD_SCALE" />
       </service>
   @endcode
 * @subsection Input Input:
 * - \b image [::fwData::Image]: images on which we run the detection.
 * @subsection In-Out In-Out:
 * - \b calInfo [::arData::CalibrationInfo]: calibration objects storing the detected images.
 * - \b detection [::fwData::PointList] (optional): detected chessboard points in image coordinates.
 * @subsection Configuration Configuration:
 * - \b board : preference keys to retrieve the number of squares of the board in width and height as well
 *              as the scaling factor to be applied to the input image.
 */
class VIDEOCALIBRATION_CLASS_API SChessBoardDetector : public ::fwServices::IController
{
public:
    fwCoreServiceMacro(SChessBoardDetector, ::fwServices::IController);

    /// Signal type sent after trying to detect a chessboard in an image. Sends whether detection was succesful.
    typedef ::fwCom::Signal<void (bool)> ChessboardDetectedSignalType;

    /// Signal type sent after a successful detection.
    typedef ::fwCom::Signal<void ()> ChessboardFoundSignalType;

    /// Constructor
    VIDEOCALIBRATION_API SChessBoardDetector() noexcept;

    /// Destructor
    VIDEOCALIBRATION_API virtual ~SChessBoardDetector() noexcept override;

protected:

    /// Configures the service.
    VIDEOCALIBRATION_API virtual void configuring() override;

    /// Initializes the chessboard size from the preferences.
    VIDEOCALIBRATION_API virtual void starting() override;

    /// Tries to detect chessboards in all input images.
    VIDEOCALIBRATION_API virtual void updating() override;

    /// Clears the detected points.
    VIDEOCALIBRATION_API virtual void stopping() override;

    /// Returns proposals to update the service when the input image is modified.
    VIDEOCALIBRATION_API virtual KeyConnectionsMap getAutoConnections() const override;

private:

    /// SLOT: stores the last detected chessboard in the CalibrationInfo structure.
    void recordPoints();

    /// SLOT: updates the chessboard size from the preferences.
    void updateChessboardSize();

    /// Runs the detection for the given input index.
    void doDetection(size_t _imageIndex);

    /// Signal emitted after detection.
    ChessboardDetectedSignalType::sptr m_sigChessboardDetected;

    /// Signal emitted if a chessboard can be seen in the image.
    ChessboardFoundSignalType::sptr m_sigChessboardFound;

    /// Preference key to retrieve the chessboard width.
    std::string m_widthKey;

    /// Preference key to retrieve the chessboard height.
    std::string m_heightKey;

    /// Preference key to retrieve the scaling factor applied to the image before detection.
    std::string m_scaleKey;

    /// Width of the chessboard we're looking for.
    size_t m_width { 11 };

    /// Height of the chessboard we're looking for.
    size_t m_height { 8 };

    float m_scale { 1.f };

    /// Last detected chessboard points in each image. Null if detection failed.
    std::vector< ::fwData::PointList::sptr > m_pointLists;

    /// Last images on which a chessboard was detected. Null if detection failed.
    std::vector< ::fwData::Image::sptr > m_images;
};

} //namespace videoCalibration
