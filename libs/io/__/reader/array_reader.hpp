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

#include "io/__/reader/generic_object_reader.hpp"

#include <core/location/single_file.hpp>

#include <data/array.hpp>

#include <filesystem>

namespace sight::io::reader
{

/**
 * @brief   Array Reader. Reads '.raw' files.
 *
 *
 * Reads '.raw' files and converts them into a 'sight::data::array'.
 */
class SIGHT_IO_CLASS_API array_reader : public generic_object_reader<data::array>,
                                        public core::location::single_file
{
public:

    SIGHT_DECLARE_CLASS(array_reader, generic_object_reader<data::array>);

    /// Destructor. Does nothing.
    SIGHT_IO_API ~array_reader() override = default;

    /// Reads the file with zlib API.
    SIGHT_IO_API void read() override;

    /// Defines extension supported by this reader ".raw"
    SIGHT_IO_API std::string extension() const override;
};

} // namespace sight::io::reader
