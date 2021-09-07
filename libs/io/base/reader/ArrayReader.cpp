/************************************************************************
 *
 * Copyright (C) 2009-2021 IRCAD France
 * Copyright (C) 2012-2020 IHU Strasbourg
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

#include "io/base/reader/ArrayReader.hpp"

#include "io/base/reader/registry/macros.hpp"

#include <core/Exception.hpp>

#include <fstream>
#include <iostream>

SIGHT_REGISTER_IO_READER(::sight::io::base::reader::ArrayReader);

namespace sight::io::base
{

namespace reader
{

//------------------------------------------------------------------------------

ArrayReader::ArrayReader(io::base::reader::IObjectReader::Key)
{
}

//------------------------------------------------------------------------------

ArrayReader::~ArrayReader()
{
}

//------------------------------------------------------------------------------

void ArrayReader::read()
{
    std::filesystem::path file = this->getFile();

    data::Array::sptr array = this->getConcreteObject();

    const auto dumpLock = array->lock();

    size_t arraySizeInBytes = array->resize(array->getSize());
    char* buff              = static_cast<char*>(array->getBuffer());

    std::ifstream fs(file.string().c_str(), std::ios::in | std::ios::binary | std::ios::ate);

    SIGHT_THROW_IF("Unable to read " << file, !fs.good());

    std::streampos fileSize = fs.tellg();
    fs.seekg(0, std::ios::beg);

    SIGHT_THROW_IF(
        file << ": Bad file size, expected: " << arraySizeInBytes << ", was: " << fileSize,
        arraySizeInBytes - static_cast<size_t>(fileSize) != 0
    );

    fs.read(buff, static_cast<std::streamsize>(arraySizeInBytes));

    fs.close();
}

//------------------------------------------------------------------------------

std::string ArrayReader::extension()
{
    return ".raw";
}

//------------------------------------------------------------------------------

} // namespace reader

} // namespace sight::io::base
