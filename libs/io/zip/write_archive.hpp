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

#include <core/macros.hpp>

#include <filesystem>
#include <ostream>

namespace sight::io::zip
{

/**
 * @brief   This interface defines functions to write a file in an archive.
 */
class write_archive
{
public:

    SIGHT_DECLARE_CLASS(write_archive);

    virtual ~write_archive()
    = default;

    /**
     * @brief Creates a new file entry in archive and returns output stream for this file.
     * @param _path file in archive.
     * @return output stream of file entry in archive.
     */
    virtual SPTR(std::ostream) create_file(const std::filesystem::path& _path) = 0;

    /**
     * @brief Writes source file in archive.
     * @param _source_file source file.
     * @param _path file in archive.
     */
    virtual void put_file(const std::filesystem::path& _source_file, const std::filesystem::path& _path) = 0;

    /**
     * @brief Creates a folder in archive.
     * @param _path folder to create in archive.
     */
    virtual bool create_dir(const std::filesystem::path& _path) = 0;

    /**
     * @brief Returns archive path.
     */
    [[nodiscard]] virtual std::filesystem::path get_archive_path() const = 0;
};

} // namespace sight::io::zip
