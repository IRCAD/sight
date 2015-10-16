/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "fwZip/exception/Write.hpp"

#include "minizip/zip.h"
#include "fwZip/WriteDirArchive.hpp"

#include <fwCore/exceptionmacros.hpp>

#include <fstream>


#include <boost/filesystem/path.hpp>
#include <boost/filesystem/operations.hpp>

#include <boost/iostreams/stream_buffer.hpp>


namespace fwZip
{

//-----------------------------------------------------------------------------

WriteDirArchive::WriteDirArchive( const ::boost::filesystem::path &archive ) :
    m_archive(archive)
{
    if(!::boost::filesystem::exists(m_archive))
    {
        ::boost::filesystem::create_directories(m_archive);
    }
}

//-----------------------------------------------------------------------------

WriteDirArchive::~WriteDirArchive()
{
}

//-----------------------------------------------------------------------------

SPTR(std::ostream) WriteDirArchive::createFile(const ::boost::filesystem::path &path)
{
    const ::boost::filesystem::path file       = m_archive / path;
    const ::boost::filesystem::path parentFile = file.parent_path();
    if(!::boost::filesystem::exists(parentFile))
    {
        ::boost::filesystem::create_directories(parentFile);
    }

    SPTR(std::ofstream) os = std::make_shared< std::ofstream >();
    os->open(file.string().c_str(), std::fstream::binary | std::fstream::out | std::fstream::trunc);
    return os;
}

//-----------------------------------------------------------------------------

void WriteDirArchive::putFile(const ::boost::filesystem::path &sourceFile, const ::boost::filesystem::path &path)
{
    const ::boost::filesystem::path fileDest = m_archive / path;
    if (!::boost::filesystem::exists(fileDest))
    {
        const ::boost::filesystem::path parentFile = fileDest.parent_path();
        if(!::boost::filesystem::exists(parentFile))
        {
            ::boost::filesystem::create_directories(parentFile);
        }
        ::boost::system::error_code err;
        ::boost::filesystem::create_hard_link( sourceFile, fileDest, err );
        if (err.value() != 0)
        {
            // Use std stream instead of boost:::filesystem::copy_file
            // because fwZip is build using std=c++11 and using copy_file also requires boost built
            // with std=c++11 (for now in c++0x).
            std::string strSource = sourceFile.string();
            std::string strDest   = fileDest.string();
            std::ifstream src(strSource.c_str(), std::ios::binary);
            std::ofstream dst(strDest.c_str(),   std::ios::binary);

            dst << src.rdbuf();

        }
    }
}

//-----------------------------------------------------------------------------

bool WriteDirArchive::createDir(const ::boost::filesystem::path &path)
{
    return ::boost::filesystem::create_directories(m_archive/path);
}

//-----------------------------------------------------------------------------

const ::boost::filesystem::path WriteDirArchive::getArchivePath() const
{
    return m_archive;
}


}

