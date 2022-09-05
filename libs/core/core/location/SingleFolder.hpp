/************************************************************************
 *
 * Copyright (C) 2009-2022 IRCAD France
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

#include "ILocation.hpp"

#include <filesystem>

namespace sight::core::location
{

/// Class to define a location that holds a single directory path
class CORE_CLASS_API SingleFolder : public ILocation
{
public:

    SIGHT_DECLARE_CLASS(SingleFolder, ILocation);

    /// String serialization function
    [[nodiscard]] inline std::string toString() const override;

    /// Sets folder
    inline void setFolder(const std::filesystem::path& folder);

    /// Gets folder
    [[nodiscard]] inline std::filesystem::path getFolder() const;

private:

    /// The directory as filesystem path
    std::filesystem::path m_folder;
};

//------------------------------------------------------------------------------

inline std::string SingleFolder::toString() const
{
    return m_folder.string();
}

//------------------------------------------------------------------------------

inline void SingleFolder::setFolder(const std::filesystem::path& folder)
{
    m_folder = folder;
}

//------------------------------------------------------------------------------

inline std::filesystem::path SingleFolder::getFolder() const
{
    return m_folder;
}

} // namespace sight::core::location
