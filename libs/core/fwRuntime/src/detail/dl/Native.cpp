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

#include "fwRuntime/detail/dl/Native.hpp"

#include "fwRuntime/detail/Module.hpp"
#include "fwRuntime/Runtime.hpp"

#include <limits.h>

#include <filesystem>
#include <string>

namespace fwRuntime
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
    std::filesystem::path result;

    result = m_searchPath / this->getPath();

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

const std::regex Native::getNativeName() const
{
    SLM_ASSERT("module path not initialized", !m_name.empty());

    std::regex nativeName;

#if defined(linux) || defined(__linux)
    nativeName = std::regex(
        "libsight_(module_)?" + m_name + "\\.so\\.?[0-9\\.]*" );
#elif defined(WIN32)
    nativeName = std::regex(
        "sight_(module_)?" + m_name + "\\.dll");
#elif defined (__APPLE__)
    nativeName = std::regex(
        "libsight_(module_)?" + m_name + "[0-9\\.]*\\.dylib" );
#endif

    return nativeName;
}

//------------------------------------------------------------------------------

const std::filesystem::path Native::getPath() const
{
    std::filesystem::path result;

    const std::regex nativeFileRegex( this->getNativeName() );

    // Walk through the module directory, seeking for a matching file.
    std::filesystem::directory_iterator curDirEntry(m_searchPath);
    std::filesystem::directory_iterator endDirEntry;
    for(; curDirEntry != endDirEntry; ++curDirEntry)
    {
        std::filesystem::path curEntryPath( *curDirEntry );
        if( std::regex_match( curEntryPath.filename().string(), nativeFileRegex ) )
        {
            result = curEntryPath.filename();
            break;
        }
    }

    return result;
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

} // namespace fwRuntime
