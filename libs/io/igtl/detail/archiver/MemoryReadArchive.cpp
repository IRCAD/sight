/************************************************************************
 *
 * Copyright (C) 2009-2021 IRCAD France
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

#include "io/igtl/detail/archiver/MemoryReadArchive.hpp"

#include <io/zip/exception/Read.hpp>

#include <archive_entry.h>

#include <boost/iostreams/stream.hpp>

#include <algorithm>

namespace sight::io::igtl::detail
{

namespace archiver
{

/**
 * @brief MemoryArchiveSource for boost::iostreams
 */
class MemoryArchiveSource
{
public:

    typedef char char_type;
    typedef ::boost::iostreams::source_tag category;

    /**
     * @brief constructor
     * @param[in] content readable content it correspond to a data of a entry in archive
     */
    MemoryArchiveSource(BufferCSPtr content) :
        m_readIndex(0),
        m_content(content)
    {
    }

    /// destructor
    ~MemoryArchiveSource()
    {
    }

    /**
     * @brief implement read method for boost::iostreams
     * @param[in] buffer buffer to fill with n bytes in data
     * @param[in] n maximum number of bytes to read
     * @return number of bytes of content it was read
     */
    std::streamsize read(char* buffer, std::streamsize n);

private:

    /// read index for position in content buffer
    size_t m_readIndex;

    /// content data
    BufferCSPtr m_content;
};

//-----------------------------------------------------------------------------

std::streamsize MemoryArchiveSource::read(char* s, std::streamsize n)
{
    size_t oldReadIndex;
    size_t endIndex;

    endIndex = m_readIndex + n;
    if(endIndex > m_content->size())
    {
        endIndex = m_content->size();
    }

    std::copy(m_content->begin() + m_readIndex, m_content->begin() + endIndex, s);
    oldReadIndex = m_readIndex;
    m_readIndex  = endIndex;
    return endIndex - oldReadIndex;
}

//-----------------------------------------------------------------------------

MemoryReadArchive::MemoryReadArchive(const char* buffer, const std::size_t size) :
    m_SIZE(size),
    m_BUFFER(buffer)
{
    SPTR(::boost::iostreams::stream<MemoryArchiveSource>) is;
    std::string filename;
    struct archive_entry* entry;
    BufferSPtr fileContent;

    m_archive = archive_read_new();
    archive_read_support_format_all(m_archive);
    if(archive_read_open_memory(m_archive, const_cast<char*>(buffer), size) != ARCHIVE_OK)
    {
        throw io::zip::exception::Read(
                  "Error when open memory archive : " + std::string(
                      archive_error_string(
                          m_archive
                      )
                  )
        );
    }
    else
    {
        while((archive_read_next_header(m_archive, &entry)) == ARCHIVE_OK)
        {
            fileContent = BufferSPtr(new std::vector<char>);
            filename    = std::string(archive_entry_pathname(entry));
            this->readEntry(fileContent);
            is                  = std::make_shared< ::boost::iostreams::stream<MemoryArchiveSource> >(fileContent);
            m_streams[filename] = is;
        }
    }

    if(archive_read_free(m_archive) != ARCHIVE_OK)
    {
        throw io::zip::exception::Read(
                  "Error when close memory archive : "
                  + std::string(archive_error_string(m_archive))
        );
    }
}

//-----------------------------------------------------------------------------

void MemoryReadArchive::readEntry(BufferSPtr content)
{
    std::int64_t ret;
    char buffer[MemoryReadArchive::s_BUFFER_READ_SIZE];

    while((ret = archive_read_data(m_archive, buffer, MemoryReadArchive::s_BUFFER_READ_SIZE)) > 0)
    {
        content->insert(content->end(), buffer, buffer + ret);
    }
}

//-----------------------------------------------------------------------------

MemoryReadArchive::~MemoryReadArchive()
{
}

//-----------------------------------------------------------------------------

SPTR(std::istream) MemoryReadArchive::getFile(const std::filesystem::path& path)
{
    if(m_streams.find(path.string()) != m_streams.end())
    {
        return m_streams[path.string()];
    }

    throw io::zip::exception::Read("Cannot get file");
}

//-----------------------------------------------------------------------------

const std::filesystem::path MemoryReadArchive::getArchivePath() const
{
    return std::filesystem::path("./");
}

} // namespace archiver

} // namespace sight::io::igtl::detail
