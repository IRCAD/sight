/************************************************************************
 *
 * Copyright (C) 2009-2022 IRCAD France
 * Copyright (C) 2012-2020 IHU Strasbourg
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

#include "core/memory/FileHolder.hpp"

#include <core/spyLog.hpp>

#include <filesystem>
#include <utility>

namespace sight::core::memory
{

struct FileAutoDelete
{
    explicit FileAutoDelete(std::filesystem::path file) :
        m_path(std::move(file))
    {
    }

    ~FileAutoDelete()
    {
        if(!m_path.empty() && std::filesystem::is_regular_file(m_path))
        {
            std::error_code ec;
            std::filesystem::remove(m_path, ec);
            SIGHT_ERROR_IF("file remove failed : " << ec.message(), ec.value() != 0);
        }
    }

    std::filesystem::path m_path;
};

//-----------------------------------------------------------------------------

FileHolder::FileHolder(const std::filesystem::path& file, bool autodelete) :
    m_path(file)
{
    if(autodelete)
    {
        m_autoDelete = std::make_shared<FileAutoDelete>(file);
    }
}

//-----------------------------------------------------------------------------

} // namespace sight::core::memory
