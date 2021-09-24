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

#include "io/base/writer/GzBufferImageWriter.hpp"

#include "io/base/writer/registry/macros.hpp"

#include <data/Image.hpp>

#include <zlib.h>

#include <iostream>

SIGHT_REGISTER_IO_WRITER(sight::io::base::writer::GzBufferImageWriter);

namespace sight::io::base
{

namespace writer
{

//------------------------------------------------------------------------------

GzBufferImageWriter::GzBufferImageWriter(io::base::writer::IObjectWriter::Key)
{
}

//------------------------------------------------------------------------------

GzBufferImageWriter::~GzBufferImageWriter()
{
}

//------------------------------------------------------------------------------

void GzBufferImageWriter::write()
{
    SIGHT_ASSERT("File path is empty.", getFile().empty() == false);

    data::Image::csptr image = getConcreteObject();

    /// test if can open archive
    gzFile rawFile = gzopen(getFile().string().c_str(), "wb1");
    SIGHT_ASSERT("rawFile not instanced", rawFile);
    if(rawFile == 0)
    {
        std::string str = "GzBufferImageWriter::write unable to open ";
        str += getFile().string();
        gzclose(rawFile);
        throw std::ios_base::failure(str);
    }

    const auto dumpLock = image->lock();

    // file is OK : process now
    const size_t imageSizeInBytes = image->getSizeInBytes();

    const char* ptr     = static_cast<char*>(image->getBuffer());
    size_t writtenBytes = 0;

    int uncompressedbyteswrited = 0;

    while(writtenBytes < imageSizeInBytes
          && (uncompressedbyteswrited =
                  gzwrite(rawFile, ptr + writtenBytes, static_cast<unsigned int>(imageSizeInBytes - writtenBytes))) > 0)
    {
        writtenBytes += static_cast<size_t>(uncompressedbyteswrited);
    }

    gzclose(rawFile);

    assert(uncompressedbyteswrited != 0 && writtenBytes == imageSizeInBytes);

    if(uncompressedbyteswrited != 0 && writtenBytes == imageSizeInBytes)
    {
        std::string str = "GzBufferImageWriter::write unable to write ";
        str += getFile().string();
        throw std::ios_base::failure(str);
    }
}

//------------------------------------------------------------------------------

std::string GzBufferImageWriter::extension() const
{
    return ".raw.gz";
}

//------------------------------------------------------------------------------

} // namespace writer

} // namespace sight::io::base
