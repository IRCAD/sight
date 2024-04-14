/************************************************************************
 *
 * Copyright (C) 2009-2024 IRCAD France
 * Copyright (C) 2012-2019 IHU Strasbourg
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

#include <sight/io/__/config.hpp>

#include "io/__/writer/generic_object_writer.hpp"

#include <core/location/single_file.hpp>

#include <data/image.hpp>

#include <filesystem>

namespace sight::io::writer
{

/**
 * @brief   image Writer. Writes .raw.gz files.
 *
 *
 * Writes `sight::data::image` into a `.raw.gz` file.
 * The buffer is written in a file using zlib.
 */
class SIGHT_IO_CLASS_API gz_buffer_image_writer : public generic_object_writer<data::image>,
                                                  public core::location::single_file
{
public:

    SIGHT_DECLARE_CLASS(gz_buffer_image_writer, generic_object_writer<data::image>)

    /// Destructor. Does nothing.
    SIGHT_IO_API ~gz_buffer_image_writer() override = default;

    /// Reads the file using the zlib API.
    SIGHT_IO_API void write() override;

    /// Defines the extensions supported by this writer. Here: ".raw.gz"
    SIGHT_IO_API std::string extension() const override;
};

} // namespace sight::io::writer
