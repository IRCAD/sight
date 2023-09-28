/************************************************************************
 *
 * Copyright (C) 2009-2023 IRCAD France
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
#include <istream>

namespace sight::io::zip
{

/**
 * @brief   This interface defines functions to read a file in an archive.
 */
class read_archive
{
public:

    SIGHT_DECLARE_CLASS(read_archive);

    virtual ~read_archive()
    = default;

    /**
     * @brief Returns input stream for the file in current archive.
     * @param path file in archive.
     * @return input stream from request file.
     */
    virtual SPTR(std::istream) get_file(const std::filesystem::path& path) = 0;

    /**
     * @brief Returns archive path.
     */
    [[nodiscard]] virtual std::filesystem::path getArchivePath() const = 0;

    [[nodiscard]] virtual read_archive::sptr clone() const = 0;
};

} // namespace sight::io::zip
