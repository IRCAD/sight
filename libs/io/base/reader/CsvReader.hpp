/************************************************************************
 *
 * Copyright (C) 2021-2022 IRCAD France
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

#include "io/base/config.hpp"

#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

namespace sight::io::base::reader
{

/**
 * @brief Read CSV file and returns parsed tokens.
 * The input file is supposed to use comma separator, but another separator can be used when reading file.
 */
class IO_BASE_CLASS_API CsvReader
{
public:

    /**
     * @brief Constructor
     *
     * @param csvStream the stream to read
     */
    IO_BASE_API CsvReader(const std::filesystem::path& csvPath);

    /// Destructor
    IO_BASE_API virtual ~CsvReader();

    /// Containers to store parsed tokens.
    typedef std::vector<std::string> TokenContainerType;

    /**
     * @brief Returns tokens on next line to read, using comma separator
     *
     * Other separators can be used
     * @param separator token separator
     *
     * @return tokens found on next line separated with given characters
     */
    IO_BASE_API TokenContainerType getLine(const std::string& separator = ",");

private:

    /// Associated stream
    std::ifstream m_stream;
};

} // namespace sight::io::base::reader
