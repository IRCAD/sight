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

#include "io/zip/WriteZipArchive.hpp"

#include "io/zip/exception/Write.hpp"

#include "minizip/zip.h"

#include <core/exceptionmacros.hpp>

#include <boost/date_time/posix_time/conversion.hpp>
#include <boost/date_time/posix_time/posix_time_types.hpp>
#include <boost/iostreams/categories.hpp> // sink_tag
#include <boost/iostreams/stream.hpp>
#include <boost/iostreams/stream_buffer.hpp>

#include <zlib.h>

#include <filesystem>
#include <fstream>
#include <functional>
#include <iosfwd> // streamsize

namespace sight::io::zip
{

//-----------------------------------------------------------------------------

zipFile openWriteZipArchive(const std::filesystem::path& archive)
{
    int append  = (std::filesystem::exists(archive)) ? APPEND_STATUS_ADDINZIP : APPEND_STATUS_CREATE;
    zipFile zip = zipOpen(archive.string().c_str(), append);

    SIGHT_THROW_EXCEPTION_IF(
        io::zip::exception::Write("Archive '" + archive.string() + "' cannot be opened."),
        zip == NULL
    );

    return zip;
}

//-----------------------------------------------------------------------------

/*
 * @brief  Open a file in the zip archive for writing
 */
std::streamsize openFile(
    zipFile zipDescriptor,
    const std::filesystem::path& path,
    const std::string& key
)
{
    const std::string filepath = path.generic_string();
    const char* const filename = filepath.c_str();

    zip_fileinfo zfileinfo;
    memset(&zfileinfo, 0, sizeof(zfileinfo));

    const char* const password = key.empty() ? nullptr : key.c_str();

    const int nRet = zipOpenNewFileInZip5(
        zipDescriptor,         // zipFile file
        filename,              // const char *filename : the filename in zip
        &zfileinfo,            // const zip_fileinfo *zipfi: contain supplemental information
        nullptr,               // (UNUSED) const void *extrafield_local: buffer to store the local header
                               // extra field data
        0,                     // (UNUSED) uint16_t size_extrafield_local: size of extrafield_local buffer
        nullptr,               // const void *extrafield_global: buffer to store the global header extra
                               // field data
        0,                     // uint16_t size_extrafield_global: size of extrafield_local buffer
        nullptr,               // const char *comment: buffer for comment string
        Z_DEFLATED,            // int method: contain the compression method
        Z_DEFAULT_COMPRESSION, // int level: contain the level of compression
        0,                     // int raw: use 0 to disable
        0,                     // (UNUSED) int windowBits: use default value
        0,                     // (UNUSED) int memLevel: use default value
        0,                     // (UNUSED) int strategy: use default value
        password,              // const char *password: NULL means no encryption
        0,                     // (UNUSED) unsigned long crc_for_crypting,
        666,                   // unsigned long version_madeby
        0,                     // uint16_t flag_base: use default value
        1                      // int zip64: use 0 to disable
    );

    return nRet;
}

//-----------------------------------------------------------------------------

// This stupid class is mandatory to allow ZipSink ctor to have more than 3 parameters.
// Ask boost why...
struct ZipSinkParameter
{
    ZipSinkParameter(
        const std::filesystem::path& archive,
        const std::filesystem::path& path,
        const std::string& comment = "",
        const std::string& key     = ""
    ) :
        m_archive(archive),
        m_path(path),
        m_comment(comment),
        m_key(key)
    {
    }

    const std::filesystem::path& m_archive;
    const std::filesystem::path& m_path;
    const std::string& m_comment;
    const std::string& m_key;
};

class ZipSink
{
public:

    typedef char char_type;
    typedef ::boost::iostreams::sink_tag category;

    ZipSink(const ZipSinkParameter& parameter) :
        m_zipDescriptor(
            openWriteZipArchive(parameter.m_archive),
            [parameter](zipFile zipDescriptor)
        {
            zipClose(zipDescriptor, parameter.m_comment.c_str());
        }),
        m_archive(parameter.m_archive),
        m_path(parameter.m_path)
    {
        const std::streamsize nRet = openFile(
            m_zipDescriptor.get(),
            m_path,
            parameter.m_key
        );
        SIGHT_THROW_EXCEPTION_IF(
            io::zip::exception::Write(
                "Cannot open file '"
                + m_path.string()
                + "' in archive '"
                + m_archive.string()
                + "'."
            ),
            nRet != Z_OK
        );
    }

    //------------------------------------------------------------------------------

    std::streamsize write(const char* s, std::streamsize n)
    {
        std::streamsize nRet = zipWriteInFileInZip(m_zipDescriptor.get(), s, static_cast<unsigned int>(n));
        SIGHT_THROW_EXCEPTION_IF(
            io::zip::exception::Write(
                "Error occurred while writing archive '" + m_archive.string()
                + ":" + m_path.string() + "'."
            ),
            nRet < 0
        );
        return n;
    }

protected:

    SPTR(void) m_zipDescriptor;
    std::filesystem::path m_archive;
    std::filesystem::path m_path;
};

//-----------------------------------------------------------------------------

SPTR(std::ostream) WriteZipArchive::createFile(const std::filesystem::path& path)
{
    return std::make_shared< ::boost::iostreams::stream<ZipSink> >(ZipSinkParameter(m_archive, path, m_comment, m_key));
}

//-----------------------------------------------------------------------------

void WriteZipArchive::putFile(const std::filesystem::path& sourceFile, const std::filesystem::path& path)
{
    std::ifstream is(sourceFile.string().c_str(), std::ios::binary);
    SIGHT_THROW_EXCEPTION_IF(
        io::zip::exception::Write("Source file '" + sourceFile.string() + "' cannot be opened."),
        !is.good()
    );

    {
        SPTR(std::ostream) os = this->createFile(path);
        *os << is.rdbuf();
    }
    is.close();
}

//-----------------------------------------------------------------------------

bool WriteZipArchive::createDir(const std::filesystem::path& path)
{
    zipFile zipDescriptor      = openWriteZipArchive(m_archive);
    const std::streamsize nRet = openFile(zipDescriptor, path, m_key);

    zipCloseFileInZip(zipDescriptor);
    zipClose(zipDescriptor, m_comment.c_str());

    return nRet == ZIP_OK;
}

//-----------------------------------------------------------------------------

const std::filesystem::path WriteZipArchive::getArchivePath() const
{
    return m_archive;
}

//-----------------------------------------------------------------------------

} // namespace sight::io
