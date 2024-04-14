/************************************************************************
 *
 * Copyright (C) 2023 IRCAD France
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

#include "temp_path.hpp"

#include <core/exceptionmacros.hpp>

#include <cstdio>
#include <filesystem>
#include <fstream>
#include <mutex>
#include <random>

namespace sight::core::os
{

// To protect the creation of the temporary directory
static std::recursive_mutex s_mutex;

//------------------------------------------------------------------------------

inline static std::string random_name(std::size_t _length)
{
    static constexpr std::string_view s_CHARS {
        "0123456789"
        "abcdefghijklmnopqrstuvwxyz"
        "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
    };

    static std::mt19937 rg {std::random_device {}()};
    static std::uniform_int_distribution<std::string_view::size_type> pick(0, s_CHARS.size() - 1);

    std::string s;
    s.reserve(_length);

    for(std::size_t i = 0 ; i < _length ; ++i)
    {
        s += s_CHARS[pick(rg)];
    }

    return s;
}

//------------------------------------------------------------------------------

inline static std::filesystem::path generate_temp_dir(
    const std::filesystem::path& _root,
    const std::string& _prefix = std::string()
)
{
    // Protect the static variables
    std::unique_lock guard(s_mutex);

    // Create root, if needed
    std::filesystem::create_directories(_root);

    // Start with 4 random characters, ut we don't more than 64 characters
    for(std::string name = _prefix + random_name(4) ; name.size() < 64 ; name += random_name(2))
    {
        try
        {
            const auto& random_path = _root / name;

            if(!std::filesystem::exists(random_path) && std::filesystem::create_directory(random_path))
            {
                return random_path;
            }
        }
        catch(...)
        {
            // retry...
        }
    }

    SIGHT_THROW("Failed to create temporary directory in '" + _root.string() + "'");
}

//------------------------------------------------------------------------------

inline static std::pair<std::filesystem::path, std::shared_ptr<std::ofstream> > open_temp_file(
    const std::filesystem::path& _path,
    const std::optional<std::ios_base::openmode>& _openmode = std::nullopt
)
{
    std::shared_ptr<std::ofstream> ofstream = std::make_shared<std::ofstream>();
    ofstream->open(_path.string(), _openmode.value_or(std::ios_base::out | std::ios_base::trunc));

    if(ofstream->is_open() && ofstream->good())
    {
        return std::make_pair(_path, _openmode ? ofstream : nullptr);
    }

    SIGHT_THROW("Failed to create a temporary file in '" + _path.string() + "'");
}

//------------------------------------------------------------------------------

inline static std::pair<std::filesystem::path, std::shared_ptr<std::ofstream> > generate_temp_file(
    const std::optional<std::ios_base::openmode>& _openmode = std::nullopt,
    const std::optional<std::filesystem::path>& _path       = std::nullopt
)
{
    if(_path)
    {
        return open_temp_file(*_path, _openmode);
    }

    // Protect the static variables
    std::unique_lock guard(s_mutex);

    const std::filesystem::path& root = temp_path::shared_directory();

    // Start with 4 random characters, ut we don't more than 64 characters
    for(auto name = std::string("f") + random_name(4) ; name.size() < 64 ; name += random_name(2))
    {
        const auto& random_path = root / name;
        if(!std::filesystem::exists(random_path))
        {
            try
            {
                return open_temp_file(random_path, _openmode);
            }
            catch(...)
            {
                // retry...
            }
        }
    }

    SIGHT_THROW("Failed to create a temporary file in '" + root.string() + "'");
}

temp_path::temp_path(const std::pair<std::filesystem::path, std::shared_ptr<std::ofstream> >& _path_and_stream) :
    m_ofstream(_path_and_stream.second),
    m_path(_path_and_stream.first)
{
}

temp_path::~temp_path() noexcept
{
    if(m_ofstream && m_ofstream->is_open())
    {
        m_ofstream->close();
        m_ofstream.reset();
    }

    // use std::error_code instead of exception
    std::error_code error;
    std::filesystem::remove_all(m_path, error);

    SIGHT_WARN_IF("Failed to remove temporary directory '" + m_path.string() + "'", error);
}

//------------------------------------------------------------------------------

std::filesystem::path temp_path::shared_directory(const std::string& _subdirectory_prefix)
{
    // Protect the static variable
    std::unique_lock guard(s_mutex);

    // Create the temporary root directory as a static variable
    static const temp_path s_ROOT(
        std::make_pair(generate_temp_dir(std::filesystem::temp_directory_path()), nullptr)
    );

    if(_subdirectory_prefix.empty())
    {
        // Return the path without creating a subdirectory
        return s_ROOT;
    }

    // Return the path with a subdirectory
    return generate_temp_dir(s_ROOT, _subdirectory_prefix);
}

temp_dir::temp_dir(const std::optional<std::filesystem::path>& _path) :
    temp_path(std::make_pair(_path ? *_path : generate_temp_dir(temp_path::shared_directory()), nullptr))
{
}

temp_file::temp_file(
    const std::optional<std::ios_base::openmode>& _openmode,
    const std::optional<std::filesystem::path>& _path
) :
    temp_path(generate_temp_file(_openmode, _path))
{
}

//------------------------------------------------------------------------------

std::filesystem::path temp_file::unique_path()
{
    return generate_temp_file(std::nullopt).first;
}

//------------------------------------------------------------------------------

std::pair<std::filesystem::path, std::shared_ptr<std::ofstream> > temp_file::unique_stream(
    const std::optional<std::ios_base::openmode>& _openmode
)
{
    return generate_temp_file(_openmode);
}

} // namespace sight::core::os
