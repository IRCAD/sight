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

#include "io/bitmap/reader.hpp"
#include "io/bitmap/writer.hpp"

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

enum class flag : std::uint64_t
{
    none        = 0,
    j2_k_stream = 1ULL << 0
};

//------------------------------------------------------------------------------

static constexpr flag operator|(flag _lhs, flag _rhs)
{
    return static_cast<flag>(
        static_cast<std::underlying_type_t<flag> >(_lhs)
        | static_cast<std::underlying_type_t<flag> >(_rhs)
    );
}

//------------------------------------------------------------------------------

static constexpr flag operator&(flag _lhs, flag _rhs)
{
    return static_cast<flag>(
        static_cast<std::underlying_type_t<flag> >(_lhs)
        & static_cast<std::underlying_type_t<flag> >(_rhs)
    );
}

/// Returns the backend associated to an extension
/// @arg extension: the selected extension
/// @return backend suitable for the given extension
inline static backend extension_to_backend(const std::string& _extension)
{
    if(_extension.ends_with(detail::JPEG_EXT) || _extension.ends_with(detail::JPG_EXT))
    {
        if(nv_jpeg())
        {
            return backend::nvjpeg;
        }

        return backend::libjpeg;
    }

    if(_extension.ends_with(detail::J2K_EXT))
    {
        if(nv_jpeg_2k())
        {
            return backend::nvjpeg2k_j2k;
        }

        return backend::openjpeg_j2_k;
    }

    if(_extension.ends_with(detail::JP2_EXT))
    {
        if(nv_jpeg_2k())
        {
            return backend::nvjpeg2k;
        }

        return backend::openjpeg;
    }

    if(_extension.ends_with(detail::TIFF_EXT) || _extension.ends_with(detail::TIF_EXT))
    {
        return backend::libtiff;
    }

    if(_extension.ends_with(detail::PNG_EXT))
    {
        return backend::libpng;
    }

    SIGHT_THROW("Unsupported extension: " << _extension);
}

//------------------------------------------------------------------------------

inline static std::pair<backend, data::sequenced_set<std::string> > guess_backend_or_extension(
    backend _backend,
    std::string _ext
)
{
    // If no backend is given, only rely on extension
    if(_backend == backend::any)
    {
        return {extension_to_backend(_ext), {_ext}};
    }

    // Enforce the extension to match the backend
    return {_backend, extensions(_backend)};
}

} // namespace sight::io::bitmap::detail
