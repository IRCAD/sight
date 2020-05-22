/************************************************************************
 *
 * Copyright (C) 2009-2020 IRCAD France
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

Native::Native( const std::filesystem::path& modulePath ) noexcept :
    m_modulePath( modulePath ),
    m_module( 0 )
{
}

//------------------------------------------------------------------------------

Native::~Native() noexcept
{
}

//------------------------------------------------------------------------------

const std::filesystem::path Native::getModuleLocation() const
{
    // Pre-condition
    SLM_ASSERT("module not initialized", m_module);
    return m_module->getLibraryLocation();
}

//------------------------------------------------------------------------------

const std::filesystem::path Native::getFullPath() const
{
    std::filesystem::path result;

    result = this->getModuleLocation() / this->getPath();

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
    std::regex nativeName;

#if defined(linux) || defined(__linux)
    nativeName = std::regex(
        "lib" + m_modulePath.filename().string() + "\\.so." +
        "[0-9]\\.[0-9]" );
#elif defined(WIN32)
    nativeName = std::regex(
        m_modulePath.filename().string() + "\\.dll");
#elif defined (__APPLE__)
    nativeName = std::regex(
        "lib" + m_modulePath.filename().string() + "[0-9\\.]*\\.dylib" );
#endif

    return nativeName;
}

//------------------------------------------------------------------------------

const std::filesystem::path Native::getPath() const
{
    SLM_ASSERT("module path not initialized", !m_modulePath.empty());

    std::filesystem::path result;

    const std::regex nativeFileRegex( this->getNativeName() );

    // Walk through the module directory, seeking for a matching file.
    std::filesystem::directory_iterator curDirEntry(this->getModuleLocation());
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

void Native::setModule( const Module* module ) noexcept
{
    // Pre-condition
    SLM_ASSERT("module already initialized", m_module == nullptr );
    m_module = module;
    // Post-condition
    SLM_ASSERT("module not correctly attached", m_module == module );
}

//------------------------------------------------------------------------------

void Native::operator=(const Native&) noexcept
{
}

//------------------------------------------------------------------------------

} // namespace dl

} // namespace detail

} // namespace fwRuntime
