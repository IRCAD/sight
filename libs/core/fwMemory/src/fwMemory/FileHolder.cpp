/************************************************************************
 *
 * Copyright (C) 2009-2019 IRCAD France
 * Copyright (C) 2012-2019 IHU Strasbourg
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

#include "fwMemory/FileHolder.hpp"

#include <fwCore/spyLog.hpp>

#include <filesystem>

namespace fwMemory
{

struct FileAutoDelete
{
    FileAutoDelete(const std::filesystem::path& file) :
        m_path(file)
    {
    }

    ~FileAutoDelete()
    {
        if(!m_path.empty() && std::filesystem::is_regular_file(m_path))
        {
            std::error_code ec;
            std::filesystem::remove(m_path, ec);
            OSLM_ERROR_IF("file remove failed : " << ec.message(), ec.value() != 0);
        }
    }

    std::filesystem::path m_path;
};

//-----------------------------------------------------------------------------

FileHolder::FileHolder(const std::filesystem::path& file, bool autodelete) :
    m_path(file)
{
    if (autodelete)
    {
        m_autoDelete = std::make_shared< FileAutoDelete >( file );
    }
}

//-----------------------------------------------------------------------------

} // namespace fwMemory
