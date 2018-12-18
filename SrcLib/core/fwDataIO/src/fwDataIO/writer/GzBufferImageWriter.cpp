/************************************************************************
 *
 * Copyright (C) 2009-2018 IRCAD France
 * Copyright (C) 2012-2018 IHU Strasbourg
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

#include "fwDataIO/writer/GzBufferImageWriter.hpp"

#include "fwDataIO/writer/registry/macros.hpp"

#include <fwData/Image.hpp>

#include <fwDataTools/helper/ImageGetter.hpp>

#include <zlib.h>

#include <iostream>

fwDataIOWriterRegisterMacro( ::fwDataIO::writer::GzBufferImageWriter);

namespace fwDataIO
{

namespace writer
{

//------------------------------------------------------------------------------

GzBufferImageWriter::GzBufferImageWriter(::fwDataIO::writer::IObjectWriter::Key key) :
    ::fwData::location::enableSingleFile< ::fwDataIO::writer::IObjectWriter >(this)
{
}

//------------------------------------------------------------------------------

GzBufferImageWriter::~GzBufferImageWriter()
{
}

//------------------------------------------------------------------------------

void GzBufferImageWriter::write()
{
    assert( getFile().empty() == false );

    ::fwData::Image::csptr image = getConcreteObject();
    OSLM_TRACE( "GzBufferImageWriter::write()" << image.get() << " " << image->getClassname());

    /// test if can open archive
    gzFile rawFile = gzopen( getFile().string().c_str(), "wb1");
    SLM_ASSERT("rawFile not instanced", rawFile);
    if ( rawFile == 0 )
    {
        std::string str = "GzBufferImageWriter::write unable to open ";
        str += getFile().string();
        gzclose(rawFile);
        throw std::ios_base::failure(str);
    }

    ::fwDataTools::helper::ImageGetter imageHelper(image);

    // file is OK : process now
    const size_t imageSizeInBytes = image->getSizeInBytes();

    const char* ptr     = static_cast<char*>(imageHelper.getBuffer());
    size_t writtenBytes = 0;

    int uncompressedbyteswrited;

    while ( writtenBytes < imageSizeInBytes
            && (uncompressedbyteswrited = gzwrite(rawFile, ptr+writtenBytes, imageSizeInBytes-writtenBytes)) > 0 )
    {
        writtenBytes += uncompressedbyteswrited;
    }

    gzclose(rawFile);

    assert( uncompressedbyteswrited != 0 && writtenBytes == imageSizeInBytes);

    if ( uncompressedbyteswrited != 0 && writtenBytes == imageSizeInBytes)
    {
        std::string str = "GzBufferImageWriter::write unable to write ";
        str += getFile().string();
        throw std::ios_base::failure(str);
    }
}

//------------------------------------------------------------------------------

std::string GzBufferImageWriter::extension()
{
    return ".raw.gz";
}

//------------------------------------------------------------------------------

} // namespace writer

} // namespace fwDataIO
