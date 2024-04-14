/************************************************************************
 *
 * Copyright (C) 2009-2024 IRCAD France
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

#include <sight/io/zip/config.hpp>

#include <core/macros.hpp>

#include <filesystem>

namespace sight::io::zip
{

/// Class that allow reading and writing an archive.
class SIGHT_IO_ZIP_CLASS_API archive
{
public:

    SIGHT_DECLARE_CLASS(archive);

    /// Delete default constructors and assignment operators, as we don't want to allow resources duplication
    archive()                          = delete;
    archive(const archive&)            = delete;
    archive(archive&&)                 = delete;
    archive& operator=(const archive&) = delete;
    archive& operator=(archive&&)      = delete;

    /// Destructor
    SIGHT_IO_ZIP_API virtual ~archive();

    /// Enum to define
    enum class archive_format : uint8_t
    {
        filesystem = 0,         /// Use the filesystem to store files.
        compatible = 2,         /// Store files in a ZIP archive, with old deflate algorithm
        optimized  = 3,         /// Store files in a ZIP archive, with zstd algorithm
        DEFAULT    = optimized, /// Default behavior if nothing is set
        invalid    = 255        /// Used for error management
    };

    /// Convenience function to convert from archiveFormat enum value to string
    constexpr static std::string_view archive_format_to_string(archive_format _archive_format) noexcept
    {
        switch(_archive_format)
        {
            case archive_format::filesystem:
                return "filesystem";

            case archive_format::compatible:
                return "compatible";

            case archive_format::optimized:
                return "optimized";

            default:
                return "default";
        }
    }

    /// Convenience function to convert from string to archiveFormat enum value
    constexpr static archive_format string_to_archive_format(std::string_view _archive_format) noexcept
    {
        if(_archive_format == archive_format_to_string(archive_format::filesystem))
        {
            return archive_format::filesystem;
        }

        if(_archive_format == archive_format_to_string(archive_format::compatible))
        {
            return archive_format::compatible;
        }

        if(_archive_format == archive_format_to_string(archive_format::optimized))
        {
            return archive_format::optimized;
        }

        if(_archive_format.empty() || _archive_format == "default")
        {
            return archive_format::DEFAULT;
        }

        // Error case
        return archive_format::invalid;
    }

    /// Returns the path of the archive
    inline const std::filesystem::path& get_archive_path() const;

protected:

    /// Constructor
    SIGHT_IO_ZIP_API archive(const std::filesystem::path& _archive_path);

private:

    const std::filesystem::path m_archive_path;
};

//------------------------------------------------------------------------------

inline const std::filesystem::path& archive::get_archive_path() const
{
    return m_archive_path;
}

} // namespace sight::io::zip
