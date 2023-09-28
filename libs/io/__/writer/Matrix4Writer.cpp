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

#include "io/__/writer/Matrix4Writer.hpp"

#include "io/__/writer/registry/macros.hpp"

#include <filesystem>
#include <fstream>
#include <iostream>
#include <vector>

SIGHT_REGISTER_IO_WRITER(sight::io::writer::Matrix4Writer);

namespace sight::io::writer
{

//------------------------------------------------------------------------------

void Matrix4Writer::write()
{
    std::fstream file;
    file.open(get_file().string().c_str(), std::fstream::out);
    if(!file.is_open())
    {
        SIGHT_ERROR("Trf file writing error for " << get_file());
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

} // namespace sight::io::writer
