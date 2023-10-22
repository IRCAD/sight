/************************************************************************
 *
 * Copyright (C) 2009-2023 IRCAD France
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

#include "io/__/writer/gz_buffer_image_writer.hpp"

#include "io/__/writer/registry/macros.hpp"

#include <data/image.hpp>

#include <zlib.h>

#include <iostream>

SIGHT_REGISTER_IO_WRITER(sight::io::writer::gz_buffer_image_writer);

namespace sight::io::writer
{

//------------------------------------------------------------------------------

void gz_buffer_image_writer::write()
{
    SIGHT_ASSERT("File path is empty.", get_file().empty() == false);

    data::image::csptr image = getConcreteObject();

    /// test if can open archive
    gzFile raw_file = gzopen(get_file().string().c_str(), "wb1");
    SIGHT_ASSERT("rawFile not instanced", raw_file);
    if(raw_file == nullptr)
    {
        std::string str = "gz_buffer_image_writer::write unable to open ";
        str += get_file().string();
        gzclose(raw_file);
        throw std::ios_base::failure(str);
    }

    const auto dump_lock = image->dump_lock();

    // file is OK : process now
    const std::size_t image_size_in_bytes = image->getSizeInBytes();

    const char* ptr           = static_cast<const char*>(image->buffer());
    std::size_t written_bytes = 0;

    int uncompressed_bytes_written = 0;

    while(written_bytes < image_size_in_bytes
          && (uncompressed_bytes_written =
                  gzwrite(
                      raw_file,
                      ptr + written_bytes,
                      static_cast<unsigned int>(image_size_in_bytes - written_bytes)
                  )
          ) > 0)
    {
        written_bytes += static_cast<std::size_t>(uncompressed_bytes_written);
    }

    gzclose(raw_file);

    assert(uncompressed_bytes_written != 0 && written_bytes == image_size_in_bytes);

    if(uncompressed_bytes_written != 0 && written_bytes == image_size_in_bytes)
    {
        std::string str = "gz_buffer_image_writer::write unable to write ";
        str += get_file().string();
        throw std::ios_base::failure(str);
    }
}

//------------------------------------------------------------------------------

std::string gz_buffer_image_writer::extension() const
{
    return ".raw.gz";
}

//------------------------------------------------------------------------------

} // namespace sight::io::writer
