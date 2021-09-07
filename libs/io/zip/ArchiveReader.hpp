/************************************************************************
 *
 * Copyright (C) 2009-2021 IRCAD France
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

#include <istream>
#include <mutex>

namespace sight::io::zip
{

/**
 * @brief Class that allow writing an archive.
 *
 */
class IO_ZIP_CLASS_API ArchiveReader : public Archive
{
public:

    SIGHT_DECLARE_CLASS(ArchiveReader, Archive);

    /// Delete copy constructors and assignment operators, as we don't want to allow resources duplication
    ArchiveReader(const ArchiveReader&)            = delete;
    ArchiveReader(ArchiveReader&&)                 = delete;
    ArchiveReader& operator=(const ArchiveReader&) = delete;
    ArchiveReader& operator=(ArchiveReader&&)      = delete;

    /// Shared factory. It uses a cache mechanism to return the same instance for the same archivePath.
    /// @param archivePath path of the archive file. The file will be kept opened as long as the instance leave.
    IO_ZIP_API static ArchiveReader::sptr shared(const std::filesystem::path& archivePath);

    /// Returns an std::istream to read an archived file
    /// @param filePath path of an archived file.
    /// @param password the password needed to decrypt the file.
    IO_ZIP_API virtual std::unique_ptr<std::istream> openFile(
        const std::filesystem::path& filePath,
        const core::crypto::secure_string& password = ""
    )                                               = 0;

protected:

    /// Constructor
    IO_ZIP_API ArchiveReader() = default;

    /// Destructor
    IO_ZIP_API ~ArchiveReader() override = default;
};

} // namespace sight::io::zip
