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

#include "io/zip/config.hpp"

#include <core/macros.hpp>

#include <filesystem>

namespace sight::io::zip
{

/// Class that allow reading and writing an archive.
class IO_ZIP_CLASS_API Archive
{
public:

    SIGHT_DECLARE_CLASS(Archive);

    /// Delete default constructors and assignment operators, as we don't want to allow resources duplication
    Archive(const Archive&)            = delete;
    Archive(Archive&&)                 = delete;
    Archive& operator=(const Archive&) = delete;
    Archive& operator=(Archive&&)      = delete;

protected:

    /// Constructor
    IO_ZIP_API Archive() = default;

    /// Destructor
    IO_ZIP_API virtual ~Archive() = default;

    /// Retrieves an archive from cache
    static Archive::sptr cache_find(const std::filesystem::path& archivePath);

    /// Stores an archive from cache
    static void cache_store(const std::filesystem::path& archivePath, const Archive::sptr& archive);

    /// Removes an archive from cache
    static void cache_clean(const std::filesystem::path& archivePath);

    /// Returns true if contained in the cache (even if expired)
    static bool cache_contains(const std::filesystem::path& archivePath);
};

} // namespace sight::io::session
