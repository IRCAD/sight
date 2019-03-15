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
 * Every update triggers detection on the current input image. The 'recordPoints' slot must be called to store
 * the chessboard positions in the CalibrationInfo structure after a successful detection.
 *
 * @section Signals Signals
 * - \b chessboardDetected(bool): Emitted after trying to detect a chessboard. Sends whether it was detected or not.
 *
 * @section Slots Slots
 * - \b recordPoints(): Request to store the current image in the calibration data, if the chessboard is detected.
 * - \b updateChessboardSize(): update the parameters of the chessboard from preferences.
 *
 * @section XML XML Configuration
 *
 * @code{.xml}
       <service uid="..." type="::videoCalibration::SChessBoardDetector" >
            <in key="image" uid="..." />
            <inout key="calInfo" uid="..." />
            <inout key="detection" uid="..." />
           <board width="CHESSBOARD_WIDTH" height="CHESSBOARD_HEIGHT" />
       </service>
   @endcode
 * @subsection Input Input:
 * - \b image [::fwData::Image]: image on which we run the detection.
 * @subsection In-Out In-Out:
 * - \b calInfo [::arData::CalibrationInfo]: calibration object storing the detected images.
 * - \b detection [::fwData::PointList] (optional): detected chessboard points in image coordinates.
 * @subsection Configuration Configuration:
 * - \b board : preference key to retrieve the number of squares of the board in width and height.
 */
class VIDEOCALIBRATION_CLASS_API SChessBoardDetector : public ::fwServices::IController
{
public:
    fwCoreServiceClassDefinitionsMacro((SChessBoardDetector)(::fwServices::IController))

    /// Type of signal sent after trying to detect a chessboard in an image. Sends whether detection was succesful.
    typedef ::fwCom::Signal<void (bool)> ChessboardDetectedSignalType;

    /// Constructor
    VIDEOCALIBRATION_API SChessBoardDetector() noexcept;

    /// Destructor
    VIDEOCALIBRATION_API virtual ~SChessBoardDetector() noexcept override;

protected:

    /// Configures the service.
    VIDEOCALIBRATION_API virtual void configuring() override;

    /// Initializes the chessboard size from the preferences.
    VIDEOCALIBRATION_API virtual void starting() override;

    /// Tries to detect a chessboard in the image.
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

    /// Tries to detect a chessboard with the given dimensions in the image.
    static ::fwData::PointList::sptr detectChessboard(const ::fwData::Image::csptr& _img, size_t _xDim, size_t _yDim);

    /// Signal emitted after detection.
    ChessboardDetectedSignalType::sptr m_sigChessboardDetected;

    /// Preference key to retrieve the chessboard width.
    std::string m_widthKey;

    /// Preference key to retrieve the chessboard height.
    std::string m_heightKey;

    /// Width of the chessboard we're looking for.
    size_t m_width;

    /// Height of the chessboard we're looking for.
    size_t m_height;

    /// Last detected chessboard points in each image. Null if detection failed.
    std::vector< ::fwData::PointList::sptr > m_pointLists;

    /// Last images on which a chessboard was detected. Null if detection failed.
    std::vector< ::fwData::Image::sptr > m_images;
};

} //namespace videoCalibration
