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

#include <openjpeg.h>

#include <type_traits>

// cspell:ignore nvjpeg NOLINTNEXTLINE numresolution cblockw cblockh sgnd CLRSPC cparameters

namespace sight::io::bitmap::detail
{

// NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
#define CHECK_OPJ(func) SIGHT_THROW_IF("The function " #func " failed.", (func) == OPJ_FALSE)

template<typename T, typename = int>
struct has_r : std::false_type {};

template<typename T>
struct has_r<T, decltype((void) T::r, 0)>: std::true_type {};

template<typename T, typename = int>
struct has_g : std::false_type {};

template<typename T>
struct has_g<T, decltype((void) T::g, 0)>: std::true_type {};

template<typename T, typename = int>
struct has_b : std::false_type {};

template<typename T>
struct has_b<T, decltype((void) T::b, 0)>: std::true_type {};

template<typename T, typename = int>
struct has_alpha : std::false_type {};

template<typename T>
struct has_alpha<T, decltype((void) T::a, 0)>: std::true_type {};

class OpenJPEGWriter final
{
public:

    /// Delete copy constructors and assignment operators
    OpenJPEGWriter(const OpenJPEGWriter&)            = delete;
    OpenJPEGWriter& operator=(const OpenJPEGWriter&) = delete;

    /// Constructor
    inline OpenJPEGWriter() noexcept
    {
        try
        {
            // Setup default parameters
            opj_set_default_encoder_parameters(&m_parameters);

            // 32 block size is not faster..
            m_parameters.cblockw_init = 64;
            m_parameters.cblockh_init = 64;

            // Lossless by default;
            m_parameters.irreversible = 0;

            m_valid = true;
        }
        catch(const std::exception& e)
        {
            // Log error
            SIGHT_ERROR(e.what());
        }
    }

    /// Destructor
    inline ~OpenJPEGWriter() noexcept = default;

    /// Writing
    inline void write(const data::Image& image, std::ostream& ostream, Writer::Mode, Flag flag = Flag::NONE)
    {
        // Create codec
        /// @warning You cannot reuse the opj_codec, the opj_stream, or the opj_image.
        /// @warning Everything must be re-created and re-destroyed in one shot.
        /// @warning Doing otherwise leads to strange memory corruption, although most image codecs allows you to do so.

        const auto& image_type = image.getType();
        SIGHT_THROW_IF(
            m_name << " - Unsupported image type: " << image_type,
            image_type != core::Type::INT8
            && image_type != core::Type::UINT8
            && image_type != core::Type::INT16
            && image_type != core::Type::UINT16
            && image_type != core::Type::INT32
            && image_type != core::Type::UINT32
        );

        const auto& pixel_format = image.getPixelFormat();
        SIGHT_THROW_IF(
            m_name << " - Unsupported image format: " << pixel_format,
            pixel_format != data::Image::PixelFormat::GRAY_SCALE
            && pixel_format != data::Image::PixelFormat::RGB
            && pixel_format != data::Image::PixelFormat::RGBA
            && pixel_format != data::Image::PixelFormat::BGR
            && pixel_format != data::Image::PixelFormat::BGRA
        );

        // Create an RAII to be sure everything is cleaned at exit
        struct Keeper final
        {
            inline Keeper() noexcept = default;

            inline ~Keeper()
            {
                // Cleanup
                if(m_image)
                {
                    opj_image_destroy(m_image);
                    m_image = nullptr;
                }

                if(m_stream)
                {
                    opj_stream_destroy(m_stream);
                    m_stream = nullptr;
                }

                if(m_codec)
                {
                    opj_destroy_codec(m_codec);
                    m_codec = nullptr;
                }
            }

            opj_codec_t* m_codec {nullptr};
            opj_stream_t* m_stream {nullptr};
            opj_image_t* m_image {nullptr};
        } keeper;

        CHECK_OPJ(
            keeper.m_codec = opj_create_compress(
                flag == Flag::J2K_STREAM ? OPJ_CODEC_J2K : OPJ_CODEC_JP2
            )
        );

        // Install info, warning, error handlers
        CHECK_OPJ(opj_set_info_handler(keeper.m_codec, infoCallback, nullptr));
        CHECK_OPJ(opj_set_warning_handler(keeper.m_codec, warningCallback, nullptr));
        CHECK_OPJ(opj_set_warning_handler(keeper.m_codec, errorCallback, nullptr));

        // Create output stream (1MB buffer by default)
        CHECK_OPJ(keeper.m_stream = opj_stream_create(OPJ_J2K_STREAM_CHUNK_SIZE, OPJ_FALSE));

        // Setup OPJ user stream
        opj_stream_set_user_data(keeper.m_stream, &ostream, freeCallback);

        // Setup stream callback
        opj_stream_set_write_function(keeper.m_stream, writeCallback);
        opj_stream_set_skip_function(keeper.m_stream, skipCallback);
        opj_stream_set_seek_function(keeper.m_stream, seekCallback);

        // Adjust parameters
        const auto& sizes       = image.getSize();
        const OPJ_UINT32 width  = OPJ_UINT32(sizes[0]);
        const OPJ_UINT32 height = OPJ_UINT32(sizes[1]);

        // Format can .jp2 or .j2k
        m_parameters.cod_format = flag == Flag::J2K_STREAM ? 0 : 1;

        // Wavelet decomposition levels. 6-5 Seems to be a good default, but should be multiple of block size
        m_parameters.numresolution = std::min(
            6,
            std::min(int(width) / m_parameters.cblockw_init, int(height) / m_parameters.cblockh_init)
        );

        const OPJ_UINT32 num_components = OPJ_UINT32(image.numComponents());
        m_parameters.tcp_mct = num_components == 1 ? 0 : 1;

        // Build the component param array
        std::vector<opj_image_cmptparm_t> component_params(num_components);

        const OPJ_UINT32 prec = OPJ_UINT32(image_type.size() * 8);
        const OPJ_UINT32 sgnd = image_type.isSigned() ? 1 : 0;

        std::ranges::fill(
            component_params,
            opj_image_cmptparm_t {
                .dx   = OPJ_UINT32(m_parameters.subsampling_dx),
                .dy   = OPJ_UINT32(m_parameters.subsampling_dy),
                .w    = width,
                .h    = height,
                .x0   = 0,
                .y0   = 0,
                .prec = prec,
                .bpp  = prec,
                .sgnd = sgnd
            });

        // Guess the color space to use
        const OPJ_COLOR_SPACE color_space = pixel_format == data::Image::GRAY_SCALE
                                            ? OPJ_CLRSPC_GRAY
                                            : OPJ_CLRSPC_SRGB;

        // Create the OPJ image
        keeper.m_image = opj_image_create(num_components, component_params.data(), color_space);

        // Set image offset and reference grid
        keeper.m_image->x0 = OPJ_UINT32(m_parameters.image_offset_x0);
        keeper.m_image->y0 = OPJ_UINT32(m_parameters.image_offset_y0);

        keeper.m_image->x1 = OPJ_UINT32(
            keeper.m_image->x0 + (width - 1) * OPJ_UINT32(m_parameters.subsampling_dx) + 1
        );

        keeper.m_image->y1 = OPJ_UINT32(
            keeper.m_image->y0 + (height - 1) * OPJ_UINT32(m_parameters.subsampling_dy) + 1
        );

        // Convert Sight interlaced pixels to planar openJPEG pixels
        switch(prec)
        {
            case 8:
                if(image_type.isSigned())
                {
                    toOpenJPEG<std::int8_t>(image, *keeper.m_image);
                }
                else
                {
                    toOpenJPEG<std::uint8_t>(image, *keeper.m_image);
                }

                break;

            case 16:
                if(image_type.isSigned())
                {
                    toOpenJPEG<std::int16_t>(image, *keeper.m_image);
                }
                else
                {
                    toOpenJPEG<std::uint16_t>(image, *keeper.m_image);
                }

                break;

            case 32:
                if(image_type.isSigned())
                {
                    toOpenJPEG<std::uint32_t>(image, *keeper.m_image);
                }
                else
                {
                    toOpenJPEG<std::uint32_t>(image, *keeper.m_image);
                }

                break;

            default:
                SIGHT_THROW(m_name << " - Unsupported precision.");
        }

        // Setup the encoder
        CHECK_OPJ(opj_setup_encoder(keeper.m_codec, &m_parameters, keeper.m_image));

        // Start compress
        CHECK_OPJ(opj_start_compress(keeper.m_codec, keeper.m_image, keeper.m_stream));

        // Encode
        CHECK_OPJ(opj_encode(keeper.m_codec, keeper.m_stream));

        // End compress
        CHECK_OPJ(opj_end_compress(keeper.m_codec, keeper.m_stream));
    }

private:

    //------------------------------------------------------------------------------

    inline static void infoCallback(const char*, void*)
    {
        // Too much noise for regular "info"
        // SIGHT_DEBUG(msg);
    }

    //------------------------------------------------------------------------------

    inline static void warningCallback(const char* msg, void*)
    {
        SIGHT_WARN(msg);
    }

    //------------------------------------------------------------------------------

    inline static void errorCallback(const char* msg, void*)
    {
        SIGHT_THROW(msg);
    }

    //------------------------------------------------------------------------------

    inline static OPJ_SIZE_T writeCallback(void* p_buffer, OPJ_SIZE_T p_nb_bytes, void* p_user_data)
    {
        if(p_user_data != nullptr)
        {
            auto* ostream = reinterpret_cast<std::ostream*>(p_user_data);
            ostream->write(reinterpret_cast<char*>(p_buffer), std::streamsize(p_nb_bytes));
            return p_nb_bytes;
        }

        return 0;
    }

    //------------------------------------------------------------------------------

    inline static OPJ_OFF_T skipCallback(OPJ_OFF_T p_nb_bytes, void* p_user_data)
    {
        if(p_user_data != nullptr)
        {
            auto* ostream = reinterpret_cast<std::ostream*>(p_user_data);
            ostream->seekp(p_nb_bytes, std::ios_base::cur);
            return p_nb_bytes;
        }

        return 0;
    }

    //------------------------------------------------------------------------------

    inline static OPJ_BOOL seekCallback(OPJ_OFF_T p_nb_bytes, void* p_user_data)
    {
        if(p_user_data != nullptr)
        {
            auto* ostream = reinterpret_cast<std::ostream*>(p_user_data);
            ostream->seekp(p_nb_bytes, std::ios_base::beg);
            return OPJ_TRUE;
        }

        return OPJ_FALSE;
    }

    //------------------------------------------------------------------------------

    inline static void freeCallback(void* /*p_user_data*/)
    {
    }

    //------------------------------------------------------------------------------

    template<typename T>
    inline static void toOpenJPEG(const data::Image& image, opj_image_t& opj_image)
    {
        switch(image.getPixelFormat())
        {
            case data::Image::GRAY_SCALE:
            {
                struct Pixel
                {
                    T a;
                };

                toOpenJPEGPixels<Pixel>(image, opj_image);
                break;
            }

            case data::Image::RGB:
            {
                struct Pixel
                {
                    T r;
                    T g;
                    T b;
                };

                toOpenJPEGPixels<Pixel>(image, opj_image);
                break;
            }

            case data::Image::RGBA:
            {
                struct Pixel
                {
                    T r;
                    T g;
                    T b;
                    T a;
                };

                toOpenJPEGPixels<Pixel>(image, opj_image);
                break;
            }

            case data::Image::BGR:
            {
                struct Pixel
                {
                    T b;
                    T g;
                    T r;
                };

                toOpenJPEGPixels<Pixel>(image, opj_image);
                break;
            }

            case data::Image::BGRA:
            {
                struct Pixel
                {
                    T b;
                    T g;
                    T r;
                    T a;
                };

                toOpenJPEGPixels<Pixel>(image, opj_image);
                break;
            }

            default:
                SIGHT_THROW("Unsupported pixel format");
        }
    }

    //------------------------------------------------------------------------------

    template<typename P>
    inline static void toOpenJPEGPixels(const data::Image& image, opj_image_t& opj_image)
    {
        const auto& sizes = image.getSize();

        auto pixel_it        = image.cbegin<P>();
        const auto pixel_end = image.cend<P>();

        for(std::size_t i = 0, end = sizes[0] * sizes[1] ; i < end && pixel_it != pixel_end ; ++pixel_it)
        {
            std::size_t c = 0;

            if constexpr(has_r<P>::value)
            {
                opj_image.comps[c++].data[i] = OPJ_INT32(pixel_it->r);
            }

            if constexpr(has_g<P>::value)
            {
                opj_image.comps[c++].data[i] = OPJ_INT32(pixel_it->g);
            }

            if constexpr(has_b<P>::value)
            {
                opj_image.comps[c++].data[i] = OPJ_INT32(pixel_it->b);
            }

            if constexpr(has_alpha<P>::value)
            {
                opj_image.comps[c].data[i] = OPJ_INT32(pixel_it->a);
            }

            ++i;
        }
    }

    opj_cparameters_t m_parameters {};

public:

    bool m_valid {false};
    static constexpr std::string_view m_name {"OpenJPEGWriter"};
};

} // namespace sight::io::bitmap::detail
