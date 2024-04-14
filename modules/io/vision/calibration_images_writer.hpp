/************************************************************************
 *
 * Copyright (C) 2019-2024 IRCAD France
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

#include <io/__/service/writer.hpp>

#include <string>

namespace sight::module::io::vision
{

/**
 * @brief Write CalibrationInfo 2D images to a folder using OpenCV.
 *
 * @section XML XML Configuration
 *
 * @code{.xml}
   <service uid="..." type="sight::module::io::vision::calibration_images_writer">
       <in key="data" uid="..." />
       <format>.tiff</format>
       <folder>...</folder>
   </service>
   @endcode
 *
 * @subsection Input Input
 * - \b data [sight::data::calibration_info]: images to write.
 * @subsection Configuration Configuration
 * - \b format (optional, values=.jpeg|.ppm|.png|.bmp|.tiff, default=.tiff): output file format.
 * - \b folder (optional): directory in which the files should be written.
 */
class calibration_images_writer : public sight::io::service::writer
{
public:

    SIGHT_DECLARE_SERVICE(calibration_images_writer, sight::io::service::writer);

    /// Constructor.
    calibration_images_writer() noexcept;

    /// Destructor.
    ~calibration_images_writer() noexcept override;

    /// Configures the folder path.
    void open_location_dialog() override;

protected:

    /// Returns the folder path type.
    sight::io::service::path_type_t get_path_type() const override;

    /// Configures the output file format.
    void configuring() override;

    /// Does nothing.
    void starting() override;

    /// Writes the images to the folder if it has been defined.
    void updating() override;

    /// Does nothing.
    void stopping() override;

private:

    /// Output file extension, can be any format handled by `cv::imwrite()`.
    std::string m_file_extension {".tiff"};
};

} // namespace sight::module::io::vision
