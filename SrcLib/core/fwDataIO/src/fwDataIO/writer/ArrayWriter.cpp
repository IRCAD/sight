/************************************************************************
 *
 * Copyright (C) 2009-2019 IRCAD France
 * Copyright (C) 2012-2019 IHU Strasbourg
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

#include "fwDataIO/writer/ArrayWriter.hpp"

#include "fwDataIO/writer/registry/macros.hpp"

#include <fwDataTools/helper/ArrayGetter.hpp>

#include <filesystem>

#include <fstream>
#include <iostream>

fwDataIOWriterRegisterMacro( ::fwDataIO::writer::ArrayWriter);

namespace fwDataIO
{
namespace writer
{

//------------------------------------------------------------------------------

ArrayWriter::ArrayWriter(::fwDataIO::writer::IObjectWriter::Key key) :
    ::fwData::location::enableSingleFile< ::fwDataIO::writer::IObjectWriter >(this)
{
}

//------------------------------------------------------------------------------

ArrayWriter::~ArrayWriter()
{
}

//------------------------------------------------------------------------------

void ArrayWriter::write()
{
    std::filesystem::path file = getFile();

    ::fwData::Array::csptr array = this->getConcreteObject();
    size_t arraySizeInBytes = array->getSizeInBytes();
    ::fwDataTools::helper::ArrayGetter arrayHelper(array);
    const char* buff = arrayHelper.begin();

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
