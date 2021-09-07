/************************************************************************
 *
 * Copyright (C) 2009-2021 IRCAD France
 * Copyright (C) 2012-2020 IHU Strasbourg
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
#include "io/zip/IWriteArchive.hpp"

#include <core/crypto/secure_string.hpp>
#include <core/macros.hpp>

#include <filesystem>
#include <ostream>

namespace sight::io::zip
{

/**
 * @brief   This class defines functions to write a file in a zip archive.
 */
class IO_ZIP_CLASS_API WriteZipArchive : public IWriteArchive
{
public:

    SIGHT_DECLARE_CLASS(WriteZipArchive, IWriteArchive);

    //------------------------------------------------------------------------------

    static sptr New(
        const std::filesystem::path& archive,
        const std::string& comment             = "",
        const core::crypto::secure_string& key = ""
)
    {
        return std::make_shared<WriteZipArchive>(archive, comment, key);
    }

    /**
     * @brief Constructors. Initializes archive path, comment and key.
     *
     */
    WriteZipArchive(
        const std::filesystem::path& archive,
        const std::string& comment             = "",
        const core::crypto::secure_string& key = ""
    ) :
        m_archive(archive),
        m_comment(comment),
        m_key(key)
    {
    }

    IO_ZIP_API ~WriteZipArchive() = default;

    /**
     * @brief Creates a new file entry in archive and returns output stream for this file.
     * @param path file in archive.
     * @return output stream of file entry in archive.
     *
     * @throw io::zip::exception::Write if archive cannot be opened.
     * @note Last output stream is automatically flushed before creation of new file entry in zip archive.
     */
    IO_ZIP_API SPTR(std::ostream) createFile(const std::filesystem::path& path) override;

    /**
     * @brief Writes source file in archive.
     * @param sourceFile source file.
     * @param path file in archive.
     *
     * @throw io::zip::exception::Read if source file cannot be opened.
     */
    IO_ZIP_API void putFile(
        const std::filesystem::path& sourceFile,
        const std::filesystem::path& path
    ) override;

    /**
     * @brief Creates a folder in archive.
     * @param path folder to create in archive.
     *
     * @todo: file attribute isn't correctly set in zip archive.
     */
    IO_ZIP_API bool createDir(const std::filesystem::path& path) override;

    /**
     * @brief Returns archive path.
     */
    IO_ZIP_API const std::filesystem::path getArchivePath() const override;

private:

    /// Path to the archive file
    std::filesystem::path m_archive;

    /// Global comment
    std::string m_comment;

    /// Key used to encrypt files
    std::string m_key;
};

} // namespace sight::io
