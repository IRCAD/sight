/************************************************************************
 *
 * Copyright (C) 2009-2023 IRCAD France
 * Copyright (C) 2012-2021 IHU Strasbourg
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

#include "base.hpp"

#include <filesystem>
#include <string>
#include <vector>

namespace sight::core::location
{

/// Class to define a location that holds one or more file path.
class CORE_CLASS_API multiple_files : public base
{
public:

    SIGHT_DECLARE_CLASS(multiple_files, base);

    /// String serialization function
    [[nodiscard]] inline std::string to_string() const override;

    /// Sets the file paths
    inline void set_files(const std::vector<std::filesystem::path>& _file_paths);

    /// Gets the file paths
    [[nodiscard]] inline std::vector<std::filesystem::path> get_files() const;

    /// Appends one file path
    inline void add_file(const std::filesystem::path& _file_paths);

private:

    /// The filesystem::path vector
    std::vector<std::filesystem::path> m_files;
};

// String serialization function
inline std::string multiple_files::to_string() const
{
    #ifdef WIN32
    std::string delim = ";";
    #else
    std::string delim = ":";
    #endif

    std::string result;
    for(size_t i = 0 ; const auto& file : m_files)
    {
        result.append(file.string());
        if(++i < m_files.size())
        {
            result.append(delim);
        }
    }

    return result;
}

//------------------------------------------------------------------------------

inline void multiple_files::set_files(const std::vector<std::filesystem::path>& _file_paths)
{
    m_files = _file_paths;
}

//------------------------------------------------------------------------------

inline std::vector<std::filesystem::path> multiple_files::get_files() const
{
    return m_files;
}

//------------------------------------------------------------------------------

inline void multiple_files::add_file(const std::filesystem::path& _file_paths)
{
    m_files.push_back(_file_paths);
}

} // namespace sight::core::location
