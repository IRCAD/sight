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

#include "ReaderImpl.hxx"

#include <png.h>

// cspell:ignore nvjpeg jmpbuf structp bytep IHDR charp

namespace sight::io::bitmap::detail
{

class LibPNGReader final
{
public:

    /// Delete copy constructors and assignment operators
    LibPNGReader(const LibPNGReader&)            = delete;
    LibPNGReader& operator=(const LibPNGReader&) = delete;

    /// Constructor
    inline LibPNGReader() noexcept = default;

    /// Destructor
    inline ~LibPNGReader() noexcept = default;

    /// Reading
    inline void read(data::image& _image, std::istream& _istream, Flag /*flag*/)
    {
        // Create an RAII to be sure everything is cleaned at exit
        struct Keeper final
        {
            Keeper()
            {
                m_png = png_create_read_struct(PNG_LIBPNG_VER_STRING, nullptr, nullptr, nullptr);
                SIGHT_THROW_IF("png_create_read_struct() failed.", m_png == nullptr);

                // Set error/warning callback because C style setjmp/longjmp error management is dangerous in C++
                png_set_error_fn(m_png, nullptr, errorCallback, warningCallback);

                m_png_info = png_create_info_struct(m_png);
                SIGHT_THROW_IF("png_create_info_struct() failed.", m_png_info == nullptr);
            }

            ~Keeper()
            {
                png_destroy_read_struct(&m_png, &m_png_info, nullptr);
                m_png      = nullptr;
                m_png_info = nullptr;
            }

            png_structp m_png {nullptr};
            png_infop m_png_info {nullptr};
        } keeper;

        // Set read callback
        png_set_read_fn(keeper.m_png, &_istream, readCallback);

        // Read header
        png_read_info(keeper.m_png, keeper.m_png_info);

        // Parse header
        const auto width  = png_get_image_width(keeper.m_png, keeper.m_png_info);
        const auto height = png_get_image_height(keeper.m_png, keeper.m_png_info);
        auto bit_depth    = png_get_bit_depth(keeper.m_png, keeper.m_png_info);
        auto channels     = png_get_channels(keeper.m_png, keeper.m_png_info);
        auto color_type   = png_get_color_type(keeper.m_png, keeper.m_png_info);

        // Convert palette color to RGB and grayscale to 8 bit depth
        if(color_type == PNG_COLOR_TYPE_PALETTE)
        {
            png_set_palette_to_rgb(keeper.m_png);
            channels = 3;
        }
        else if(color_type == PNG_COLOR_TYPE_GRAY && bit_depth < 8)
        {
            png_set_expand_gray_1_2_4_to_8(keeper.m_png);
            bit_depth = 8;
        }

        // Convert transparency to alpha channel
        if(png_get_valid(keeper.m_png, keeper.m_png_info, PNG_INFO_tRNS))
        {
            png_set_tRNS_to_alpha(keeper.m_png);
            channels += 1;
        }

        // Adjust gamma if a gamma is set in the file
        if(double gamma = 1.0; png_get_gAMA(keeper.m_png, keeper.m_png_info, &gamma))
        {
            png_set_gamma(keeper.m_png, 1.0, gamma);
        }

        // Convert to little endian if needed
        if constexpr(std::endian::native == std::endian::little)
        {
            png_set_swap(keeper.m_png);
        }

        // Update the information struct appropriately
        png_read_update_info(keeper.m_png, keeper.m_png_info);

        // Convert bit_depth and channels to Sight format
        const auto component_type =
            [&]
            {
                switch(bit_depth)
                {
                    case 8:
                        return core::type::UINT8;

                    case 16:
                        return core::type::UINT16;

                    case 32:
                        return core::type::UINT32;

                    case 64:
                        return core::type::UINT64;

                    default:
                        SIGHT_THROW("Unsupported bit depth: " << bit_depth);
                }
            }();

        const auto pixel_format =
            [&]
            {
                switch(channels)
                {
                    case 1:
                        return data::image::PixelFormat::GRAY_SCALE;

                    case 2:
                        return data::image::PixelFormat::RG;

                    case 3:
                        return data::image::PixelFormat::RGB;

                    case 4:
                        return data::image::PixelFormat::RGBA;

                    default:
                        SIGHT_THROW("Unsupported number of channels: " << channels);
                }
            }();

        // Allocate destination image
        _image.resize({width, height, 0}, component_type, pixel_format);

        // Create an array of row pointers and assign them to the image buffer
        std::vector<png_bytep> row_pointers;
        row_pointers.reserve(height);

        for(size_t row = 0 ; row < height ; ++row)
        {
            row_pointers.emplace_back(reinterpret_cast<png_bytep>(_image.getPixel(row * width)));
        }

        // Read pixel data
        png_read_image(keeper.m_png, row_pointers.data());
        png_read_end(keeper.m_png, keeper.m_png_info);
    }

private:

    //------------------------------------------------------------------------------

    inline static void readCallback(png_structp _png_ptr, png_bytep _data, png_size_t _length)
    {
        auto* istream = reinterpret_cast<std::istream*>(png_get_io_ptr(_png_ptr));
        istream->read(reinterpret_cast<char*>(_data), std::streamsize(_length));
    }

    //------------------------------------------------------------------------------

    inline static void warningCallback(png_structp, png_const_charp _msg)
    {
        SIGHT_WARN(_msg);
    }

    //------------------------------------------------------------------------------

    inline static void errorCallback(png_structp, png_const_charp _msg)
    {
        SIGHT_THROW(_msg);
    }

public:

    bool m_valid {true};
    static constexpr std::string_view m_name {"LibPNGReader"};
};

} // namespace sight::io::bitmap::detail
