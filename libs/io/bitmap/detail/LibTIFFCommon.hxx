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

#pragma once

#include <tiffio.h>

#include <cstdarg>
#include <cstdio>

namespace sight::io::bitmap::detail::tiff
{

//------------------------------------------------------------------------------

inline static int mapProc(thandle_t, void**, toff_t*)
{
    return 0;
}

//------------------------------------------------------------------------------

inline static void unmapProc(thandle_t, void*, toff_t)
{
}

//------------------------------------------------------------------------------

inline static void errorHandler(const char* module, const char* fmt, va_list args)
{
    char error_buffer[0xFFFF];
    vsnprintf(error_buffer, sizeof(error_buffer), fmt, args);

    std::string msg("Tiff Error: ");
    msg += module;
    msg += ": ";
    msg += error_buffer;

    SIGHT_THROW(msg);
}

//------------------------------------------------------------------------------

inline static void warningHandler(const char* module, const char* fmt, va_list args)
{
    char warning_buffer[0xFFFF];
    vsnprintf(warning_buffer, sizeof(warning_buffer), fmt, args);

    std::string msg("Tiff Warning: ");
    msg += module;
    msg += ": ";
    msg += warning_buffer;

    SIGHT_WARN(msg);
}

static const struct HandlerRegistry final
{
    inline HandlerRegistry() noexcept
    {
        TIFFSetErrorHandler(&errorHandler);
        TIFFSetWarningHandler(&warningHandler);
    }
} registry;

} // namespace sight::io::bitmap::detail::tiff