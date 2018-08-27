/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2018.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "fwZip/ReadZipArchive.hpp"

#include "fwZip/exception/Read.hpp"

#include "minizip/unzip.h"

#include <fwCore/exceptionmacros.hpp>

#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/path.hpp>
#include <boost/iostreams/categories.hpp>  // source_tag
#include <boost/iostreams/stream.hpp>

#include <iosfwd>    // streamsize

namespace fwZip
{

//------------------------------------------------------------------------------

void* openReadZipArchive( const ::boost::filesystem::path& archive )
{
    FW_RAISE_EXCEPTION_IF(
        ::fwZip::exception::Read("Archive '" + archive.string() + "' doesn't exist."),
        !::boost::filesystem::exists(archive));

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

    ZipSource( const ::boost::filesystem::path& archive) :
        m_zipDescriptor( openReadZipArchive(archive), &unzClose ),
        m_archive(archive)
    {

    }

    ZipSource( const ::boost::filesystem::path& archive, const ::boost::filesystem::path& path ) :
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
    ::boost::filesystem::path m_archive;
    ::boost::filesystem::path m_path;
};

//-----------------------------------------------------------------------------

ReadZipArchive::ReadZipArchive( const ::boost::filesystem::path& archive ) :
    m_archive(archive)
{
}

//-----------------------------------------------------------------------------

SPTR(std::istream) ReadZipArchive::getFile(const ::boost::filesystem::path& path)
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

const ::boost::filesystem::path ReadZipArchive::getArchivePath() const
{
    return m_archive;
}

}
