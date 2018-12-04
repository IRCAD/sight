/************************************************************************
 *
 * Copyright (C) 2009-2016 IRCAD France
 * Copyright (C) 2012-2016 IHU Strasbourg
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

#include <boost/filesystem/path.hpp>
#include <boost/filesystem/operations.hpp>

#include <fwCore/exceptionmacros.hpp>

#include "minizip/unzip.h"
#include "fwZip/ReadDirArchive.hpp"
#include "fwZip/exception/Read.hpp"

namespace fwZip
{

//-----------------------------------------------------------------------------

ReadDirArchive::ReadDirArchive( const ::boost::filesystem::path &archive ) :
    m_archive(archive)
{
}

//-----------------------------------------------------------------------------

ReadDirArchive::~ReadDirArchive()
{
}

//-----------------------------------------------------------------------------

SPTR(std::istream) ReadDirArchive::getFile(const ::boost::filesystem::path &path)
{
    FW_RAISE_EXCEPTION_IF(
        ::fwZip::exception::Read("File '" +  path.string() + "' "
                                 "in archive '" + m_archive.string() + "' doesn't exist."),
        !::boost::filesystem::exists(m_archive / path));

    SPTR(std::ifstream) is = std::make_shared< std::ifstream >();
    is->open((m_archive / path).string().c_str(), std::fstream::binary | std::fstream::in);
    return is;
}

//-----------------------------------------------------------------------------

const ::boost::filesystem::path ReadDirArchive::getArchivePath() const
{
    return m_archive;
}

}

