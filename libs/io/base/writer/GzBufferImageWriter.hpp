/************************************************************************
 *
 * Copyright (C) 2009-2022 IRCAD France
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

#include "io/base/config.hpp"
#include "io/base/writer/GenericObjectWriter.hpp"

#include <core/location/SingleFile.hpp>

#include <data/Image.hpp>

#include <filesystem>

namespace sight::io::base::writer
{

/**
 * @brief   Image Writer. Writes .raw.gz files.
 *
 *
 * Writes `sight::data::Image` into a `.raw.gz` file.
 * The buffer is written in a file using zlib.
 */
class IO_BASE_CLASS_API GzBufferImageWriter : public GenericObjectWriter<data::Image>,
                                              public core::location::SingleFile
{
public:

    SIGHT_DECLARE_CLASS(
        GzBufferImageWriter,
        GenericObjectWriter<data::Image>,
        io::base::writer::factory::New<GzBufferImageWriter>
    )

    /// Constructor. Does nothing.
    IO_BASE_API GzBufferImageWriter(io::base::writer::IObjectWriter::Key key);

    /// Destructor. Does nothing.
    IO_BASE_API ~GzBufferImageWriter() override;

    /// Reads the file using the zlib API.
    IO_BASE_API void write() override;

    /// Defines the extensions supported by this writer. Here: ".raw.gz"
    IO_BASE_API std::string extension() const override;
};

} // namespace sight::io::base::writer
