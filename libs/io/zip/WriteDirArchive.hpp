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

#include "io/zip/config.hpp"
#include "io/zip/IWriteArchive.hpp"

#include <core/macros.hpp>

#include <filesystem>
#include <fstream>

namespace sight::io::zip
{

/**
 * @brief   This class defines functions to write a file in a file system archive.
 */
class IO_ZIP_CLASS_API WriteDirArchive : public IWriteArchive
{
public:

    SIGHT_DECLARE_CLASS(WriteDirArchive, IWriteArchive);

    //------------------------------------------------------------------------------

    static sptr New(const std::filesystem::path& archive)
    {
        return std::make_shared<WriteDirArchive>(archive);
    }

    /// Constructors. Initializes archive path and creates archive directories if doesn't exist.
    IO_ZIP_API WriteDirArchive(const std::filesystem::path& archive);

    /// Destructor. Flush and close last output file stream.
    IO_ZIP_API ~WriteDirArchive();

    /**
     * @brief Creates a new file entry in archive and returns output stream for this file.
     * @param path file in archive.
     * @return output stream of file entry in archive.
     *
     * @note Creates all parent directories of the path in archive.
     */
    IO_ZIP_API SPTR(std::ostream) createFile(const std::filesystem::path& path) override;

    /**
     * @brief Writes source file in archive. If possible, creates hard link otherwise copy source file in archive.
     * @param sourceFile source file.
     * @param destinationFile file in archive.
     *
     * @note Do nothing if destinationFile already exists in archive.
     */
    IO_ZIP_API void putFile(
        const std::filesystem::path& sourceFile,
        const std::filesystem::path& destinationFile
    ) override;

    /**
     * @brief Creates a folder in archive.
     * @param path folder to create in archive.
     */
    IO_ZIP_API bool createDir(const std::filesystem::path& path) override;

    /**
     * @brief Returns archive path.
     */
    IO_ZIP_API const std::filesystem::path getArchivePath() const override;

protected:

    std::filesystem::path m_archive;
};

}
