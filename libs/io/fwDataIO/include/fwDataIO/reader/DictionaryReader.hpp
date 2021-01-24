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

#include "fwDataIO/config.hpp"
#include "fwDataIO/reader/GenericObjectReader.hpp"

#include <data/location/SingleFile.hpp>
#include <data/StructureTraitsDictionary.hpp>

#include <filesystem>

namespace sight::data
{
class StructureTraitsDictionary;
}

namespace fwDataIO
{

namespace reader
{

/**
 * @brief   Dictionary reader. Read file with .dic extension.
 *
 *
 * Ircad reader to read a data::StructureTraitsDictionary on filesystem which the file format
 * is .dic. This is an ascii file the structure of each line is defined on the first line of this file.
 */
class FWDATAIO_CLASS_API DictionaryReader :
    public GenericObjectReader< data::StructureTraitsDictionary>,
    public data::location::enableSingleFile< IObjectReader >

{

public:

    fwCoreClassMacro(DictionaryReader, GenericObjectReader< data::StructureTraitsDictionary >,
                     ::fwDataIO::reader::factory::New< DictionaryReader >)

    /// Constructor. Do nothing .
    FWDATAIO_API DictionaryReader(::fwDataIO::reader::IObjectReader::Key key);

    /// Destructor. Do nothing.
    FWDATAIO_API virtual ~DictionaryReader();

    /// Read the file with standard iostream API.
    FWDATAIO_API virtual void read() override;

    FWDATAIO_API std::string extension() override;

    /// Returns the default dictionary path ( file in rc directory path of fwDataIO library )
    FWDATAIO_API static std::filesystem::path  getDefaultDictionaryPath();
};

} // namespace reader

} // namespace fwDataIO
