/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <fstream>
#include <iostream>

#include <boost/cstdint.hpp>

#include <fwCore/Exception.hpp>

#include <fwTools/ClassRegistrar.hpp>

#include "fwDataIO/reader/ArrayReader.hpp"


REGISTER_BINDING_BYCLASSNAME( ::fwDataIO::reader::IObjectReader , ::fwDataIO::reader::ArrayReader, ::fwDataIO::reader::ArrayReader );


namespace fwDataIO
{

namespace reader
{

//------------------------------------------------------------------------------

ArrayReader::ArrayReader() : ::fwData::location::enableSingleFile< IObjectReader >(this)
{}

//------------------------------------------------------------------------------

ArrayReader::~ArrayReader()
{}

//------------------------------------------------------------------------------

void ArrayReader::read()
{
    assert( ::fwData::location::SingleFile::dynamicCast(m_location) );
    ::boost::filesystem::path file = ::fwData::location::SingleFile::dynamicCast(m_location)->getPath();

    ::fwData::Array::sptr array = this->getConcreteObject();
    size_t arraySizeInBytes = array->resize(array->getSize());
    char* buff = array->begin();

    std::ifstream fs(file.string().c_str(), std::ios::in|std::ios::binary|std::ios::ate);

    FW_RAISE_IF("Unable to read " << file, !fs.good());

    std::streampos fileSize = fs.tellg();
    fs.seekg(0, std::ios::beg);

    FW_RAISE_IF(file << ": Bad file size, expected: " << arraySizeInBytes << ", was: " << fileSize,
            arraySizeInBytes - fileSize == 0);

    fs.read(buff, arraySizeInBytes);

    fs.close();
}

//------------------------------------------------------------------------------

std::string  ArrayReader::extension()
{
    return (".raw");
}

//------------------------------------------------------------------------------

} // namespace reader
} // namespace fwDataIO
