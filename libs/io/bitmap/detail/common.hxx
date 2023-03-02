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

#include "io/bitmap/Reader.hpp"
#include "io/bitmap/Writer.hpp"

namespace sight::io::bitmap::detail
{

static constexpr auto JPEG_EXT {".jpeg"};
static constexpr auto JPG_EXT {".jpg"};
static constexpr auto TIF_EXT {".tif"};
static constexpr auto TIFF_EXT {".tiff"};
static constexpr auto PNG_EXT {".png"};
static constexpr auto JP2_EXT {".jp2"};
static constexpr auto J2K_EXT {".j2k"};

static constexpr auto JPEG_LABEL {"JPEG image"};
static constexpr auto TIFF_LABEL {"TIFF image"};
static constexpr auto PNG_LABEL {"PNG image"};
static constexpr auto J2K_LABEL {"JPEG2000 image"};

enum class Flag : std::uint64_t
{
    NONE       = 0,
    J2K_STREAM = 1ULL << 0
};

//------------------------------------------------------------------------------

static constexpr Flag operator|(Flag lhs, Flag rhs)
{
    return static_cast<Flag>(
        static_cast<std::underlying_type_t<Flag> >(lhs)
        | static_cast<std::underlying_type_t<Flag> >(rhs)
    );
}

//------------------------------------------------------------------------------

static constexpr Flag operator&(Flag lhs, Flag rhs)
{
    return static_cast<Flag>(
        static_cast<std::underlying_type_t<Flag> >(lhs)
        & static_cast<std::underlying_type_t<Flag> >(rhs)
    );
}

/// Returns the backend associated to an extension
/// @arg extension: the selected extension
/// @return backend suitable for the given extension
inline static Backend extensionToBackend(const std::string& extension)
{
    if(extension.ends_with(detail::JPEG_EXT) || extension.ends_with(detail::JPG_EXT))
    {
        if(nvJPEG())
        {
            return Backend::NVJPEG;
        }

        return Backend::LIBJPEG;
    }

    if(extension.ends_with(detail::J2K_EXT))
    {
        if(nvJPEG2K())
        {
            return Backend::NVJPEG2K_J2K;
        }

        return Backend::OPENJPEG_J2K;
    }

    if(extension.ends_with(detail::JP2_EXT))
    {
        if(nvJPEG2K())
        {
            return Backend::NVJPEG2K;
        }

        return Backend::OPENJPEG;
    }

    if(extension.ends_with(detail::TIFF_EXT) || extension.ends_with(detail::TIF_EXT))
    {
        return Backend::LIBTIFF;
    }

    if(extension.ends_with(detail::PNG_EXT))
    {
        return Backend::LIBPNG;
    }

    SIGHT_THROW("Unsupported extension: " << extension);
}

//------------------------------------------------------------------------------

inline static std::pair<Backend, data::sequenced_set<std::string> > guessBackendOrExtension(
    Backend backend,
    std::string ext
)
{
    // If no backend is given, only rely on extension
    if(backend == Backend::ANY)
    {
        return {extensionToBackend(ext), {ext}};
    }

    // Enforce the extension to match the backend
    return {backend, extensions(backend)};
}

} // namespace sight::io::bitmap::detail
