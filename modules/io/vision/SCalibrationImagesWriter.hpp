/************************************************************************
 *
 * Copyright (C) 2019-2021 IRCAD France
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

#include <io/base/service/IWriter.hpp>

#include <string>

namespace sight::module::io::vision
{

/**
 * @brief Write CalibrationInfo 2D images to a folder using OpenCV.
 *
 * @section XML XML Configuration
 *
 * @code{.xml}
   <service uid="..." type="sight::module::io::vision::SCalibrationImagesWriter">
       <in key="data" uid="..." />
       <format>.tiff</format>
       <folder>...</folder>
   </service>
   @endcode
 *
 * @subsection Input Input
 * - \b data [sight::data::CalibrationInfo]: images to write.
 * @subsection Configuration Configuration
 * - \b format (optional, values=.jpeg|.ppm|.png|.bmp|.tiff, default=.tiff): output file format.
 * - \b folder (optional): directory in which the files should be written.
 */
class MODULE_IO_VISION_CLASS_API SCalibrationImagesWriter : public sight::io::base::service::IWriter
{
public:

    SIGHT_DECLARE_SERVICE(SCalibrationImagesWriter, sight::io::base::service::IWriter);

    /// Constructor.
    MODULE_IO_VISION_API SCalibrationImagesWriter() noexcept;

    /// Destructor.
    MODULE_IO_VISION_API ~SCalibrationImagesWriter() noexcept override;

    /// Configures the folder path.
    MODULE_IO_VISION_API void openLocationDialog() override;

protected:

    /// Returns the folder path type.
    MODULE_IO_VISION_API sight::io::base::service::IOPathType getIOPathType() const override;

    /// Configures the output file format.
    MODULE_IO_VISION_API void configuring() override;

    /// Does nothing.
    MODULE_IO_VISION_API void starting() override;

    /// Writes the images to the folder if it has been defined.
    MODULE_IO_VISION_API void updating() override;

    /// Does nothing.
    MODULE_IO_VISION_API void stopping() override;

private:

    /// Output file extension, can be any format handled by `cv::imwrite()`.
    std::string m_fileExtension;
};

} // namespace sight::module::io::vision
