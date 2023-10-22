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

#include "io/bitmap/config.hpp"

#include <data/container.hpp>

namespace sight::io::bitmap
{

/// @brief Supported backends. Some of them require an NVidia GPU
enum class Backend : std::uint8_t
{
    ANY          = 0,     ///< Use the file extension to guess the backend. Use DEFAULT if the extension is unknown.
    NVJPEG       = 1,
    NVJPEG2K     = 2,
    NVJPEG2K_J2K = 3,
    LIBJPEG      = 4,
    LIBTIFF      = 5,
    LIBPNG       = 6,
    OPENJPEG     = 7,
    OPENJPEG_J2K = 8
};

/// Return the extension associated with the given backend
/// @arg backend: the selected backend
/// @return one or more extension as string set
[[nodiscard]] IO_BITMAP_API data::sequenced_set<std::string> extensions(Backend _backend);

/// Returns the label and the wildcard to use in file open dialog to filter input
/// @arg backend: the selected backend
/// @return default filter to use with the given backend
[[nodiscard]] IO_BITMAP_API std::pair<std::string, std::string> wildcard_filter(Backend _backend);

/// @return true is nvJPEG is available (support built in AND CUDA capable GPU available)
[[nodiscard]] IO_BITMAP_API bool nv_jpeg();

/// @return true is nv_jpeg_2k is available (support built in AND CUDA capable GPU available)
[[nodiscard]] IO_BITMAP_API bool nv_jpeg_2k();

} // namespace sight::io::bitmap
