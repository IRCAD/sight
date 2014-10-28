/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2013.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWMEMORY_FILEHOLDER_HPP__
#define __FWMEMORY_FILEHOLDER_HPP__

#include <fwCore/macros.hpp>

#include <boost/filesystem/path.hpp>

#include "fwMemory/config.hpp"

namespace fwMemory
{

struct FileAutoDelete;

class FWMEMORY_CLASS_API FileHolder
{
public:
    FileHolder(){};
    FWMEMORY_API FileHolder(const ::boost::filesystem::path &file, bool autodelete = false);

    operator ::boost::filesystem::path () const
    {
        return m_path;
    }

    bool empty() const
    {
        return m_path.empty();
    }

    void clear()
    {
        m_path.clear();
        m_autoDelete.reset();
    }

    std::string string()
    {
        return m_path.string();
    }

protected:
    ::boost::filesystem::path m_path;
    SPTR(FileAutoDelete) m_autoDelete;
};


} // namespace fwMemory

#endif // __FWMEMORY_FILEHOLDER_HPP__

