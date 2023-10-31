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

#include "io/igtl/detail/archiver/memory_read_archive.hpp"

#include <io/zip/exception/read.hpp>

#include <archive_entry.h>

#include <boost/iostreams/stream.hpp>

#include <algorithm>
#include <array>
#include <memory>
#include <utility>

namespace sight::io::igtl::detail::archiver
{

/**
 * @brief MemoryArchiveSource for boost::iostreams
 */
class memory_archive_source
{
public:

    using char_type = char;
    using category  = boost::iostreams::source_tag;

    /**
     * @brief constructor
     * @param[in] content readable content it correspond to a data of a entry in archive
     */
    explicit memory_archive_source(buffer_cs_ptr _content) :
        m_content(std::move(_content))
    {
    }

    /// destructor
    ~memory_archive_source()
    = default;

    /**
     * @brief implement read method for boost::iostreams
     * @param[in] buffer buffer to fill with n bytes in data
     * @param[in] n maximum number of bytes to read
     * @return number of bytes of content it was read
     */
    std::streamsize read(char* _s, std::streamsize _n);

private:

    /// read index for position in content buffer
    std::size_t m_read_index {0};

    /// content data
    buffer_cs_ptr m_content;
};

//-----------------------------------------------------------------------------

std::streamsize memory_archive_source::read(char* _s, std::streamsize _n)
{
    std::size_t old_read_index = 0;
    std::size_t end_index      = 0;

    end_index = std::size_t(std::int64_t(m_read_index) + _n);
    if(end_index > m_content->size())
    {
        end_index = m_content->size();
    }

    std::copy(m_content->begin() + std::int64_t(m_read_index), m_content->begin() + std::int64_t(end_index), _s);
    old_read_index = m_read_index;
    m_read_index   = end_index;
    return std::int64_t(end_index - old_read_index);
}

//-----------------------------------------------------------------------------

memory_read_archive::memory_read_archive(const char* _buffer, const std::size_t _size) :
    m_size(_size),
    m_buffer(_buffer),
    m_archive(archive_read_new())
{
    SPTR(boost::iostreams::stream<memory_archive_source>) is;
    std::string filename;
    struct archive_entry* entry = nullptr;
    buffer_s_ptr file_content;

    archive_read_support_format_all(m_archive);
    if(archive_read_open_memory(m_archive, _buffer, _size) != ARCHIVE_OK)
    {
        throw io::zip::exception::read(
                  "Error when open memory archive : " + std::string(
                      archive_error_string(
                          m_archive
                      )
                  )
        );
    }

    while((archive_read_next_header(m_archive, &entry)) == ARCHIVE_OK)
    {
        file_content = std::make_shared<std::vector<char> >();
        filename     = std::string(archive_entry_pathname(entry));
        this->read_entry(file_content);
        is                  = std::make_shared<boost::iostreams::stream<memory_archive_source> >(file_content);
        m_streams[filename] = is;
    }

    if(archive_read_free(m_archive) != ARCHIVE_OK)
    {
        throw io::zip::exception::read(
                  "Error when close memory archive : "
                  + std::string(archive_error_string(m_archive))
        );
    }
}

//-----------------------------------------------------------------------------

void memory_read_archive::read_entry(buffer_s_ptr _content)
{
    std::int64_t ret = 0;
    std::array<char, memory_read_archive::BUFFER_READ_SIZE> buffer {};

    while((ret = archive_read_data(m_archive, buffer.data(), memory_read_archive::BUFFER_READ_SIZE)) > 0)
    {
        _content->insert(_content->end(), buffer.data(), buffer.data() + ret);
    }
}

//-----------------------------------------------------------------------------

memory_read_archive::~memory_read_archive()
= default;

//-----------------------------------------------------------------------------

SPTR(std::istream) memory_read_archive::get_file(const std::filesystem::path& _path)
{
    if(m_streams.find(_path.string()) != m_streams.end())
    {
        return m_streams[_path.string()];
    }

    throw io::zip::exception::read("Cannot get file");
}

//-----------------------------------------------------------------------------

std::filesystem::path memory_read_archive::get_archive_path() const
{
    return {"./"};
}

} // namespace sight::io::igtl::detail::archiver
