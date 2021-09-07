/************************************************************************
 *
 * Copyright (C) 2021 IRCAD France
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

#include "Archive.hpp"

#include <mutex>
#include <set>

namespace sight::io::zip
{

// Global static map of opened archive
static std::set<std::filesystem::path> s_archives;
static std::mutex s_archives_mutex;

//------------------------------------------------------------------------------

void Archive::lock(const std::filesystem::path& archivePath)
{
    // Normalize path
    const std::filesystem::path normalizedPath = archivePath.lexically_normal();

    // Lock the global archives map
    std::lock_guard lock(s_archives_mutex);
    s_archives.insert(archivePath);
}

//------------------------------------------------------------------------------

void Archive::unlock(const std::filesystem::path& archivePath)
{
    // Normalize path
    const std::filesystem::path normalizedPath = archivePath.lexically_normal();

    // Lock the global archives map
    std::lock_guard lock(s_archives_mutex);

    // Remove completely the archive if not used anymore
    s_archives.erase(archivePath);
}

//------------------------------------------------------------------------------

bool Archive::is_locked(const std::filesystem::path& archivePath)
{
    // Normalize path
    const std::filesystem::path normalizedPath = archivePath.lexically_normal();

    // Lock the global archives map
    std::lock_guard lock(s_archives_mutex);

    return s_archives.find(normalizedPath) != s_archives.end();
}

} // namespace sight::io::zip
