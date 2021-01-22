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

#ifdef _WIN32

#include "fwRuntime/detail/dl/Win32.hpp"

#include <core/base.hpp>

namespace fwRuntime
{

namespace detail
{

namespace dl
{

//------------------------------------------------------------------------------

Win32::Win32( const std::filesystem::path& modulePath ) noexcept :
    Native(modulePath.string()),
    m_handle( 0 )
{
}

//------------------------------------------------------------------------------

bool Win32::isLoaded() const noexcept
{
    return m_handle != 0;
}

//------------------------------------------------------------------------------

void* Win32::getSymbol( const std::string& name ) const
{
    FARPROC symbol;

    symbol = GetProcAddress( m_handle, name.c_str() );
    if(symbol == 0)
    {
        throw RuntimeException("'" + name + "': symbol retrieval failed.");
    }
    return symbol;
}

//------------------------------------------------------------------------------

void Win32::load()
{
    if(m_handle == 0)
    {
        // Opens the dynamic library.
        std::string lib(getFullPath().string());
        m_handle = LoadLibrary( lib.c_str() );
        if(m_handle == 0)
        {
            // Retrieves the last error message.
            DWORD lastError = GetLastError();
            char buffer[1024];
            FormatMessage( FORMAT_MESSAGE_FROM_SYSTEM, 0, lastError, 0, buffer, 1024, 0 );

            // Builds the error message and throws the exception.
            std::string message( buffer );
            throw RuntimeException( message );
        }
    }
}

//------------------------------------------------------------------------------

void Win32::unload()
{
    if(m_handle != 0)
    {
        BOOL result;
        result = FreeLibrary(m_handle);
        if(!result)
        {
            throw RuntimeException("Module unload failed.");
        }
        m_handle = 0;
    }
}

//------------------------------------------------------------------------------

} // namespace dl

} // namespace detail

} // namespace fwRuntime

#endif // #ifdef _WIN32
