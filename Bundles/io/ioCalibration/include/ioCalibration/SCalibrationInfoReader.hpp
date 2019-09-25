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

#include "ioCalibration/config.hpp"

#include <fwIO/IReader.hpp>

#include <string>

namespace ioCalibration
{

/**
 * @brief Read calibration input datasets.
 *
 * Load images of chessboards to fill a calibration info structure.
 *
 * @section XML XML Configuration
 *
 * @code{.xml}
   <service uid="..." type="::ioCalibration::SCalibrationInfoReader">
        <inout key="data" uid="..." />
        <board width="CHESSBOARD_WIDTH" height="CHESSBOARD_HEIGHT" scale="CHESSBOARD_SCALE" />
   </service>
   @endcode
 *
 * @subsection InOut InOut
 * - \b data [::arData::CalibrationInfo]: structure to which the loaded data is appended
 * @subsection Configuration Configuration:
 * - \b board : preference keys to retrieve the number of squares of the board in width and height as well
 *              as the (optional) scaling factor to be applied to the input image.
 */
class IOCALIBRATION_CLASS_API SCalibrationInfoReader : public ::fwIO::IReader
{
public:

    fwCoreServiceClassDefinitionsMacro( (SCalibrationInfoReader)( ::fwIO::IReader) )

    /// Constructor.
    IOCALIBRATION_API SCalibrationInfoReader() noexcept;

    /// Destructor.
    IOCALIBRATION_API virtual ~SCalibrationInfoReader() noexcept override;

    /// Configures the folder path.
    IOCALIBRATION_API virtual void configureWithIHM() override;

protected:

    /// Returns the folder path type.
    IOCALIBRATION_API virtual ::fwIO::IOPathType getIOPathType() const override;

    /// Configures the output file format and the chessboard preference keys.
    IOCALIBRATION_API virtual void configuring() override;

    /// Updates the chessboard parameters from the preferences.
    IOCALIBRATION_API virtual void starting() override;

    /// Loads the images and attempts to detect a chessboard.
    IOCALIBRATION_API virtual void updating() override;

    /// Does nothing.
    IOCALIBRATION_API virtual void stopping() override;

private:

    /// SLOT: updates the chessboard size from the preferences.
    void updateChessboardSize();

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

    /// Scale applied to the images before running the detection algorithm.
    float m_scale { 1.f };

};

} // namespace ioCalibration
