/************************************************************************
 *
 * Copyright (C) 2017-2021 IRCAD France
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

#include "io/base/reader/CsvReader.hpp"

#include <core/exceptionmacros.hpp>
#include <core/spyLog.hpp>

#include <boost/tokenizer.hpp>

#include <fstream>
#include <string>

namespace sight::io::base
{

namespace reader
{

//------------------------------------------------------------------------------

CsvReader::CsvReader(const std::filesystem::path& csvPath) :
    m_stream(csvPath)
{
    SIGHT_THROW_IF(
        "CSV File Path Directory can not be Found",
        !std::filesystem::exists(csvPath)
    );
}

//------------------------------------------------------------------------------

CsvReader::~CsvReader()
{
}

//------------------------------------------------------------------------------

typedef ::boost::char_separator<char> CharSeparatorType;
typedef ::boost::tokenizer<CharSeparatorType> TokenizerType;

//------------------------------------------------------------------------------

CsvReader::TokenContainerType CsvReader::getLine(const std::string& separator)
{
    std::string line;
    TokenContainerType tokens;
    const CharSeparatorType sep(separator.c_str(), "", ::boost::keep_empty_tokens);

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

        TokenizerType tokenizer(line, sep);
        tokens.assign(tokenizer.begin(), tokenizer.end());
    }

    return tokens;
}

//------------------------------------------------------------------------------

} // namespace reader

} // namespace sight::io::base