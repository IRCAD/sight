/************************************************************************
 *
 * Copyright (C) 2009-2023 IRCAD France
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

#include "io/__/config.hpp"
#include "io/__/reader/GenericObjectReader.hpp"

#include <core/location/single_file.hpp>

#include <data/Array.hpp>

#include <filesystem>

namespace sight::io::reader
{

/**
 * @brief   Array Reader. Reads '.raw' files.
 *
 *
 * Reads '.raw' files and converts them into a 'sight::data::Array'.
 */
class IO_CLASS_API ArrayReader : public GenericObjectReader<data::Array>,
                                 public core::location::single_file
{
public:

    SIGHT_DECLARE_CLASS(ArrayReader, GenericObjectReader<data::Array>);

    /// Destructor. Does nothing.
    IO_API ~ArrayReader() override = default;

    /// Reads the file with zlib API.
    IO_API void read() override;

    /// Defines extension supported by this reader ".raw"
    IO_API std::string extension() const override;
};

} // namespace sight::io::reader
