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

#include "io/__/writer/array_writer.hpp"

#include "io/__/writer/registry/macros.hpp"

#include <filesystem>
#include <fstream>
#include <iostream>

SIGHT_REGISTER_IO_WRITER(sight::io::writer::array_writer);

namespace sight::io::writer
{

//------------------------------------------------------------------------------

void array_writer::write()
{
    std::filesystem::path file = get_file();

    data::array::csptr array        = this->getConcreteObject();
    std::size_t array_size_in_bytes = array->getSizeInBytes();
    const auto dump_lock            = array->dump_lock();
    const char* buff                = static_cast<const char*>(array->buffer());

    std::ofstream fs(file.string().c_str(), std::ios::binary | std::ios::trunc);

    SIGHT_THROW_IF("Unable to open " << file, !fs.good());

    fs.write(buff, static_cast<std::streamsize>(array_size_in_bytes));
    fs.close();
}

//------------------------------------------------------------------------------

std::string array_writer::extension() const
{
    return ".raw";
}

//------------------------------------------------------------------------------

} // namespace sight::io::writer
