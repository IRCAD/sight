/************************************************************************
 *
 * Copyright (C) 2009-2022 IRCAD France
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

SIGHT_REGISTER_IO_WRITER(sight::io::base::writer::GzArrayWriter);

namespace sight::io::base::writer
{

//------------------------------------------------------------------------------

GzArrayWriter::GzArrayWriter(io::base::writer::IObjectWriter::Key /*unused*/)
{
}

//------------------------------------------------------------------------------

GzArrayWriter::~GzArrayWriter()
= default;

//------------------------------------------------------------------------------

void GzArrayWriter::write()
{
    SIGHT_ASSERT("File path is empty.", getFile().empty() == false);

    data::Array::csptr array = this->getConcreteObject();

    /// test if can open archive
    gzFile rawFile = gzopen(this->getFile().string().c_str(), "wb1");
    if(rawFile == nullptr)
    {
        std::string str = "GzArrayWriter::write unable to open ";
        str += getFile().string();
        gzclose(rawFile);
        throw std::ios_base::failure(str);
    }

    const auto dumpLock = array->dump_lock();

    // file is OK : process now
    const std::size_t arraySizeInBytes = array->getSizeInBytes();

    const int uncompressed_bytes_written =
        gzwrite(rawFile, array->getBuffer(), static_cast<unsigned int>(arraySizeInBytes));
    gzclose(rawFile);
    if(uncompressed_bytes_written != static_cast<int>(arraySizeInBytes))
    {
        std::string str = "GzArrayWriter::write unable to write ";
        str += getFile().string();
        throw std::ios_base::failure(str);
    }
}

//------------------------------------------------------------------------------

std::string GzArrayWriter::extension() const
{
    return ".raw.gz";
}

//------------------------------------------------------------------------------

} // namespace sight::io::base::writer
