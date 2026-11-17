/************************************************************************
 *
 * Copyright (C) 2023-2026 IRCAD France
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

#include "helper.hxx"

#include <core/os/temp_path.hpp>

#include <io/bitmap/reader.hpp>
#include <io/bitmap/writer.hpp>

#include <doctest/doctest.h>

namespace sight::io::bitmap::ut
{

//------------------------------------------------------------------------------

template<enum data::image::pixel_format_t F, core::type::type_t T, backend B>
inline static void test_backend(bool _write_must_fail = false)
{
    // Create a temporary directory
    core::os::temp_dir tmp_dir;

    // Create the synthetic image
    const auto& expected_image = get_synthetic_image(0, T, F);

    // Create the writer
    auto writer = std::make_shared<io::bitmap::writer>();
    CHECK_NOTHROW(writer->set_object(expected_image));

    // Create the reader
    auto actual_image = std::make_shared<data::image>();
    auto reader       = std::make_shared<io::bitmap::reader>();
    reader->set_object(actual_image);

    // Test write modes
    for(const auto& mode : {writer::mode::best, writer::mode::fast})
    {
        constexpr core::type type = T;

        const auto& file_path = tmp_dir / (
            pixel_format_to_string(F) + "_" + type.name() + "_" + file_suffix(B, mode)
        );

        CHECK_NOTHROW(writer->set_file(file_path));

        if(_write_must_fail)
        {
            CHECK_THROWS(writer->write(B, mode));

            // Do not try to read the image
            continue;
        }

        CHECK_NOTHROW(writer->write(B, mode));

        // Read back the image
        reader->set_file(file_path);
        CHECK_NOTHROW(reader->read(B));

        // Check the image
        if constexpr(B != backend::libjpeg && B != backend::nvjpeg)
        {
            CHECK_MESSAGE(
                *expected_image == *actual_image,
                ("The image are not equal for backend '" + backend_to_string(B).first + "', mode '" + mode_to_string(
                     mode
                )
                 + "', format '" + pixel_format_to_string(F) + "', type '" + type.name() + "'")
            );
        }
        else
        {
            // Compare at least sizes...
            const auto& expected_size = expected_image->size();
            const auto& actual_size   = actual_image->size();
            CHECK_EQ(expected_size[0], actual_size[0]);
            CHECK_EQ(expected_size[1], actual_size[1]);
            CHECK_EQ(expected_size[2], actual_size[2]);
            CHECK_EQ(expected_image->pixel_format(), actual_image->pixel_format());
            CHECK_EQ(expected_image->type(), actual_image->type());

            // Ensure that psnr is at least > 20
            const double psnr = compute_psnr(expected_image, actual_image);
            CHECK_MESSAGE(
                psnr > 20,
                ("The image seems to be different with backend '"
                 + backend_to_string(B).first
                 + "', PSNR="
                 + std::to_string(psnr)
                 + "dB")
            );
        }
    }
}

//------------------------------------------------------------------------------

TEST_SUITE("sight::io::bitmap::reader_writer")
{
    TEST_CASE("grayscale_uint_8")
    {
        test_backend<data::image::pixel_format_t::gray_scale, core::type::type_t::uint_8, backend::libjpeg>();
        test_backend<data::image::pixel_format_t::gray_scale, core::type::type_t::uint_8, backend::libpng>();
        test_backend<data::image::pixel_format_t::gray_scale, core::type::type_t::uint_8, backend::libtiff>();
        test_backend<data::image::pixel_format_t::gray_scale, core::type::type_t::uint_8, backend::openjpeg>();

        if(io::bitmap::nvjpeg())
        {
            test_backend<data::image::pixel_format_t::gray_scale, core::type::type_t::uint_8, backend::nvjpeg>(true);
        }

        if(io::bitmap::nvjpeg2k())
        {
            test_backend<data::image::pixel_format_t::gray_scale, core::type::type_t::uint_8, backend::nvjpeg2k>();
        }
    }

//------------------------------------------------------------------------------

    TEST_CASE("grayscale_uint_16")
    {
        test_backend<data::image::pixel_format_t::gray_scale, core::type::type_t::uint_16, backend::libjpeg>(true);
        test_backend<data::image::pixel_format_t::gray_scale, core::type::type_t::uint_16, backend::libpng>();
        test_backend<data::image::pixel_format_t::gray_scale, core::type::type_t::uint_16, backend::libtiff>();
        test_backend<data::image::pixel_format_t::gray_scale, core::type::type_t::uint_16, backend::openjpeg>();

        if(io::bitmap::nvjpeg())
        {
            test_backend<data::image::pixel_format_t::gray_scale, core::type::type_t::uint_16, backend::nvjpeg>(true);
        }

        if(io::bitmap::nvjpeg2k())
        {
            test_backend<data::image::pixel_format_t::gray_scale, core::type::type_t::uint_16, backend::nvjpeg2k>();
        }
    }

//------------------------------------------------------------------------------

    TEST_CASE("grayscale_int_8")
    {
        test_backend<data::image::pixel_format_t::gray_scale, core::type::type_t::int_8, backend::libjpeg>(true);
        test_backend<data::image::pixel_format_t::gray_scale, core::type::type_t::int_8, backend::libpng>(true);
        test_backend<data::image::pixel_format_t::gray_scale, core::type::type_t::int_8, backend::libtiff>();
        test_backend<data::image::pixel_format_t::gray_scale, core::type::type_t::int_8, backend::openjpeg>();

        if(io::bitmap::nvjpeg())
        {
            test_backend<data::image::pixel_format_t::gray_scale, core::type::type_t::int_8, backend::nvjpeg>(true);
        }

        if(io::bitmap::nvjpeg2k())
        {
            test_backend<data::image::pixel_format_t::gray_scale, core::type::type_t::int_8, backend::nvjpeg2k>(true);
        }
    }

//------------------------------------------------------------------------------

    TEST_CASE("grayscale_int_16")
    {
        test_backend<data::image::pixel_format_t::gray_scale, core::type::type_t::int_16, backend::libjpeg>(true);
        test_backend<data::image::pixel_format_t::gray_scale, core::type::type_t::int_16, backend::libpng>(true);
        test_backend<data::image::pixel_format_t::gray_scale, core::type::type_t::int_16, backend::libtiff>();
        test_backend<data::image::pixel_format_t::gray_scale, core::type::type_t::int_16, backend::openjpeg>();

        if(io::bitmap::nvjpeg())
        {
            test_backend<data::image::pixel_format_t::gray_scale, core::type::type_t::int_16, backend::nvjpeg>(true);
        }

        if(io::bitmap::nvjpeg2k())
        {
            test_backend<data::image::pixel_format_t::gray_scale, core::type::type_t::int_16, backend::nvjpeg2k>(true);
        }
    }

//------------------------------------------------------------------------------

    TEST_CASE("rgb_uint_8")
    {
        test_backend<data::image::pixel_format_t::rgb, core::type::type_t::uint_8, backend::libjpeg>();
        test_backend<data::image::pixel_format_t::rgb, core::type::type_t::uint_8, backend::libpng>();
        test_backend<data::image::pixel_format_t::rgb, core::type::type_t::uint_8, backend::libtiff>();
        test_backend<data::image::pixel_format_t::rgb, core::type::type_t::uint_8, backend::openjpeg>();

        if(io::bitmap::nvjpeg())
        {
            test_backend<data::image::pixel_format_t::rgb, core::type::type_t::uint_8, backend::nvjpeg>();
        }

        if(io::bitmap::nvjpeg2k())
        {
            test_backend<data::image::pixel_format_t::rgb, core::type::type_t::uint_8, backend::nvjpeg2k>();
        }
    }

//------------------------------------------------------------------------------

    TEST_CASE("rgb_uint_16")
    {
        test_backend<data::image::pixel_format_t::rgb, core::type::type_t::uint_16, backend::libjpeg>(true);
        test_backend<data::image::pixel_format_t::rgb, core::type::type_t::uint_16, backend::libpng>();
        test_backend<data::image::pixel_format_t::rgb, core::type::type_t::uint_16, backend::libtiff>();
        test_backend<data::image::pixel_format_t::rgb, core::type::type_t::uint_16, backend::openjpeg>();

        if(io::bitmap::nvjpeg())
        {
            test_backend<data::image::pixel_format_t::rgb, core::type::type_t::uint_16, backend::nvjpeg>(true);
        }

        if(io::bitmap::nvjpeg2k())
        {
            test_backend<data::image::pixel_format_t::rgb, core::type::type_t::uint_16, backend::nvjpeg2k>();
        }
    }

//------------------------------------------------------------------------------

    TEST_CASE("rgb_int_8")
    {
        test_backend<data::image::pixel_format_t::rgb, core::type::type_t::int_8, backend::libjpeg>(true);
        test_backend<data::image::pixel_format_t::rgb, core::type::type_t::int_8, backend::libpng>(true);
        test_backend<data::image::pixel_format_t::rgb, core::type::type_t::int_8, backend::libtiff>();
        test_backend<data::image::pixel_format_t::rgb, core::type::type_t::int_8, backend::openjpeg>();

        if(io::bitmap::nvjpeg())
        {
            test_backend<data::image::pixel_format_t::rgb, core::type::type_t::int_8, backend::nvjpeg>(true);
        }

        if(io::bitmap::nvjpeg2k())
        {
            test_backend<data::image::pixel_format_t::rgb, core::type::type_t::int_8, backend::nvjpeg2k>(true);
        }
    }

//------------------------------------------------------------------------------

    TEST_CASE("rgb_int_16")
    {
        test_backend<data::image::pixel_format_t::rgb, core::type::type_t::int_16, backend::libjpeg>(true);
        test_backend<data::image::pixel_format_t::rgb, core::type::type_t::int_16, backend::libpng>(true);
        test_backend<data::image::pixel_format_t::rgb, core::type::type_t::int_16, backend::libtiff>();
        test_backend<data::image::pixel_format_t::rgb, core::type::type_t::int_16, backend::openjpeg>();

        if(io::bitmap::nvjpeg())
        {
            test_backend<data::image::pixel_format_t::rgb, core::type::type_t::int_16, backend::nvjpeg>(true);
        }

        if(io::bitmap::nvjpeg2k())
        {
            test_backend<data::image::pixel_format_t::rgb, core::type::type_t::int_16, backend::nvjpeg2k>(true);
        }
    }

//------------------------------------------------------------------------------

    TEST_CASE("rgba_uint_8")
    {
        test_backend<data::image::pixel_format_t::rgba, core::type::type_t::uint_8, backend::libjpeg>(true);
        test_backend<data::image::pixel_format_t::rgba, core::type::type_t::uint_8, backend::libpng>();
        test_backend<data::image::pixel_format_t::rgba, core::type::type_t::uint_8, backend::libtiff>();
        test_backend<data::image::pixel_format_t::rgba, core::type::type_t::uint_8, backend::openjpeg>();

        if(io::bitmap::nvjpeg())
        {
            test_backend<data::image::pixel_format_t::rgba, core::type::type_t::uint_8, backend::nvjpeg>(true);
        }

        if(io::bitmap::nvjpeg2k())
        {
            test_backend<data::image::pixel_format_t::rgba, core::type::type_t::uint_8, backend::nvjpeg2k>();
        }
    }

//------------------------------------------------------------------------------

    TEST_CASE("rgba_uint_16")
    {
        test_backend<data::image::pixel_format_t::rgba, core::type::type_t::uint_16, backend::libjpeg>(true);
        test_backend<data::image::pixel_format_t::rgba, core::type::type_t::uint_16, backend::libpng>();
        test_backend<data::image::pixel_format_t::rgba, core::type::type_t::uint_16, backend::libtiff>();
        test_backend<data::image::pixel_format_t::rgba, core::type::type_t::uint_16, backend::openjpeg>();

        if(io::bitmap::nvjpeg())
        {
            test_backend<data::image::pixel_format_t::rgba, core::type::type_t::uint_16, backend::nvjpeg>(true);
        }

        if(io::bitmap::nvjpeg2k())
        {
            test_backend<data::image::pixel_format_t::rgba, core::type::type_t::uint_16, backend::nvjpeg2k>(true);
        }
    }

//------------------------------------------------------------------------------

    TEST_CASE("rgba_int_8")
    {
        test_backend<data::image::pixel_format_t::rgba, core::type::type_t::int_8, backend::libjpeg>(true);
        test_backend<data::image::pixel_format_t::rgba, core::type::type_t::int_8, backend::libpng>(true);
        test_backend<data::image::pixel_format_t::rgba, core::type::type_t::int_8, backend::libtiff>();
        test_backend<data::image::pixel_format_t::rgba, core::type::type_t::int_8, backend::openjpeg>();

        if(io::bitmap::nvjpeg())
        {
            test_backend<data::image::pixel_format_t::rgba, core::type::type_t::int_8, backend::nvjpeg>(true);
        }

        if(io::bitmap::nvjpeg2k())
        {
            test_backend<data::image::pixel_format_t::rgba, core::type::type_t::int_8, backend::nvjpeg2k>(true);
        }
    }

//------------------------------------------------------------------------------

    TEST_CASE("rgba_int_16")
    {
        test_backend<data::image::pixel_format_t::rgba, core::type::type_t::int_16, backend::libjpeg>(true);
        test_backend<data::image::pixel_format_t::rgba, core::type::type_t::int_16, backend::libpng>(true);
        test_backend<data::image::pixel_format_t::rgba, core::type::type_t::int_16, backend::libtiff>();
        test_backend<data::image::pixel_format_t::rgba, core::type::type_t::int_16, backend::openjpeg>();

        if(io::bitmap::nvjpeg())
        {
            test_backend<data::image::pixel_format_t::rgba, core::type::type_t::int_16, backend::nvjpeg>(true);
        }

        if(io::bitmap::nvjpeg2k())
        {
            test_backend<data::image::pixel_format_t::rgba, core::type::type_t::int_16, backend::nvjpeg2k>(true);
        }
    }
} // TEST_SUITE

} // namespace sight::io::bitmap::ut
