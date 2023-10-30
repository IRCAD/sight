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

#include <data/structure_traits_dictionary.hpp>

#include <filesystem>

namespace sight::data
{

class StructureTraitsDictionary;

} // namespace sight::data

namespace sight::io::reader
{

/**
 * @brief   Dictionary reader. Reads '.dic' files.
 *
 *
 * Reads '.dic' files and converts them into a 'sight::data::structure_traits_dictionary'.
 * This is an ASCII file. The structure of each line is defined on the first line of this file.
 */
class IO_CLASS_API dictionary_reader :
    public generic_object_reader<data::structure_traits_dictionary>,
    public core::location::single_file
{
public:

    SIGHT_DECLARE_CLASS(dictionary_reader, generic_object_reader<data::structure_traits_dictionary>)

    IO_API ~dictionary_reader() override = default;

    /// Read the file with standard iostream API.
    IO_API void read() override;

    IO_API std::string extension() const override;

    /// Returns the default dictionary path ( file in rc directory path of fwDataIO library )
    IO_API static std::filesystem::path get_default_dictionary_path();
};

} // namespace sight::io::reader
