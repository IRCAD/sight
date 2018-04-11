/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2018.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "fwDataIO/writer/GzArrayWriter.hpp"

#include "fwDataIO/writer/registry/macros.hpp"

#include <fwDataTools/helper/ArrayGetter.hpp>

#include <boost/filesystem/path.hpp>

#include <zlib.h>

#include <iostream>

fwDataIOWriterRegisterMacro( ::fwDataIO::writer::GzArrayWriter);

namespace fwDataIO
{
namespace writer
{

//------------------------------------------------------------------------------

GzArrayWriter::GzArrayWriter(::fwDataIO::writer::IObjectWriter::Key key) :
    ::fwData::location::enableSingleFile< ::fwDataIO::writer::IObjectWriter >(this)
{
}

//------------------------------------------------------------------------------

GzArrayWriter::~GzArrayWriter()
{
}

//------------------------------------------------------------------------------

void GzArrayWriter::write()
{
    assert( getFile().empty() == false );

    ::fwData::Array::csptr array = this->getConcreteObject();

    /// test if can open archive
    gzFile rawFile = gzopen( this->getFile().string().c_str(), "wb1");
    if ( rawFile == 0 )
    {
        std::string str = "GzArrayWriter::write unable to open ";
        str += getFile().string();
        gzclose(rawFile);
        throw std::ios_base::failure(str);
    }

    ::fwDataTools::helper::ArrayGetter arrayHelper(array);
    // file is OK : process now
    const size_t arraySizeInBytes = array->getSizeInBytes();

    const unsigned int uncompressedbyteswrited = gzwrite(rawFile, arrayHelper.getBuffer(), arraySizeInBytes);
    gzclose(rawFile);
    if ( uncompressedbyteswrited != arraySizeInBytes )
    {
        std::string str = "GzArrayWriter::write unable to write ";
        str += getFile().string();
        throw std::ios_base::failure(str);
    }
}

//------------------------------------------------------------------------------

std::string GzArrayWriter::extension()
{
    return ".raw.gz";
}

//------------------------------------------------------------------------------

} // namespace writer

} // namespace fwDataIO
