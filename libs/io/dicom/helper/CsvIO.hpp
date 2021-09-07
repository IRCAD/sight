/************************************************************************
 *
 * Copyright (C) 2021 IRCAD France
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

#pragma once

#include "io/dicom/config.hpp"

#include <iostream>
#include <string>
#include <vector>

namespace sight::io::dicom
{

namespace helper
{

/**
 * @brief Read CSV file and returns parsed tokens.
 * The input file is supposed to use comma separator, but another separator can be used when reading file.
 */
class IO_DICOM_CLASS_API CsvIO
{
public:

    /**
     * @brief Constructor
     *
     * @param csvStream the stream to read
     */
    IO_DICOM_API CsvIO(std::istream& csvStream);

    /// Desctructor
    IO_DICOM_API virtual ~CsvIO();

    /// Containers to store parsed tokens.
    typedef std::vector<std::string> TokenContainerType;

    /**
     * @brief Returns tokens on next line to read, using comma separator
     *
     * Other sperators can be used
     * @param separator torken separator
     *
     * @return tokens found on next line separated with given characters
     */
    IO_DICOM_API TokenContainerType getLine(const std::string& separator = ",");

private:

    /// Associated stream
    std::istream& m_stream;
};

} // namespace helper

} // namespace sight::io::dicom
