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

#include "io/base/writer/Matrix4Writer.hpp"

#include "io/base/writer/registry/macros.hpp"

#include <filesystem>
#include <fstream>
#include <iostream>
#include <vector>

SIGHT_REGISTER_IO_WRITER(sight::io::base::writer::Matrix4Writer);

namespace sight::io::base::writer
{

//------------------------------------------------------------------------------

Matrix4Writer::Matrix4Writer(io::base::writer::IObjectWriter::Key /*unused*/)
{
}

//------------------------------------------------------------------------------

Matrix4Writer::~Matrix4Writer()
= default;

//------------------------------------------------------------------------------

void Matrix4Writer::write()
{
    std::fstream file;
    file.open(getFile().string().c_str(), std::fstream::out);
    if(!file.is_open())
    {
        SIGHT_ERROR("Trf file writing error for " << getFile());
        return;
    }

    int i = 0;
    for(auto val : *(this->getConcreteObject()))
    {
        file << val;
        i++;
        if(i % 4 == 0 && i != 16)
        {
            file << std::endl;
        }
        else
        {
            file << " ";
        }
    }

    file.close();
}

//------------------------------------------------------------------------------

std::string Matrix4Writer::extension() const
{
    static std::string ext(".trf");
    return ext;
}

//------------------------------------------------------------------------------

} // namespace sight::io::base::writer
