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

#include <fwCore/macros.hpp>

#include <filesystem>

#include <istream>

namespace fwZip
{

/**
 * @brief   This interface defines functions to read a file in an archive.
 */
class IReadArchive
{

public:

    fwCoreClassMacro(IReadArchive);

    virtual ~IReadArchive()
    {
    }

    /**
     * @brief Returns input stream for the file in current archive.
     * @param path file in archive.
     * @return input stream from request file.
     */
    virtual SPTR(std::istream) getFile(const std::filesystem::path& path) = 0;

    /**
     * @brief Returns archive path.
     */
    virtual const std::filesystem::path getArchivePath() const = 0;

    virtual IReadArchive::sptr clone() const = 0;
};

}
