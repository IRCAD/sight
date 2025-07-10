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

#include "io/zip/write_dir_archive.hpp"

#include "io/zip/exception/write.hpp"

#include <core/exceptionmacros.hpp>

#include <boost/iostreams/stream_buffer.hpp>

#include <filesystem>
#include <fstream>
#include <utility>

namespace sight::io::zip
{

//-----------------------------------------------------------------------------

write_dir_archive::write_dir_archive(std::filesystem::path _archive) :
    m_archive(std::move(_archive))
{
    if(!std::filesystem::exists(m_archive))
    {
        std::filesystem::create_directories(m_archive);
    }
}

//-----------------------------------------------------------------------------

write_dir_archive::~write_dir_archive()
= default;

//-----------------------------------------------------------------------------

SPTR(std::ostream) write_dir_archive::create_file(const std::filesystem::path& _path)
{
    const std::filesystem::path file        = m_archive / _path;
    const std::filesystem::path parent_file = file.parent_path();
    if(!std::filesystem::exists(parent_file))
    {
        std::filesystem::create_directories(parent_file);
    }

    SPTR(std::ofstream) os = std::make_shared<std::ofstream>();
    os->open(file.string().c_str(), std::fstream::binary | std::fstream::out | std::fstream::trunc);
    return os;
}

//-----------------------------------------------------------------------------

void write_dir_archive::put_file(
    const std::filesystem::path& _source_file,
    const std::filesystem::path& _destination_file
)
{
    const std::filesystem::path file_dest = m_archive / _destination_file;
    if(!std::filesystem::exists(file_dest))
    {
        const std::filesystem::path parent_file = file_dest.parent_path();
        if(!std::filesystem::exists(parent_file))
        {
            std::filesystem::create_directories(parent_file);
        }

        std::error_code err;
        std::filesystem::create_hard_link(_source_file, file_dest, err);
        if(err.value() != 0)
        {
            // Use std stream instead of boost:::filesystem::copy_file
            // because fwZip is build using std=c++11 and using copy_file also requires boost built
            // with std=c++11 (for now in c++0x).
            std::string str_source = _source_file.string();
            std::string str_dest   = file_dest.string();
            std::ifstream src(str_source.c_str(), std::ios::binary);
            std::ofstream dst(str_dest.c_str(), std::ios::binary);

            dst << src.rdbuf();
        }
    }
}

//-----------------------------------------------------------------------------

bool write_dir_archive::create_dir(const std::filesystem::path& _path)
{
    return std::filesystem::create_directories(m_archive / _path);
}

//-----------------------------------------------------------------------------

std::filesystem::path write_dir_archive::get_archive_path() const
{
    return m_archive;
}

} // namespace sight::io::zip
