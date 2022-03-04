/************************************************************************
 *
 * Copyright (C) 2009-2021 IRCAD France
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

#include <data/Array.hpp>

namespace sight::io::base
{

namespace writer
{

/**
 * @brief   Array Writer. Write .raw.gz files.
 *
 *
 * Writes 'sight::data::Array' into a '.raw.gz' file.
 * The buffer is written in a file using zlib.
 */
class IO_BASE_CLASS_API GzArrayWriter : public GenericObjectWriter<data::Array>,
                                        public core::location::SingleFile
{
public:

    SIGHT_DECLARE_CLASS(
        GzArrayWriter,
        GenericObjectWriter<data::Array>,
        io::base::writer::factory::New<GzArrayWriter>
    )

    /// Constructor. Does nothing.
    IO_BASE_API GzArrayWriter(io::base::writer::IObjectWriter::Key key);

    /// Destructor. Does nothing.
    IO_BASE_API virtual ~GzArrayWriter();

    /// Writes the file using zlib API.
    IO_BASE_API void write() override;

    /// Defines the extensions supported by this writer. Here: ".raw.gz"
    IO_BASE_API std::string extension() const override;
};

} // namespace writer

} // namespace sight::io::base
