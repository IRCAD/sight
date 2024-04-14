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

#include <sight/io/zip/config.hpp>

#include "io/zip/write_archive.hpp"

#include <core/macros.hpp>

#include <filesystem>
#include <fstream>

namespace sight::io::zip
{

/**
 * @brief   This class defines functions to write a file in a file system archive.
 */
class SIGHT_IO_ZIP_CLASS_API write_dir_archive : public write_archive
{
public:

    SIGHT_DECLARE_CLASS(write_dir_archive, write_archive);

    //------------------------------------------------------------------------------

    static sptr make(const std::filesystem::path& _archive)
    {
        return std::make_shared<write_dir_archive>(_archive);
    }

    /// Constructors. Initializes archive path and creates archive directories if doesn't exist.
    SIGHT_IO_ZIP_API write_dir_archive(std::filesystem::path _archive);

    /// Destructor. Flush and close last output file stream.
    SIGHT_IO_ZIP_API ~write_dir_archive() override;

    /**
     * @brief Creates a new file entry in archive and returns output stream for this file.
     * @param _path file in archive.
     * @return output stream of file entry in archive.
     *
     * @note Creates all parent directories of the path in archive.
     */
    SIGHT_IO_ZIP_API SPTR(std::ostream) create_file(const std::filesystem::path& _path) override;

    /**
     * @brief Writes source file in archive. If possible, creates hard link otherwise copy source file in archive.
     * @param _source_file source file.
     * @param _destination_file file in archive.
     *
     * @note Do nothing if destinationFile already exists in archive.
     */
    SIGHT_IO_ZIP_API void put_file(
        const std::filesystem::path& _source_file,
        const std::filesystem::path& _destination_file
    ) override;

    /**
     * @brief Creates a folder in archive.
     * @param _path folder to create in archive.
     */
    SIGHT_IO_ZIP_API bool create_dir(const std::filesystem::path& _path) override;

    /**
     * @brief Returns archive path.
     */
    [[nodiscard]] SIGHT_IO_ZIP_API std::filesystem::path get_archive_path() const override;

protected:

    std::filesystem::path m_archive;
};

} // namespace sight::io::zip
