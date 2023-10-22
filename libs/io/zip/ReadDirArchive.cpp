/************************************************************************
 *
 * Copyright (C) 2009-2023 IRCAD France
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

#include "io/zip/ReadDirArchive.hpp"

#include "io/zip/exception/Read.hpp"

#include <core/exceptionmacros.hpp>

#include <filesystem>
#include <utility>

namespace sight::io::zip
{

//-----------------------------------------------------------------------------

ReadDirArchive::ReadDirArchive(std::filesystem::path _archive) :
    m_archive(std::move(_archive))
{
}

//-----------------------------------------------------------------------------

ReadDirArchive::~ReadDirArchive()
= default;

//-----------------------------------------------------------------------------

SPTR(std::istream) ReadDirArchive::get_file(const std::filesystem::path& _path)
{
    SIGHT_THROW_EXCEPTION_IF(
        io::zip::exception::Read(
            "File '" + _path.string() + "' "
                                        "in archive '" + m_archive.string() + "' doesn't exist."
        ),
        !std::filesystem::exists(m_archive / _path)
    );

    SPTR(std::ifstream) is = std::make_shared<std::ifstream>();
    is->open((m_archive / _path).string().c_str(), std::fstream::binary | std::fstream::in);
    return is;
}

//-----------------------------------------------------------------------------

std::filesystem::path ReadDirArchive::getArchivePath() const
{
    return m_archive;
}

} // namespace sight::io::zip
