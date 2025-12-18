/************************************************************************
 *
 * Copyright (C) 2009-2025 IRCAD France
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

#include "io/__/reader/gz_array_reader.hpp"

#include <core/progress/observer.hpp>

#include <zlib.h>

#include <iostream>
#include <utility>

namespace sight::io::reader
{

//------------------------------------------------------------------------------

void gz_array_reader::read(sight::core::progress::observer::sptr _progress)
{
    std::filesystem::path file = this->get_file();

    assert(file.empty() == false);

    data::array::sptr array         = this->get_concrete_object();
    std::size_t array_size_in_bytes = array->resize(array->size());
    const auto dump_lock            = array->dump_lock();

    void* buff = array->buffer();

    gzFile raw_file = gzopen(file.string().c_str(), "rb");
    if(raw_file == nullptr)
    {
        gzclose(raw_file);
        std::string str = "Unable to open ";
        str += file.string();
        _progress->done();
        throw std::ios_base::failure(str);
    }

    const int un_compressed_bytes_read = gzread(raw_file, buff, static_cast<unsigned int>(array_size_in_bytes));
    gzclose(raw_file);
    if(std::cmp_not_equal(un_compressed_bytes_read, array_size_in_bytes))
    {
        std::string str = "Unable to read ";
        str += file.string();
        _progress->done();
        throw std::ios_base::failure(str);
    }

    _progress->done();
}

//------------------------------------------------------------------------------

std::string gz_array_reader::extension() const
{
    return ".raw.gz";
}

//------------------------------------------------------------------------------

} // namespace sight::io::reader
