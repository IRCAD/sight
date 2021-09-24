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

#include "io/base/reader/GzBufferImageReader.hpp"

#include "io/base/reader/registry/macros.hpp"

#include <data/Image.hpp>

#include <zlib.h>

#include <iostream>

SIGHT_REGISTER_IO_READER(sight::io::base::reader::GzBufferImageReader);

namespace sight::io::base
{

namespace reader
{

//------------------------------------------------------------------------------

GzBufferImageReader::GzBufferImageReader(io::base::reader::IObjectReader::Key)
{
}

//------------------------------------------------------------------------------

GzBufferImageReader::~GzBufferImageReader()
{
}

//------------------------------------------------------------------------------

void GzBufferImageReader::read()
{
    std::filesystem::path file = this->getFile();

    assert(file.empty() == false);

    data::Image::sptr image = getConcreteObject();
    size_t imageSizeInBytes = image->getSizeInBytes();

    image->resize();
    const auto dumpLock = image->lock();
    char* ptr           = static_cast<char*>(image->getBuffer());

    gzFile rawFile = gzopen(file.string().c_str(), "rb");

    SIGHT_ASSERT("rawFile not instanced", rawFile);
    if(rawFile == 0)
    {
        std::string str = "Unable to open ";
        str += file.string();
        throw std::ios_base::failure(str);
    }

    int uncompressedbytesreaded;
    size_t readBytes = 0;

    while((uncompressedbytesreaded =
               gzread(rawFile, ptr + readBytes, static_cast<unsigned int>(imageSizeInBytes - readBytes))) > 0)
    {
        readBytes += static_cast<size_t>(uncompressedbytesreaded);
    }

    gzclose(rawFile);

    if(uncompressedbytesreaded == -1)
    {
        std::string str = "Unable to read ";
        str += file.string();
        throw std::ios_base::failure(str);
    }
}

//------------------------------------------------------------------------------

std::string GzBufferImageReader::extension() const
{
    return ".raw.gz";
}

//------------------------------------------------------------------------------

} // namespace reader

} // namespace sight::io::base
