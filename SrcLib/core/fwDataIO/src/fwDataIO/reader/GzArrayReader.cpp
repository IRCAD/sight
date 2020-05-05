/************************************************************************
 *
 * Copyright (C) 2009-2020 IRCAD France
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

#include "fwDataIO/reader/GzArrayReader.hpp"

#include "fwDataIO/reader/registry/macros.hpp"

#include <zlib.h>

#include <iostream>

fwDataIOReaderRegisterMacro( ::fwDataIO::reader::GzArrayReader );

namespace fwDataIO
{

namespace reader
{

//------------------------------------------------------------------------------

GzArrayReader::GzArrayReader(::fwDataIO::reader::IObjectReader::Key) :
    ::fwData::location::enableSingleFile< IObjectReader >(this)
{
}

//------------------------------------------------------------------------------

GzArrayReader::~GzArrayReader()
{
}

//------------------------------------------------------------------------------

void GzArrayReader::read()
{
    assert( ::fwData::location::SingleFile::dynamicCast(m_location) );
    std::filesystem::path file = ::fwData::location::SingleFile::dynamicCast(m_location)->getPath();

    assert( file.empty() == false );

    ::fwData::Array::sptr array = this->getConcreteObject();
    size_t arraySizeInBytes = array->resize(array->getSize());
    const auto dumpLock     = array->lock();

    void* buff = array->getBuffer();

    gzFile rawFile = gzopen(file.string().c_str(), "rb");
    if ( rawFile == 0 )
    {
        gzclose(rawFile);
        std::string str = "Unable to open ";
        str += file.string();
        throw std::ios_base::failure(str);
    }

    const int uncompressedBytesReaded = gzread(rawFile, buff, static_cast<unsigned int>(arraySizeInBytes));
    gzclose(rawFile);
    if ( uncompressedBytesReaded != static_cast<int>(arraySizeInBytes) )
    {
        std::string str = "Unable to read ";
        str += file.string();
        throw std::ios_base::failure(str);
    }
}

//------------------------------------------------------------------------------

std::string GzArrayReader::extension()
{
    return (".raw.gz");
}

//------------------------------------------------------------------------------

} // namespace reader
} // namespace fwDataIO
