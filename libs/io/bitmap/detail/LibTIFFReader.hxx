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
#include "ReaderImpl.hxx"

// cspell:ignore nvjpeg NOLINTNEXTLINE TIFFTAG IMAGEWIDTH IMAGELENGTH BITSPERSAMPLE SAMPLESPERPIXEL MINISBLACK
// cspell:ignore PLANARCONFIG TOPLEFT ROWSPERSTRIP Scanline XRESOLUTION YRESOLUTION thandle SAMPLEFORMAT
// cspell:ignore PACKBITS EXTRASAMPLE RESOLUTIONUNIT RESUNIT EXTRASAMPLES tiffio tmsize

namespace sight::io::bitmap::detail
{

// NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
#define CHECK_TIFF(func) SIGHT_THROW_IF("The function " #func " failed.", (func) == 0)

class LibTIFFReader final
{
public:

    /// Delete copy constructors and assignment operators
    LibTIFFReader(const LibTIFFReader&)            = delete;
    LibTIFFReader& operator=(const LibTIFFReader&) = delete;

    /// Constructor
    inline LibTIFFReader() noexcept = default;

    /// Destructor
    inline ~LibTIFFReader() noexcept = default;

    /// Reading
    inline void read(data::image& image, std::istream& istream, Flag /*flag*/)
    {
        // Create an RAII to be sure everything is cleaned at exit
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
        } keeper;

        // Open the tiff file for reading
        keeper.m_tiff = tiffStreamOpen(istream);
        SIGHT_THROW_IF("TIFFOpen() failed.", keeper.m_tiff == nullptr);

        // Get the image size and format
        std::uint32_t width              = 0;
        std::uint32_t height             = 0;
        std::uint16_t samples_per_pixels = 0;
        std::uint16_t sample_format      = 0;
        std::uint16_t bits_per_sample    = 0;
        std::uint16_t photometric        = 0;
        std::uint16_t planar_config      = 0;

        CHECK_TIFF(TIFFGetField(keeper.m_tiff, TIFFTAG_IMAGEWIDTH, &width));
        CHECK_TIFF(TIFFGetField(keeper.m_tiff, TIFFTAG_IMAGELENGTH, &height));
        CHECK_TIFF(TIFFGetField(keeper.m_tiff, TIFFTAG_SAMPLESPERPIXEL, &samples_per_pixels));
        CHECK_TIFF(TIFFGetField(keeper.m_tiff, TIFFTAG_BITSPERSAMPLE, &bits_per_sample));
        CHECK_TIFF(TIFFGetField(keeper.m_tiff, TIFFTAG_PHOTOMETRIC, &photometric));
        CHECK_TIFF(TIFFGetField(keeper.m_tiff, TIFFTAG_PLANARCONFIG, &planar_config));

        // Sample format may be not present
        if(TIFFGetField(keeper.m_tiff, TIFFTAG_SAMPLEFORMAT, &sample_format) != 1)
        {
            sample_format = SAMPLEFORMAT_UINT;
        }

        // Depending of the format, we use scanline approach if possible, or libtiff automatic rgba conversion if we are
        // not able to interpret pixels data
        if(samples_per_pixels > 4
           || (sample_format != SAMPLEFORMAT_UINT
               && sample_format != SAMPLEFORMAT_INT
               && sample_format != SAMPLEFORMAT_IEEEFP)
           || (photometric != PHOTOMETRIC_MINISBLACK
               && photometric != PHOTOMETRIC_RGB)
           || (planar_config != PLANARCONFIG_CONTIG))
        {
            // TIFFReadRGBAImage approach
            // Allocate destination image
            image.resize(
                {width, height, 0},
                sample_format == SAMPLEFORMAT_INT ? core::type::INT8 : core::type::UINT8,
                data::image::PixelFormat::RGBA
            );

            CHECK_TIFF(
                TIFFReadRGBAImage(
                    keeper.m_tiff,
                    width,
                    height,
                    reinterpret_cast<std::uint32_t*>(image.buffer()),
                    0
                )
            );
        }
        else
        {
            // Scanline approach
            // Convert bits_per_sample to Sight format
            const auto component_type =
                [&]
                {
                    if(sample_format == SAMPLEFORMAT_IEEEFP)
                    {
                        switch(bits_per_sample)
                        {
                            case 32:
                                return core::type::FLOAT;

                            case 64:
                                return core::type::DOUBLE;

                            default:
                                SIGHT_THROW("Unsupported bit depth for float format: '" << bits_per_sample << "'");
                        }
                    }
                    else if(sample_format == SAMPLEFORMAT_UINT)
                    {
                        switch(bits_per_sample)
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
                                SIGHT_THROW("Unsupported bits per sample: '" << bits_per_sample << "'");
                        }
                    }
                    else if(sample_format == SAMPLEFORMAT_INT)
                    {
                        switch(bits_per_sample)
                        {
                            case 8:
                                return core::type::INT8;

                            case 16:
                                return core::type::INT16;

                            case 32:
                                return core::type::INT32;

                            case 64:
                                return core::type::INT64;

                            default:
                                SIGHT_THROW("Unsupported bits per sample: '" << bits_per_sample << "'");
                        }
                    }
                    else
                    {
                        SIGHT_THROW("Unsupported sample format: '" << sample_format << "'");
                    }
                }();

            // Convert photometric to Sight format. Except PHOTOMETRIC_MINISBLACK and PHOTOMETRIC_RGB all others are
            // decoded with TIFFReadRGBAImage, so we only look at sample per pixels
            const auto pixel_format =
                [&]
                {
                    switch(samples_per_pixels)
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
                            SIGHT_THROW("Unsupported sample per pixels: '" << samples_per_pixels << "'");
                    }
                }();

            // Allocate destination image
            image.resize({width, height, 0}, component_type, pixel_format);

            for(std::uint32_t row = 0 ; row < height ; ++row)
            {
                CHECK_TIFF(TIFFReadScanline(keeper.m_tiff, image.getPixel(row * width), row));
            }
        }
    }

private:

    /// TIFF c++ API (tiffio.hxx and tif_stream.cxx) is not available on Windows. We simply recreate it
    /// @{
    struct tiff_stream_data
    {
        std::istream& istream;
        std::ios::pos_type start_pos;
    };

    //------------------------------------------------------------------------------

    inline static TIFF* tiffStreamOpen(std::istream& istream)
    {
        tiff_stream_data* const data = new tiff_stream_data {.istream = istream, .start_pos = istream.tellg()};

        // Open for reading.
        TIFF* tiff = TIFFClientOpen(
            "istream",
            "r",
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

    inline static tmsize_t tiffReadProc(thandle_t fd, void* buff, tmsize_t size)
    {
        tiff_stream_data* const data = reinterpret_cast<tiff_stream_data*>(fd);
        data->istream.read(reinterpret_cast<char*>(buff), size);
        return data->istream.gcount();
    }

    //------------------------------------------------------------------------------

    inline static tmsize_t tiffWriteProc(thandle_t, void*, tmsize_t)
    {
        return -1;
    }

    //------------------------------------------------------------------------------

    inline static toff_t tiffSeekProc(thandle_t fd, toff_t off, int whence)
    {
        tiff_stream_data* const data = reinterpret_cast<tiff_stream_data*>(fd);

        switch(whence)
        {
            case SEEK_SET:
                data->istream.seekg(data->start_pos + std::ios::off_type(off), std::ios::beg);
                break;

            case SEEK_CUR:
                data->istream.seekg(std::streamoff(off), std::ios::cur);
                break;

            case SEEK_END:
                data->istream.seekg(std::streamoff(off), std::ios::end);
                break;
        }

        return toff_t(data->istream.tellg() - data->start_pos);
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

        const auto initial_pos = data->istream.tellg();

        data->istream.seekg(0, std::ios::end);
        const auto size = data->istream.tellg();
        data->istream.seekg(initial_pos);

        return toff_t(size);
    }

    /// @}

public:

    bool m_valid {true};
    static constexpr std::string_view m_name {"LibTIFFReader"};
};

} // namespace sight::io::bitmap::detail
