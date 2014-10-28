/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2013.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <iosfwd>    // streamsize

#include <boost/make_shared.hpp>

#include <boost/filesystem/path.hpp>
#include <boost/filesystem/operations.hpp>

#include <boost/iostreams/stream.hpp>
#include <boost/iostreams/categories.hpp>  // source_tag

#include <fwCore/exceptionmacros.hpp>

#include "minizip/unzip.h"
#include "fwZip/ReadZipArchive.hpp"
#include "fwZip/exception/Read.hpp"


namespace fwZip
{



void * openReadZipArchive( const ::boost::filesystem::path &archive )
{
    FW_RAISE_EXCEPTION_IF(
            ::fwZip::exception::Read("Archive '" + archive.string() + "' doesn't exist."),
            !::boost::filesystem::exists(archive));

    void * zip = unzOpen(archive.string().c_str());

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
    typedef ::boost::iostreams::source_tag  category;

    ZipSource( const ::boost::filesystem::path &archive, const ::boost::filesystem::path &path ) :
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

    std::streamsize read(char* s, std::streamsize n)
    {
        int nRet = unzReadCurrentFile(m_zipDescriptor.get(), s, n);
        FW_RAISE_EXCEPTION_IF(
                            ::fwZip::exception::Read("Error occurred while reading archive '" + m_archive.string()
                                                     + ":" + m_path.string() + "'."),
                            nRet < 0);
        return nRet;
    }

protected:
    SPTR(void) m_zipDescriptor;
    ::boost::filesystem::path m_archive;
    ::boost::filesystem::path m_path;
};



//-----------------------------------------------------------------------------

ReadZipArchive::ReadZipArchive( const ::boost::filesystem::path &archive ) :
    m_archive(archive)
{}

//-----------------------------------------------------------------------------

SPTR(std::istream) ReadZipArchive::getFile(const ::boost::filesystem::path &path)
{
    SPTR(::boost::iostreams::stream<ZipSource>) is
        = ::boost::make_shared< ::boost::iostreams::stream<ZipSource> >(m_archive, path);

    return is;
}


//-----------------------------------------------------------------------------

const ::boost::filesystem::path ReadZipArchive::getArchivePath() const
{
    return m_archive;
}


}

