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

#include "io/zip/Archive.hpp"

#include <core/crypto/secure_string.hpp>

#include <ostream>

namespace sight::io::zip
{

/// Compression method: STORE (no compression), DEFLATE (standard zip), ZSTD (new fast compression algorithm)
enum class Method : std::uint8_t
{
    DEFAULT = 0,
    STORE   = 1,
    DEFLATE = 2,
    ZSTD    = 3
};

/// Compression level. Default is usually a good compromise between speed and compression ratio
enum class Level : std::uint8_t
{
    DEFAULT = 0,
    FAST    = 1,
    BEST    = 2,
    ULTRA   = 3
};

/**
 * @brief Class that allow writing files in an archive.
 *
 */
class IO_ZIP_CLASS_API ArchiveWriter : public Archive
{
public:

    SIGHT_DECLARE_CLASS(ArchiveWriter, Archive);

    /// Delete copy constructors and assignment operators, as we don't want to allow resources duplication
    ArchiveWriter()                                = delete;
    ArchiveWriter(const ArchiveWriter&)            = delete;
    ArchiveWriter(ArchiveWriter&&)                 = delete;
    ArchiveWriter& operator=(const ArchiveWriter&) = delete;
    ArchiveWriter& operator=(ArchiveWriter&&)      = delete;

    /// Destructor
    IO_ZIP_API ~ArchiveWriter() override = default;

    /// Shared factory. It uses a cache mechanism to return the same instance for the same archive_path.
    /// @param archive_path path of the archive file. The file will be kept opened as long as the instance leave.
    /// @param format the format of the archive. @see sight::io::zip::Archive::ArchiveFormat
    IO_ZIP_API static ArchiveWriter::uptr get(
        const std::filesystem::path& _archive_path,
        ArchiveFormat _format = ArchiveFormat::DEFAULT
    );

    /// Returns an std::ostream to read an archived file
    /// @param file_path path of the file inside the archive.
    /// @param password the password needed to encrypt the file.
    /// @param method the compression algorithm to use.
    /// @param level the compression level to use.
    IO_ZIP_API virtual std::unique_ptr<std::ostream> openFile(
        const std::filesystem::path& _file_path,
        const core::crypto::secure_string& _password = "",
        Method _method                               = Method::DEFAULT,
        Level _level                                 = Level::DEFAULT
    )                                                = 0;

    /// Returns true for raw archive
    [[nodiscard]] IO_ZIP_API virtual bool isRaw() const = 0;

protected:

    /// Constructor
    IO_ZIP_API ArchiveWriter(const std::filesystem::path& _archive_path);
};

} // namespace sight::io::zip
