/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2013.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <iosfwd>    // streamsize

#include <boost/filesystem/path.hpp>
#include <boost/filesystem/operations.hpp>

#include <boost/iostreams/stream_buffer.hpp>

#include <fwCore/exceptionmacros.hpp>

#include "unzip.h"
#include "fwZip/ReadZipArchive.hpp"
#include "fwZip/exception/Read.hpp"


namespace fwZip
{

std::streamsize ZipSource::read(char* s, std::streamsize n)
{
    int nRet = unzReadCurrentFile(m_zipDescriptor, s, n);
    FW_RAISE_EXCEPTION_IF(
                        ::fwZip::exception::Read("Error occurred while reading archive."),
                         nRet < 0);
    return nRet;
}


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

ReadZipArchive::ReadZipArchive( const ::boost::filesystem::path &archive ) :
    m_archive(archive),
    m_zipDescriptor(openReadZipArchive(archive)),
    m_streambuf(m_zipDescriptor),
    m_istream( &m_streambuf )
{}

//-----------------------------------------------------------------------------

ReadZipArchive::~ReadZipArchive()
{
    unzClose(m_zipDescriptor);
}

//-----------------------------------------------------------------------------

std::istream& ReadZipArchive::getFile(const ::boost::filesystem::path &path)
{
    FW_RAISE_EXCEPTION_IF(
            ::fwZip::exception::Read("File '" +  path.string() + "' in archive '" +
                                     m_archive.string() + "' doesn't exist."),
             !this->exists(path));

    int nRet = unzOpenCurrentFile(m_zipDescriptor);
    FW_RAISE_EXCEPTION_IF(
            ::fwZip::exception::Read("Cannot retrieve file '" + path.string() +
                                     "' in archive '"+ m_archive.string() +"'."),
            nRet != UNZ_OK);

    return m_istream;
}

//-----------------------------------------------------------------------------

bool ReadZipArchive::exists(const ::boost::filesystem::path &path)
{
    int nRet = unzLocateFile(m_zipDescriptor, path.string().c_str(), 0);
    return (nRet == UNZ_OK);
}

//-----------------------------------------------------------------------------

const ::boost::filesystem::path ReadZipArchive::getArchivePath() const
{
    return m_archive;
}


}

