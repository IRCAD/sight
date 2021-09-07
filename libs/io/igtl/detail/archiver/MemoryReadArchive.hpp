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

#pragma once

#include "io/igtl/config.hpp"

#include <core/macros.hpp>

#include <io/zip/exception/Read.hpp>
#include <io/zip/IReadArchive.hpp>

#include <archive.h>

#include <boost/iostreams/categories.hpp>

#include <map>
#include <vector>

namespace sight::io::igtl::detail
{

namespace archiver
{

typedef SPTR(std::vector<char>)  BufferSPtr;
typedef CSPTR(std::vector<char>) BufferCSPtr;

/**
 *
 * @brief MemoryReadArchive is a memory archive reader
 */
class IO_IGTL_CLASS_API MemoryReadArchive : public io::zip::IReadArchive
{
public:

    typedef SPTR(MemoryReadArchive) sptr;

    /// constructor
    IO_IGTL_API MemoryReadArchive(const char* buffer, const std::size_t size);

    /// destructor
    IO_IGTL_API ~MemoryReadArchive();

    /**
     * @brief Returns input stream for the file in current archive (zip).
     * @param[in] path file in archive.
     * @return input stream from request file.
     *
     * @throw io::zip::exception::Read if file doesn't exist in archive.
     * @throw io::zip::exception::Read if cannot retrieve file in archive.
     */
    IO_IGTL_API SPTR(std::istream) getFile(const std::filesystem::path& path) override;

    /**
     * @brief Returns archive path.
     *
     * @return the archive path
     */
    IO_IGTL_API const std::filesystem::path getArchivePath() const override;

    /**
     * @brief clone the instance
     *
     * @return cloned instance
     */
    IReadArchive::sptr clone() const override
    {
        return SPTR(MemoryReadArchive)(new MemoryReadArchive(m_BUFFER, m_SIZE));
    }

private:

    /**
     * @brief read file data from buffer
     *
     * @param[in] buffer buffer data to fill
     */
    void readEntry(BufferSPtr buffer);

private:

    /// size of archive
    const std::size_t m_SIZE;

    /// buffer contain the archive data
    const char* m_BUFFER;

    /// archive instance
    struct archive* m_archive;

    /// stream created when unarchive is done
    std::map<std::string, SPTR(std::istream)> m_streams;

    /// buffer read size in memory(huge because we have to read in memory, big buffer should be efficient)
    static const int s_BUFFER_READ_SIZE = 20000;
};

} // namespace archiver

} // namespace sight::io::igtl::detail
