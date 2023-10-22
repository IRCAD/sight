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

#include "io/__/writer/gz_array_writer.hpp"

#include "io/__/writer/registry/macros.hpp"

#include <zlib.h>

#include <filesystem>
#include <iostream>

SIGHT_REGISTER_IO_WRITER(sight::io::writer::gz_array_writer);

namespace sight::io::writer
{

//------------------------------------------------------------------------------

void gz_array_writer::write()
{
    SIGHT_ASSERT("File path is empty.", get_file().empty() == false);

    data::array::csptr array = this->getConcreteObject();

    /// test if can open archive
    gzFile raw_file = gzopen(this->get_file().string().c_str(), "wb1");
    if(raw_file == nullptr)
    {
        std::string str = "gz_array_writer::write unable to open ";
        str += get_file().string();
        gzclose(raw_file);
        throw std::ios_base::failure(str);
    }

    const auto dump_lock = array->dump_lock();

    // file is OK : process now
    const std::size_t array_size_in_bytes = array->getSizeInBytes();

    const int uncompressed_bytes_written =
        gzwrite(raw_file, array->buffer(), static_cast<unsigned int>(array_size_in_bytes));
    gzclose(raw_file);
    if(uncompressed_bytes_written != static_cast<int>(array_size_in_bytes))
    {
        std::string str = "gz_array_writer::write unable to write ";
        str += get_file().string();
        throw std::ios_base::failure(str);
    }
}

//------------------------------------------------------------------------------

std::string gz_array_writer::extension() const
{
    return ".raw.gz";
}

//------------------------------------------------------------------------------

} // namespace sight::io::writer
