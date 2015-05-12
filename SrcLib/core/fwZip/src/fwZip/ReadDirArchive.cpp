/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

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
        !this->exists(m_archive / path));

    SPTR(std::ifstream) is = ::boost::make_shared< std::ifstream >();
    is->open((m_archive / path).string().c_str(), std::fstream::binary | std::fstream::in);
    return is;
}

//-----------------------------------------------------------------------------

bool ReadDirArchive::exists(const ::boost::filesystem::path &path)
{
    return !::boost::filesystem::exists(m_archive / path);
}

//-----------------------------------------------------------------------------

const ::boost::filesystem::path ReadDirArchive::getArchivePath() const
{
    return m_archive;
}

}

