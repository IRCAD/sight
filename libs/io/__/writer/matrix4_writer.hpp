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

#include <data/matrix4.hpp>

namespace sight::io::writer
{

/**
 * @brief   Writer for transformation of 3D Matrix. Writes .trf files.
 *
 *
 * Writes 'sight::data::matrix4' into a '.trf' file.
 * This is an ascii file which contains matrix 4x4 values
 */
class SIGHT_IO_CLASS_API matrix4_writer :
    public generic_object_writer<data::matrix4>,
    public core::location::single_file
{
public:

    SIGHT_DECLARE_CLASS(matrix4_writer, generic_object_writer<data::matrix4>)

    /// Destructor. Does nothing.
    SIGHT_IO_API ~matrix4_writer() override = default;

    /// Writes the file using the standard iostream API.
    SIGHT_IO_API void write() override;

    /// Defines the extensions supported by this writer. Here: ".trf"
    SIGHT_IO_API std::string extension() const override;
};

} // namespace sight::io::writer
