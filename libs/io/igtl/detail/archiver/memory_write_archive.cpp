/************************************************************************
 *
 * Copyright (C) 2009-2023 IRCAD France
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

#include "io/igtl/detail/archiver/memory_write_archive.hpp"

#include <io/zip/exception/write.hpp>

#include <archive_entry.h>

#include <boost/date_time.hpp>

#include <filesystem>
#include <fstream>
#include <utility>

namespace sight::io::igtl::detail::archiver
{

memory_archive_sink::memory_archive_sink(
    struct archive* _archive,
    std::filesystem::path _path
) :
    m_archive(_archive),
    m_path(std::move(_path))
{
}

//-----------------------------------------------------------------------------

memory_archive_sink::~memory_archive_sink()
= default;

//-----------------------------------------------------------------------------

void memory_archive_sink::archive()
{
    struct archive_entry* entry  = archive_entry_new();
    boost::posix_time::ptime now = boost::posix_time::microsec_clock::universal_time();
    archive_entry_set_pathname(entry, m_path.string().c_str());
    archive_entry_set_filetype(entry, AE_IFREG);
    archive_entry_set_perm(entry, 0444);
    archive_entry_set_size(entry, la_int64_t(m_buffer.size()));
    const time_t seconds   = now.time_of_day().total_seconds();
    const long nanoseconds = static_cast<long>(now.time_of_day().total_nanoseconds()); // NOLINT(google-runtime-int)
    archive_entry_set_atime(entry, seconds, nanoseconds);
    archive_entry_set_birthtime(entry, seconds, nanoseconds);
    archive_entry_set_ctime(entry, seconds, nanoseconds);
    archive_entry_set_mtime(entry, seconds, nanoseconds);
    if(archive_write_header(m_archive, entry) != ARCHIVE_OK)
    {
        throw io::zip::exception::write("Cannot write header");
    }

    archive_entry_free(entry);
    for(std::size_t i = 0 ; i < m_buffer.size() ; i += memory_archive_sink::WRITE_BUFFER_SIZE)
    {
        std::size_t size = memory_archive_sink::WRITE_BUFFER_SIZE;
        if(std::size_t(i + memory_archive_sink::WRITE_BUFFER_SIZE) > m_buffer.size())
        {
            size = m_buffer.size() - i;
        }

        if(archive_write_data(m_archive, &m_buffer[i], size) < 0)
        {
            throw io::zip::exception::write("Cannot write data in archive");
        }
    }
}

//-----------------------------------------------------------------------------

std::streamsize memory_archive_sink::write(const char* _buf, std::streamsize _n)
{
    m_buffer.insert(m_buffer.end(), _buf, _buf + _n);
    return _n;
}

//-----------------------------------------------------------------------------

int memory_write_archive::open(struct archive* /*archive*/, void* /*client_data*/)
{
    return ARCHIVE_OK;
}

//-----------------------------------------------------------------------------

ssize_t memory_write_archive::write(struct archive* /*a*/, void* _client_data, const void* _buff, std::size_t _size)
{
    auto* bytes                = reinterpret_cast<std::vector<char>*>(_client_data);
    const char* bytes_to_write = reinterpret_cast<const char*>(_buff);

    bytes->insert(bytes->end(), bytes_to_write, bytes_to_write + _size);
    return ssize_t(_size);
}

//-----------------------------------------------------------------------------

int memory_write_archive::close(struct archive* /*archive*/, void* /*client_data*/)
{
    return 0;
}

//-----------------------------------------------------------------------------

memory_write_archive::memory_write_archive(std::vector<char>& _buffer) :
    m_archive(archive_write_new()),
    m_buffer(_buffer)
{
    int ret         = 0;
    void* user_data = nullptr;

    archive_write_add_filter_none(m_archive);
    archive_write_set_format_ustar(m_archive);
    archive_write_set_bytes_in_last_block(m_archive, 1);
    user_data = reinterpret_cast<void*>(&m_buffer);
    ret       = archive_write_open(
        m_archive,
        user_data,
        &memory_write_archive::open,
        &memory_write_archive::write,
        &memory_write_archive::close
    );
    if(ret != ARCHIVE_OK)
    {
        throw io::zip::exception::write("Cannot open archive in write mode");
    }
}

//-----------------------------------------------------------------------------

memory_write_archive::~memory_write_archive()
= default;

//-----------------------------------------------------------------------------

void memory_write_archive::write_archive()
{
    std::vector<stream_sptr>::iterator it;

    for(it = m_sinks.begin() ; it != m_sinks.end() ; ++it)
    {
        it->get()->flush();
        it->get()->component()->archive();
    }

    archive_write_close(m_archive);
    archive_write_free(m_archive);
}

//-----------------------------------------------------------------------------

bool memory_write_archive::create_dir(const std::filesystem::path& /*path*/)
{
    return true;
}

//-----------------------------------------------------------------------------

SPTR(std::ostream) memory_write_archive::create_file(const std::filesystem::path& _path)
{
    SPTR(boost::iostreams::stream<memory_archive_sink>) os;

    os = std::make_shared<boost::iostreams::stream<memory_archive_sink> >(m_archive, _path);
    m_sinks.push_back(os);
    return os;
}

//-----------------------------------------------------------------------------

void memory_write_archive::put_file(
    const std::filesystem::path& _source_file,
    const std::filesystem::path& _archive_file
)
{
    SPTR(std::ostream)  os;
    std::ifstream is(_source_file.string().c_str(), std::ios::binary);

    if(is.is_open())
    {
        os = this->create_file(_archive_file);
        *os << is.rdbuf();
        is.close();
    }
    else
    {
        throw io::zip::exception::write("Cannot open file : " + _source_file.string());
    }
}

//-----------------------------------------------------------------------------

std::filesystem::path memory_write_archive::get_archive_path() const
{
    return m_archive_path;
}

} // namespace sight::io::igtl::detail::archiver
