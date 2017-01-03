/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2016-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWCSVIO_CSVREADER_HPP__
#define __FWCSVIO_CSVREADER_HPP__

#include "fwCsvIO/config.hpp"

#include <string>
#include <vector>

namespace fwCsvIO
{

/**
 * @brief   CSV files reader. Read file with .csv fomat (the extension is not important)
 * Reader to fill an std::Vector of strings with the content of a .csv file.
 */
class FWCSVIO_CLASS_API CsvReader
{

public:

    typedef std::vector<std::string> CSVLineValuesType;
    typedef std::vector<CSVLineValuesType> CSVFileValuesType;

    typedef enum
    {
        COMMA     = ',',
        SEMICOLON = ';'
    } SeparatorType;

    /// Constructor. Do nothing .
    FWCSVIO_API CsvReader(const std::string& location, SeparatorType separatorType = SeparatorType::COMMA);

    /// Destructor. Do nothing.
    FWCSVIO_API virtual ~CsvReader();

    /// Read the file with standard iostream API. Result vector is return empty if file could not be opened.
    FWCSVIO_API CSVFileValuesType read() const;

    /// Get the path of the file in the file system.
    FWCSVIO_API std::string getLocation() const;

    /// Set the path of the file in the file system.
    FWCSVIO_API void setLocation(const std::string& location);

private:

    /// Contains the path of the csv formated file to be parsed.
    std::string m_location;

    /// Indicates the separator that will be used for file parsing.
    SeparatorType m_separatorType;
};

} // namespace fwCsvIO

#endif // __FWCSVIO_CSVREADER_HPP__
