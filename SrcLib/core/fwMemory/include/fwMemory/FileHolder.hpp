/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2018.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#pragma once

#include "fwMemory/config.hpp"

#include <fwCore/macros.hpp>

#include <boost/filesystem/path.hpp>

namespace fwMemory
{

struct FileAutoDelete;

class FWMEMORY_CLASS_API FileHolder
{
public:
    FileHolder()
    {
    }
    FWMEMORY_API FileHolder(const ::boost::filesystem::path& file, bool autodelete = false);

    operator :: boost::filesystem::path() const
    {
        return m_path;
    }

    //------------------------------------------------------------------------------

    bool empty() const
    {
        return m_path.empty();
    }

    //------------------------------------------------------------------------------

    void clear()
    {
        m_path.clear();
        m_autoDelete.reset();
    }

    //------------------------------------------------------------------------------

    std::string string() const
    {
        return m_path.string();
    }

protected:
    ::boost::filesystem::path m_path;
    SPTR(FileAutoDelete) m_autoDelete;
};

} // namespace fwMemory
