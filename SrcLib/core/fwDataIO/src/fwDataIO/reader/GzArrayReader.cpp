/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <iostream>
#include <boost/cstdint.hpp>

#include <zlib.h>

#include <fwTools/ClassRegistrar.hpp>

#include "fwDataIO/reader/GzArrayReader.hpp"


REGISTER_BINDING_BYCLASSNAME( ::fwDataIO::reader::IObjectReader , ::fwDataIO::reader::GzArrayReader, ::fwDataIO::reader::GzArrayReader );


namespace fwDataIO
{

namespace reader
{

//------------------------------------------------------------------------------

GzArrayReader::GzArrayReader() : ::fwData::location::enableSingleFile< IObjectReader >(this)
{}

//------------------------------------------------------------------------------

GzArrayReader::~GzArrayReader()
{}

//------------------------------------------------------------------------------

void GzArrayReader::read()
{
    assert( ::fwData::location::SingleFile::dynamicCast(m_location) );
    ::boost::filesystem::path file = ::fwData::location::SingleFile::dynamicCast(m_location)->getPath();

    assert( file.empty() ==  false );

    ::fwData::Array::sptr array = this->getConcreteObject();
    size_t arraySizeInBytes = array->resize(array->getSize());
    void* buff = array->getBuffer();

    gzFile rawFile = gzopen(file.string().c_str(), "rb");
    if ( rawFile == 0 )
    {
        gzclose(rawFile);
        std::string str = "Unable to open ";
        str+= file.string();
        throw std::ios_base::failure(str);
    }

    unsigned int uncompressedBytesReaded = gzread(rawFile, buff, arraySizeInBytes);
    gzclose(rawFile);
    if ( uncompressedBytesReaded != arraySizeInBytes )
    {
        std::string str = "Unable to read ";
        str+= file.string();
        throw std::ios_base::failure(str);
    }
}

//------------------------------------------------------------------------------

std::string  GzArrayReader::extension()
{
    return (".raw.gz");
}

//------------------------------------------------------------------------------

} // namespace reader
} // namespace fwDataIO
