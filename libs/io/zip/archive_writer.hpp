/************************************************************************
 *
 * Copyright (C) 2009-2023 IRCAD France
 * Copyright (C) 2012-2021 IHU Strasbourg
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

#include "io/zip/archive.hpp"

#include <core/crypto/secure_string.hpp>

#include <ostream>

namespace sight::io::zip
{

/// Compression method: STORE (no compression), DEFLATE (standard zip), ZSTD (new fast compression algorithm)
enum class method : std::uint8_t
{
    DEFAULT = 0,
    store   = 1,
    deflate = 2,
    zstd    = 3
};

/// Compression level. Default is usually a good compromise between speed and compression ratio
enum class level : std::uint8_t
{
    DEFAULT = 0,
    fast    = 1,
    best    = 2,
    ultra   = 3
};

/**
 * @brief Class that allow writing files in an archive.
 *
 */
class IO_ZIP_CLASS_API archive_writer : public archive
{
public:

    SIGHT_DECLARE_CLASS(archive_writer, archive);

    /// Delete copy constructors and assignment operators, as we don't want to allow resources duplication
    archive_writer()                                 = delete;
    archive_writer(const archive_writer&)            = delete;
    archive_writer(archive_writer&&)                 = delete;
    archive_writer& operator=(const archive_writer&) = delete;
    archive_writer& operator=(archive_writer&&)      = delete;

    /// Destructor
    IO_ZIP_API ~archive_writer() override = default;

    /// Shared factory. It uses a cache mechanism to return the same instance for the same archive_path.
    /// @param archive_path path of the archive file. The file will be kept opened as long as the instance leave.
    /// @param format the format of the archive. @see sight::io::zip::archive::archiveFormat
    IO_ZIP_API static archive_writer::uptr get(
        const std::filesystem::path& _archive_path,
        archive_format _format = archive_format::DEFAULT
    );

    /// Returns an std::ostream to read an archived file
    /// @param file_path path of the file inside the archive.
    /// @param password the password needed to encrypt the file.
    /// @param method the compression algorithm to use.
    /// @param level the compression level to use.
    IO_ZIP_API virtual std::unique_ptr<std::ostream> open_file(
        const std::filesystem::path& _file_path,
        const core::crypto::secure_string& _password = "",
        method _method                               = method::DEFAULT,
        level _level                                 = level::DEFAULT
    )                                                = 0;

    /// Returns true for raw archive
    [[nodiscard]] IO_ZIP_API virtual bool is_raw() const = 0;

protected:

    /// Constructor
    IO_ZIP_API archive_writer(const std::filesystem::path& _archive_path);
};

} // namespace sight::io::zip
