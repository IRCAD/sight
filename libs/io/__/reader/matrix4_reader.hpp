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
#include "io/__/reader/generic_object_reader.hpp"

#include <core/location/single_file.hpp>

#include <data/matrix4.hpp>

namespace sight::io::reader
{

/**
 * @brief   Reader for transformation of 3D Matrix. Read .trf files.
 *
 *
 * Reads '.trf' files and converts them into a 'sight::data::matrix4'.
 */
class IO_CLASS_API matrix4_reader :
    public generic_object_reader<data::matrix4>,
    public core::location::single_file
{
public:

    SIGHT_DECLARE_CLASS(matrix4_reader, generic_object_reader<data::matrix4>)

    /// Destructor. Does nothing.
    IO_API ~matrix4_reader() override = default;

    /// Reads the file using standard iostream API.
    IO_API void read() override;

    /// Defines the extensions supported by this reader. Here: ".trf"
    IO_API std::string extension() const override;
};

} // namespace sight::io::reader
