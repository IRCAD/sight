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

#include "File.hpp"

#include <filesystem>
#include <fstream>

namespace sight::utestData
{

//------------------------------------------------------------------------------

bool File::contentEquals(const std::filesystem::path& l_file, const std::filesystem::path& r_file)
{
    using namespace std;
    const streambuf::int_type eof = streambuf::traits_type::eof();

    std::ifstream l_stream(l_file.c_str());
    std::ifstream r_stream(r_file.c_str());

    streambuf* l_buf = l_stream.rdbuf();
    streambuf* r_buf = r_stream.rdbuf();

    char l_char, r_char;
    while(true)
    {
        l_char = static_cast<char>(l_buf->sbumpc());
        r_char = static_cast<char>(r_buf->sbumpc());

        if(l_char == eof && r_char == eof)
        {
            return true;
        }

        if(l_char != r_char || l_char == eof || r_char == eof)
        {
            break;
        }
    }

    return false;
}

} //namespace sight::utestData
