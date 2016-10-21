/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "fwDataIO/writer/ArrayWriter.hpp"

#include "fwDataIO/writer/registry/macros.hpp"

#include <fwDataTools/helper/Array.hpp>

#include <boost/cstdint.hpp>
#include <boost/filesystem/path.hpp>

#include <fstream>
#include <iostream>


fwDataIOWriterRegisterMacro( ::fwDataIO::writer::ArrayWriter);

namespace fwDataIO
{
namespace writer
{

//------------------------------------------------------------------------------

ArrayWriter::ArrayWriter(::fwDataIO::writer::IObjectWriter::Key key)
    : ::fwData::location::enableSingleFile< ::fwDataIO::writer::IObjectWriter >(this)
{
}

//------------------------------------------------------------------------------

ArrayWriter::~ArrayWriter()
{
}

//------------------------------------------------------------------------------

void ArrayWriter::write()
{
    ::boost::filesystem::path file = getFile();


    ::fwData::Array::sptr array = this->getConcreteObject();
    size_t arraySizeInBytes = array->getSizeInBytes();
    ::fwDataTools::helper::Array arrayHelper(array);
    char* buff = arrayHelper.begin();

    std::ofstream fs(file.string().c_str(), std::ios::binary|std::ios::trunc);

    FW_RAISE_IF("Unable to open " << file, !fs.good());

    fs.write(buff, arraySizeInBytes);
    fs.close();

}

//------------------------------------------------------------------------------

std::string ArrayWriter::extension()
{
    return ".raw";
}

//------------------------------------------------------------------------------

} // namespace writer

} // namespace fwDataIO
