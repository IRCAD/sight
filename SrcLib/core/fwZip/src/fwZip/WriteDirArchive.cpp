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

#include "fwZip/WriteDirArchive.hpp"

#include "fwZip/exception/Write.hpp"

#include "minizip/zip.h"

#include <fwCore/exceptionmacros.hpp>

#include <boost/iostreams/stream_buffer.hpp>

#include <filesystem>

#include <fstream>

namespace fwZip
{

//-----------------------------------------------------------------------------

WriteDirArchive::WriteDirArchive( const std::filesystem::path& archive ) :
    m_archive(archive)
{
    if(!std::filesystem::exists(m_archive))
    {
        std::filesystem::create_directories(m_archive);
    }
}

//-----------------------------------------------------------------------------

WriteDirArchive::~WriteDirArchive()
{
}

//-----------------------------------------------------------------------------

SPTR(std::ostream) WriteDirArchive::createFile(const std::filesystem::path& path)
{
    const std::filesystem::path file       = m_archive / path;
    const std::filesystem::path parentFile = file.parent_path();
    if(!std::filesystem::exists(parentFile))
    {
        std::filesystem::create_directories(parentFile);
    }

    SPTR(std::ofstream) os = std::make_shared< std::ofstream >();
    os->open(file.string().c_str(), std::fstream::binary | std::fstream::out | std::fstream::trunc);
    return os;
}

//-----------------------------------------------------------------------------

void WriteDirArchive::putFile(const std::filesystem::path& sourceFile,
                              const std::filesystem::path& destinationFile)
{
    const std::filesystem::path fileDest = m_archive / destinationFile;
    if (!std::filesystem::exists(fileDest))
    {
        const std::filesystem::path parentFile = fileDest.parent_path();
        if(!std::filesystem::exists(parentFile))
        {
            std::filesystem::create_directories(parentFile);
        }
        std::error_code err;
        std::filesystem::create_hard_link( sourceFile, fileDest, err );
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

bool WriteDirArchive::createDir(const std::filesystem::path& path)
{
    return std::filesystem::create_directories(m_archive/path);
}

//-----------------------------------------------------------------------------

const std::filesystem::path WriteDirArchive::getArchivePath() const
{
    return m_archive;
}

}
