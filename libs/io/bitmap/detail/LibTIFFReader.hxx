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

class lib_tiff_reader final
{
public:

    /// Delete copy constructors and assignment operators
    lib_tiff_reader(const lib_tiff_reader&)            = delete;
    lib_tiff_reader& operator=(const lib_tiff_reader&) = delete;

    /// Constructor
    inline lib_tiff_reader() noexcept = default;

    /// Destructor
    inline ~lib_tiff_reader() noexcept = default;

    /// Reading
    inline void read(data::image& _image, std::istream& _istream, flag /*flag*/)
    {
        // Create an RAII to be sure everything is cleaned at exit
        struct keeper final
        {
            inline ~keeper()
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
        keeper.m_tiff = tiff_stream_open(_istream);
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
            _image.resize(
                {width, height, 0},
                sample_format == SAMPLEFORMAT_INT ? core::type::INT8 : core::type::UINT8,
                data::image::pixel_format::rgba
            );

            CHECK_TIFF(
                TIFFReadRGBAImage(
                    keeper.m_tiff,
                    width,
                    height,
                    reinterpret_cast<std::uint32_t*>(_image.buffer()),
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
                            return data::image::pixel_format::gray_scale;

                        case 2:
                            return data::image::pixel_format::rg;

                        case 3:
                            return data::image::pixel_format::rgb;

                        case 4:
                            return data::image::pixel_format::rgba;

                        default:
                            SIGHT_THROW("Unsupported sample per pixels: '" << samples_per_pixels << "'");
                    }
                }();

            // Allocate destination image
            _image.resize({width, height, 0}, component_type, pixel_format);

            for(std::uint32_t row = 0 ; row < height ; ++row)
            {
                CHECK_TIFF(TIFFReadScanline(keeper.m_tiff, _image.get_pixel(row * width), row));
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

    inline static TIFF* tiff_stream_open(std::istream& _istream)
    {
        tiff_stream_data* const data = new tiff_stream_data {.istream = _istream, .start_pos = _istream.tellg()};

        // Open for reading.
        TIFF* tiff = TIFFClientOpen(
            "istream",
            "r",
            reinterpret_cast<thandle_t>(data),
            tiff_read_proc,
            tiff_write_proc,
            tiff_seek_proc,
            tiff_close_proc,
            tiff_size_proc,
            tiff::map_proc,
            tiff::unmap_proc
        );

        return tiff;
    }

    //------------------------------------------------------------------------------

    inline static tmsize_t tiff_read_proc(thandle_t _fd, void* _buff, tmsize_t _size)
    {
        tiff_stream_data* const data = reinterpret_cast<tiff_stream_data*>(_fd);
        data->istream.read(reinterpret_cast<char*>(_buff), _size);
        return data->istream.gcount();
    }

    //------------------------------------------------------------------------------

    inline static tmsize_t tiff_write_proc(thandle_t, void*, tmsize_t)
    {
        return -1;
    }

    //------------------------------------------------------------------------------

    inline static toff_t tiff_seek_proc(thandle_t _fd, toff_t _off, int _whence)
    {
        tiff_stream_data* const data = reinterpret_cast<tiff_stream_data*>(_fd);

        switch(_whence)
        {
            case SEEK_SET:
                data->istream.seekg(data->start_pos + std::ios::off_type(_off), std::ios::beg);
                break;

            case SEEK_CUR:
                data->istream.seekg(std::streamoff(_off), std::ios::cur);
                break;

            case SEEK_END:
                data->istream.seekg(std::streamoff(_off), std::ios::end);
                break;
        }

        return toff_t(data->istream.tellg() - data->start_pos);
    }

    //------------------------------------------------------------------------------

    inline static int tiff_close_proc(thandle_t _fd)
    {
        // Our stream was not allocated by us, so it shouldn't be closed by us.
        delete reinterpret_cast<tiff_stream_data*>(_fd);
        return 0;
    }

    //------------------------------------------------------------------------------

    inline static toff_t tiff_size_proc(thandle_t _fd)
    {
        tiff_stream_data* const data = reinterpret_cast<tiff_stream_data*>(_fd);

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
