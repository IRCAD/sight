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

#include <data/array.hpp>

namespace sight::io::writer
{

/**
 * @brief   Array Writer. Writes .raw files.
 *
 *
 * Writes 'sight::data::array' into a '.raw' file.
 */
class SIGHT_IO_CLASS_API array_writer : public generic_object_writer<data::array>,
                                        public core::location::single_file
{
public:

    SIGHT_DECLARE_CLASS(array_writer, generic_object_writer<data::array>)

    /// Destructor. Does nothing.
    SIGHT_IO_API ~array_writer() override = default;

    /// Writes the file using zlib API.
    SIGHT_IO_API void write() override;

    /// Defines extensions supported by this writer. Here: ".raw"
    SIGHT_IO_API std::string extension() const override;
};

} // namespace sight::io::writer
