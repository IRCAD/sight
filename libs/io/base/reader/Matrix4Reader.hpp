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

#include <data/location/SingleFile.hpp>
#include <data/Matrix4.hpp>

namespace sight::io::base
{

namespace reader
{

/**
 * @brief   Reader for transformation of 3D Matrix. Read file format .trf
 *
 *
 * Ircad reader to read a data::Matrix4 on filesystem which the file format
 * is .trf. This format is basic. This is an ascii file which contains matrix 4x4 values
 */
class IO_BASE_CLASS_API Matrix4Reader :
    public GenericObjectReader< data::Matrix4 >,
    public data::location::enableSingleFile< IObjectReader >
{

public:

    SIGHT_DECLARE_CLASS(Matrix4Reader, GenericObjectReader< data::Matrix4 >,
                        io::base::reader::factory::New< Matrix4Reader >)

    /// Constructor. Do nothing.
    IO_BASE_API Matrix4Reader(io::base::reader::IObjectReader::Key key);

    /// Destructor. Do nothing.
    IO_BASE_API virtual ~Matrix4Reader();

    /// Read the file with standard iostream API.
    IO_BASE_API void read() override;

    /// Defines extension supported by this reader ".trf"
    IO_BASE_API std::string extension() override;

};

}

}
