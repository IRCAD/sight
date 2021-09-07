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

///// ugly but we must for windows system typedef SSIZE_T to ssize_t for portability
#pragma once

#if defined(_WIN32)
#include <BaseTsd.h>
typedef SSIZE_T ssize_t;
#endif

#include "io/igtl/config.hpp"

#include <io/zip/IWriteArchive.hpp>
#include <io/zip/exception/Write.hpp>
#include <core/macros.hpp>

#include <boost/iostreams/stream.hpp>
#include <filesystem>
#include <boost/iostreams/categories.hpp>

#include <archive.h>

#include <ctype.h>
#include <vector>

namespace sight::io::igtl::detail
{

namespace archiver
{

/**
 *
 * @brief class provide utility to write a file to a archive. This class is used locally
 */
class MemoryArchiveSink
{
public:

    typedef char char_type;
    typedef ::boost::iostreams::sink_tag category;

    /**
     * @brief constructor
     *
     * @param[in] archive archive instance
     * @param[in] path virtual path in archive if file exist it get the stat of file otherwise create fake stat
     */
    MemoryArchiveSink(struct archive* archive, const std::filesystem::path& path);

    /// Destructor
    ~MemoryArchiveSink();

    /**
     * @brief write buffer in archive buffer
     */
    void archive();

    /**
     * @brief writter to write data in archive entry
     */
    std::streamsize write(const char* buf, std::streamsize n);

protected:

    /// instance archive
    struct archive* m_archive;

    /// dynamic buffer to write data
    std::vector<char> m_buffer;

    /// path of entry file
    std::filesystem::path m_path;

    /// buffer write size is large because it is in memory
    static const size_t s_WRITE_BUFFER_SIZE = 20000;
};

/**
 *
 * @brief MemoryWriteArchive is a memory archive writer
 */
class IO_IGTL_CLASS_API MemoryWriteArchive : public io::zip::IWriteArchive
{
public:

    typedef SPTR(MemoryWriteArchive) sptr;

    /// Constructor
    IO_IGTL_API MemoryWriteArchive(std::vector<char>& buffer);

    /// Destructor
    IO_IGTL_API ~MemoryWriteArchive();

    /**
     * @brief create new entry in archive and return output stream for this memory file
     *
     * @param[in] path file in archive
     * @return output stream of memory entry archive
     */
    IO_IGTL_API SPTR(std::ostream) createFile(const std::filesystem::path& path) override;

    /**
     * @brief Write source file in memory archive
     *
     * @param[in] sourceFile source file
     * @param[in] path file in archive
     * @throw io::zip::exception::Write when file cannot be opened
     */
    IO_IGTL_API void putFile(
        const std::filesystem::path& sourceFile,
        const std::filesystem::path& path
    ) override;

    /**
     * @brief Create folder in archive
     *
     * @param[in] path folder to create in memory archive in reality it create nothing
     */
    IO_IGTL_API bool createDir(const std::filesystem::path& path) override;

    /**
     * @return archive path
     */
    IO_IGTL_API const std::filesystem::path getArchivePath() const override;

    /**
     * @brief write all data stored in archive
     */
    IO_IGTL_API void writeArchive();

public:

    /**
     * @brief open callback for archive instance
     *
     * @param[in] archive archive instance
     * @param[in] client_data contain a pointer std::vector internally
     */
    static int open(struct archive* archive, void* client_data);

    /**
     * @brief write callback for archive instance
     *
     * @param[in] archive archive instance
     * @param[out] client_data contain a pointer to a std::vector
     * @param[in] buff buff to copy in vector
     * @param[in] n number of byte to write
     * @return number of byte have been written
     */
    static ssize_t write(struct archive* archive, void* client_data, const void* buff, size_t n);

    /**
     * @brief close callback for archive instance
     *
     *
     * @param[in] archive archive instance
     * @param[in] client_data callback user parameter
     */
    static int close(struct archive* archive, void* client_data);

protected:

    typedef SPTR(::boost::iostreams::stream<MemoryArchiveSink>) StreamSPtr;

    /// archive path
    std::filesystem::path m_archivePath;

    /// archive structure
    struct archive* m_archive;

    /// buffer where is stored archive data
    std::vector<char>& m_buffer;

    /// vector of stream memory archive files
    std::vector<StreamSPtr> m_sinks;
};

} // namespace archiver

} // namespace sight::io::igtl::detail
