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
#include "io/zip/IReadArchive.hpp"

#include <core/macros.hpp>

#include <filesystem>
#include <fstream>

namespace sight::io::zip
{

/**
 * @brief   This class defines functions to read a file in a file system archive.
 */
class IO_ZIP_CLASS_API ReadDirArchive : public IReadArchive
{

public:
    fwCoreClassMacro(ReadDirArchive, IReadArchive)

    //------------------------------------------------------------------------------

    static sptr New(const std::filesystem::path& archive)
    {
        return std::make_shared< ReadDirArchive >(archive);
    }

    /// Constructors. Initializes archive path.
    IO_ZIP_API ReadDirArchive( const std::filesystem::path& archive );

    /// Destructor. Close automatically last input file stream.
    IO_ZIP_API ~ReadDirArchive();

    /**
     * @brief Returns input stream for the file in current archive.
     * @param path file in archive.
     * @return input stream from request file.
     *
     * @throw io::zip::exception::Read if file doesn't exist in archive.
     */
    IO_ZIP_API SPTR(std::istream) getFile(const std::filesystem::path& path) override;

    /**
     * @brief Returns archive path.
     */
    IO_ZIP_API const std::filesystem::path getArchivePath() const override;

    //------------------------------------------------------------------------------

    IReadArchive::sptr clone() const override
    {
        return ReadDirArchive::New(m_archive);
    }

protected:

    std::filesystem::path m_archive;
};

}
