/************************************************************************
 *
 * Copyright (C) 2023-2025 IRCAD France
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

#include "core/exceptionmacros.hpp"

#include "data/image.hpp"

#include "io/bitmap/reader.hpp"
#include "io/bitmap/writer.hpp"

#include <functional>
#include <optional>

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

enum class flag : std::uint8_t
{
    none       = 0,
    j2k_stream = 1ULL << 0
};

class reader_backend
{
public:

    reader_backend()          = default;
    virtual ~reader_backend() = default;

    virtual void read(
        data::image& _image,
        std::istream& _istream,
        flag _flag
    ) = 0;

    //------------------------------------------------------------------------------

    /// Default implementation using buffer. This is inefficient as it may use a temporary image but should work for
    /// every backend
    virtual void read(
        const std::optional<std::reference_wrapper<data::image> >& _image,
        const std::uint8_t* const _input,
        std::size_t _input_size,
        std::uint8_t* const _output,
        flag _flag
)
    {
        SIGHT_WARN("Using default inefficient buffer implementation. Prefer stream version.");

        SIGHT_THROW_IF("image or a buffer must be provided", !_image.has_value() && _output == nullptr);

        std::istringstream stream(
            std::string(
                reinterpret_cast<const char*>(_input),
                _input_size
            )
        );

        if(_output == nullptr)
        {
            read(_image->get(), stream, _flag);
        }
        else if(_image.has_value())
        {
            data::image& image = _image->get();

            read(image, stream, _flag);
            std::memcpy(_output, image.buffer(), image.size_in_bytes());
        }
        else
        {
            // Create a temporary image
            data::image image;
            const auto dump_lock = image.dump_lock();

            read(image, stream, _flag);
            std::memcpy(_output, image.buffer(), image.size_in_bytes());
        }
    }

    [[nodiscard]] virtual bool is_valid() const noexcept = 0;
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
        if(nvjpeg())
        {
            return backend::nvjpeg;
        }

        return backend::libjpeg;
    }

    if(_extension.ends_with(detail::J2K_EXT))
    {
        if(nvjpeg2k())
        {
            return backend::nvjpeg2k_j2k;
        }

        return backend::openjpeg_j2k;
    }

    if(_extension.ends_with(detail::JP2_EXT))
    {
        if(nvjpeg2k())
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
