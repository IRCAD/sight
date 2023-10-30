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

class open_jpeg_writer final
{
public:

    /// Delete copy constructors and assignment operators
    open_jpeg_writer(const open_jpeg_writer&)            = delete;
    open_jpeg_writer& operator=(const open_jpeg_writer&) = delete;

    /// Constructor
    inline open_jpeg_writer() noexcept
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
    inline ~open_jpeg_writer() noexcept = default;

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
        const data::image& _image,
        O& _output,
        writer::mode,
        flag _flag = flag::none
)
    {
        // Create codec
        /// @warning You cannot reuse the opj_codec, the opj_stream, or the opj_image.
        /// @warning Everything must be re-created and re-destroyed in one shot.
        /// @warning Doing otherwise leads to strange memory corruption, although most image codecs allows you to do so.

        const auto& image_type = _image.type();
        SIGHT_THROW_IF(
            m_name << " - Unsupported image type: " << image_type,
            image_type != core::type::INT8
            && image_type != core::type::UINT8
            && image_type != core::type::INT16
            && image_type != core::type::UINT16
            && image_type != core::type::INT32
            && image_type != core::type::UINT32
        );

        const auto& pixel_format = _image.pixel_format();
        SIGHT_THROW_IF(
            m_name << " - Unsupported image format: " << pixel_format,
            pixel_format != data::image::pixel_format::gray_scale
            && pixel_format != data::image::pixel_format::rgb
            && pixel_format != data::image::pixel_format::rgba
            && pixel_format != data::image::pixel_format::bgr
            && pixel_format != data::image::pixel_format::bgra
        );

        // Create an RAII to be sure everything is cleaned at exit
        struct keeper final
        {
            inline keeper() noexcept = default;

            inline ~keeper()
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

            // For buffer mode
            std::stringstream m_buffer;
        } keeper;

        CHECK_OPJ(
            keeper.m_codec = opj_create_compress(
                _flag == flag::j2_k_stream ? OPJ_CODEC_J2K : OPJ_CODEC_JP2
            )
        );

        // Install info, warning, error handlers
        CHECK_OPJ(opj_set_info_handler(keeper.m_codec, info_callback, nullptr));
        CHECK_OPJ(opj_set_warning_handler(keeper.m_codec, warning_callback, nullptr));
        CHECK_OPJ(opj_set_warning_handler(keeper.m_codec, error_callback, nullptr));

        // Create output stream (10 MB buffer by default)
        CHECK_OPJ(keeper.m_stream = opj_stream_create(OPJ_J2K_STREAM_CHUNK_SIZE * 10, OPJ_FALSE));

        // Setup OPJ user stream
        if constexpr(std::is_base_of_v<std::ostream, O>)
        {
            opj_stream_set_user_data(keeper.m_stream, &_output, free_callback);
        }
        else
        {
            opj_stream_set_user_data(keeper.m_stream, &keeper.m_buffer, free_callback);
        }

        // Setup stream callback
        opj_stream_set_write_function(keeper.m_stream, write_callback);
        opj_stream_set_skip_function(keeper.m_stream, skip_callback);
        opj_stream_set_seek_function(keeper.m_stream, seek_callback);

        // Adjust parameters
        const auto& sizes       = _image.size();
        const OPJ_UINT32 width  = OPJ_UINT32(sizes[0]);
        const OPJ_UINT32 height = OPJ_UINT32(sizes[1]);

        // Format can .jp2 or .j2k
        m_parameters.cod_format = _flag == flag::j2_k_stream ? 0 : 1;

        // Wavelet decomposition levels. 6-5 Seems to be a good default, but should be multiple of block size
        m_parameters.numresolution = std::min(
            6,
            std::min(int(width) / m_parameters.cblockw_init, int(height) / m_parameters.cblockh_init)
        );

        const OPJ_UINT32 num_components = OPJ_UINT32(_image.num_components());
        m_parameters.tcp_mct = num_components == 1 ? 0 : 1;

        // Build the component param array
        std::vector<opj_image_cmptparm_t> component_params(num_components);

        const OPJ_UINT32 prec = OPJ_UINT32(image_type.size() * 8);
        const OPJ_UINT32 sgnd = image_type.is_signed() ? 1 : 0;

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
        const OPJ_COLOR_SPACE color_space = pixel_format == data::image::gray_scale
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
                if(image_type.is_signed())
                {
                    to_open_jpeg<std::int8_t>(_image, *keeper.m_image);
                }
                else
                {
                    to_open_jpeg<std::uint8_t>(_image, *keeper.m_image);
                }

                break;

            case 16:
                if(image_type.is_signed())
                {
                    to_open_jpeg<std::int16_t>(_image, *keeper.m_image);
                }
                else
                {
                    to_open_jpeg<std::uint16_t>(_image, *keeper.m_image);
                }

                break;

            case 32:
                if(image_type.is_signed())
                {
                    to_open_jpeg<std::uint32_t>(_image, *keeper.m_image);
                }
                else
                {
                    to_open_jpeg<std::uint32_t>(_image, *keeper.m_image);
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

        if constexpr(std::is_same_v<std::uint8_t*, O>
                     || std::is_same_v<std::uint8_t**, O>
                     || std::is_same_v<std::vector<std::uint8_t>, O>)
        {
            // Zero copy string conversion, work only with C++20
            const std::string output_buffer = std::move(keeper.m_buffer).str();
            const auto output_buffer_size   = output_buffer.size();

            if constexpr(std::is_same_v<std::uint8_t**, O>)
            {
                (*_output) = new std::uint8_t[output_buffer_size];
                std::memcpy((*_output), output_buffer.data(), output_buffer_size);
            }
            else if constexpr(std::is_same_v<std::uint8_t*, O>)
            {
                std::memcpy(_output, output_buffer.data(), output_buffer_size);
            }
            else if constexpr(std::is_same_v<std::vector<std::uint8_t>, O>)
            {
                if(_output.size() < output_buffer_size)
                {
                    _output.resize(output_buffer_size);
                }

                std::memcpy(_output.data(), output_buffer.data(), output_buffer_size);
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

    //------------------------------------------------------------------------------

    inline static void info_callback(const char*, void*)
    {
        // Too much noise for regular "info"
        // SIGHT_DEBUG(msg);
    }

    //------------------------------------------------------------------------------

    inline static void warning_callback(const char* _msg, void*)
    {
        SIGHT_WARN(_msg);
    }

    //------------------------------------------------------------------------------

    inline static void error_callback(const char* _msg, void*)
    {
        SIGHT_THROW(_msg);
    }

    //------------------------------------------------------------------------------

    inline static OPJ_SIZE_T write_callback(void* _p_buffer, OPJ_SIZE_T _p_nb_bytes, void* _p_user_data)
    {
        if(_p_user_data != nullptr)
        {
            auto* ostream = reinterpret_cast<std::ostream*>(_p_user_data);
            ostream->write(reinterpret_cast<char*>(_p_buffer), std::streamsize(_p_nb_bytes));
            return _p_nb_bytes;
        }

        return 0;
    }

    //------------------------------------------------------------------------------

    inline static OPJ_OFF_T skip_callback(OPJ_OFF_T _p_nb_bytes, void* _p_user_data)
    {
        if(_p_user_data != nullptr)
        {
            auto* ostream = reinterpret_cast<std::ostream*>(_p_user_data);
            ostream->seekp(_p_nb_bytes, std::ios_base::cur);
            return _p_nb_bytes;
        }

        return 0;
    }

    //------------------------------------------------------------------------------

    inline static OPJ_BOOL seek_callback(OPJ_OFF_T _p_nb_bytes, void* _p_user_data)
    {
        if(_p_user_data != nullptr)
        {
            auto* ostream = reinterpret_cast<std::ostream*>(_p_user_data);
            ostream->seekp(_p_nb_bytes, std::ios_base::beg);
            return OPJ_TRUE;
        }

        return OPJ_FALSE;
    }

    //------------------------------------------------------------------------------

    inline static void free_callback(void* /*p_user_data*/)
    {
    }

    //------------------------------------------------------------------------------

    template<typename T>
    inline static void to_open_jpeg(const data::image& _image, opj_image_t& _opj_image)
    {
        switch(_image.pixel_format())
        {
            case data::image::gray_scale:
            {
                struct pixel
                {
                    T a;
                };

                to_open_jpeg_pixels<pixel>(_image, _opj_image);
                break;
            }

            case data::image::rgb:
            {
                struct pixel
                {
                    T r;
                    T g;
                    T b;
                };

                to_open_jpeg_pixels<pixel>(_image, _opj_image);
                break;
            }

            case data::image::rgba:
            {
                struct pixel
                {
                    T r;
                    T g;
                    T b;
                    T a;
                };

                to_open_jpeg_pixels<pixel>(_image, _opj_image);
                break;
            }

            case data::image::bgr:
            {
                struct pixel
                {
                    T b;
                    T g;
                    T r;
                };

                to_open_jpeg_pixels<pixel>(_image, _opj_image);
                break;
            }

            case data::image::bgra:
            {
                struct pixel
                {
                    T b;
                    T g;
                    T r;
                    T a;
                };

                to_open_jpeg_pixels<pixel>(_image, _opj_image);
                break;
            }

            default:
                SIGHT_THROW("Unsupported pixel format");
        }
    }

    //------------------------------------------------------------------------------

    template<typename P>
    inline static void to_open_jpeg_pixels(const data::image& _image, opj_image_t& _opj_image)
    {
        const auto& sizes = _image.size();

        auto pixel_it        = _image.cbegin<P>();
        const auto pixel_end = _image.cend<P>();

        for(std::size_t i = 0, end = sizes[0] * sizes[1] ; i < end && pixel_it != pixel_end ; ++pixel_it)
        {
            std::size_t c = 0;

            if constexpr(has_r<P>::value)
            {
                _opj_image.comps[c++].data[i] = OPJ_INT32(pixel_it->r);
            }

            if constexpr(has_g<P>::value)
            {
                _opj_image.comps[c++].data[i] = OPJ_INT32(pixel_it->g);
            }

            if constexpr(has_b<P>::value)
            {
                _opj_image.comps[c++].data[i] = OPJ_INT32(pixel_it->b);
            }

            if constexpr(has_alpha<P>::value)
            {
                _opj_image.comps[c].data[i] = OPJ_INT32(pixel_it->a);
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
