/************************************************************************
 *
 * Copyright (C) 2009-2024 IRCAD France
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

///// ugly but we must for windows system typedef SSIZE_T to ssize_t for portability
#pragma once

#if defined(_WIN32)
#include <BaseTsd.h>
typedef SSIZE_T ssize_t;
#endif

#include <sight/io/igtl/config.hpp>

#include <io/zip/write_archive.hpp>
#include <io/zip/exception/write.hpp>
#include <core/macros.hpp>

#include <archive.h>

#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable:4702)
#endif

#include <boost/iostreams/categories.hpp>
#include <boost/iostreams/stream.hpp>

#ifdef _MSC_VER
#pragma warning(pop)
#endif

#include <cctype>
#include <filesystem>
#include <vector>

namespace sight::io::igtl::detail::archiver
{

/**
 *
 * @brief class provide utility to write a file to a archive. This class is used locally
 */
class memory_archive_sink
{
public:

    using char_type = char;
    using category  = boost::iostreams::sink_tag;

    /**
     * @brief constructor
     *
     * @param[in] _archive archive instance
     * @param[in] _path virtual path in archive if file exist it get the stat of file otherwise create fake stat
     */
    memory_archive_sink(struct archive* _archive, std::filesystem::path _path);

    /// Destructor
    ~memory_archive_sink();

    /**
     * @brief write buffer in archive buffer
     */
    void archive();

    /**
     * @brief writter to write data in archive entry
     */
    std::streamsize write(const char* _buf, std::streamsize _n);

protected:

    /// instance archive
    struct archive* m_archive;

    /// dynamic buffer to write data
    std::vector<char> m_buffer;

    /// path of entry file
    std::filesystem::path m_path;

    /// buffer write size is large because it is in memory
    static const std::size_t WRITE_BUFFER_SIZE = 20000;
};

/**
 *
 * @brief MemoryWriteArchive is a memory archive writer
 */
class SIGHT_IO_IGTL_CLASS_API memory_write_archive : public io::zip::write_archive
{
public:

    using sptr = std::shared_ptr<memory_write_archive>;

    /// Constructor
    SIGHT_IO_IGTL_API memory_write_archive(std::vector<char>& _buffer);

    /// Destructor
    SIGHT_IO_IGTL_API ~memory_write_archive() override;

    /**
     * @brief create new entry in archive and return output stream for this memory file
     *
     * @param[in] _path file in archive
     * @return output stream of memory entry archive
     */
    SIGHT_IO_IGTL_API SPTR(std::ostream) create_file(const std::filesystem::path& _path) override;

    /**
     * @brief Write source file in memory archive
     *
     * @param[in] _source_file source file
     * @param[in] _path file in archive
     * @throw io::zip::exception::Write when file cannot be opened
     */
    SIGHT_IO_IGTL_API void put_file(
        const std::filesystem::path& _source_file,
        const std::filesystem::path& _path
    ) override;

    /**
     * @brief Create folder in archive
     *
     * @param[in] _path folder to create in memory archive in reality it create nothing
     */
    SIGHT_IO_IGTL_API bool create_dir(const std::filesystem::path& _path) override;

    /**
     * @return archive path
     */
    [[nodiscard]] SIGHT_IO_IGTL_API std::filesystem::path get_archive_path() const override;

    /**
     * @brief write all data stored in archive
     */
    SIGHT_IO_IGTL_API void write_archive();

public:

    /**
     * @brief open callback for archive instance
     *
     * @param[in] _archive archive instance
     * @param[in] _client_data contain a pointer std::vector internally
     */
    static int open(struct archive* _archive, void* _client_data);

    /**
     * @brief write callback for archive instance
     *
     * @param[in] _archive archive instance
     * @param[out] _client_data contain a pointer to a std::vector
     * @param[in] _buff buff to copy in vector
     * @param[in] _size number of byte to write
     * @return number of byte have been written
     */
    static ssize_t write(struct archive* _archive, void* _client_data, const void* _buff, std::size_t _size);

    /**
     * @brief close callback for archive instance
     *
     *
     * @param[in] _archive archive instance
     * @param[in] _client_data callback user parameter
     */
    static int close(struct archive* _archive, void* _client_data);

protected:

    using stream_sptr = std::shared_ptr<boost::iostreams::stream<memory_archive_sink> >;

    /// archive path
    std::filesystem::path m_archive_path;

    /// archive structure
    struct archive* m_archive;

    /// buffer where is stored archive data
    std::vector<char>& m_buffer;

    /// vector of stream memory archive files
    std::vector<stream_sptr> m_sinks;
};

} // namespace sight::io::igtl::detail::archiver
