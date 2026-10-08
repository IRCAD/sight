/************************************************************************
 *
 * Copyright (C) 2023-2026 IRCAD France
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

#include <core/exceptionmacros.hpp>

#include <tiffio.h>

#include <array>
#include <cstdarg>
#include <cstdio>

namespace sight::io::bitmap::detail::tiff
{

//------------------------------------------------------------------------------

inline static int map_proc(thandle_t /*unused*/, void** /*unused*/, toff_t* /*unused*/)
{
    return 0;
}

//------------------------------------------------------------------------------

inline static void unmap_proc(thandle_t /*unused*/, void* /*unused*/, toff_t /*unused*/)
{
}

//------------------------------------------------------------------------------

inline static void error_handler(const char* _module, const char* _fmt, va_list _args)
{
    std::array<char, 0xFFFF> error_buffer {};
    // NOLINTNEXTLINE(cert-err33-c)
    vsnprintf(error_buffer.data(), error_buffer.size(), _fmt, _args);

    std::string msg("Tiff Error: ");
    msg += _module;
    msg += ": ";
    msg += error_buffer.data();

    SIGHT_THROW(msg);
}

//------------------------------------------------------------------------------

inline static void warning_handler(const char* _module, const char* _fmt, va_list _args)
{
    std::array<char, 0xFFFF> warning_buffer {};
    // NOLINTNEXTLINE(cert-err33-c)
    vsnprintf(warning_buffer.data(), warning_buffer.size(), _fmt, _args);

    std::string msg("Tiff Warning: ");
    msg += _module;
    msg += ": ";
    msg += warning_buffer.data();

    SIGHT_WARN(msg);
}

static const struct handler_registry final
{
    handler_registry() noexcept
    {
        TIFFSetErrorHandler(&error_handler);
        TIFFSetWarningHandler(&warning_handler);
    }
} REGISTRY;

} // namespace sight::io::bitmap::detail::tiff
