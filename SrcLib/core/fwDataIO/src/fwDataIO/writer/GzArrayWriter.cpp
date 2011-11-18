/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <iostream>
#include <boost/filesystem/path.hpp>
#include <boost/cstdint.hpp>

#include <zlib.h>

#include <fwTools/ClassRegistrar.hpp>

#include "fwDataIO/writer/GzArrayWriter.hpp"


REGISTER_BINDING_BYCLASSNAME( ::fwDataIO::writer::IObjectWriter , ::fwDataIO::writer::GzArrayWriter,  ::fwDataIO::writer::GzArrayWriter);

namespace fwDataIO
{
namespace writer
{

//------------------------------------------------------------------------------

GzArrayWriter::GzArrayWriter()
: ::fwData::location::enableSingleFile< ::fwDataIO::writer::IObjectWriter >(this)
{}

//------------------------------------------------------------------------------

GzArrayWriter::~GzArrayWriter()
{}

//------------------------------------------------------------------------------

void GzArrayWriter::write()
{
    assert( getFile().empty() ==  false );

    ::fwData::Array::sptr array = this->getConcreteObject();

    /// test if can open archive
    gzFile rawFile = gzopen( this->getFile().string().c_str(), "wb1");
    if ( rawFile == 0 )
    {
        std::string str = "GzArrayWriter::write unable to open ";
        str+= getFile().string();
        gzclose(rawFile);
        throw std::ios_base::failure(str);
    }

    // file is OK : process now
    size_t arraySizeInBytes = array->getElementSizeInBytes();

    unsigned int uncompressedbyteswrited = gzwrite(rawFile, array->getBuffer(), arraySizeInBytes);
    gzclose(rawFile);
    if ( uncompressedbyteswrited != arraySizeInBytes )
    {
        std::string str = "GzArrayWriter::write unable to write ";
        str+=  getFile().string();
        throw std::ios_base::failure(str);
    }
}

//------------------------------------------------------------------------------

std::string  GzArrayWriter::extension()
{
    return ".raw.gz";
}

//------------------------------------------------------------------------------

} // namespace writer

} // namespace fwDataIO
