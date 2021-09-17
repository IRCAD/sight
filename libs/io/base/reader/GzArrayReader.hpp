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
#include "io/base/reader/GenericObjectReader.hpp"

#include <core/location/SingleFile.hpp>

#include <data/Array.hpp>

#include <filesystem>

namespace sight::io::base
{

namespace reader
{

/**
 * @brief   Array Reader. Reads '.raw.gz' files.
 *
 *
 *  Reads '.raw.gz' files and converts them into a 'sight::data::Array'.
 *  The buffer is writen in a file using zlib.
 */
class IO_BASE_CLASS_API GzArrayReader : public GenericObjectReader<data::Array>,
                                        public core::location::SingleFile
{
public:

    SIGHT_DECLARE_CLASS(
        GzArrayReader,
        GenericObjectReader<data::Array>,
        io::base::reader::factory::New<GzArrayReader>
    )

    /// Constructor. Does nothing.
    IO_BASE_API GzArrayReader(io::base::reader::IObjectReader::Key key);

    /// Destructor. Does nothing.
    IO_BASE_API virtual ~GzArrayReader();

    /// Reads the file using the zlib API.
    IO_BASE_API void read() override;

    /// Defines extensions supported by this reader. Here: ".raw.gz"
    IO_BASE_API std::string extension() const override;
};

} // namespace reader

} // namespace sight::io::base
