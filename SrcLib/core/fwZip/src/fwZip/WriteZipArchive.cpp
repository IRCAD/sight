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

#include <iosfwd>    // streamsize
#include <fstream>
#include <functional>


#include <boost/iostreams/stream.hpp>
#include <boost/iostreams/stream_buffer.hpp>
#include <boost/iostreams/categories.hpp>  // sink_tag

#include <boost/filesystem/path.hpp>
#include <boost/filesystem/operations.hpp>

#include <boost/date_time/posix_time/posix_time_types.hpp>
#include <boost/date_time/posix_time/conversion.hpp>

#include <fwCore/exceptionmacros.hpp>

#include "minizip/zip.h"
#include "fwZip/WriteZipArchive.hpp"
#include "fwZip/exception/Write.hpp"


namespace fwZip
{

//-----------------------------------------------------------------------------

zipFile openWriteZipArchive( const ::boost::filesystem::path &archive )
{
    int append  = (::boost::filesystem::exists(archive)) ? APPEND_STATUS_ADDINZIP : APPEND_STATUS_CREATE;
    zipFile zip = zipOpen(archive.string().c_str(), append);

    FW_RAISE_EXCEPTION_IF(
        ::fwZip::exception::Write("Archive '" + archive.string() + "' cannot be opened."),
        zip == NULL);

    return zip;
}

//-----------------------------------------------------------------------------

/*
 * @brief  Open a file in the zip archive for writing
 * @note Z_BEST_SPEED compression level for '.raw' files,
 *       Z_NO_COMPRESSION for 'raw.gz', Z_DEFAULT_COMPRESSION otherwise.
 */
std::streamsize openFile(zipFile zipDescriptor, const ::boost::filesystem::path &path)
{
    const std::string extension = path.extension().string();
    int compressLevel           = Z_DEFAULT_COMPRESSION;
    if(extension == ".raw")
    {
        compressLevel = Z_BEST_SPEED;
    }
    else if(extension == ".raw.gz")
    {
        compressLevel = Z_NO_COMPRESSION;
    }
    zip_fileinfo zfi;
    zfi.internal_fa = 0;
    zfi.external_fa = 0;// @todo FIXME

    ::boost::posix_time::ptime now = boost::posix_time::second_clock::local_time();
    std::tm ptm = ::boost::posix_time::to_tm(now);
    zfi.dosDate          = 0;
    zfi.tmz_date.tm_year = ptm.tm_year;
    zfi.tmz_date.tm_mon  = ptm.tm_mon;
    zfi.tmz_date.tm_mday = ptm.tm_mday;
    zfi.tmz_date.tm_hour = ptm.tm_hour;
    zfi.tmz_date.tm_min  = ptm.tm_min;
    zfi.tmz_date.tm_sec  = ptm.tm_sec;

    std::streamsize nRet = zipOpenNewFileInZip(zipDescriptor,
                                               path.generic_string().c_str(),
                                               &zfi,
                                               NULL,
                                               0,
                                               NULL,
                                               0,
                                               NULL,
                                               Z_DEFLATED,
                                               compressLevel);

    return nRet;
}

//-----------------------------------------------------------------------------

class ZipSink
{
public:
    typedef char char_type;
    typedef ::boost::iostreams::sink_tag category;

    ZipSink( const ::boost::filesystem::path &archive, const ::boost::filesystem::path &path,
             const std::string comment ) :
        m_zipDescriptor(
            openWriteZipArchive(archive),
            [comment](zipFile zipDescriptor)
        {
            zipClose(zipDescriptor, comment.c_str());
        }),
        m_archive(archive),
        m_path(path)
    {
        std::streamsize nRet = openFile(m_zipDescriptor.get(), m_path);
        FW_RAISE_EXCEPTION_IF(
            ::fwZip::exception::Write("Cannot open file '" + path.string() +
                                      "' in archive '"+ archive.string() + "'."),
            nRet != Z_OK);
    }

    std::streamsize write(const char* s, std::streamsize n)
    {
        std::streamsize nRet = zipWriteInFileInZip(m_zipDescriptor.get(), s, static_cast< unsigned int >(n));
        FW_RAISE_EXCEPTION_IF(
            ::fwZip::exception::Write("Error occurred while writing archive '" + m_archive.string()
                                      + ":" + m_path.string() + "'."),
            nRet < 0);
        return n;
    }

protected:
    SPTR(void) m_zipDescriptor;
    ::boost::filesystem::path m_archive;
    ::boost::filesystem::path m_path;
};

//-----------------------------------------------------------------------------

WriteZipArchive::WriteZipArchive( const ::boost::filesystem::path &archive ) :
    m_archive(archive),
    m_comment("")
{
}

//-----------------------------------------------------------------------------

WriteZipArchive::WriteZipArchive( const ::boost::filesystem::path &archive, const std::string& comment ) :
    m_archive(archive),
    m_comment(comment)
{
}

//-----------------------------------------------------------------------------

WriteZipArchive::~WriteZipArchive()
{
}

//-----------------------------------------------------------------------------

SPTR(std::ostream) WriteZipArchive::createFile(const ::boost::filesystem::path &path)
{
    SPTR(::boost::iostreams::stream<ZipSink>) os
        = std::make_shared< ::boost::iostreams::stream<ZipSink> >(m_archive, path, m_comment);
    return os;
}

//-----------------------------------------------------------------------------

void WriteZipArchive::putFile(const ::boost::filesystem::path &sourceFile, const ::boost::filesystem::path &path)
{
    std::ifstream is(sourceFile.string().c_str(), std::ios::binary);
    FW_RAISE_EXCEPTION_IF(::fwZip::exception::Write("Source file '" + sourceFile.string() + "' cannot be opened."),
                          !is.good());

    {
        SPTR(std::ostream) os = this->createFile(path);
        *os << is.rdbuf();
    }
    is.close();
}

//-----------------------------------------------------------------------------

bool WriteZipArchive::createDir(const ::boost::filesystem::path &path)
{
    zipFile zipDescriptor      = openWriteZipArchive(m_archive);
    const std::streamsize nRet = openFile(zipDescriptor, path);

    zipCloseFileInZip(zipDescriptor);
    zipClose(zipDescriptor, m_comment.c_str());

    return nRet == ZIP_OK;
}

//-----------------------------------------------------------------------------

const ::boost::filesystem::path WriteZipArchive::getArchivePath() const
{
    return m_archive;
}

//-----------------------------------------------------------------------------

}

