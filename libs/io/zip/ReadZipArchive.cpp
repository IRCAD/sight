/************************************************************************
 *
 * Copyright (C) 2009-2021 IRCAD France
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

#include "io/zip/ReadZipArchive.hpp"

#include "io/zip/exception/Read.hpp"

#include "minizip/unzip.h"

#include <core/exceptionmacros.hpp>

#include <boost/iostreams/categories.hpp>  // source_tag
#include <boost/iostreams/stream.hpp>

#include <filesystem>
#include <iosfwd>    // streamsize

namespace sight::io::zip
{

//------------------------------------------------------------------------------

void* openReadZipArchive( const std::filesystem::path& archive )
{
    SIGHT_THROW_EXCEPTION_IF(
        io::zip::exception::Read("Archive '" + archive.string() + "' doesn't exist."),
        !std::filesystem::exists(archive));

    void* zip = unzOpen(archive.string().c_str());

    SIGHT_THROW_EXCEPTION_IF(
        io::zip::exception::Read("Archive '" + archive.string() + "' cannot be opened."),
        zip == NULL);

    return zip;
}

//-----------------------------------------------------------------------------

class ZipSource
{
public:
    typedef char char_type;
    typedef ::boost::iostreams::source_tag category;

    ZipSource( const std::filesystem::path& archive, const std::string& key = "") :
        m_zipDescriptor( openReadZipArchive(archive), &unzClose ),
        m_archive(archive),
        m_key(key)
    {
    }

    ZipSource( const std::filesystem::path& archive, const std::filesystem::path& path, const std::string& key = "") :
        m_zipDescriptor( openReadZipArchive(archive), &unzClose ),
        m_archive(archive),
        m_path(path),
        m_key(key)
    {
        int nRet = unzLocateFile(m_zipDescriptor.get(), m_path.string().c_str(), 0);

        SIGHT_THROW_EXCEPTION_IF(
            io::zip::exception::Read(
                "File '"
                + m_path.string()
                + "' in archive '"
                + m_archive.string()
                + "' doesn't exist."
                ),
            nRet != UNZ_OK);

        if(key.empty())
        {
            nRet = unzOpenCurrentFile(m_zipDescriptor.get());
        }
        else
        {
            nRet = unzOpenCurrentFilePassword(m_zipDescriptor.get(), m_key.c_str());
        }

        SIGHT_THROW_EXCEPTION_IF(
            io::zip::exception::Read(
                "Cannot retrieve file '"
                + m_path.string()
                + "' in archive '"
                + m_archive.string()
                + "'."
                ),
            nRet != UNZ_OK);
    }

    //------------------------------------------------------------------------------

    std::streamsize read(char* s, std::streamsize n)
    {
        const int nRet = unzReadCurrentFile(m_zipDescriptor.get(), s, static_cast< unsigned int >(n));
        SIGHT_THROW_EXCEPTION_IF(
            io::zip::exception::Read(
                "Error occurred while reading archive '"
                + m_archive.string()
                + ":"
                + m_path.string()
                + "'."
                ),
            nRet < 0);
        return nRet;
    }

    //------------------------------------------------------------------------------

    std::string getComment()
    {
        unz_global_info info;

        int nRet = unzGetGlobalInfo(m_zipDescriptor.get(), &info);

        SIGHT_THROW_EXCEPTION_IF(
            io::zip::exception::Read(
                "Error occurred while reading information archive '"
                + m_archive.string()
                + "'."),
            nRet < 0);

        std::string comment;
        comment.resize(info.size_comment);

        nRet = unzGetGlobalComment(m_zipDescriptor.get(), comment.data(), static_cast<uint16_t>(comment.size()));

        SIGHT_THROW_EXCEPTION_IF(
            io::zip::exception::Read(
                "Error occurred while reading archive's global comment '"
                + m_archive.string()
                + "'."),
            nRet < 0);

        return comment;
    }

private:
    SPTR(void) m_zipDescriptor;
    std::filesystem::path m_archive;
    std::filesystem::path m_path;
    std::string m_key;
};

//-----------------------------------------------------------------------------

ReadZipArchive::ReadZipArchive(const std::filesystem::path& archive, const std::string& key) :
    m_archive(archive),
    m_key(key)
{
}

//-----------------------------------------------------------------------------

SPTR(std::istream) ReadZipArchive::getFile(const std::filesystem::path& path)
{
    return std::make_shared< ::boost::iostreams::stream<ZipSource> >(m_archive, path, m_key);
}

//-----------------------------------------------------------------------------

std::string ReadZipArchive::getComment()
{
    SPTR(ZipSource) zip = std::make_shared<ZipSource>(m_archive, m_key);

    return zip->getComment();
}

//-----------------------------------------------------------------------------

const std::filesystem::path ReadZipArchive::getArchivePath() const
{
    return m_archive;
}

}
