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

#include <jpeglib.h>

// cspell:ignore nvjpeg JDIMENSION jerr JSAMPROW JSAMPLE scanline scanlines JMSG BGRX RGBX

namespace sight::io::bitmap::detail
{

class LibJPEGReader final
{
public:

    /// Delete copy constructors and assignment operators
    LibJPEGReader(const LibJPEGReader&)            = delete;
    LibJPEGReader& operator=(const LibJPEGReader&) = delete;

    /// Constructor
    inline LibJPEGReader() noexcept
    {
        try
        {
            m_cinfo.err = jpeg_std_error(&m_jerr);

            // Do it after jpeg_std_error() which clears the error handlers
            m_jerr.error_exit     = jpegErrorExit;
            m_jerr.output_message = jpegOutputMessage;

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
    inline ~LibJPEGReader() noexcept
    {
        free();
    }

    /// Reading
    inline void read(data::Image& image, std::istream& istream, Flag /*flag*/)
    {
        // Get input size
        istream.seekg(0, std::ios::end);
        const auto stream_size = istream.tellg();
        istream.seekg(0, std::ios::beg);

        SIGHT_THROW_IF("The stream cannot be read.", stream_size <= 0);

        // Allocate input buffer
        const std::size_t input_buffer_size = std::size_t(stream_size);
        if(m_input_buffer.size() < input_buffer_size)
        {
            m_input_buffer.resize(input_buffer_size);
        }

        // Read input data..
        istream.read(reinterpret_cast<char*>(m_input_buffer.data()), stream_size);

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
                        return core::Type::UINT8;

                    case 16:
                        return core::Type::UINT16;

                    case 32:
                        return core::Type::UINT32;

                    case 64:
                        return core::Type::UINT64;

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
                            "data::Image::PixelFormat::GRAY_SCALE must have exactly one component.",
                            m_cinfo.num_components != 1
                        );

                        return data::Image::PixelFormat::GRAY_SCALE;
                    }

                    case JCS_RGB:
                    case JCS_EXT_RGB:
                    {
                        SIGHT_THROW_IF(
                            "data::Image::PixelFormat::RGB must have exactly three components.",
                            m_cinfo.num_components != 3
                        );

                        return data::Image::PixelFormat::RGB;
                    }

                    case JCS_EXT_BGR:
                    {
                        SIGHT_THROW_IF(
                            "data::Image::PixelFormat::BGR must have exactly three components.",
                            m_cinfo.num_components != 3
                        );

                        return data::Image::PixelFormat::BGR;
                    }

                    case JCS_EXT_RGBA:
                    case JCS_EXT_RGBX:
                    {
                        SIGHT_THROW_IF(
                            "data::Image::PixelFormat::RGBA must have exactly four components.",
                            m_cinfo.num_components != 4
                        );

                        return data::Image::PixelFormat::RGBA;
                    }

                    case JCS_EXT_BGRA:
                    case JCS_EXT_BGRX:
                    {
                        SIGHT_THROW_IF(
                            "data::Image::PixelFormat::BGRA must have exactly four components.",
                            m_cinfo.num_components != 4
                        );

                        return data::Image::PixelFormat::BGRA;
                    }

                    default:
                        SIGHT_THROW("Unsupported color space: " << m_cinfo.out_color_space);
                }
            }();

        // Allocate destination image
        image.resize(
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
                image.getPixel(m_cinfo.output_scanline * m_cinfo.image_width)
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
    inline static void jpegErrorExit(j_common_ptr cinfo)
    {
        char jpegLastErrorMsg[JMSG_LENGTH_MAX];

        // Create the message
        (*(cinfo->err->format_message))(cinfo, jpegLastErrorMsg);

        // Use exception instead of longjmp/setjmp
        SIGHT_THROW(jpegLastErrorMsg);
    }

    //------------------------------------------------------------------------------

    inline static void jpegOutputMessage(j_common_ptr cinfo)
    {
        char jpegLastErrorMsg[JMSG_LENGTH_MAX];

        // Create the message
        (*(cinfo->err->format_message))(cinfo, jpegLastErrorMsg);

        // Log recoverable error
        SIGHT_WARN(jpegLastErrorMsg);
    }

    struct jpeg_error_mgr m_jerr {};
    struct jpeg_decompress_struct m_cinfo {};

    std::vector<unsigned char> m_input_buffer;

public:

    bool m_valid {false};
    static constexpr std::string_view m_name {"LibJPEGReader"};
};

} // namespace sight::io::bitmap::detail