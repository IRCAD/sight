/************************************************************************
 *
 * Copyright (C) 2009-2015 IRCAD France
 * Copyright (C) 2012-2015 IHU Strasbourg
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

#include <fwCore/spyLog.hpp>
#include <boost/filesystem/path.hpp>
#include <boost/filesystem/operations.hpp>

#include "fwMemory/FileHolder.hpp"

namespace fwMemory
{

struct FileAutoDelete
{
    FileAutoDelete(const ::boost::filesystem::path &file) : m_path(file)
    {
    }

    ~FileAutoDelete()
    {
        if(!m_path.empty() && ::boost::filesystem::is_regular_file(m_path))
        {
            ::boost::system::error_code ec;
            ::boost::filesystem::remove(m_path, ec);
            OSLM_ERROR_IF("file remove failed : " << ec.message(), ec.value()!=0);
        }
    }

    ::boost::filesystem::path m_path;
};


//-----------------------------------------------------------------------------

FileHolder::FileHolder(const ::boost::filesystem::path &file, bool autodelete) :
    m_path(file)
{
    if (autodelete)
    {
        m_autoDelete = std::make_shared< FileAutoDelete >( file );
    }
}

//-----------------------------------------------------------------------------


} // namespace fwMemory

