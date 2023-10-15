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

#include <jpeglib.h>

#include <ostream>

// cspell:ignore nvjpeg JDIMENSION jerr JSAMPROW JSAMPLE scanline scanlines JMSG

namespace sight::io::bitmap::detail
{

class LibJPEGWriter final
{
public:

    /// Delete copy constructors and assignment operators
    LibJPEGWriter(const LibJPEGWriter&)            = delete;
    LibJPEGWriter& operator=(const LibJPEGWriter&) = delete;

    /// Constructor
    inline LibJPEGWriter() noexcept
    {
        try
        {
            m_cinfo.err = jpeg_std_error(&m_jerr);

            // Do it after jpeg_std_error() which clears the error handlers
            m_jerr.error_exit     = jpegErrorExit;
            m_jerr.output_message = jpegOutputMessage;

            // Initialize the JPEG compression object
            jpeg_create_compress(&m_cinfo);

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
    inline ~LibJPEGWriter() noexcept
    {
        free();
    }

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
        const data::image& image,
        O& output,
        Writer::Mode mode,
        Flag = Flag::NONE
)
    {
        //  JCS_EXT_RGBA is not yet fully supported by libjpeg-turbo, at least for writing
        const auto& pixel_format = image.getPixelFormat();
        SIGHT_THROW_IF(
            m_name << " - Unsupported image pixel format: " << pixel_format,
            pixel_format == data::image::PixelFormat::RG
            || pixel_format == data::image::PixelFormat::RGBA
            || pixel_format == data::image::PixelFormat::BGRA
        );

        const auto& pixel_type = image.getType();
        SIGHT_THROW_IF(
            m_name << " - Unsupported image type: " << pixel_type,
            pixel_type != core::type::UINT8
        );

        // Prepare the output buffers
        // LibJPEG will realloc memory, as needed, but its upon to the caller to free it...
        const auto image_byte_size = image.getSizeInBytes();
        if(image_byte_size > m_output_initial_buffer_size)
        {
            m_output_buffer              = reinterpret_cast<unsigned char*>(realloc(m_output_buffer, image_byte_size));
            m_output_initial_buffer_size = image_byte_size;

            // m_output_buffer_size will be adjusted to the size of JPEG data.
            // We need to keep the original allocated size
            m_output_buffer_size = static_cast<unsigned long>(image_byte_size);
        }

        jpeg_mem_dest(&m_cinfo, &m_output_buffer, &m_output_buffer_size);

        // Configure libJPEG
        const auto& sizes = image.size();
        m_cinfo.image_width  = JDIMENSION(sizes[0]);
        m_cinfo.image_height = JDIMENSION(sizes[1]);

        m_cinfo.input_components = int(image.numComponents());
        m_cinfo.in_color_space   =
            [pixel_format]
            {
                if(pixel_format == data::image::PixelFormat::RGB)
                {
                    return JCS_RGB;
                }

                if(pixel_format == data::image::PixelFormat::RGBA)
                {
                    return JCS_EXT_RGBA;
                }

                if(pixel_format == data::image::PixelFormat::BGR)
                {
                    return JCS_EXT_BGR;
                }

                if(pixel_format == data::image::PixelFormat::BGRA)
                {
                    return JCS_EXT_BGRA;
                }

                if(pixel_format == data::image::PixelFormat::GRAY_SCALE)
                {
                    return JCS_GRAYSCALE;
                }

                return JCS_UNKNOWN;
            }();

        // Use the defaults from libJPEG
        jpeg_set_defaults(&m_cinfo);

        // Set the quality to max
        jpeg_set_quality(&m_cinfo, 100, true);

        // Optimize or not huffman code. 10% slower - 20% smaller
        switch(mode)
        {
            case Writer::Mode::BEST:
                m_cinfo.optimize_coding = true;
                break;

            default:
                m_cinfo.optimize_coding = false;
                break;
        }

        // Start the compression process
        jpeg_start_compress(&m_cinfo, true);

        // Compression loop
        JSAMPROW row_pointer[1] {};

        while(m_cinfo.next_scanline < m_cinfo.image_height)
        {
            // jpeg_write_scanlines expects an array of pointers to scanlines.
            // libjpeg API is old -> const_cast
            row_pointer[0] = reinterpret_cast<unsigned char*>(
                const_cast<void*>(
                    image.getPixel(m_cinfo.next_scanline * m_cinfo.image_width)
                )
            );

            SIGHT_THROW_IF(
                "jpeg_write_scanlines failed.",
                jpeg_write_scanlines(&m_cinfo, row_pointer, 1) != 1
            );
        }

        // End compression
        jpeg_finish_compress(&m_cinfo);

        // Write to stream or buffer...
        if constexpr(std::is_base_of_v<std::ostream, O>)
        {
            output.write(reinterpret_cast<char*>(m_output_buffer), std::streamsize(m_output_buffer_size));
        }
        else if constexpr(std::is_same_v<std::uint8_t**, O>)
        {
            (*output) = new std::uint8_t[m_output_buffer_size];
            std::memcpy(*output, m_output_buffer, m_output_buffer_size);
        }
        else if constexpr(std::is_same_v<std::uint8_t*, O>)
        {
            std::memcpy(output, m_output_buffer, m_output_buffer_size);
        }
        else if constexpr(std::is_same_v<std::vector<std::uint8_t>, O>)
        {
            if(output.size() < m_output_buffer_size)
            {
                output.resize(m_output_buffer_size);
            }

            std::memcpy(output.data(), m_output_buffer, m_output_buffer_size);
        }
        else
        {
            SIGHT_THROW("No output stream or buffer provided.");
        }

        return m_output_buffer_size;
    }

private:

    //------------------------------------------------------------------------------

    inline void free() noexcept
    {
        try
        {
            ::free(m_output_buffer);
            m_output_buffer              = nullptr;
            m_output_buffer_size         = 0;
            m_output_initial_buffer_size = 0;
        }
        catch(const std::exception& e)
        {
            // Log error
            SIGHT_ERROR(e.what());
        }

        try
        {
            jpeg_destroy_compress(&m_cinfo);
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
    struct jpeg_compress_struct m_cinfo {};

    unsigned char* m_output_buffer {nullptr};
    unsigned long m_output_buffer_size {0};
    std::size_t m_output_initial_buffer_size {0};

public:

    bool m_valid {false};
    static constexpr std::string_view m_name {"LibJPEGWriter"};
};

} // namespace sight::io::bitmap::detail
