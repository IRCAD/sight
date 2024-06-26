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

#include "io/zip/archive.hpp"

#include <core/crypto/secure_string.hpp>

#include <istream>

namespace sight::io::zip
{

/**
 * @brief Class that allow writing an archive.
 *
 */
class SIGHT_IO_ZIP_CLASS_API archive_reader : public archive
{
public:

    SIGHT_DECLARE_CLASS(archive_reader, archive);

    /// Delete copy constructors and assignment operators, as we don't want to allow resources duplication
    archive_reader()                                 = delete;
    archive_reader(const archive_reader&)            = delete;
    archive_reader(archive_reader&&)                 = delete;
    archive_reader& operator=(const archive_reader&) = delete;
    archive_reader& operator=(archive_reader&&)      = delete;

    /// Destructor
    SIGHT_IO_ZIP_API ~archive_reader() override = default;

    /// Shared factory. It uses a cache mechanism to return the same instance for the same _archive_path.
    /// @param _archive_path path of the archive file. The file will be kept opened as long as the instance leave.
    /// @param _format the format of the archive. @see sight::io::zip::archive::archiveFormat
    SIGHT_IO_ZIP_API static archive_reader::uptr get(
        const std::filesystem::path& _archive_path,
        archive_format _format = archive_format::DEFAULT
    );

    /// Returns an std::istream to read an archived file
    /// @param _file_path path of an archived file.
    /// @param _password the password needed to decrypt the file.
    SIGHT_IO_ZIP_API virtual std::unique_ptr<std::istream> open_file(
        const std::filesystem::path& _file_path,
        const core::crypto::secure_string& _password = ""
    )                                                = 0;

    /// Extracts all the content of the archive in the specified folder
    /// @param _output_path the output folder
    /// @param _password the password needed to decrypt the file.
    SIGHT_IO_ZIP_API virtual void extract_all_to(
        const std::filesystem::path& _output_path,
        const core::crypto::secure_string& _password = ""
    )                                                = 0;

    /// Returns true for raw archive
    [[nodiscard]] SIGHT_IO_ZIP_API virtual bool is_raw() const = 0;

protected:

    /// Constructor
    SIGHT_IO_ZIP_API archive_reader(const std::filesystem::path& _archive_path);
};

} // namespace sight::io::zip
