/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

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

