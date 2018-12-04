/************************************************************************
 *
 * Copyright (C) 2009-2017 IRCAD France
 * Copyright (C) 2012-2017 IHU Strasbourg
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

#include "fwDataIO/reader/GzBufferImageReader.hpp"

#include "fwDataIO/reader/registry/macros.hpp"

#include <fwData/Image.hpp>
#include <fwData/location/SingleFile.hpp>

#include <fwDataTools/helper/Image.hpp>

#include <zlib.h>

#include <iostream>

fwDataIOReaderRegisterMacro( ::fwDataIO::reader::GzBufferImageReader );

namespace fwDataIO
{

namespace reader
{

//------------------------------------------------------------------------------

GzBufferImageReader::GzBufferImageReader(::fwDataIO::reader::IObjectReader::Key key) :
    ::fwData::location::
    enableSingleFile< IObjectReader >(
        this)
{
}

//------------------------------------------------------------------------------

GzBufferImageReader::~GzBufferImageReader()
{
}

//------------------------------------------------------------------------------

void GzBufferImageReader::read()
{
    assert( ::fwData::location::SingleFile::dynamicCast(m_location) );
    ::boost::filesystem::path file = ::fwData::location::SingleFile::dynamicCast(m_location)->getPath();

    assert( file.empty() == false );

    ::fwData::Image::sptr image = getConcreteObject();
    size_t imageSizeInBytes = image->getSizeInBytes();

    image->allocate();
    ::fwDataTools::helper::Image helper(image);
    char* ptr = static_cast<char*>(helper.getBuffer());

    gzFile rawFile = gzopen(file.string().c_str(), "rb");

    SLM_ASSERT("rawFile not instanced", rawFile);
    if ( rawFile == 0 )
    {
        std::string str = "Unable to open ";
        str += file.string();
        throw std::ios_base::failure(str);
    }

    int uncompressedbytesreaded;
    size_t readBytes = 0;

    while ( (uncompressedbytesreaded = gzread(rawFile, ptr + readBytes, imageSizeInBytes - readBytes)) > 0 )
    {
        readBytes += uncompressedbytesreaded;
    }

    gzclose(rawFile);

    if ( uncompressedbytesreaded == -1 )
    {
        std::string str = "Unable to read ";
        str += file.string();
        throw std::ios_base::failure(str);
    }
}

//------------------------------------------------------------------------------

std::string GzBufferImageReader::extension()
{
    return (".raw.gz");
}

//------------------------------------------------------------------------------

} // namespace reader

} // namespace fwDataIO
