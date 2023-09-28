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

#include "LibTIFFCommon.hxx"
#include "WriterImpl.hxx"

// cspell:ignore nvjpeg NOLINTNEXTLINE TIFFTAG IMAGEWIDTH IMAGELENGTH BITSPERSAMPLE SAMPLESPERPIXEL MINISBLACK
// cspell:ignore PLANARCONFIG TOPLEFT ROWSPERSTRIP Scanline XRESOLUTION YRESOLUTION thandle SAMPLEFORMAT
// cspell:ignore PACKBITS EXTRASAMPLE RESOLUTIONUNIT RESUNIT EXTRASAMPLES tiffio tmsize

namespace sight::io::bitmap::detail
{

// NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
#define CHECK_TIFF(func) SIGHT_THROW_IF("The function " #func " failed.", (func) == 0)

class LibTIFFWriter final
{
public:

    /// Delete copy constructors and assignment operators
    LibTIFFWriter(const LibTIFFWriter&)            = delete;
    LibTIFFWriter& operator=(const LibTIFFWriter&) = delete;

    /// Constructor
    inline LibTIFFWriter() noexcept = default;

    /// Destructor
    inline ~LibTIFFWriter() noexcept = default;

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
        const auto& pixel_format = image.getPixelFormat();
        SIGHT_THROW_IF(
            m_name << " - Unsupported image pixel format: " << pixel_format,
            pixel_format != data::Image::PixelFormat::RGB
            && pixel_format != data::Image::PixelFormat::RGBA
            && pixel_format != data::Image::PixelFormat::GRAY_SCALE
        );

        const auto& pixel_type = image.getType();
        SIGHT_THROW_IF(
            m_name << " - Unsupported image pixel type: " << pixel_type,
            pixel_type != core::type::INT8
            && pixel_type != core::type::UINT8
            && pixel_type != core::type::INT16
            && pixel_type != core::type::UINT16
            && pixel_type != core::type::FLOAT
        );

        struct Keeper final
        {
            inline ~Keeper()
            {
                if(m_tiff != nullptr)
                {
                    TIFFClose(m_tiff);
                    m_tiff = nullptr;
                }
            }

            TIFF* m_tiff {nullptr};

            // For buffer mode
            std::stringstream m_buffer;
        } keeper;

        // Open the stream for writing
        if constexpr(std::is_base_of_v<std::ostream, O>)
        {
            keeper.m_tiff = tiffStreamOpen(output);
        }
        else
        {
            keeper.m_tiff = tiffStreamOpen(keeper.m_buffer);
        }

        SIGHT_THROW_IF("TIFFOpen() failed.", keeper.m_tiff == nullptr);

        // Set the configuration
        const auto& sizes        = image.size();
        const auto& image_width  = sizes[0];
        const auto& image_height = sizes[1];
        CHECK_TIFF(TIFFSetField(keeper.m_tiff, TIFFTAG_IMAGEWIDTH, image_width));
        CHECK_TIFF(TIFFSetField(keeper.m_tiff, TIFFTAG_IMAGELENGTH, image_height));
        CHECK_TIFF(TIFFSetField(keeper.m_tiff, TIFFTAG_BITSPERSAMPLE, pixel_type.size() * 8));

        if(pixel_type == core::type::INT8 || pixel_type == core::type::INT16)
        {
            CHECK_TIFF(TIFFSetField(keeper.m_tiff, TIFFTAG_SAMPLEFORMAT, SAMPLEFORMAT_INT));
        }
        else if(pixel_type == core::type::UINT8 || pixel_type == core::type::UINT16)
        {
            CHECK_TIFF(TIFFSetField(keeper.m_tiff, TIFFTAG_SAMPLEFORMAT, SAMPLEFORMAT_UINT));
        }
        else if(pixel_type == core::type::FLOAT || pixel_type == core::type::DOUBLE)
        {
            CHECK_TIFF(TIFFSetField(keeper.m_tiff, TIFFTAG_SAMPLEFORMAT, SAMPLEFORMAT_IEEEFP));
        }

        // Compression
        switch(mode)
        {
            case Writer::Mode::BEST:
            {
                // COMPRESSION_LZW + PREDICTOR_HORIZONTAL is 2x slower than COMPRESSION_PACKBITS, but 30% smaller
                // CHECK_TIFF(TIFFSetField(m_tiff, TIFFTAG_COMPRESSION, COMPRESSION_LZW));
                // CHECK_TIFF(TIFFSetField(m_tiff, TIFFTAG_PREDICTOR, PREDICTOR_HORIZONTAL));

                // ZSTD support is not yet standard for Adobe... :(
                // This settings produce 10% smaller file than COMPRESSION_LZW as same speed than COMPRESSION_PACKBITS
                CHECK_TIFF(TIFFSetField(keeper.m_tiff, TIFFTAG_COMPRESSION, COMPRESSION_ZSTD));
                CHECK_TIFF(TIFFSetField(keeper.m_tiff, TIFFTAG_PREDICTOR, PREDICTOR_HORIZONTAL));
                CHECK_TIFF(TIFFSetField(keeper.m_tiff, TIFFTAG_ZSTD_LEVEL, 1));
                break;
            }

            default:
            {
                // COMPRESSION_PACKBITS (RLE) is the fastest
                TIFFSetField(keeper.m_tiff, TIFFTAG_COMPRESSION, COMPRESSION_PACKBITS);
                break;
            }
        }

        const auto& num_components = image.numComponents();
        CHECK_TIFF(TIFFSetField(keeper.m_tiff, TIFFTAG_SAMPLESPERPIXEL, num_components));
        CHECK_TIFF(
            TIFFSetField(
                keeper.m_tiff,
                TIFFTAG_PHOTOMETRIC,
                num_components == 1 ? PHOTOMETRIC_MINISBLACK : PHOTOMETRIC_RGB
            )
        );

        // Manage alpha channel, if present
        if(num_components > 3)
        {
            const std::uint16_t extra_samples = std::uint16_t(num_components - 3);
            std::vector<std::uint16_t> sample_info(extra_samples);

            sample_info[0] = EXTRASAMPLE_ASSOCALPHA;

            for(std::uint16_t i = 1 ; i < extra_samples ; ++i)
            {
                sample_info[i] = EXTRASAMPLE_UNSPECIFIED;
            }

            CHECK_TIFF(TIFFSetField(keeper.m_tiff, TIFFTAG_EXTRASAMPLES, extra_samples, sample_info.data()));
        }

        CHECK_TIFF(TIFFSetField(keeper.m_tiff, TIFFTAG_ORIENTATION, ORIENTATION_TOPLEFT));
        CHECK_TIFF(TIFFSetField(keeper.m_tiff, TIFFTAG_PLANARCONFIG, PLANARCONFIG_CONTIG));

        // Store "spacing"
        if(const auto& spacings = image.getSpacing(); spacings[0] > 0.0 && spacings[1] > 0.0)
        {
            CHECK_TIFF(TIFFSetField(keeper.m_tiff, TIFFTAG_XRESOLUTION, 10 / spacings[0]));
            CHECK_TIFF(TIFFSetField(keeper.m_tiff, TIFFTAG_YRESOLUTION, 10 / spacings[1]));
            CHECK_TIFF(TIFFSetField(keeper.m_tiff, TIFFTAG_RESOLUTIONUNIT, RESUNIT_CENTIMETER));
        }

        // length in memory of one row of pixel in the image.
        const std::uint32_t strip_size = std::uint32_t(image_width * num_components * pixel_type.size());
        CHECK_TIFF(TIFFSetField(keeper.m_tiff, TIFFTAG_ROWSPERSTRIP, TIFFDefaultStripSize(keeper.m_tiff, strip_size)));

        // Allocate a row buffer. LibTIFF modify source buffer !
        if(m_row_buffer.size() < strip_size)
        {
            m_row_buffer.resize(strip_size);
        }

        // Start writing
        for(std::uint32_t row = 0 ; row < image_height ; ++row)
        {
            // Copy to row buffer the original data
            std::memcpy(m_row_buffer.data(), image.getPixel(row * image_width), strip_size);

            SIGHT_THROW_IF(
                "TIFFWriteScanline() failed.",
                TIFFWriteScanline(keeper.m_tiff, m_row_buffer.data(), row) < 0
            );
        }

        if constexpr(std::is_same_v<std::uint8_t*, O>
                     || std::is_same_v<std::uint8_t**, O>
                     || std::is_same_v<std::vector<std::uint8_t>, O>)
        {
            // Zero copy string conversion, work only with C++20
            const std::string output_buffer = std::move(keeper.m_buffer).str();
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
        else if constexpr(!std::is_base_of_v<std::ostream, O>)
        {
            SIGHT_THROW("No output stream or buffer provided.");
        }

        return 1;
    }

private:

    /// TIFF c++ API (tiffio.hxx and tif_stream.cxx) is not available on Windows. We simply recreate it
    /// @{
    struct tiff_stream_data
    {
        std::ostream& ostream;
        std::ios::pos_type start_pos;
    };

    //------------------------------------------------------------------------------

    inline static TIFF* tiffStreamOpen(std::ostream& ostream)
    {
        // If os is either a ostrstream or ostringstream, and has no data
        // written to it yet, then tellp() will return -1 which will break us.
        // We workaround this by writing out a dummy character and
        // then seek back to the beginning.
        if(!ostream.fail() && ostream.tellp() < 0)
        {
            ostream << '\0';
            ostream.seekp(0);
        }

        tiff_stream_data* data = new tiff_stream_data {.ostream = ostream, .start_pos = ostream.tellp()};

        // Open for writing.
        TIFF* tiff = TIFFClientOpen(
            "stream",
            "wm",
            reinterpret_cast<thandle_t>(data),
            tiffReadProc,
            tiffWriteProc,
            tiffSeekProc,
            tiffCloseProc,
            tiffSizeProc,
            tiff::mapProc,
            tiff::unmapProc
        );

        return tiff;
    }

    //------------------------------------------------------------------------------

    inline static tmsize_t tiffReadProc(thandle_t, void*, tmsize_t)
    {
        return -1;
    }

    //------------------------------------------------------------------------------

    inline static tmsize_t tiffWriteProc(thandle_t fd, void* buf, tmsize_t size)
    {
        tiff_stream_data* const data = reinterpret_cast<tiff_stream_data*>(fd);
        const auto pos               = data->ostream.tellp();

        // Verify that type does not overflow.
        std::streamsize request_size = size;
        if(tmsize_t(request_size) != size)
        {
            return -1;
        }

        data->ostream.write(reinterpret_cast<const char*>(buf), request_size);

        return tmsize_t(data->ostream.tellp() - pos);
    }

    //------------------------------------------------------------------------------

    inline static toff_t tiffSeekProc(thandle_t fd, toff_t off, int whence)
    {
        tiff_stream_data* const data = reinterpret_cast<tiff_stream_data*>(fd);

        // if the stream has already failed, don't do anything
        if(data->ostream.fail())
        {
            return toff_t(-1);
        }

        switch(whence)
        {
            case SEEK_SET:
            {
                // Compute 64-bit offset
                const std::uint64_t new_offset = std::uint64_t(data->start_pos) + off;

                // Verify that value does not overflow
                const std::ios::off_type offset = std::ios::off_type(new_offset);
                if(std::uint64_t(offset) != new_offset)
                {
                    return std::uint64_t(-1);
                }

                data->ostream.seekp(offset, std::ios::beg);
                break;
            }

            case SEEK_CUR:
            {
                // Verify that value does not overflow
                const std::ios::off_type offset = std::ios::off_type(off);
                if(std::uint64_t(offset) != off)
                {
                    return std::uint64_t(-1);
                }

                data->ostream.seekp(offset, std::ios::cur);
                break;
            }

            case SEEK_END:
            {
                // Verify that value does not overflow
                const std::ios::off_type offset = std::ios::off_type(off);
                if(std::uint64_t(offset) != off)
                {
                    return std::uint64_t(-1);
                }

                data->ostream.seekp(offset, std::ios::end);
                break;
            }
        }

        // Attempt to workaround problems with seeking past the end of the
        // stream.  ofstream doesn't have a problem with this but
        // ostrstream/ostringstream does. In that situation, add intermediate
        // '\0' characters.
        if(data->ostream.fail())
        {
            const auto old_state      = data->ostream.rdstate();
            std::ios::pos_type origin = 0;

            // reset the fail bit or else tellp() won't work below
            data->ostream.clear(data->ostream.rdstate() & ~std::ios::failbit);

            switch(whence)
            {
                case SEEK_SET:
                default:
                    origin = data->start_pos;
                    break;

                case SEEK_CUR:
                    origin = data->ostream.tellp();
                    break;

                case SEEK_END:
                    data->ostream.seekp(0, std::ios::end);
                    origin = data->ostream.tellp();
                    break;
            }

            // restore original stream state
            data->ostream.clear(old_state);

            const std::uint64_t new_offset = std::uint64_t(origin) + off;

            // only do something if desired seek position is valid
            if(new_offset > std::uint64_t(data->start_pos))
            {
                // clear the fail bit
                data->ostream.clear(data->ostream.rdstate() & ~std::ios::failbit);

                // extend the stream to the expected size
                data->ostream.seekp(0, std::ios::end);
                const std::uint64_t num_fill = new_offset - std::uint64_t(data->ostream.tellp());
                for(std::uint64_t i = 0 ; i < num_fill ; ++i)
                {
                    data->ostream.put('\0');
                }

                // retry the seek
                data->ostream.seekp(std::ios::off_type(new_offset), std::ios::beg);
            }
        }

        return toff_t(data->ostream.tellp());
    }

    //------------------------------------------------------------------------------

    inline static int tiffCloseProc(thandle_t fd)
    {
        // Our stream was not allocated by us, so it shouldn't be closed by us.
        delete reinterpret_cast<tiff_stream_data*>(fd);
        return 0;
    }

    //------------------------------------------------------------------------------

    inline static toff_t tiffSizeProc(thandle_t fd)
    {
        tiff_stream_data* const data = reinterpret_cast<tiff_stream_data*>(fd);
        const auto initial_pos       = data->ostream.tellp();

        data->ostream.seekp(0, std::ios::end);
        const auto size = data->ostream.tellp();
        data->ostream.seekp(initial_pos);

        return toff_t(size);
    }

    /// @}

    std::vector<std::uint8_t> m_row_buffer;

public:

    bool m_valid {true};
    static constexpr std::string_view m_name {"LibTIFFWriter"};
};

} // namespace sight::io::bitmap::detail
