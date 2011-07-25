/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <iostream>
#include <boost/cstdint.hpp>

#include <zlib.h>
#include <fwData/Object.hpp>
#include <fwData/Image.hpp>

#include <fwTools/ClassRegistrar.hpp>

#include "fwDataIO/writer/GzBufferImageWriter.hpp"



REGISTER_BINDING_BYCLASSNAME( ::fwDataIO::writer::IObjectWriter , ::fwDataIO::writer::GzBufferImageWriter,  ::fwDataIO::writer::GzBufferImageWriter);


namespace fwDataIO
{

namespace writer
{

//------------------------------------------------------------------------------

GzBufferImageWriter::GzBufferImageWriter()
: ::fwData::location::enableSingleFile< ::fwDataIO::writer::IObjectWriter >(this)
{}

//------------------------------------------------------------------------------

GzBufferImageWriter::~GzBufferImageWriter()
{}

//------------------------------------------------------------------------------

void GzBufferImageWriter::write()
{
    assert( getFile().empty() ==  false );

    ::fwData::Image::sptr image = getConcreteObject();
    OSLM_TRACE( "GzBufferImageWriter::write()" << image.get() << " " << image->className()) ;

    /// test if can open archive
    gzFile rawFile = gzopen( getFile().string().c_str(), "wb1");
    SLM_ASSERT("rawFile not instanced", rawFile);
    if ( rawFile == 0 )
    {
        std::string str = "GzBufferImageWriter::write unable to open ";
        str+= getFile().string();
        gzclose(rawFile);
        throw std::ios_base::failure(str);
    }

    // file is OK : process now
    ::boost::uint32_t imageSizeInBytes = ::fwData::imageSizeInBytes(*image);

    unsigned int uncompressedbyteswrited=gzwrite(rawFile,image->getBuffer(),imageSizeInBytes);
    assert(uncompressedbyteswrited==imageSizeInBytes);

    if ( uncompressedbyteswrited!=imageSizeInBytes )
    {
        std::string str = "GzBufferImageWriter::write unable to write ";
        str+=  getFile().string();
        gzclose(rawFile);
        throw std::ios_base::failure(str);
    }

    gzclose(rawFile);
}

//------------------------------------------------------------------------------

std::string  GzBufferImageWriter::extension()
{
    return ".raw.gz";
}

//------------------------------------------------------------------------------

} // namespace writer

} // namespace fwDataIO
