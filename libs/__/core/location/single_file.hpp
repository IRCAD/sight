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

namespace sight::core::location
{

/// Class to define a location that holds one single file path.
class CORE_CLASS_API single_file : public base
{
public:

    SIGHT_DECLARE_CLASS(single_file, base);

    /// String serialization function
    [[nodiscard]] inline std::string to_string() const override;

    /// Sets the file path
    inline void set_file(const std::filesystem::path& file_path);

    /// Gets the file path
    [[nodiscard]] inline std::filesystem::path get_file() const;

private:

    /// The filesystem path to the target file
    std::filesystem::path m_file;
};

//------------------------------------------------------------------------------

inline std::string single_file::to_string() const
{
    return m_file.string();
}

//------------------------------------------------------------------------------

inline void single_file::set_file(const std::filesystem::path& file_path)
{
    m_file = file_path;
}

//------------------------------------------------------------------------------

inline std::filesystem::path single_file::get_file() const
{
    return m_file;
}

} // namespace sight::core::location
