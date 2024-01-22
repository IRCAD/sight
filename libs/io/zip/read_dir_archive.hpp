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

#include "io/zip/config.hpp"
#include "io/zip/read_archive.hpp"

#include <core/macros.hpp>

#include <filesystem>
#include <fstream>

namespace sight::io::zip
{

/**
 * @brief   This class defines functions to read a file in a file system archive.
 */
class IO_ZIP_CLASS_API read_dir_archive : public read_archive
{
public:

    SIGHT_DECLARE_CLASS(read_dir_archive, read_archive);

    //------------------------------------------------------------------------------

    static sptr make(const std::filesystem::path& _archive)
    {
        return std::make_shared<read_dir_archive>(_archive);
    }

    /// Constructors. Initializes archive path.
    IO_ZIP_API read_dir_archive(std::filesystem::path _archive);

    /// Destructor. Close automatically last input file stream.
    IO_ZIP_API ~read_dir_archive() override;

    /**
     * @brief Returns input stream for the file in current archive.
     * @param _path file in archive.
     * @return input stream from request file.
     *
     * @throw io::zip::exception::Read if file doesn't exist in archive.
     */
    IO_ZIP_API SPTR(std::istream) get_file(const std::filesystem::path& _path) override;

    /**
     * @brief Returns archive path.
     */
    [[nodiscard]] IO_ZIP_API std::filesystem::path get_archive_path() const override;

    //------------------------------------------------------------------------------

    [[nodiscard]] read_archive::sptr clone() const override
    {
        return read_dir_archive::make(m_archive);
    }

protected:

    std::filesystem::path m_archive;
};

} // namespace sight::io::zip
