/************************************************************************
 *
 * Copyright (C) 2017-2023 IRCAD France
 * Copyright (C) 2017 IHU Strasbourg
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

#include "io/__/reader/csv_reader.hpp"

#include <core/exceptionmacros.hpp>
#include <core/spy_log.hpp>

#include <boost/tokenizer.hpp>

#include <fstream>
#include <string>

namespace sight::io::reader
{

//------------------------------------------------------------------------------

csv_reader::csv_reader(const std::filesystem::path& _csv_path) :
    m_stream(_csv_path)
{
    SIGHT_THROW_IF(
        "CSV File Path Directory can not be Found",
        !std::filesystem::exists(_csv_path)
    );
}

//------------------------------------------------------------------------------

csv_reader::~csv_reader()
= default;

//------------------------------------------------------------------------------

using char_separator_t = boost::char_separator<char>;
using tokenizer_t      = boost::tokenizer<char_separator_t>;

//------------------------------------------------------------------------------

bool csv_reader::eof() const
{
    return m_stream.eof();
}

//------------------------------------------------------------------------------

csv_reader::token_container_t csv_reader::get_line(const std::string& _separator)
{
    std::string line;
    token_container_t tokens;
    const char_separator_t sep(_separator.c_str(), "", boost::keep_empty_tokens);

    if(std::getline(m_stream, line))
    {
        if(!line.empty() && line.back() == '\n')
        {
            line.pop_back();
        }

        if(!line.empty() && line.back() == '\r')
        {
            line.pop_back();
        }

        tokenizer_t tokenizer(line, sep);
        tokens.assign(tokenizer.begin(), tokenizer.end());
    }

    return tokens;
}

//------------------------------------------------------------------------------

} // namespace sight::io::reader
