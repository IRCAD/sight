/************************************************************************
 *
 * Copyright (C) 2017 IRCAD France
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

#include "fwGdcmIO/helper/CsvIO.hpp"

#include <fwCore/spyLog.hpp>

#include <boost/tokenizer.hpp>

#include <fstream>
#include <string>

namespace fwGdcmIO
{

namespace helper
{

//------------------------------------------------------------------------------

CsvIO::CsvIO(std::istream& csvStream) :
    m_stream(csvStream)
{
}

//------------------------------------------------------------------------------

CsvIO::~CsvIO()
{
}

//------------------------------------------------------------------------------

typedef ::boost::char_separator<char> CharSeparatorType;
typedef ::boost::tokenizer< CharSeparatorType > TokenizerType;

//------------------------------------------------------------------------------

CsvIO::TokenContainerType CsvIO::getLine(const std::string& separator)
{
    std::string line;
    TokenContainerType tokens;
    const CharSeparatorType sep(separator.c_str(), "", ::boost::keep_empty_tokens);

    if(std::getline(m_stream, line))
    {
        TokenizerType tokenizer(line, sep);
        tokens.assign(tokenizer.begin(), tokenizer.end());
    }

    return tokens;
}

//------------------------------------------------------------------------------

} // namespace helper
} // namespace fwGdcmIO

