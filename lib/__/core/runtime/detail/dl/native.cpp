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

#include "core/runtime/detail/dl/native.hpp"

#include "core/runtime/detail/module.hpp"

#include <climits>
#include <filesystem>
#include <string>
#include <unordered_map>
#include <utility>

namespace sight::core::runtime::detail::dl
{

//------------------------------------------------------------------------------

native::native(std::string _name) noexcept :
    m_name(std::move(_name))
{
}

//------------------------------------------------------------------------------

native::~native() noexcept =
    default;

//------------------------------------------------------------------------------

std::filesystem::path native::get_full_path() const
{
#if defined(linux) || defined(__linux)
    // Cache the list of all libraries for each location to speed-up searches
    // { <searchPath>: { <library name>: <library_location>}, ... }, ...}
    //
    // With a library folder of 5,000 elements, this takes around 10ms the first time and then few microseconds
    static std::map<std::filesystem::path, std::unordered_map<std::string, std::filesystem::path> > s_cache;

    if(auto it = s_cache.find(m_search_path); it == s_cache.end())
    {
        {
            static const std::regex s_LIBRARY_REGEX("lib(.*).so(\\.?[0-9\\.]*)?");

            auto& map = s_cache[m_search_path];
            for(const auto& p : std::filesystem::directory_iterator(m_search_path))
            {
                {
                    const std::filesystem::path filename = p.path().filename();
                    const std::string filename_str       = filename.string();

                    if(std::smatch match; std::regex_match(filename_str, match, s_LIBRARY_REGEX))
                    {
                        const auto library_name = match[1].str();
                        const auto it_find      = map.find(library_name);

                        if(it_find == map.end())
                        {
                            map[library_name] = filename;
                        }
                        else
                        {
                            const auto extension = it_find->second.extension();

                            if(extension != ".so")
                            {
                                map[library_name] = filename;
                            }
                        }
                    }
                }
            }
        }
    }

    std::filesystem::path result = m_search_path / s_cache[m_search_path][m_name];
#elif defined(WIN32)
    std::filesystem::path result = m_search_path / (this->name() + ".dll");
#endif

    // Test that the result path exists.
    if(result.empty())
    {
        throw runtime_exception("Unable to find a native library for the module.");
    }

    if(!std::filesystem::exists(result))
    {
        throw runtime_exception("'" + result.string() + "': invalid native module file name.");
    }

    if(std::filesystem::is_directory(result))
    {
        throw runtime_exception("'" + result.string() + "': is a directory. Dynamic library is missing.");
    }

    return result;
}

//------------------------------------------------------------------------------

std::string native::name() const
{
    return m_name;
}

//------------------------------------------------------------------------------

void native::set_search_path(const std::filesystem::path& _path) noexcept
{
    m_search_path = std::filesystem::canonical(_path);
}

//------------------------------------------------------------------------------

} // namespace sight::core::runtime::detail::dl
