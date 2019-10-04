/************************************************************************
 *
 * Copyright (C) 2009-2019 IRCAD France
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

#include "fwDataIO/reader/ArrayReader.hpp"

#include "fwDataIO/reader/registry/macros.hpp"

#include <fwCore/Exception.hpp>

#include <fwDataTools/helper/Array.hpp>

#include <fstream>
#include <iostream>

fwDataIOReaderRegisterMacro( ::fwDataIO::reader::ArrayReader );

namespace fwDataIO
{

namespace reader
{

//------------------------------------------------------------------------------

ArrayReader::ArrayReader(::fwDataIO::reader::IObjectReader::Key key) :
    ::fwData::location::enableSingleFile<
        IObjectReader >(this)
{
}

//------------------------------------------------------------------------------

ArrayReader::~ArrayReader()
{
}

//------------------------------------------------------------------------------

void ArrayReader::read()
{
    assert( ::fwData::location::SingleFile::dynamicCast(m_location) );
    std::filesystem::path file = ::fwData::location::SingleFile::dynamicCast(m_location)->getPath();

    ::fwData::Array::sptr array = this->getConcreteObject();
    ::fwDataTools::helper::Array arrayHelper(array);

    size_t arraySizeInBytes = array->resize(array->getSize());
    char* buff              = arrayHelper.begin();

    std::ifstream fs(file.string().c_str(), std::ios::in|std::ios::binary|std::ios::ate);

    FW_RAISE_IF("Unable to read " << file, !fs.good());

    std::streampos fileSize = fs.tellg();
    fs.seekg(0, std::ios::beg);

    FW_RAISE_IF(file << ": Bad file size, expected: " << arraySizeInBytes << ", was: " << fileSize,
                arraySizeInBytes - fileSize != 0);

    fs.read(buff, arraySizeInBytes);

    fs.close();
}

//------------------------------------------------------------------------------

std::string ArrayReader::extension()
{
    return (".raw");
}

//------------------------------------------------------------------------------

} // namespace reader
} // namespace fwDataIO
