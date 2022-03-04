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

#include <data/StructureTraitsDictionary.hpp>

#include <filesystem>

namespace sight::data
{

class StructureTraitsDictionary;

}

namespace sight::io::base
{

namespace reader
{

/**
 * @brief   Dictionary reader. Reads '.dic' files.
 *
 *
 * Reads '.dic' files and converts them into a 'sight::data::StructureTraitsDictionary'.
 * This is an ASCII file. The structure of each line is defined on the first line of this file.
 */
class IO_BASE_CLASS_API DictionaryReader :
    public GenericObjectReader<data::StructureTraitsDictionary>,
    public core::location::SingleFile
{
public:

    SIGHT_DECLARE_CLASS(
        DictionaryReader,
        GenericObjectReader<data::StructureTraitsDictionary>,
        io::base::reader::factory::New<DictionaryReader>
    )

    /// Constructor. Do nothing .
    IO_BASE_API DictionaryReader(io::base::reader::IObjectReader::Key key);

    /// Destructor. Do nothing.
    IO_BASE_API virtual ~DictionaryReader();

    /// Read the file with standard iostream API.
    IO_BASE_API void read() override;

    IO_BASE_API std::string extension() const override;

    /// Returns the default dictionary path ( file in rc directory path of fwDataIO library )
    IO_BASE_API static std::filesystem::path getDefaultDictionaryPath();
};

} // namespace reader

} // namespace sight::io::base
