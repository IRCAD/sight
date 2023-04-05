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

#include <openjpeg.h>

#include <type_traits>

// cspell:ignore nvjpeg NOLINTNEXTLINE numresolution cblockw cblockh sgnd CLRSPC cparameters decod dparameters

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

class OpenJPEGReader final
{
public:

    /// Delete copy constructors and assignment operators
    OpenJPEGReader(const OpenJPEGReader&)            = delete;
    OpenJPEGReader& operator=(const OpenJPEGReader&) = delete;

    /// Constructor
    inline OpenJPEGReader() noexcept
    {
        // Set the default decoding parameters
        opj_set_default_decoder_parameters(&m_parameters);

        m_valid = true;
    }

    /// Destructor
    inline ~OpenJPEGReader() noexcept = default;

    /// Reading
    inline void read(data::Image& image, std::istream& istream, Flag flag)
    {
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
            keeper.m_codec = opj_create_decompress(
                flag == Flag::J2K_STREAM ? OPJ_CODEC_J2K : OPJ_CODEC_JP2
            )
        );

        // Install info, warning, error handlers
        CHECK_OPJ(opj_set_info_handler(keeper.m_codec, infoCallback, nullptr));
        CHECK_OPJ(opj_set_warning_handler(keeper.m_codec, warningCallback, nullptr));
        CHECK_OPJ(opj_set_warning_handler(keeper.m_codec, errorCallback, nullptr));

        // Create output stream (1MB buffer by default)
        CHECK_OPJ(keeper.m_stream = opj_stream_create(OPJ_J2K_STREAM_CHUNK_SIZE, OPJ_TRUE));

        // Get input size
        istream.seekg(0, std::ios::end);
        const auto stream_size = istream.tellg();
        istream.seekg(0, std::ios::beg);

        // Setup OPJ user stream
        opj_stream_set_user_data(keeper.m_stream, &istream, freeCallback);
        opj_stream_set_user_data_length(keeper.m_stream, OPJ_UINT64(stream_size));

        // Setup stream callback
        opj_stream_set_read_function(keeper.m_stream, readCallback);
        opj_stream_set_skip_function(keeper.m_stream, skipCallback);
        opj_stream_set_seek_function(keeper.m_stream, seekCallback);

        // Format can .jp2 or .j2k
        m_parameters.decod_format = flag == Flag::J2K_STREAM ? 0 : 1;

        // Setup the decoder
        CHECK_OPJ(opj_setup_decoder(keeper.m_codec, &m_parameters));

        // Read the header
        CHECK_OPJ(opj_read_header(keeper.m_stream, keeper.m_codec, &keeper.m_image));

        // Decode the image
        CHECK_OPJ(opj_decode(keeper.m_codec, keeper.m_stream, keeper.m_image));

        // End decoding
        CHECK_OPJ(opj_end_decompress(keeper.m_codec, keeper.m_stream));

        // Get image type
        const auto& [width, height, type] =
            [&]
            {
                // Find the max precision and max dimension, so we can allocate enough memory in all cases
                OPJ_UINT32 prec    = 0;
                std::size_t width  = 0;
                std::size_t height = 0;

                for(std::size_t i = 0 ; i < keeper.m_image->numcomps ; ++i)
                {
                    const auto& comp = keeper.m_image->comps[i];
                    prec   = std::max(prec, comp.prec);
                    width  = std::max(width, std::size_t(comp.w));
                    height = std::max(height, std::size_t(comp.h));
                }

                const bool sgnd = keeper.m_image->comps[0].sgnd != 0;

                if(prec <= 8)
                {
                    return std::make_tuple(width, height, sgnd ? core::Type::INT8 : core::Type::UINT8);
                }
                else if(prec <= 16)
                {
                    return std::make_tuple(width, height, sgnd ? core::Type::INT16 : core::Type::UINT16);
                }
                else if(prec <= 32)
                {
                    return std::make_tuple(width, height, sgnd ? core::Type::INT32 : core::Type::UINT32);
                }
                else
                {
                    SIGHT_THROW("Unsupported precision: '" << prec << "'");
                }
            }();

        // Get pixel_format
        const auto pixel_format =
            [&]
            {
                switch(keeper.m_image->color_space)
                {
                    case OPJ_CLRSPC_GRAY:
                    {
                        SIGHT_THROW_IF(
                            "data::Image::PixelFormat::GRAY_SCALE must have exactly one component.",
                            keeper.m_image->numcomps != 1
                        );

                        return data::Image::PixelFormat::GRAY_SCALE;
                    }

                    case OPJ_CLRSPC_SRGB:
                    {
                        switch(keeper.m_image->numcomps)
                        {
                            case 3:
                                return data::Image::PixelFormat::RGB;

                            case 4:
                                return data::Image::PixelFormat::RGBA;

                            default:
                                SIGHT_THROW(
                                    "Unsupported number of components: '" << keeper.m_image->numcomps << "'."
                                );
                        }
                    }

                    case OPJ_CLRSPC_UNKNOWN:
                    case OPJ_CLRSPC_UNSPECIFIED:
                    {
                        switch(keeper.m_image->numcomps)
                        {
                            case 1:
                                return data::Image::PixelFormat::GRAY_SCALE;

                            case 3:
                                return data::Image::PixelFormat::RGB;

                            case 4:
                                return data::Image::PixelFormat::RGBA;

                            default:
                                SIGHT_THROW(
                                    "Unsupported number of components: '" << keeper.m_image->numcomps << "'."
                                );
                        }
                    }

                    default:
                        SIGHT_THROW("Unsupported color space: '" << keeper.m_image->color_space << "'.");
                }
            }();

        // Allocate destination image
        image.resize(
            {width, height, 0},
            type,
            pixel_format
        );

        // Convert Sight interlaced pixels to planar openJPEG pixels
        switch(type)
        {
            case core::Type::INT8:
                toSight<std::int8_t>(*keeper.m_image, image);
                break;

            case core::Type::UINT8:
                toSight<std::uint8_t>(*keeper.m_image, image);
                break;

            case core::Type::INT16:
                toSight<std::int16_t>(*keeper.m_image, image);
                break;

            case core::Type::UINT16:
                toSight<std::uint16_t>(*keeper.m_image, image);
                break;

            case core::Type::INT32:
                toSight<std::int32_t>(*keeper.m_image, image);
                break;

            case core::Type::UINT32:
                toSight<std::uint32_t>(*keeper.m_image, image);
                break;

            default:
                SIGHT_THROW("Unsupported precision: '" << type << "'");
        }
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

    inline static OPJ_SIZE_T readCallback(void* p_buffer, OPJ_SIZE_T p_nb_bytes, void* p_user_data)
    {
        if(p_user_data == nullptr || p_nb_bytes == 0)
        {
            return OPJ_SIZE_T(-1);
        }

        auto* istream = reinterpret_cast<std::istream*>(p_user_data);
        istream->read(reinterpret_cast<char*>(p_buffer), std::streamsize(p_nb_bytes));
        const auto count = istream->gcount();

        if(count <= 0)
        {
            return OPJ_SIZE_T(-1);
        }

        // If we have read something, we must clear the failbit
        istream->clear();

        return OPJ_SIZE_T(count);
    }

    //------------------------------------------------------------------------------

    inline static OPJ_OFF_T skipCallback(OPJ_OFF_T p_nb_bytes, void* p_user_data)
    {
        if(p_user_data == nullptr)
        {
            return -1;
        }

        auto* istream = reinterpret_cast<std::istream*>(p_user_data);
        istream->seekg(p_nb_bytes, std::ios_base::cur);

        if(istream->fail())
        {
            return -1;
        }

        return p_nb_bytes;
    }

    //------------------------------------------------------------------------------

    inline static OPJ_BOOL seekCallback(OPJ_OFF_T p_nb_bytes, void* p_user_data)
    {
        if(p_user_data == nullptr)
        {
            return OPJ_FALSE;
        }

        auto* istream = reinterpret_cast<std::istream*>(p_user_data);
        istream->seekg(p_nb_bytes, std::ios_base::beg);

        if(istream->fail())
        {
            return OPJ_FALSE;
        }

        return OPJ_TRUE;
    }

    //------------------------------------------------------------------------------

    inline static void freeCallback(void* /*p_user_data*/)
    {
        // Nothing to do, istream comes from the outside
    }

    //------------------------------------------------------------------------------

    template<typename T>
    inline static void toSight(const opj_image_t& opj_image, data::Image& image)
    {
        switch(image.getPixelFormat())
        {
            case data::Image::GRAY_SCALE:
            {
                struct Pixel
                {
                    T a;
                };

                toSightPixels<Pixel>(opj_image, image);
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

                toSightPixels<Pixel>(opj_image, image);
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

                toSightPixels<Pixel>(opj_image, image);
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

                toSightPixels<Pixel>(opj_image, image);
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

                toSightPixels<Pixel>(opj_image, image);
                break;
            }

            default:
                SIGHT_THROW("Unsupported pixel format");
        }
    }

    //------------------------------------------------------------------------------

    template<typename P>
    inline static void toSightPixels(const opj_image_t& opj_image, data::Image& image)
    {
        const auto& sizes = image.getSize();

        auto pixel_it        = image.begin<P>();
        const auto pixel_end = image.end<P>();

        for(std::size_t i = 0, end = sizes[0] * sizes[1] ; i < end && pixel_it != pixel_end ; ++pixel_it)
        {
            std::size_t c = 0;

            if constexpr(has_r<P>::value)
            {
                pixel_it->r = decltype(pixel_it->r)(opj_image.comps[c++].data[i]);
            }

            if constexpr(has_g<P>::value)
            {
                pixel_it->g = decltype(pixel_it->g)(opj_image.comps[c++].data[i]);
            }

            if constexpr(has_b<P>::value)
            {
                pixel_it->b = decltype(pixel_it->b)(opj_image.comps[c++].data[i]);
            }

            if constexpr(has_alpha<P>::value)
            {
                pixel_it->a = decltype(pixel_it->a)(opj_image.comps[c].data[i]);
            }

            ++i;
        }
    }

    opj_dparameters_t m_parameters {};

public:

    bool m_valid {false};
    static constexpr std::string_view m_name {"OpenJPEGReader"};
};

} // namespace sight::io::bitmap::detail
