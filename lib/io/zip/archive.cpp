/************************************************************************
 *
 * Copyright (C) 2021-2023 IRCAD France
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

#include "archive.hpp"

#include <core/exceptionmacros.hpp>
#include <core/macros.hpp>

#include <mutex>
#include <set>

namespace sight::io::zip
{

// Global static map of opened archive
static std::set<std::filesystem::path> s_archives;
static std::mutex s_archives_mutex;

/// Constructor
archive::archive(const std::filesystem::path& _archive_path) :
    m_archive_path(_archive_path.lexically_normal())
{
    std::unique_lock guard(s_archives_mutex);

    SIGHT_THROW_IF(
        "The archive file '" + m_archive_path.string() + "' is already opened.",
        s_archives.find(m_archive_path) != s_archives.end()
    );

    // Store the path as long as the archive is opened
    s_archives.insert(m_archive_path);
}

archive::~archive()
{
    std::unique_lock guard(s_archives_mutex);

    // Remove completely the archive if not used anymore
    s_archives.erase(m_archive_path);
}

} // namespace sight::io::zip
