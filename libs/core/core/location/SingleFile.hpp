/************************************************************************
 *
 * Copyright (C) 2009-2021 IRCAD France
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

/// Class to define a location that holds one single file path.
class CORE_CLASS_API SingleFile : public ILocation
{
public:

    SIGHT_DECLARE_CLASS(SingleFile, ILocation);

    /// String serialization function
    inline std::string toString() const override;

    /// Sets the file path
    inline void setFile(const std::filesystem::path& filePath);

    /// Gets the file path
    inline std::filesystem::path getFile() const;

private:

    /// The filesystem path to the target file
    std::filesystem::path m_file;
};

//------------------------------------------------------------------------------

inline std::string SingleFile::toString() const
{
    return m_file.string();
}

//------------------------------------------------------------------------------

inline void SingleFile::setFile(const std::filesystem::path& filePath)
{
    m_file = filePath;
}

//------------------------------------------------------------------------------

inline std::filesystem::path SingleFile::getFile() const
{
    return m_file;
}

} // namespace sight::io::base::location
