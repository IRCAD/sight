#include <iosfwd>    // streamsize
#include <fstream>

#include <boost/filesystem/path.hpp>
#include <boost/filesystem/operations.hpp>

#include <boost/iostreams/stream_buffer.hpp>

#include <boost/date_time/posix_time/posix_time_types.hpp>
#include <boost/date_time/posix_time/conversion.hpp>

#include <fwCore/exceptionmacros.hpp>

#include "zip.h"
#include "fwZip/WriteZipArchive.hpp"
#include "fwZip/exception/Write.hpp"


namespace fwZip
{

std::streamsize ZipSink::write(const char* s, std::streamsize n)
{
    int nRet = zipWriteInFileInZip(m_zipDescriptor, s, n);
    FW_RAISE_EXCEPTION_IF(
                    ::fwZip::exception::Write("Error occurred while writing archive."),
                     nRet < 0);
    return n;
}

//-----------------------------------------------------------------------------

void * openWriteZipArchive( const ::boost::filesystem::path &archive )
{
    FW_RAISE_EXCEPTION_IF(
            ::fwZip::exception::Write("Archive '" + archive.string() + "' already exists."),
            ::boost::filesystem::exists(archive));

    void * zip = zipOpen(archive.string().c_str(), APPEND_STATUS_CREATE);

    FW_RAISE_EXCEPTION_IF(
                ::fwZip::exception::Write("Archive '" + archive.string() + "' cannot be opened."),
                 zip == NULL);

    return zip;
}

//-----------------------------------------------------------------------------

WriteZipArchive::WriteZipArchive( const ::boost::filesystem::path &archive ) :
    m_archive(archive),
    m_zipDescriptor(openWriteZipArchive(archive)),
    m_streambuf(m_zipDescriptor),
    m_ostream( &m_streambuf )
{}

//-----------------------------------------------------------------------------

WriteZipArchive::~WriteZipArchive()
{
    m_ostream.flush();
    zipClose(m_zipDescriptor, NULL);
}

//-----------------------------------------------------------------------------

std::ostream& WriteZipArchive::createFile(const ::boost::filesystem::path &path)
{
    m_ostream.flush();
    int nRet = this->openFile(path);
    return m_ostream;
}

//-----------------------------------------------------------------------------

void WriteZipArchive::putFile(const ::boost::filesystem::path &sourceFile, const ::boost::filesystem::path &path)
{
    std::ifstream sourceStream(sourceFile.string().c_str(), std::ios::binary);
    FW_RAISE_EXCEPTION_IF(::fwZip::exception::Write("Source file '" + sourceFile.string() + "' cannot be opened."),
                         !sourceStream.good());

    std::ostream& oStream = this->createFile(path);
    oStream << sourceStream.rdbuf();
    sourceStream.close();
}

//-----------------------------------------------------------------------------

bool WriteZipArchive::createDir(const ::boost::filesystem::path &path)
{
    int nRet = this->openFile(path);
    this->closeFile();
    return nRet == ZIP_OK;
}

//-----------------------------------------------------------------------------

int WriteZipArchive::openFile(const ::boost::filesystem::path &path)
{
    const std::string extension = path.extension().string();
    int compressLevel = Z_DEFAULT_COMPRESSION;
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
    zfi.dosDate = 0;
    zfi.tmz_date.tm_year = ptm.tm_year;
    zfi.tmz_date.tm_mon  = ptm.tm_mon;
    zfi.tmz_date.tm_mday = ptm.tm_mday;
    zfi.tmz_date.tm_hour = ptm.tm_hour;
    zfi.tmz_date.tm_min  = ptm.tm_min;
    zfi.tmz_date.tm_sec  = ptm.tm_sec;

    int nRet = zipOpenNewFileInZip(m_zipDescriptor,
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

void WriteZipArchive::closeFile()
{
    zipCloseFileInZip(m_zipDescriptor);
}

//-----------------------------------------------------------------------------

const ::boost::filesystem::path WriteZipArchive::getArchivePath() const
{
    return m_archive;
}

//-----------------------------------------------------------------------------

}

