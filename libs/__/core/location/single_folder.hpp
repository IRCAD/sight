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

/// Class to define a location that holds a single directory path
class CORE_CLASS_API single_folder : public base
{
public:

    SIGHT_DECLARE_CLASS(single_folder, base);

    /// String serialization function
    [[nodiscard]] inline std::string to_string() const override;

    /// Sets folder
    inline void set_folder(const std::filesystem::path& _folder);

    /// Gets folder
    [[nodiscard]] inline std::filesystem::path get_folder() const;

private:

    /// The directory as filesystem path
    std::filesystem::path m_folder;
};

//------------------------------------------------------------------------------

inline std::string single_folder::to_string() const
{
    return m_folder.string();
}

//------------------------------------------------------------------------------

inline void single_folder::set_folder(const std::filesystem::path& _folder)
{
    m_folder = _folder;
}

//------------------------------------------------------------------------------

inline std::filesystem::path single_folder::get_folder() const
{
    return m_folder;
}

} // namespace sight::core::location
