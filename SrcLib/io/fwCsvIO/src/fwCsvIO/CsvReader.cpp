/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2016-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "fwCsvIO/CsvReader.hpp"

#include <fwCore/spyLog.hpp>

#include <boost/filesystem.hpp>

#include <fstream>

namespace fwCsvIO
{

CsvReader::CsvReader(const std::string& location, SeparatorType separatorType ) :
    m_location(location),
    m_separatorType(separatorType)
{
    SLM_ASSERT("The file path is empty.", !location.empty());
}

//------------------------------------------------------------------------------

CsvReader::~CsvReader()
{
}

//------------------------------------------------------------------------------

CsvReader::CSVFileValuesType CsvReader::read() const
{
    std::ifstream file;
    // only the input is needed
    file.open(m_location.c_str(), std::ios::in );

    // result vector and line that will be pushed progressively to that vector
    CSVFileValuesType resultVector;
    CSVLineValuesType lineValues;

    // in case of incorrect path, return empty vector.
    if(!file.is_open())
    {
        file.close();
        return resultVector;
    }

    // line currently read, and value currently being parsed
    std::string readLine;
    std::string value;

    // whether we are currently parsing a quote or not, used when parsing quote escape character ' "" ' or in-value '\n'
    bool isQuote = false;

    // parse the whole file
    while (!file.eof())
    {
        // parse it line by line
        std::getline(file, readLine);

        // either line is empty or the line is a comment (considering we are not in a quoted value)
        if( (readLine.empty() || readLine[0] == '#') && !isQuote)
        {
            continue;
        }

        // parse the line character by character.
        for(size_t i = 0; i < readLine.size(); i++)
        {
            // separator, value parsing is over, push it to the current line
            if(readLine[i] == m_separatorType && !isQuote)
            {
                lineValues.push_back(value);
                value.erase();

            }
            // if we find a double quote character and we are not in a quoted value already,
            // either it is the beginning of a quote, either it is an escaped quote
            else if(readLine[i] == '"' && !isQuote)
            {
                if(readLine[i+1] == '"')
                {
                    value += readLine[i];
                    i++;
                }
                else
                {
                    isQuote = true;
                }
            }
            // if the double quote is within a quoted value, either it is the end of the quoted value,
            // either it is an escaped quote
            else if(readLine[i] == '"' && isQuote)
            {
                if(readLine[i+1] == '"')
                {
                    value += readLine[i];
                    i++;
                }
                else
                {
                    isQuote = false;
                }
            }
            // if it is only a normal character add it to the currant value.
            else
            {
                value += readLine[i];
            }
        }

        // if line is over and this is not a quote, push the read values of this line to the result vector
        // and clear current containers
        if( !isQuote )
        {
            lineValues.push_back(value);
            resultVector.push_back(lineValues);

            lineValues.clear();
            value.erase();
        }
        // if the line is over and this is a quote, add the end line character to our current value.
        // DO NOT push any value to the lineVector.
        else
        {
            value += '\n';
        }
    }

    // reading is done file can be closed.
    file.close();

    return resultVector;
}

//------------------------------------------------------------------------------

std::string CsvReader::getLocation() const
{
    return this->m_location;
}

//------------------------------------------------------------------------------

void CsvReader::setLocation(const std::string& location)
{
    this->m_location = location;

    SLM_ASSERT("The file path is empty.", !location.empty());
}

//------------------------------------------------------------------------------

} // namespace fwDataIO
