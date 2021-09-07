/************************************************************************
 *
 * Copyright (C) 2009-2021 IRCAD France
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

bool File::contentEquals(const std::filesystem::path& lfile, const std::filesystem::path& rfile)
{
    using namespace std;
    const streambuf::int_type eof = streambuf::traits_type::eof();

    std::ifstream lstream(lfile.c_str());
    std::ifstream rstream(rfile.c_str());

    streambuf* lbuf = lstream.rdbuf();
    streambuf* rbuf = rstream.rdbuf();

    char lchar, rchar;
    while(true)
    {
        lchar = static_cast<char>(lbuf->sbumpc());
        rchar = static_cast<char>(rbuf->sbumpc());

        if(lchar == eof && rchar == eof)
        {
            return true;
        }

        if(lchar != rchar || lchar == eof || rchar == eof)
        {
            break;
        }
    }

    return false;
}

} //namespace sight::utestData
