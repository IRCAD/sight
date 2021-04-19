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

#include <data/Array.hpp>
#include <data/location/SingleFile.hpp>

#include <filesystem>

namespace sight::io::base
{
namespace reader
{

/**
 * @brief   Array Reader. Reads '.raw' files.  
 *
 *
 * Reads '.raw' files and converts them into a 'sight::data::Array'.
 */
class IO_BASE_CLASS_API ArrayReader :  public GenericObjectReader< data::Array >,
                                       public data::location::enableSingleFile< IObjectReader >
{

public:

    SIGHT_DECLARE_CLASS(ArrayReader, GenericObjectReader< data::Array>, io::base::reader::factory::New<ArrayReader>)

    /// Constructor. Does nothing.
    IO_BASE_API ArrayReader(io::base::reader::IObjectReader::Key key);

    /// Destructor. Does nothing.
    IO_BASE_API virtual ~ArrayReader();

    /// Reads the file with zlib API.
    IO_BASE_API void read() override;

    /// Defines extension supported by this reader ".raw"
    IO_BASE_API std::string extension() override;

};

} // namespace reader
} // namespace sight::io::base
