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

#include "io/__/reader/matrix4_reader.hpp"

#include "io/__/reader/registry/macros.hpp"

#include <cmath>
#include <filesystem>
#include <fstream>
#include <iostream>

SIGHT_REGISTER_IO_READER(sight::io::reader::matrix4_reader);

namespace sight::io::reader
{

//------------------------------------------------------------------------------

void matrix4_reader::read()
{
    std::filesystem::path file = this->get_file();

    assert(std::filesystem::exists(file));

    std::ifstream in_file(file.string().c_str(), std::ifstream::in);
    assert(in_file.good());

    char read_value = 0;
    double value    = NAN;

    while(!in_file.eof() && read_value < 16)
    {
        in_file >> value;
        (*this->get_concrete_object())[static_cast<std::size_t>(read_value)] = value;
        read_value++;
    }

    SIGHT_ASSERT("Wrong matrix size", this->get_concrete_object()->size() == 16);
}

//------------------------------------------------------------------------------

std::string matrix4_reader::extension() const
{
    return ".trf";
}

//------------------------------------------------------------------------------

} // namespace sight::io::reader
