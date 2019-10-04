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

#include "fwZip/ReadZipArchive.hpp"

#include "fwZip/exception/Read.hpp"

#include "minizip/unzip.h"

#include <fwCore/exceptionmacros.hpp>

#include <boost/iostreams/categories.hpp>  // source_tag
#include <boost/iostreams/stream.hpp>

#include <filesystem>

#include <iosfwd>    // streamsize

namespace fwZip
{

//------------------------------------------------------------------------------

void* openReadZipArchive( const std::filesystem::path& archive )
{
    FW_RAISE_EXCEPTION_IF(
        ::fwZip::exception::Read("Archive '" + archive.string() + "' doesn't exist."),
        !std::filesystem::exists(archive));

    void* zip = unzOpen(archive.string().c_str());

    FW_RAISE_EXCEPTION_IF(
        ::fwZip::exception::Read("Archive '" + archive.string() + "' cannot be opened."),
        zip == NULL);

    return zip;
}

//-----------------------------------------------------------------------------

class ZipSource
{
public:
    typedef char char_type;
    typedef ::boost::iostreams::source_tag category;

    ZipSource( const std::filesystem::path& archive) :
        m_zipDescriptor( openReadZipArchive(archive), &unzClose ),
        m_archive(archive)
    {

    }

    ZipSource( const std::filesystem::path& archive, const std::filesystem::path& path ) :
        m_zipDescriptor( openReadZipArchive(archive), &unzClose ),
        m_archive(archive),
        m_path(path)
    {
        int nRet = unzLocateFile(m_zipDescriptor.get(), path.string().c_str(), 0);

        FW_RAISE_EXCEPTION_IF(
            ::fwZip::exception::Read("File '" +  path.string() + "' in archive '" +
                                     archive.string() + "' doesn't exist."),
            nRet != UNZ_OK);

        nRet = unzOpenCurrentFile(m_zipDescriptor.get());
        FW_RAISE_EXCEPTION_IF(
            ::fwZip::exception::Read("Cannot retrieve file '" + path.string() +
                                     "' in archive '"+ archive.string() + "'."),
            nRet != UNZ_OK);
    }

    //------------------------------------------------------------------------------

    std::streamsize read(char* s, std::streamsize n)
    {
        std::streamsize nRet = unzReadCurrentFile(m_zipDescriptor.get(), s, static_cast< unsigned int >(n));
        FW_RAISE_EXCEPTION_IF(
            ::fwZip::exception::Read("Error occurred while reading archive '" + m_archive.string()
                                     + ":" + m_path.string() + "'."),
            nRet < 0);
        return nRet;
    }

    //------------------------------------------------------------------------------

    std::string getComment()
    {
        unz_global_info* info = new unz_global_info;

        std::streamsize nRet = unzGetGlobalInfo(m_zipDescriptor.get(), info);

        FW_RAISE_EXCEPTION_IF(
            ::fwZip::exception::Read("Error occurred while reading information archive '" +
                                     m_archive.string() + "'."),
            nRet < 0);

        char* comment = new char[info->size_comment];
        nRet = unzGetGlobalComment(m_zipDescriptor.get(), comment, info->size_comment);

        FW_RAISE_EXCEPTION_IF(
            ::fwZip::exception::Read("Error occurred while reading archive's global comment '" +
                                     m_archive.string() + "'."),
            nRet < 0);

        std::string stringComment(comment, info->size_comment);

        delete info;
        delete[] comment;

        return stringComment;
    }

protected:
    SPTR(void) m_zipDescriptor;
    std::filesystem::path m_archive;
    std::filesystem::path m_path;
};

//-----------------------------------------------------------------------------

ReadZipArchive::ReadZipArchive( const std::filesystem::path& archive ) :
    m_archive(archive)
{
}

//-----------------------------------------------------------------------------

SPTR(std::istream) ReadZipArchive::getFile(const std::filesystem::path& path)
{
    SPTR(::boost::iostreams::stream<ZipSource>) is
        = std::make_shared< ::boost::iostreams::stream<ZipSource> >(m_archive, path);

    return is;
}

//-----------------------------------------------------------------------------

std::string ReadZipArchive::getComment()
{
    SPTR(ZipSource) zip = std::make_shared<ZipSource>(m_archive);

    return zip->getComment();
}

//-----------------------------------------------------------------------------

const std::filesystem::path ReadZipArchive::getArchivePath() const
{
    return m_archive;
}

}
