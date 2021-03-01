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

#include <data/Image.hpp>
#include <data/location/SingleFile.hpp>

#include <filesystem>

namespace sight::io::base
{

namespace reader
{

/**
 * @brief   Image Reader. Read file format .raw.gz
 *
 *
 * Ircad reader to read a data::Image on filesystem which the file format
 * is .raw.gz. This format is basic. The buffer is writen in a file with zlib.
 */
class IO_BASE_CLASS_API GzBufferImageReader :  public GenericObjectReader< data::Image >,
                                               public data::location::enableSingleFile< IObjectReader >
{

public:

    SIGHT_DECLARE_CLASS(GzBufferImageReader, GenericObjectReader< data::Image>,
                        io::base::reader::factory::New< GzBufferImageReader >)

    /// Constructor. Do nothing.
    IO_BASE_API GzBufferImageReader(io::base::reader::IObjectReader::Key key);

    /// Destructor. Do nothing.
    IO_BASE_API virtual ~GzBufferImageReader();

    /// Read the file with zlib API.
    IO_BASE_API virtual void read() override;

    /// Defines extension supported by this reader ".raw.gz"
    IO_BASE_API std::string extension() override;

};

} // namespace reader

} // namespace sight::io::base
