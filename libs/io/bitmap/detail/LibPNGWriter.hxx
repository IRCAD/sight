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

#include "WriterImpl.hxx"

#include <png.h>

// cspell:ignore nvjpeg jmpbuf structp bytep IHDR charp

namespace sight::io::bitmap::detail
{

class LibPNGWriter final
{
public:

    /// Delete copy constructors and assignment operators
    LibPNGWriter(const LibPNGWriter&)            = delete;
    LibPNGWriter& operator=(const LibPNGWriter&) = delete;

    /// Constructor
    inline LibPNGWriter() noexcept
    {
        m_valid = true;
    }

    /// Destructor
    inline ~LibPNGWriter() noexcept = default;

    /// Writing
    template<
        typename O,
        std::enable_if_t<
            std::is_base_of_v<std::ostream, O>
            || std::is_same_v<std::uint8_t*, O>
            || std::is_same_v<std::uint8_t**, O>
            || std::is_same_v<std::vector<uint8_t>, O>,
            bool
        > = true
    >
    inline std::size_t write(
        const data::Image& image,
        O& output,
        Writer::Mode mode,
        Flag = Flag::NONE
)
    {
        const auto& type = image.getType();
        SIGHT_THROW_IF(
            m_name << " - Unsupported image type: " << type,
            type != core::Type::UINT8
            && type != core::Type::UINT16
            && type != core::Type::UINT64
            && type != core::Type::UINT64
        );

        // png_create_write_struct() must be called for each png_write_png() calls
        // Create an RAII to be sure everything is cleaned at exit
        struct Keeper final
        {
            Keeper()
            {
                m_png = png_create_write_struct(PNG_LIBPNG_VER_STRING, nullptr, nullptr, nullptr);
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
                        SIGHT_THROW(m_name << " - Unsupported pixel format: " << pixelFormat);
                }
            }();

        const auto& sizes        = image.getSize();
        const auto& image_width  = sizes[0];
        const auto& image_height = sizes[1];

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
            case Writer::Mode::BEST:
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

        int transform = PNG_TRANSFORM_IDENTITY;

        if(pixelFormat == data::Image::PixelFormat::BGR || pixelFormat == data::Image::PixelFormat::BGRA)
        {
            transform |= PNG_TRANSFORM_BGR;
        }

        if constexpr(std::endian::native == std::endian::little)
        {
            // PNG is big endian
            transform |= PNG_TRANSFORM_SWAP_ENDIAN;
        }

        if constexpr(std::is_base_of_v<std::ostream, O>)
        {
            png_set_write_fn(keeper.m_png, &output, writeCallback, 0);
            png_write_png(keeper.m_png, keeper.m_png_info, transform, NULL);
        }
        else if constexpr(std::is_same_v<std::uint8_t*, O>
                          || std::is_same_v<std::uint8_t**, O>
                          || std::is_same_v<std::vector<std::uint8_t>, O>)
        {
            std::stringstream ss;

            png_set_write_fn(keeper.m_png, &ss, 0, 0);
            png_write_png(keeper.m_png, keeper.m_png_info, transform, NULL);

            // Zero copy string conversion, work only with C++20
            const std::string output_buffer = std::move(ss).str();
            const auto output_buffer_size   = output_buffer.size();

            if constexpr(std::is_same_v<std::uint8_t**, O>)
            {
                (*output) = new std::uint8_t[output_buffer_size];
                std::memcpy((*output), output_buffer.data(), output_buffer_size);
            }
            else if constexpr(std::is_same_v<std::uint8_t*, O>)
            {
                std::memcpy(output, output_buffer.data(), output_buffer_size);
            }
            else if constexpr(std::is_same_v<std::vector<std::uint8_t>, O>)
            {
                if(output.size() < output_buffer_size)
                {
                    output.resize(output_buffer_size);
                }

                std::memcpy(output.data(), output_buffer.data(), output_buffer_size);
            }

            return output_buffer_size;
        }
        else
        {
            SIGHT_THROW("No output stream or buffer provided.");
        }

        return 1;
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
    static constexpr std::string_view m_name {"LibPNGWriter"};
};

} // namespace sight::io::bitmap::detail
