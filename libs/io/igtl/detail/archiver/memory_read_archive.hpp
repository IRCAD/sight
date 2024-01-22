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

#pragma once

#include "io/igtl/config.hpp"

#include <core/macros.hpp>

#include <io/zip/exception/read.hpp>
#include <io/zip/read_archive.hpp>

#include <archive.h>

#ifdef _MSC_VER
// warning for unreachable code in Release/RelWithDebInfo in boost::iostreams whereas it should be ignored,
// see https://developercommunity.visualstudio.com/t/error:-C4702-with-external:w0/1696694
#pragma warning(disable : 4702)
#endif // _MSC_VER
#include <boost/iostreams/categories.hpp>

#include <map>
#include <vector>

namespace sight::io::igtl::detail::archiver
{

using buffer_s_ptr  = std::shared_ptr<std::vector<char> >;
using buffer_cs_ptr = std::shared_ptr<const std::vector<char> >;

/**
 *
 * @brief MemoryReadArchive is a memory archive reader
 */
class IO_IGTL_CLASS_API memory_read_archive : public io::zip::read_archive
{
public:

    using sptr = std::shared_ptr<memory_read_archive>;

    /// constructor
    IO_IGTL_API memory_read_archive(const char* _buffer, std::size_t _size);

    /// destructor
    IO_IGTL_API ~memory_read_archive() override;

    /**
     * @brief Returns input stream for the file in current archive (zip).
     * @param[in] _path file in archive.
     * @return input stream from request file.
     *
     * @throw io::zip::exception::Read if file doesn't exist in archive.
     * @throw io::zip::exception::Read if cannot retrieve file in archive.
     */
    IO_IGTL_API SPTR(std::istream) get_file(const std::filesystem::path& _path) override;

    /**
     * @brief Returns archive path.
     *
     * @return the archive path
     */
    [[nodiscard]] IO_IGTL_API std::filesystem::path get_archive_path() const override;

    /**
     * @brief clone the instance
     *
     * @return cloned instance
     */
    [[nodiscard]] read_archive::sptr clone() const override
    {
        return SPTR(memory_read_archive)(new memory_read_archive(m_buffer, m_size));
    }

private:

    /**
     * @brief read file data from buffer
     *
     * @param[in] _content buffer data to fill
     */
    void read_entry(buffer_s_ptr _content);

private:

    /// size of archive
    const std::size_t m_size;

    /// buffer contain the archive data
    const char* m_buffer;

    /// archive instance
    struct archive* m_archive;

    /// stream created when unarchive is done
    std::map<std::string, SPTR(std::istream)> m_streams;

    /// buffer read size in memory(huge because we have to read in memory, big buffer should be efficient)
    static const int BUFFER_READ_SIZE = 20000;
};

} // namespace sight::io::igtl::detail::archiver
