/************************************************************************
 *
 * Copyright (C) 2009-2021 IRCAD France
 * Copyright (C) 2012-2020 IHU Strasbourg
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

#include "io/base/writer/GzArrayWriter.hpp"

#include "io/base/writer/registry/macros.hpp"

#include <zlib.h>

#include <filesystem>
#include <iostream>

SIGHT_REGISTER_IO_WRITER(::sight::io::base::writer::GzArrayWriter);

namespace sight::io::base
{

namespace writer
{

//------------------------------------------------------------------------------

GzArrayWriter::GzArrayWriter(io::base::writer::IObjectWriter::Key)
{
}

//------------------------------------------------------------------------------

GzArrayWriter::~GzArrayWriter()
{
}

//------------------------------------------------------------------------------

void GzArrayWriter::write()
{
    SIGHT_ASSERT("File path is empty.", getFile().empty() == false);

    data::Array::csptr array = this->getConcreteObject();

    /// test if can open archive
    gzFile rawFile = gzopen(this->getFile().string().c_str(), "wb1");
    if(rawFile == 0)
    {
        std::string str = "GzArrayWriter::write unable to open ";
        str += getFile().string();
        gzclose(rawFile);
        throw std::ios_base::failure(str);
    }

    const auto dumpLock = array->lock();

    // file is OK : process now
    const size_t arraySizeInBytes = array->getSizeInBytes();

    const int uncompressedbyteswrited =
        gzwrite(rawFile, array->getBuffer(), static_cast<unsigned int>(arraySizeInBytes));
    gzclose(rawFile);
    if(uncompressedbyteswrited != static_cast<int>(arraySizeInBytes))
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

} // namespace sight::io::base
