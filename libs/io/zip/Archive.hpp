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

#include "io/zip/config.hpp"

#include <core/macros.hpp>

#include <filesystem>

namespace sight::io::zip
{

/// Class that allow reading and writing an archive.
class IO_ZIP_CLASS_API Archive
{
public:

    SIGHT_DECLARE_CLASS(Archive);

    /// Delete default constructors and assignment operators, as we don't want to allow resources duplication
    Archive()                          = delete;
    Archive(const Archive&)            = delete;
    Archive(Archive&&)                 = delete;
    Archive& operator=(const Archive&) = delete;
    Archive& operator=(Archive&&)      = delete;

    /// Destructor
    IO_ZIP_API virtual ~Archive();

    /// Enum to define
    enum class ArchiveFormat : uint8_t
    {
        FILESYSTEM = 0,         /// Use the filesystem to store files.
        COMPATIBLE = 2,         /// Store files in a ZIP archive, with old deflate algorithm
        OPTIMIZED  = 3,         /// Store files in a ZIP archive, with zstd algorithm
        DEFAULT    = OPTIMIZED, /// Default behavior if nothing is set
        INVALID    = 255        /// Used for error management
    };

    /// Convenience function to convert from ArchiveFormat enum value to string
    constexpr static std::string_view archiveFormatToString(ArchiveFormat archiveFormat) noexcept
    {
        switch(archiveFormat)
        {
            case ArchiveFormat::FILESYSTEM:
                return "filesystem";

            case ArchiveFormat::COMPATIBLE:
                return "compatible";

            case ArchiveFormat::OPTIMIZED:
                return "optimized";

            default:
                return "default";
        }
    }

    /// Convenience function to convert from string to ArchiveFormat enum value
    constexpr static ArchiveFormat stringToArchiveFormat(std::string_view archiveFormat) noexcept
    {
        if(archiveFormat == archiveFormatToString(ArchiveFormat::FILESYSTEM))
        {
            return ArchiveFormat::FILESYSTEM;
        }

        if(archiveFormat == archiveFormatToString(ArchiveFormat::COMPATIBLE))
        {
            return ArchiveFormat::COMPATIBLE;
        }

        if(archiveFormat == archiveFormatToString(ArchiveFormat::OPTIMIZED))
        {
            return ArchiveFormat::OPTIMIZED;
        }

        if(archiveFormat.empty() || archiveFormat == "default")
        {
            return ArchiveFormat::DEFAULT;
        }

        // Error case
        return ArchiveFormat::INVALID;
    }

    /// Returns the path of the archive
    inline const std::filesystem::path& getArchivePath() const;

protected:

    /// Constructor
    IO_ZIP_API Archive(const std::filesystem::path& archive_path);

private:

    const std::filesystem::path m_archive_path;
};

//------------------------------------------------------------------------------

inline const std::filesystem::path& Archive::getArchivePath() const
{
    return m_archive_path;
}

} // namespace sight::io::zip
