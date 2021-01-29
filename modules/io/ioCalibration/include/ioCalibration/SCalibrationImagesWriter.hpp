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

#include "ioCalibration/config.hpp"

#include <io/base/services/IWriter.hpp>

#include <string>

namespace ioCalibration
{

/**
 * @brief Write CalibrationInfo 2D images to a folder using OpenCV.
 *
 * @section XML XML Configuration
 *
 * @code{.xml}
   <service uid="..." type="::ioCalibration::SCalibrationImagesWriter">
       <in key="data" uid="..." />
       <format>.tiff</format>
       <folder>...</folder>
   </service>
   @endcode
 *
 * @subsection Input Input
 * - \b data [data::CalibrationInfo]: images to write.
 * @subsection Configuration Configuration
 * - \b format (optional, values=.jpeg|.ppm|.png|.bmp|.tiff, default=.tiff): output file format.
 * - \b folder (optional): directory in which the files should be written.
 */
class IOCALIBRATION_CLASS_API SCalibrationImagesWriter : public sight::io::base::services::IWriter
{
public:

    fwCoreServiceMacro(SCalibrationImagesWriter, sight::io::base::services::IWriter)

    /// Constructor.
    IOCALIBRATION_API SCalibrationImagesWriter() noexcept;

    /// Destructor.
    IOCALIBRATION_API virtual ~SCalibrationImagesWriter() noexcept override;

    /**
     * @brief Configures the folder path.
     * @deprecated Will be removed in sight 22.0. Use openLocationDialog() instead.
     */
    [[deprecated("Will be removed in sight 22.0. Use openLocationDialog() instead.")]]
    IOCALIBRATION_API virtual void configureWithIHM() override;

    /// Configures the folder path.
    IOCALIBRATION_API virtual void openLocationDialog() override;

protected:

    /// Returns the folder path type.
    IOCALIBRATION_API virtual sight::io::base::services::IOPathType getIOPathType() const override;

    /// Configures the output file format.
    IOCALIBRATION_API virtual void configuring() override;

    /// Does nothing.
    IOCALIBRATION_API virtual void starting() override;

    /// Writes the images to the folder if it has been defined.
    IOCALIBRATION_API virtual void updating() override;

    /// Does nothing.
    IOCALIBRATION_API virtual void stopping() override;

private:

    /// Output file extension, can be any format handled by `cv::imwrite()`.
    std::string m_fileExtension;

};

} // namespace ioCalibration
