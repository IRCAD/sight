/************************************************************************
 *
 * Copyright (C) 2009-2019 IRCAD France
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

#include "fwZip/config.hpp"
#include "fwZip/IWriteArchive.hpp"

#include <fwCore/macros.hpp>

#include <boost/filesystem/path.hpp>

#include <fstream>

namespace fwZip
{

/**
 * @brief   This class defines functions to write a file in a file system archive.
 */
class FWZIP_CLASS_API WriteDirArchive : public IWriteArchive
{

public:

    fwCoreClassMacro(WriteDirArchive, IWriteArchive);

    //------------------------------------------------------------------------------

    static sptr New(const ::boost::filesystem::path& archive)
    {
        return std::make_shared<WriteDirArchive>(archive);
    }

    /// Constructors. Initializes archive path and creates archive directories if doesn't exist.
    FWZIP_API WriteDirArchive( const ::boost::filesystem::path& archive );

    /// Destructor. Flush and close last output file stream.
    FWZIP_API ~WriteDirArchive();

    /**
     * @brief Creates a new file entry in archive and returns output stream for this file.
     * @param path file in archive.
     * @return output stream of file entry in archive.
     *
     * @note Creates all parent directories of the path in archive.
     */
    FWZIP_API SPTR(std::ostream) createFile(const ::boost::filesystem::path& path) override;

    /**
     * @brief Writes source file in archive. If possible, creates hard link otherwise copy source file in archive.
     * @param sourceFile source file.
     * @param destinationFile file in archive.
     *
     * @note Do nothing if destinationFile already exists in archive.
     */
    FWZIP_API void putFile(const ::boost::filesystem::path& sourceFile,
                           const ::boost::filesystem::path& destinationFile) override;

    /**
     * @brief Creates a folder in archive.
     * @param path folder to create in archive.
     */
    FWZIP_API bool createDir(const ::boost::filesystem::path& path) override;

    /**
     * @brief Returns archive path.
     */
    FWZIP_API const ::boost::filesystem::path getArchivePath() const override;

protected:

    ::boost::filesystem::path m_archive;
};

}
