/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "fwDataIO/writer/GzBufferImageWriter.hpp"

#include "fwDataIO/writer/registry/macros.hpp"

#include <fwData/Image.hpp>

#include <fwDataTools/helper/Image.hpp>

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

    ::fwData::Image::sptr image = getConcreteObject();
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

    ::fwDataTools::helper::Image imageHelper(image);

    // file is OK : process now
    size_t imageSizeInBytes = image->getSizeInBytes();

    char* ptr           = static_cast<char*>(imageHelper.getBuffer());
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
