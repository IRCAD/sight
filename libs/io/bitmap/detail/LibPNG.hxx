/************************************************************************
 *
 * Copyright (C) 2022 IRCAD France
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

#include "types.hpp"
#include "WriterImpl.hxx"

#include <png.h>

// cspell:ignore nvjpeg jmpbuf structp bytep IHDR charp

namespace sight::io::bitmap::detail
{

class LibPNG final
{
public:

    /// Delete copy constructors and assignment operators
    LibPNG(const LibPNG&)            = delete;
    LibPNG& operator=(const LibPNG&) = delete;

    /// Constructor
    inline LibPNG() noexcept
    {
        m_valid = true;
    }

    /// Destructor
    inline ~LibPNG() noexcept = default;

    /// Writing
    inline void write(const data::Image& image, std::ostream& ostream, ExtendedMode mode)
    {
        // png_create_write_struct() must be called for each png_write_png() calls
        // Create an RAII to be sure everything is cleaned at exit
        class Keeper final
        {
        public:

            Keeper()
            {
                m_png = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
                SIGHT_THROW_IF("png_create_write_struct() failed.", m_png == nullptr);

                // Set error/warning callback because C style setjmp/longjmp error management is dangerous in C++
                png_set_error_fn(m_png, nullptr, errorCallback, warningCallback);

                m_png_info = png_create_info_struct(m_png);
                SIGHT_THROW_IF("png_create_info_struct() failed.", m_png_info == nullptr);
            }

            ~Keeper()
            {
                // Cleanup
                png_destroy_write_struct(&m_png, &m_png_info);
                m_png      = nullptr;
                m_png_info = nullptr;
            }

            png_structp m_png {nullptr};
            png_infop m_png_info {nullptr};
        } keeper;

        const auto& pixelFormat = image.getPixelFormat();
        const int png_format    =
            [&]
            {
                switch(pixelFormat)
                {
                    case data::Image::PixelFormat::RGB:
                    case data::Image::PixelFormat::BGR:
                        return PNG_COLOR_TYPE_RGB;

                    case data::Image::PixelFormat::RGBA:
                    case data::Image::PixelFormat::BGRA:
                        return PNG_COLOR_TYPE_RGB_ALPHA;

                    case data::Image::PixelFormat::GRAY_SCALE:
                        return PNG_COLOR_TYPE_GRAY;

                    default:
                        SIGHT_THROW("Unsupported pixel format: " << pixelFormat);
                }
            }();

        const auto& sizes        = image.getSize();
        const auto& image_width  = sizes[0];
        const auto& image_height = sizes[1];
        const auto& type         = image.getType();

        png_set_IHDR(
            keeper.m_png,
            keeper.m_png_info,
            png_uint_32(image_width),
            png_uint_32(image_height),
            int(type.size() * 8),
            png_format,
            PNG_INTERLACE_NONE,
            PNG_COMPRESSION_TYPE_DEFAULT,
            PNG_FILTER_TYPE_DEFAULT
        );

        // Use more memory to be faster
        png_set_compression_mem_level(keeper.m_png, 9 /*9=MAX_MEM_LEVEL*/);

        switch(mode)
        {
            case ExtendedMode::BEST:
            case ExtendedMode::J2K_BEST:
            {
                // Best compression
                png_set_compression_level(keeper.m_png, 9);

                // Use more memory to compress better
                png_set_compression_window_bits(keeper.m_png, 15 /*[8..15]*/);
                break;
            }

            default:
            {
                // Fastest compression
                png_set_compression_level(keeper.m_png, 1);

                // Faster than default and compress better than fixed or huffman only
                png_set_compression_strategy(keeper.m_png, 3 /*3=Z_RLE*/);
                break;
            }
        }

        // Allocate memory
        if(m_rows.size() < image_height)
        {
            m_rows.resize(image_height);
        }

        // Prepare row pointers vector
        for(std::size_t row = 0 ; row < image_height ; ++row)
        {
            // scanlines should only be read, so the const_cast should be ok..
            m_rows[row] = reinterpret_cast<std::uint8_t*>(const_cast<void*>(image.getPixel(row * image_width)));
        }

        // Use the row pointers vector
        png_set_rows(keeper.m_png, keeper.m_png_info, &m_rows[0]);

        png_set_write_fn(keeper.m_png, &ostream, writeCallback, NULL);

        const int transform =
            pixelFormat == data::Image::PixelFormat::BGR || pixelFormat == data::Image::PixelFormat::BGRA
            ? PNG_TRANSFORM_BGR
            : PNG_TRANSFORM_IDENTITY;

        png_write_png(keeper.m_png, keeper.m_png_info, transform, NULL);
    }

private:

    //------------------------------------------------------------------------------

    inline static void writeCallback(png_structp png_ptr, png_bytep data, png_size_t length)
    {
        auto* ostream = reinterpret_cast<std::ostream*>(png_get_io_ptr(png_ptr));
        ostream->write(reinterpret_cast<char*>(data), std::streamsize(length));
    }

    //------------------------------------------------------------------------------

    inline static void warningCallback(png_structp, png_const_charp msg)
    {
        SIGHT_WARN(msg);
    }

    //------------------------------------------------------------------------------

    inline static void errorCallback(png_structp, png_const_charp msg)
    {
        SIGHT_THROW(msg);
    }

    std::vector<png_bytep> m_rows;

public:

    bool m_valid {false};
    static constexpr std::string_view m_name {"LibPNG"};
};

} // namespace sight::io::bitmap::detail
