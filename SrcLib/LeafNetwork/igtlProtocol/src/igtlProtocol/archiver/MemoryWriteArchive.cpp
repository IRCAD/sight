/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "igtlProtocol/archiver/MemoryWriteArchive.hpp"

#include <fwZip/exception/Write.hpp>

#include <boost/integer_traits.hpp>
#include <boost/date_time.hpp>
#include <boost/iostreams/stream.hpp>
#include <boost/filesystem/operations.hpp>

#include <archive_entry.h>

namespace igtlProtocol
{
namespace archiver
{

MemoryArchiveSink::MemoryArchiveSink(struct archive* archive,
                                     const ::boost::filesystem::path& path) :
    m_archive(archive),
    m_path(path)
{
}

//-----------------------------------------------------------------------------

MemoryArchiveSink::~MemoryArchiveSink()
{
}

//-----------------------------------------------------------------------------

void MemoryArchiveSink::archive() throw (::fwZip::exception::Write)
{
    size_t size;

    struct archive_entry* entry;
    ::boost::posix_time::ptime now = boost::date_time::not_a_date_time;

    entry = archive_entry_new();
    now   = ::boost::posix_time::microsec_clock::universal_time();
    archive_entry_set_pathname(entry, m_path.string().c_str());
    archive_entry_set_filetype(entry, AE_IFREG);
    archive_entry_set_perm(entry, 0444);
    archive_entry_set_size(entry, m_buffer.size());
    const time_t seconds   = now.time_of_day().total_seconds();
    const long nanoseconds = static_cast<long>(now.time_of_day().total_nanoseconds());
    archive_entry_set_atime(entry, seconds, nanoseconds);
    archive_entry_set_birthtime(entry, seconds, nanoseconds);
    archive_entry_set_ctime(entry, seconds, nanoseconds);
    archive_entry_set_mtime(entry, seconds, nanoseconds);
    if (archive_write_header(m_archive, entry) != ARCHIVE_OK)
    {
        throw ::fwZip::exception::Write("Cannot write header");
    }
    archive_entry_free(entry);
    for (int i = 0; i < m_buffer.size(); i += MemoryArchiveSink::s_WRITE_BUFFER_SIZE)
    {
        size = MemoryArchiveSink::s_WRITE_BUFFER_SIZE;
        if (i + MemoryArchiveSink::s_WRITE_BUFFER_SIZE > m_buffer.size())
        {
            size = m_buffer.size() - i;
        }
        if (archive_write_data(m_archive, &m_buffer[i], size) < 0)
        {
            ::fwZip::exception::Write("Cannot write data in archive");
        }
    }
}

//-----------------------------------------------------------------------------

std::streamsize MemoryArchiveSink::write(const char* buf, std::streamsize n)
{
    m_buffer.insert(m_buffer.end(), buf, buf + n);
    return n;
}

//-----------------------------------------------------------------------------

int MemoryWriteArchive::open(struct archive* archive, void* client_data)
{
    return ARCHIVE_OK;
}

//-----------------------------------------------------------------------------

ssize_t MemoryWriteArchive::write(struct archive* a, void* client_data, const void* buff, size_t size)
{
    std::vector<char>* bytes = reinterpret_cast<std::vector<char>*>(client_data);
    const char* bytesToWrite = reinterpret_cast<const char*>(buff);

    bytes->insert(bytes->end(), bytesToWrite, bytesToWrite + size);
    return size;
}

//-----------------------------------------------------------------------------

int MemoryWriteArchive::close(struct archive* archive, void* client_data)
{
    return 0;
}

//-----------------------------------------------------------------------------

MemoryWriteArchive::MemoryWriteArchive(std::vector< char >& buffer) :
    m_buffer(buffer)
{
    int ret;
    void* userData;

    m_archive = archive_write_new();
    archive_write_add_filter_none(m_archive);
    archive_write_set_format_ustar(m_archive);
    archive_write_set_bytes_in_last_block(m_archive, 1);
    userData = reinterpret_cast<void*>(&m_buffer);
    ret      = archive_write_open(m_archive,
                                  userData,
                                  &MemoryWriteArchive::open,
                                  &MemoryWriteArchive::write,
                                  &MemoryWriteArchive::close);
    if (ret != ARCHIVE_OK)
    {
        throw ::fwZip::exception::Write("Cannot open archive in write mode");
    }
}

//-----------------------------------------------------------------------------

MemoryWriteArchive::~MemoryWriteArchive()
{
}

//-----------------------------------------------------------------------------

void MemoryWriteArchive::writeArchive() throw (::fwZip::exception::Write)
{
    std::vector<StreamSPtr>::iterator it;

    for (it = m_sinks.begin(); it != m_sinks.end(); ++it)
    {
        it->get()->flush();
        it->get()->component()->archive();
    }
    archive_write_close(m_archive);
    archive_write_free(m_archive);
}

//-----------------------------------------------------------------------------

bool MemoryWriteArchive::createDir(const ::boost::filesystem::path& path)
{
    return true;
}

//-----------------------------------------------------------------------------

SPTR(std::ostream) MemoryWriteArchive::createFile(const ::boost::filesystem::path &path)
{
    SPTR(::boost::iostreams::stream<MemoryArchiveSink>) os;

    os = std::make_shared< ::boost::iostreams::stream<MemoryArchiveSink> >(m_archive, path);
    m_sinks.push_back(os);
    return os;
}

//-----------------------------------------------------------------------------

void MemoryWriteArchive::putFile(const ::boost::filesystem::path& sourceFile,
                                 const ::boost::filesystem::path& archiveFile) throw(::fwZip::exception::Write)
{
    SPTR(std::ostream)  os;
    std::ifstream is(sourceFile.string().c_str(), std::ios::binary);

    if (is.is_open())
    {
        os = this->createFile(archiveFile);
        *os << is.rdbuf();
        is.close();
    }
    else
    {
        throw fwZip::exception::Write("Cannot open file : " + sourceFile.string());
    }
}

//-----------------------------------------------------------------------------

const ::boost::filesystem::path MemoryWriteArchive::getArchivePath() const
{
    return m_archivePath;
}

} // namespace archiver
} // namespace igtlProtocol
