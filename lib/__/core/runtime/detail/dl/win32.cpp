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

#ifdef _WIN32

#include "core/runtime/detail/dl/win32.hpp"

#include <core/base.hpp>

namespace sight::core::runtime
{

namespace detail
{

namespace dl
{

//------------------------------------------------------------------------------

win32::win32(const std::filesystem::path& module_path) noexcept :
    native(module_path.string()),
    m_handle(0)
{
}

//------------------------------------------------------------------------------

bool win32::is_loaded() const noexcept
{
    return m_handle != 0;
}

//------------------------------------------------------------------------------

void* win32::get_symbol(const std::string& name) const
{
    FARPROC symbol;

    symbol = GetProcAddress(m_handle, name.c_str());
    if(symbol == 0)
    {
        throw runtime_exception("'" + name + "': symbol retrieval failed.");
    }

    return symbol;
}

//------------------------------------------------------------------------------

void win32::load()
{
    if(m_handle == 0)
    {
        // Opens the dynamic library.
        std::string lib(get_full_path().string());
        m_handle = LoadLibrary(lib.c_str());
        if(m_handle == 0)
        {
            // Retrieves the last error message.
            DWORD last_error = GetLastError();
            char buffer[1024];
            FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM, 0, last_error, 0, buffer, 1024, 0);

            // Builds the error message and throws the exception.
            std::string message(buffer);
            throw runtime_exception(message);
        }
    }
}

//------------------------------------------------------------------------------

void win32::unload()
{
    if(m_handle != 0)
    {
        BOOL result;
        result = FreeLibrary(m_handle);
        if(!result)
        {
            throw runtime_exception("Module unload failed.");
        }

        m_handle = 0;
    }
}

//------------------------------------------------------------------------------

} // namespace dl

} // namespace detail

} // namespace sight::core::runtime

#endif // #ifdef _WIN32
