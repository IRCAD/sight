/************************************************************************
 *
 * Copyright (C) 2009-2021 IRCAD France
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

#include <limits.h>

#include <filesystem>
#include <string>

namespace sight::core::runtime
{

namespace detail
{

namespace dl
{

//------------------------------------------------------------------------------

Native::Native( const std::string& name ) noexcept :
    m_name( name )
{
}

//------------------------------------------------------------------------------

Native::~Native() noexcept
{
}

//------------------------------------------------------------------------------

const std::filesystem::path Native::getFullPath() const
{
    const std::string repoName = m_searchPath.filename().string() + "_";
#if defined(linux) || defined(__linux)
    const std::filesystem::path result = m_searchPath / ("lib" + repoName + this->getName() + ".so");
#elif defined(WIN32)
    const std::filesystem::path result = m_searchPath / (repoName + this->getName() + ".dll");
#elif defined (__APPLE__)
    const std::filesystem::path result = m_searchPath / ("lib" + repoName + this->getName() + ".dylib");
#endif

    // Test that the result path exists.
    if(result.empty())
    {
        throw RuntimeException("Unable to find a native library for the module.");
    }
    if( !std::filesystem::exists(result) )
    {
        throw RuntimeException("'" + result.string() + "': invalid native module file name.");
    }
    if( std::filesystem::is_directory(result) )
    {
        throw RuntimeException("'" + result.string() + "': is a directory. Dynamic library is missing.");
    }
    return result;
}

//------------------------------------------------------------------------------

const std::string Native::getName() const
{
    return m_name;
}

//------------------------------------------------------------------------------

void Native::setSearchPath( const std::filesystem::path& path ) noexcept
{
    m_searchPath = path;
}

//------------------------------------------------------------------------------

void Native::operator=(const Native&) noexcept
{
}

//------------------------------------------------------------------------------

} // namespace dl

} // namespace detail

} // namespace sight::core::runtime
