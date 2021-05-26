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

#include <map>
#include <mutex>

namespace sight::io::zip
{

// Global static map of opened archive
static std::map<std::filesystem::path, Archive::wptr> s_archives_cache;
static std::mutex s_archives_cache_mutex;

//------------------------------------------------------------------------------

Archive::sptr Archive::cache_find(const std::filesystem::path& archivePath)
{
    Archive::sptr archive;

    // Normalize path
    const std::filesystem::path normalized_path = archivePath.lexically_normal();

    // Lock the global archives cache
    std::lock_guard lock(s_archives_cache_mutex);

    // Search into the cache
    auto found = s_archives_cache.find(normalized_path);
    if(found != s_archives_cache.end())
    {
        // Use the found instance
        archive = found->second.lock();
    }

    return archive;
}

//------------------------------------------------------------------------------

void Archive::cache_store(const std::filesystem::path& archivePath, const Archive::sptr& archive)
{
    // Normalize path
    std::filesystem::path normalized_path = archivePath.lexically_normal();

    // Lock the global archives map
    std::lock_guard lock(s_archives_cache_mutex);
    s_archives_cache[normalized_path] = archive;
}

//------------------------------------------------------------------------------

void Archive::cache_clean(const std::filesystem::path& archivePath)
{
    // Normalize path
    std::filesystem::path normalized_path = archivePath.lexically_normal();

    // Lock the global archives map
    std::lock_guard lock(s_archives_cache_mutex);

    // Clean all expired pointer
    for(auto it = s_archives_cache.cbegin() ; it != s_archives_cache.cend() ; )
    {
        if(it->second.expired())
        {
            it = s_archives_cache.erase(it);
        }
        else
        {
            ++it;
        }
    }

    // Remove completely the archive if not used anymore
    auto found = s_archives_cache.find(normalized_path);
    if(found != s_archives_cache.end())
    {
        // If use count == 1, it means that the caller is the only left
        if(found->second.use_count() <= 1)
        {
            s_archives_cache.erase(archivePath);
        }
    }
}

//------------------------------------------------------------------------------

bool Archive::cache_contains(const std::filesystem::path& archivePath)
{
    // Normalize path
    std::filesystem::path normalized_path = archivePath.lexically_normal();

    // Lock the global archives map
    std::lock_guard lock(s_archives_cache_mutex);

    return s_archives_cache.find(normalized_path) != s_archives_cache.end();
}

} // namespace sight::io::zip
