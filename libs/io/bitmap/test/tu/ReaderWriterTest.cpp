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

#include "ReaderWriterTest.hpp"

#include "helper.hxx"

#include <core/os/temp_path.hpp>

#include <io/bitmap/Reader.hpp>
#include <io/bitmap/Writer.hpp>

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION(sight::io::bitmap::ut::ReaderWriterTest);

namespace sight::io::bitmap::ut
{

//------------------------------------------------------------------------------

template<data::Image::PixelFormat F, core::type::type_t T, Backend B>
inline static void testBackend(bool write_must_fail = false)
{
    // Create a temporary directory
    core::os::temp_dir tmp_dir;

    // Create the synthetic image
    const auto& expected_image = getSyntheticImage(0, T, F);

    // Create the writer
    auto writer = std::make_shared<Writer>();
    CPPUNIT_ASSERT_NO_THROW_MESSAGE(backendToString(B).first, writer->setObject(expected_image));

    // Create the reader
    auto actual_image = std::make_shared<data::Image>();
    auto reader       = std::make_shared<Reader>();
    reader->setObject(actual_image);

    // Test write modes
    for(const auto& mode : {Writer::Mode::BEST, Writer::Mode::FAST})
    {
        constexpr core::type type = T;

        const auto& file_path = tmp_dir / (
            pixelFormatToString(F) + "_" + type.name() + "_" + fileSuffix(B, mode)
        );

        CPPUNIT_ASSERT_NO_THROW_MESSAGE(backendToString(B).first, writer->set_file(file_path));

        if(write_must_fail)
        {
            CPPUNIT_ASSERT_THROW_MESSAGE(backendToString(B).first, writer->write(B, mode), core::exception);

            // Do not try to read the image
            continue;
        }

        CPPUNIT_ASSERT_NO_THROW_MESSAGE(backendToString(B).first, writer->write(B, mode));

        // Read back the image
        reader->set_file(file_path);
        CPPUNIT_ASSERT_NO_THROW(reader->read(B));

        // Check the image
        if constexpr(B != Backend::LIBJPEG && B != Backend::NVJPEG)
        {
            CPPUNIT_ASSERT_MESSAGE(
                "The image are not equal for backend '" + backendToString(B).first + "', mode '" + modeToString(mode)
                + "', format '" + pixelFormatToString(F) + "', type '" + type.name() + "'",
                *expected_image == *actual_image
            );
        }
        else
        {
            // Compare at least sizes...
            const auto& expected_size = expected_image->size();
            const auto& actual_size   = actual_image->size();
            CPPUNIT_ASSERT_EQUAL_MESSAGE(backendToString(B).first, expected_size[0], actual_size[0]);
            CPPUNIT_ASSERT_EQUAL_MESSAGE(backendToString(B).first, expected_size[1], actual_size[1]);
            CPPUNIT_ASSERT_EQUAL_MESSAGE(backendToString(B).first, expected_size[2], actual_size[2]);
            CPPUNIT_ASSERT_EQUAL_MESSAGE(
                backendToString(B).first,
                expected_image->getPixelFormat(),
                actual_image->getPixelFormat()
            );
            CPPUNIT_ASSERT_EQUAL_MESSAGE(backendToString(B).first, expected_image->getType(), actual_image->getType());

            // Ensure that psnr is at least > 20
            const double psnr = computePSNR(expected_image, actual_image);
            CPPUNIT_ASSERT_MESSAGE(
                "The image seems to be different with backend '"
                + backendToString(B).first
                + "', PSNR="
                + std::to_string(psnr)
                + "dB",
                psnr > 20
            );
        }
    }
}

//------------------------------------------------------------------------------

void ReaderWriterTest::setUp()
{
}

//------------------------------------------------------------------------------

void ReaderWriterTest::tearDown()
{
}

//------------------------------------------------------------------------------

void ReaderWriterTest::grayscaleUINT8Test()
{
    testBackend<data::Image::PixelFormat::GRAY_SCALE, core::type::type_t::UINT8, Backend::LIBJPEG>();
    testBackend<data::Image::PixelFormat::GRAY_SCALE, core::type::type_t::UINT8, Backend::LIBPNG>();
    testBackend<data::Image::PixelFormat::GRAY_SCALE, core::type::type_t::UINT8, Backend::LIBTIFF>();
    testBackend<data::Image::PixelFormat::GRAY_SCALE, core::type::type_t::UINT8, Backend::OPENJPEG>();

    if(io::bitmap::nvJPEG())
    {
        testBackend<data::Image::PixelFormat::GRAY_SCALE, core::type::type_t::UINT8, Backend::NVJPEG>(true);
    }

    if(io::bitmap::nvJPEG2K())
    {
        testBackend<data::Image::PixelFormat::GRAY_SCALE, core::type::type_t::UINT8, Backend::NVJPEG2K>();
    }
}

//------------------------------------------------------------------------------

void ReaderWriterTest::grayscaleUINT16Test()
{
    testBackend<data::Image::PixelFormat::GRAY_SCALE, core::type::type_t::UINT16, Backend::LIBJPEG>(true);
    testBackend<data::Image::PixelFormat::GRAY_SCALE, core::type::type_t::UINT16, Backend::LIBPNG>();
    testBackend<data::Image::PixelFormat::GRAY_SCALE, core::type::type_t::UINT16, Backend::LIBTIFF>();
    testBackend<data::Image::PixelFormat::GRAY_SCALE, core::type::type_t::UINT16, Backend::OPENJPEG>();

    if(io::bitmap::nvJPEG())
    {
        testBackend<data::Image::PixelFormat::GRAY_SCALE, core::type::type_t::UINT16, Backend::NVJPEG>(true);
    }

    if(io::bitmap::nvJPEG2K())
    {
        testBackend<data::Image::PixelFormat::GRAY_SCALE, core::type::type_t::UINT16, Backend::NVJPEG2K>();
    }
}

//------------------------------------------------------------------------------

void ReaderWriterTest::grayscaleINT8Test()
{
    testBackend<data::Image::PixelFormat::GRAY_SCALE, core::type::type_t::INT8, Backend::LIBJPEG>(true);
    testBackend<data::Image::PixelFormat::GRAY_SCALE, core::type::type_t::INT8, Backend::LIBPNG>(true);
    testBackend<data::Image::PixelFormat::GRAY_SCALE, core::type::type_t::INT8, Backend::LIBTIFF>();
    testBackend<data::Image::PixelFormat::GRAY_SCALE, core::type::type_t::INT8, Backend::OPENJPEG>();

    if(io::bitmap::nvJPEG())
    {
        testBackend<data::Image::PixelFormat::GRAY_SCALE, core::type::type_t::INT8, Backend::NVJPEG>(true);
    }

    if(io::bitmap::nvJPEG2K())
    {
        testBackend<data::Image::PixelFormat::GRAY_SCALE, core::type::type_t::INT8, Backend::NVJPEG2K>(true);
    }
}

//------------------------------------------------------------------------------

void ReaderWriterTest::grayscaleINT16Test()
{
    testBackend<data::Image::PixelFormat::GRAY_SCALE, core::type::type_t::INT16, Backend::LIBJPEG>(true);
    testBackend<data::Image::PixelFormat::GRAY_SCALE, core::type::type_t::INT16, Backend::LIBPNG>(true);
    testBackend<data::Image::PixelFormat::GRAY_SCALE, core::type::type_t::INT16, Backend::LIBTIFF>();
    testBackend<data::Image::PixelFormat::GRAY_SCALE, core::type::type_t::INT16, Backend::OPENJPEG>();

    if(io::bitmap::nvJPEG())
    {
        testBackend<data::Image::PixelFormat::GRAY_SCALE, core::type::type_t::INT16, Backend::NVJPEG>(true);
    }

    if(io::bitmap::nvJPEG2K())
    {
        testBackend<data::Image::PixelFormat::GRAY_SCALE, core::type::type_t::INT16, Backend::NVJPEG2K>(true);
    }
}

//------------------------------------------------------------------------------

void ReaderWriterTest::rgbUINT8Test()
{
    testBackend<data::Image::PixelFormat::RGB, core::type::type_t::UINT8, Backend::LIBJPEG>();
    testBackend<data::Image::PixelFormat::RGB, core::type::type_t::UINT8, Backend::LIBPNG>();
    testBackend<data::Image::PixelFormat::RGB, core::type::type_t::UINT8, Backend::LIBTIFF>();
    testBackend<data::Image::PixelFormat::RGB, core::type::type_t::UINT8, Backend::OPENJPEG>();

    if(io::bitmap::nvJPEG())
    {
        testBackend<data::Image::PixelFormat::RGB, core::type::type_t::UINT8, Backend::NVJPEG>();
    }

    if(io::bitmap::nvJPEG2K())
    {
        testBackend<data::Image::PixelFormat::RGB, core::type::type_t::UINT8, Backend::NVJPEG2K>();
    }
}

//------------------------------------------------------------------------------

void ReaderWriterTest::rgbUINT16Test()
{
    testBackend<data::Image::PixelFormat::RGB, core::type::type_t::UINT16, Backend::LIBJPEG>(true);
    testBackend<data::Image::PixelFormat::RGB, core::type::type_t::UINT16, Backend::LIBPNG>();
    testBackend<data::Image::PixelFormat::RGB, core::type::type_t::UINT16, Backend::LIBTIFF>();
    testBackend<data::Image::PixelFormat::RGB, core::type::type_t::UINT16, Backend::OPENJPEG>();

    if(io::bitmap::nvJPEG())
    {
        testBackend<data::Image::PixelFormat::RGB, core::type::type_t::UINT16, Backend::NVJPEG>(true);
    }

    if(io::bitmap::nvJPEG2K())
    {
        testBackend<data::Image::PixelFormat::RGB, core::type::type_t::UINT16, Backend::NVJPEG2K>();
    }
}

//------------------------------------------------------------------------------

void ReaderWriterTest::rgbINT8Test()
{
    testBackend<data::Image::PixelFormat::RGB, core::type::type_t::INT8, Backend::LIBJPEG>(true);
    testBackend<data::Image::PixelFormat::RGB, core::type::type_t::INT8, Backend::LIBPNG>(true);
    testBackend<data::Image::PixelFormat::RGB, core::type::type_t::INT8, Backend::LIBTIFF>();
    testBackend<data::Image::PixelFormat::RGB, core::type::type_t::INT8, Backend::OPENJPEG>();

    if(io::bitmap::nvJPEG())
    {
        testBackend<data::Image::PixelFormat::RGB, core::type::type_t::INT8, Backend::NVJPEG>(true);
    }

    if(io::bitmap::nvJPEG2K())
    {
        testBackend<data::Image::PixelFormat::RGB, core::type::type_t::INT8, Backend::NVJPEG2K>(true);
    }
}

//------------------------------------------------------------------------------

void ReaderWriterTest::rgbINT16Test()
{
    testBackend<data::Image::PixelFormat::RGB, core::type::type_t::INT16, Backend::LIBJPEG>(true);
    testBackend<data::Image::PixelFormat::RGB, core::type::type_t::INT16, Backend::LIBPNG>(true);
    testBackend<data::Image::PixelFormat::RGB, core::type::type_t::INT16, Backend::LIBTIFF>();
    testBackend<data::Image::PixelFormat::RGB, core::type::type_t::INT16, Backend::OPENJPEG>();

    if(io::bitmap::nvJPEG())
    {
        testBackend<data::Image::PixelFormat::RGB, core::type::type_t::INT16, Backend::NVJPEG>(true);
    }

    if(io::bitmap::nvJPEG2K())
    {
        testBackend<data::Image::PixelFormat::RGB, core::type::type_t::INT16, Backend::NVJPEG2K>(true);
    }
}

//------------------------------------------------------------------------------

void ReaderWriterTest::rgbaUINT8Test()
{
    testBackend<data::Image::PixelFormat::RGBA, core::type::type_t::UINT8, Backend::LIBJPEG>(true);
    testBackend<data::Image::PixelFormat::RGBA, core::type::type_t::UINT8, Backend::LIBPNG>();
    testBackend<data::Image::PixelFormat::RGBA, core::type::type_t::UINT8, Backend::LIBTIFF>();
    testBackend<data::Image::PixelFormat::RGBA, core::type::type_t::UINT8, Backend::OPENJPEG>();

    if(io::bitmap::nvJPEG())
    {
        testBackend<data::Image::PixelFormat::RGBA, core::type::type_t::UINT8, Backend::NVJPEG>(true);
    }

    if(io::bitmap::nvJPEG2K())
    {
        testBackend<data::Image::PixelFormat::RGBA, core::type::type_t::UINT8, Backend::NVJPEG2K>();
    }
}

//------------------------------------------------------------------------------

void ReaderWriterTest::rgbaUINT16Test()
{
    testBackend<data::Image::PixelFormat::RGBA, core::type::type_t::UINT16, Backend::LIBJPEG>(true);
    testBackend<data::Image::PixelFormat::RGBA, core::type::type_t::UINT16, Backend::LIBPNG>();
    testBackend<data::Image::PixelFormat::RGBA, core::type::type_t::UINT16, Backend::LIBTIFF>();
    testBackend<data::Image::PixelFormat::RGBA, core::type::type_t::UINT16, Backend::OPENJPEG>();

    if(io::bitmap::nvJPEG())
    {
        testBackend<data::Image::PixelFormat::RGBA, core::type::type_t::UINT16, Backend::NVJPEG>(true);
    }

    if(io::bitmap::nvJPEG2K())
    {
        testBackend<data::Image::PixelFormat::RGBA, core::type::type_t::UINT16, Backend::NVJPEG2K>(true);
    }
}

//------------------------------------------------------------------------------

void ReaderWriterTest::rgbaINT8Test()
{
    testBackend<data::Image::PixelFormat::RGBA, core::type::type_t::INT8, Backend::LIBJPEG>(true);
    testBackend<data::Image::PixelFormat::RGBA, core::type::type_t::INT8, Backend::LIBPNG>(true);
    testBackend<data::Image::PixelFormat::RGBA, core::type::type_t::INT8, Backend::LIBTIFF>();
    testBackend<data::Image::PixelFormat::RGBA, core::type::type_t::INT8, Backend::OPENJPEG>();

    if(io::bitmap::nvJPEG())
    {
        testBackend<data::Image::PixelFormat::RGBA, core::type::type_t::INT8, Backend::NVJPEG>(true);
    }

    if(io::bitmap::nvJPEG2K())
    {
        testBackend<data::Image::PixelFormat::RGBA, core::type::type_t::INT8, Backend::NVJPEG2K>(true);
    }
}

//------------------------------------------------------------------------------

void ReaderWriterTest::rgbaINT16Test()
{
    testBackend<data::Image::PixelFormat::RGBA, core::type::type_t::INT16, Backend::LIBJPEG>(true);
    testBackend<data::Image::PixelFormat::RGBA, core::type::type_t::INT16, Backend::LIBPNG>(true);
    testBackend<data::Image::PixelFormat::RGBA, core::type::type_t::INT16, Backend::LIBTIFF>();
    testBackend<data::Image::PixelFormat::RGBA, core::type::type_t::INT16, Backend::OPENJPEG>();

    if(io::bitmap::nvJPEG())
    {
        testBackend<data::Image::PixelFormat::RGBA, core::type::type_t::INT16, Backend::NVJPEG>(true);
    }

    if(io::bitmap::nvJPEG2K())
    {
        testBackend<data::Image::PixelFormat::RGBA, core::type::type_t::INT16, Backend::NVJPEG2K>(true);
    }
}

} // namespace sight::io::bitmap::ut
