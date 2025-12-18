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

#include "io/__/reader/matrix4_reader.hpp"

#include "core/exceptionmacros.hpp"

#include <core/progress/observer.hpp>

#include <cmath>
#include <filesystem>
#include <fstream>
#include <iomanip>
#include <iostream>

namespace sight::io::reader
{

//------------------------------------------------------------------------------

void matrix4_reader::read(sight::core::progress::observer::sptr _progress)
{
    std::filesystem::path file = this->get_file();

    SIGHT_THROW_IF("File " << std::quoted(file.string()) << " does not exist", !std::filesystem::exists(file));

    std::ifstream in_file(file.string().c_str(), std::ifstream::in);
    SIGHT_THROW_IF("Failed to open file " << std::quoted(file.string()), !in_file.good());

    char read_value = 0;
    double value    = NAN;

    while(!in_file.eof() && read_value < 16)
    {
        in_file >> value;
        (*this->get_concrete_object())[static_cast<std::size_t>(read_value)] = value;
        read_value++;
    }

    SIGHT_THROW_IF("Wrong matrix size", this->get_concrete_object()->size() != 16);
    _progress->done();
}

//------------------------------------------------------------------------------

std::string matrix4_reader::extension() const
{
    return ".trf";
}

//------------------------------------------------------------------------------

} // namespace sight::io::reader
