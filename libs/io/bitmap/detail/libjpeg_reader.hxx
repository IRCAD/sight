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

#include "reader_impl.hxx"

#include <jpeglib.h>

// cspell:ignore nvjpeg JDIMENSION jerr JSAMPROW JSAMPLE scanline scanlines JMSG BGRX RGBX

namespace sight::io::bitmap::detail
{

class lib_jpeg_reader final
{
public:

    /// Delete copy constructors and assignment operators
    lib_jpeg_reader(const lib_jpeg_reader&)            = delete;
    lib_jpeg_reader& operator=(const lib_jpeg_reader&) = delete;

    /// Constructor
    inline lib_jpeg_reader() noexcept
    {
        try
        {
            m_cinfo.err = jpeg_std_error(&m_jerr);

            // Do it after jpeg_std_error() which clears the error handlers
            m_jerr.error_exit     = jpeg_error_exit;
            m_jerr.output_message = jpeg_output_message;

            // Initialize the JPEG decompression object
            jpeg_create_decompress(&m_cinfo);

            m_valid = true;
        }
        catch(const std::exception& e)
        {
            // Log error
            SIGHT_ERROR(e.what());

            // Free already allocated resources...
            free();
        }
    }

    /// Destructor
    inline ~lib_jpeg_reader() noexcept
    {
        free();
    }

    /// Reading
    inline void read(data::image& _image, std::istream& _istream, flag /*flag*/)
    {
        // Get input size
        _istream.seekg(0, std::ios::end);
        const auto stream_size = _istream.tellg();
        _istream.seekg(0, std::ios::beg);

        SIGHT_THROW_IF("The stream cannot be read.", stream_size <= 0);

        // Allocate input buffer
        const std::size_t input_buffer_size = std::size_t(stream_size);
        if(m_input_buffer.size() < input_buffer_size)
        {
            m_input_buffer.resize(input_buffer_size);
        }

        // Read input data..
        _istream.read(reinterpret_cast<char*>(m_input_buffer.data()), stream_size);

        // Set jpeg source
        jpeg_mem_src(&m_cinfo, m_input_buffer.data(), static_cast<unsigned long>(input_buffer_size));

        // Read header
        if(const auto result = jpeg_read_header(&m_cinfo, true); result != JPEG_HEADER_OK)
        {
            SIGHT_THROW("Error while reading JPEG header: " << result);
        }

        // Start decompression
        if(!jpeg_start_decompress(&m_cinfo))
        {
            SIGHT_THROW("Error while starting JPEG decompression.");
        }

        // Convert bit_depth and channels to Sight format
        const auto component_type =
            [&]
            {
                switch(m_cinfo.data_precision)
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
                        SIGHT_THROW("Unsupported bit depth: " << m_cinfo.data_precision);
                }
            }();

        const auto pixel_format =
            [&]
            {
                switch(m_cinfo.out_color_space)
                {
                    case JCS_GRAYSCALE:
                    {
                        SIGHT_THROW_IF(
                            "data::image::pixel_format::gray_scale must have exactly one component.",
                            m_cinfo.num_components != 1
                        );

                        return data::image::pixel_format::gray_scale;
                    }

                    case JCS_RGB:
                    case JCS_EXT_RGB:
                    {
                        SIGHT_THROW_IF(
                            "data::image::pixel_format::rgb must have exactly three components.",
                            m_cinfo.num_components != 3
                        );

                        return data::image::pixel_format::rgb;
                    }

                    case JCS_EXT_BGR:
                    {
                        SIGHT_THROW_IF(
                            "data::image::pixel_format::bgr must have exactly three components.",
                            m_cinfo.num_components != 3
                        );

                        return data::image::pixel_format::bgr;
                    }

                    case JCS_EXT_RGBA:
                    case JCS_EXT_RGBX:
                    {
                        SIGHT_THROW_IF(
                            "data::image::pixel_format::rgba must have exactly four components.",
                            m_cinfo.num_components != 4
                        );

                        return data::image::pixel_format::rgba;
                    }

                    case JCS_EXT_BGRA:
                    case JCS_EXT_BGRX:
                    {
                        SIGHT_THROW_IF(
                            "data::image::pixel_format::bgra must have exactly four components.",
                            m_cinfo.num_components != 4
                        );

                        return data::image::pixel_format::bgra;
                    }

                    default:
                        SIGHT_THROW("Unsupported color space: " << m_cinfo.out_color_space);
                }
            }();

        // Allocate destination image
        _image.resize(
            {m_cinfo.output_width, m_cinfo.output_height, 0},
            component_type,
            pixel_format
        );

        // Decompression loop
        JSAMPROW row_pointer[1] {};

        while(m_cinfo.output_scanline < m_cinfo.image_height)
        {
            // jpeg_read_scanlines expects an array of pointers to scanlines.
            row_pointer[0] = reinterpret_cast<unsigned char*>(
                _image.get_pixel(m_cinfo.output_scanline * m_cinfo.image_width)
            );

            SIGHT_THROW_IF(
                "jpeg_read_scanlines failed.",
                jpeg_read_scanlines(&m_cinfo, row_pointer, 1) != 1
            );
        }

        // Finish decompression
        if(!jpeg_finish_decompress(&m_cinfo))
        {
            SIGHT_THROW("Error while finishing JPEG decompression.");
        }
    }

private:

    //------------------------------------------------------------------------------

    inline void free() noexcept
    {
        try
        {
            jpeg_destroy_decompress(&m_cinfo);
        }
        catch(const std::exception& e)
        {
            // Log error
            SIGHT_ERROR(e.what());
        }

        m_valid = false;
    }

    /// Error handler for libJPEG
    inline static void jpeg_error_exit(j_common_ptr _cinfo)
    {
        char jpeg_last_error_msg[JMSG_LENGTH_MAX];

        // Create the message
        (*(_cinfo->err->format_message))(_cinfo, jpeg_last_error_msg);

        // Use exception instead of longjmp/setjmp
        SIGHT_THROW(jpeg_last_error_msg);
    }

    //------------------------------------------------------------------------------

    inline static void jpeg_output_message(j_common_ptr _cinfo)
    {
        char jpeg_last_error_msg[JMSG_LENGTH_MAX];

        // Create the message
        (*(_cinfo->err->format_message))(_cinfo, jpeg_last_error_msg);

        // Log recoverable error
        SIGHT_WARN(jpeg_last_error_msg);
    }

    struct jpeg_error_mgr m_jerr {};
    struct jpeg_decompress_struct m_cinfo {};

    std::vector<unsigned char> m_input_buffer;

public:

    bool m_valid {false};
    static constexpr std::string_view m_name {"LibJPEGReader"};
};

} // namespace sight::io::bitmap::detail
