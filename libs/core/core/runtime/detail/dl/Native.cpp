/************************************************************************
 *
 * Copyright (C) 2009-2022 IRCAD France
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

#include "core/runtime/detail/dl/Native.hpp"

#include "core/runtime/detail/Module.hpp"
#include "core/runtime/Runtime.hpp"

#include <climits>
#include <filesystem>
#include <string>
#include <utility>

namespace sight::core::runtime::detail::dl
{

//------------------------------------------------------------------------------

Native::Native(std::string name) noexcept :
    m_name(std::move(name))
{
}

//------------------------------------------------------------------------------

Native::~Native() noexcept =
    default;

//------------------------------------------------------------------------------

std::filesystem::path Native::getFullPath() const
{
#if defined(linux) || defined(__linux)
    // Cache the list of all libraries for each location to speed-up searches
    // { <searchPath>: { <library name>: <library_location>}, ... }, ...}
    //
    // With a library folder of 5,000 elements, this takes around 10ms the first time and then few microseconds
    static std::map<std::filesystem::path, std::unordered_map<std::string, std::filesystem::path> > s_cache;

    if(auto it = s_cache.find(m_searchPath); it == s_cache.end())
    {
        {
            static const std::regex libraryRegex("lib(.*).so(\\.?[0-9\\.]*)?");

            auto& map = s_cache[m_searchPath];
            for(const auto& p : std::filesystem::directory_iterator(m_searchPath))
            {
                {
                    const std::filesystem::path filename = p.path().filename();
                    const std::string filename_str       = filename.string();

                    if(std::smatch match; std::regex_match(filename_str, match, libraryRegex))
                    {
                        const auto library_name = match[1].str();
                        const auto itFind       = map.find(library_name);

                        if(itFind == map.end())
                        {
                            map[library_name] = filename;
                        }
                        else
                        {
                            const auto extension = itFind->second.extension();

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

    std::filesystem::path result = m_searchPath / s_cache[m_searchPath][m_name];
#elif defined(WIN32)
    std::filesystem::path result = m_searchPath / (this->getName() + ".dll");
#endif

    // Test that the result path exists.
    if(result.empty())
    {
        throw RuntimeException("Unable to find a native library for the module.");
    }

    if(!std::filesystem::exists(result))
    {
        throw RuntimeException("'" + result.string() + "': invalid native module file name.");
    }

    if(std::filesystem::is_directory(result))
    {
        throw RuntimeException("'" + result.string() + "': is a directory. Dynamic library is missing.");
    }

    return result;
}

//------------------------------------------------------------------------------

std::string Native::getName() const
{
    return m_name;
}

//------------------------------------------------------------------------------

void Native::setSearchPath(const std::filesystem::path& path) noexcept
{
    m_searchPath = path;
}

//------------------------------------------------------------------------------

} // namespace sight::core::runtime::detail::dl
