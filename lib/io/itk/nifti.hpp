/************************************************************************
 *
 * Copyright (C) 2026 IRCAD France
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

#include <sight/io/itk/config.hpp>

#include <data/image.hpp>

#include <filesystem>

namespace sight::io::itk
{

/**
 * @brief Reads a NIfTI image from the specified file path.
 * @param _path The file path from which the NIfTI image will be read.
 * @param _image The image data to be populated.
 * @return `true` if the image was successfully read, `false` otherwise.
 */
SIGHT_IO_ITK_API bool read_nifti_image(const std::filesystem::path& _path, const sight::data::image::sptr& _image);

//------------------------------------------------------------------------------

/**
 * @brief Writes a NIfTI image to the specified file path.
 * @param _path The file path where the NIfTI image will be written.
 * @param _image The image data to be written.
 * @return `true` if the image was successfully written, `false` otherwise.
 */
SIGHT_IO_ITK_API bool write_nifti_image(const std::filesystem::path& _path, const sight::data::image::csptr& _image);

} // namespace sight::io::itk
