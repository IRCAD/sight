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
#include "io/zip/IReadArchive.hpp"

#include <core/macros.hpp>

#include <filesystem>
#include <istream>

namespace sight::io::zip
{

/**
 * @brief   This class defines functions to read a file in a zip archive.
 */
class IO_ZIP_CLASS_API ReadZipArchive : public IReadArchive
{

public:
    fwCoreClassMacro(ReadZipArchive, IReadArchive)

    //------------------------------------------------------------------------------

    static sptr New(const std::filesystem::path& archive, const std::string& key = "")
    {
        return std::make_shared< ReadZipArchive >(archive, key);
    }

    /*
     * @brief Constructors. Initializes archive path, zip descriptor and zip stream.
     *
     * @throw io::zip::exception::Read if archive doesn't exist.
     * @throw io::zip::exception::Read if archive cannot be opened.
     */
    IO_ZIP_API ReadZipArchive( const std::filesystem::path& archive, const std::string& key = "");

    /**
     * @brief Returns input stream for the file in current archive (zip).
     * @param path file in archive.
     * @return input stream from request file.
     *
     * @throw io::zip::exception::Read if file doesn't exist in archive.
     * @throw io::zip::exception::Read if cannot retrieve file in archive.
     */
    IO_ZIP_API SPTR(std::istream) getFile(const std::filesystem::path& path) override;

    /**
     * @brief Returns comment from the current archive (zip).
     * @param path file in archive.
     * @return comment
     */
    IO_ZIP_API std::string getComment();

    /**
     * @brief Returns archive path.
     */
    IO_ZIP_API const std::filesystem::path getArchivePath() const override;

    //------------------------------------------------------------------------------

    IReadArchive::sptr clone() const override
    {
        return ReadZipArchive::New(m_archive, m_key);
    }

private:

    /// Path of the archive file
    std::filesystem::path m_archive;

    /// Key used to decrypt encrypted files
    std::string m_key;
};

}
