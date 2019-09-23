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

#include <ostream>

namespace fwZip
{

/**
 * @brief   This class defines functions to write a file in a zip archive.
 */
class FWZIP_CLASS_API WriteZipArchive : public IWriteArchive
{

public:

    fwCoreClassMacro(WriteZipArchive, IWriteArchive);

    //------------------------------------------------------------------------------

    static sptr New(const ::boost::filesystem::path& archive, const std::string& comment = "")
    {
        return std::make_shared<WriteZipArchive>(archive, comment);
    }

    /**
     * @brief Constructors. Initializes archive path.
     *
     */
    FWZIP_API WriteZipArchive( const ::boost::filesystem::path& archive );

    /**
     * @brief Constructors. Initializes archive path and comment.
     *
     */
    FWZIP_API WriteZipArchive( const ::boost::filesystem::path& archive, const std::string& comment );

    FWZIP_API ~WriteZipArchive();

    /**
     * @brief Creates a new file entry in archive and returns output stream for this file.
     * @param path file in archive.
     * @return output stream of file entry in archive.
     *
     * @throw ::fwZip::exception::Write if archive cannot be opened.
     * @note Last output stream is automatically flushed before creation of new file entry in zip archive.
     */
    FWZIP_API SPTR(std::ostream) createFile(const ::boost::filesystem::path& path) override;

    /**
     * @brief Writes source file in archive.
     * @param sourceFile source file.
     * @param path file in archive.
     *
     * @throw ::fwZip::exception::Read if source file cannot be opened.
     */
    FWZIP_API void putFile(const ::boost::filesystem::path& sourceFile,
                           const ::boost::filesystem::path& path) override;

    /**
     * @brief Creates a folder in archive.
     * @param path folder to create in archive.
     *
     * @todo: file attribute isn't correctly set in zip archive.
     */
    FWZIP_API bool createDir(const ::boost::filesystem::path& path) override;

    /**
     * @brief Returns archive path.
     */
    FWZIP_API const ::boost::filesystem::path getArchivePath() const override;

protected:

    ::boost::filesystem::path m_archive;

    std::string m_comment;
};

}
