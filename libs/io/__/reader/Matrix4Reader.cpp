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

#include "io/__/reader/Matrix4Reader.hpp"

#include "io/__/reader/registry/macros.hpp"

#include <cmath>
#include <filesystem>
#include <fstream>
#include <iostream>

SIGHT_REGISTER_IO_READER(sight::io::reader::Matrix4Reader);

namespace sight::io::reader
{

//------------------------------------------------------------------------------

void Matrix4Reader::read()
{
    std::filesystem::path file = this->get_file();

    assert(std::filesystem::exists(file));

    std::ifstream inFile(file.string().c_str(), std::ifstream::in);
    assert(inFile.good());

    char readValue = 0;
    double value   = NAN;

    while(!inFile.eof() && readValue < 16)
    {
        inFile >> value;
        (*this->getConcreteObject())[static_cast<std::size_t>(readValue)] = value;
        readValue++;
    }

    SIGHT_ASSERT("Wrong matrix size", this->getConcreteObject()->size() == 16);
}

//------------------------------------------------------------------------------

std::string Matrix4Reader::extension() const
{
    return ".trf";
}

//------------------------------------------------------------------------------

} // namespace sight::io::reader
