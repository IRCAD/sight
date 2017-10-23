/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

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

