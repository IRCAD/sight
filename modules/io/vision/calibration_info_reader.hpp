/************************************************************************
 *
 * Copyright (C) 2019-2023 IRCAD France
 * Copyright (C) 2019-2020 IHU Strasbourg
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

#include "modules/io/vision/config.hpp"

#include <io/__/service/reader.hpp>

#include <string>

namespace sight::module::io::vision
{

/**
 * @brief Read calibration input datasets.
 *
 * Load images of chessboards to fill a calibration info structure.
 *
 * @section XML XML Configuration
 *
 * @code{.xml}
   <service uid="..." type="sight::module::io::vision::calibration_info_reader">
        <inout key="data" uid="..." />
        <board width="CHESSBOARD_WIDTH" height="CHESSBOARD_HEIGHT" scale="CHESSBOARD_SCALE" />
   </service>
   @endcode
 *
 * @subsection InOut InOut
 * - \b data [sight::data::calibration_info]: structure to which the loaded data is appended
 * @subsection Configuration Configuration:
 * - \b board : preference keys to retrieve the number of squares of the board in width and height as well
 *              as the (optional) scaling factor to be applied to the input image.
 */
class MODULE_IO_VISION_CLASS_API calibration_info_reader : public sight::io::service::reader
{
public:

    SIGHT_DECLARE_SERVICE(calibration_info_reader, sight::io::service::reader);

    /// Constructor.
    MODULE_IO_VISION_API calibration_info_reader() noexcept;

    /// Destructor.
    MODULE_IO_VISION_API ~calibration_info_reader() noexcept override;

    /// Configures the folder path.
    MODULE_IO_VISION_API void openLocationDialog() override;

protected:

    /// Returns the folder path type.
    MODULE_IO_VISION_API sight::io::service::IOPathType getIOPathType() const override;

    /// Configures the output file format and the chessboard preference keys.
    MODULE_IO_VISION_API void configuring() override;

    /// Updates the chessboard parameters from the preferences.
    MODULE_IO_VISION_API void starting() override;

    /// Loads the images and attempts to detect a chessboard.
    MODULE_IO_VISION_API void updating() override;

    /// Does nothing.
    MODULE_IO_VISION_API void stopping() override;

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
    std::size_t m_width {11};

    /// Height of the chessboard we're looking for.
    std::size_t m_height {8};

    /// Scale applied to the images before running the detection algorithm.
    float m_scale {1.F};
};

} // namespace sight::module::io::vision
