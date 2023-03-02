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

#include "ReaderTest.hpp"
#include "helper.hxx"

#include <io/bitmap/Reader.hpp>
#include <core/os/TempPath.hpp>

#include <utest/Filter.hpp>
#include <utest/profiling.hpp>

#include <cstdlib>
#include <future>

// This is for putenv() which is part of <cstdlib>
// cspell:ignore hicpp nvjpeg LIBJPEG LIBTIFF LUMA Acuson IMWRITE IMREAD ANYDEPTH ANYCOLOR OPENCV stoull
// NOLINTNEXTLINE(hicpp-deprecated-headers,modernize-deprecated-headers)
#include <stdlib.h>

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION(sight::io::bitmap::ut::ReaderTest);

namespace sight::io::bitmap::ut
{

//------------------------------------------------------------------------------

inline static void testBackend(
    const std::filesystem::path& file,
    Backend backend,
    data::Image::sptr expected_image = data::Image::sptr()
)
{
    core::os::TempDir temp_dir;
    std::filesystem::path filepath;

    if(expected_image)
    {
        filepath = temp_dir / file;

        // Write a expected image to disk
        const auto& mat = imageToMat(expected_image);
        cv::imwrite(
            filepath.string(),
            mat,
            {
                // Use best possible quality
                cv::IMWRITE_JPEG_QUALITY, 100,
                cv::IMWRITE_JPEG_CHROMA_QUALITY, 100,
                cv::IMWRITE_JPEG_LUMA_QUALITY, 100,
                cv::IMWRITE_JPEG_OPTIMIZE, 1
            });
    }
    else if(std::filesystem::exists(file) && std::filesystem::is_regular_file(file))
    {
        filepath       = file;
        expected_image = readImage(filepath);
    }
    else
    {
        CPPUNIT_FAIL("File not found: " + file.string());
    }

    auto actual_image = data::Image::New();

    // Read the image from disk
    {
        auto reader = Reader::New();
        reader->setObject(actual_image);
        reader->setFile(filepath);

        CPPUNIT_ASSERT_NO_THROW(reader->read(backend));
    }

    const auto& expected_sizes = expected_image->getSize();
    const auto& actual_sizes   = actual_image->getSize();

    CPPUNIT_ASSERT_EQUAL(expected_sizes[0], actual_sizes[0]);
    CPPUNIT_ASSERT_EQUAL(expected_sizes[1], actual_sizes[1]);
    CPPUNIT_ASSERT_EQUAL(expected_sizes[2], actual_sizes[2]);
    CPPUNIT_ASSERT_EQUAL(expected_image->getPixelFormat(), actual_image->getPixelFormat());
    CPPUNIT_ASSERT_EQUAL(expected_image->getType(), actual_image->getType());

    if(backend == Backend::LIBJPEG || backend == Backend::NVJPEG)
    {
        CPPUNIT_ASSERT(computePSNR(expected_image, actual_image) > 20.0);
    }
    else
    {
        CPPUNIT_ASSERT(*expected_image == *actual_image);
    }

    // test non existant file
    {
        constexpr auto path = "<<[{:}]>>";

        auto reader = Reader::New();
        reader->setObject(actual_image);
        reader->setFile(path);

        CPPUNIT_ASSERT_THROW(reader->read(backend), core::Exception);
    }

    // test existing corrupted file
    {
        core::os::TempFile corrupted_file(std::ios_base::out);
        corrupted_file << "This is a corrupted file";
        corrupted_file.stream().close();

        auto reader = Reader::New();
        reader->setObject(actual_image);
        reader->setFile(corrupted_file);

        CPPUNIT_ASSERT_THROW(reader->read(backend), core::Exception);
    }

    // test file with bad extension
    if(std::filesystem::exists(filepath))
    {
        auto reader = Reader::New();
        reader->setObject(actual_image);
        reader->setFile(filepath.replace_extension(".bad"));

        CPPUNIT_ASSERT_THROW(reader->read(backend), core::Exception);
    }
}

//------------------------------------------------------------------------------

inline static void profileReader(
    std::size_t loop,
    Backend backend
)
{
    auto reader = Reader::New();

    auto actual_image = data::Image::New();
    reader->setObject(actual_image);

    const auto& filename = "wild" + extensions(backend).front();
    const auto& filepath = utestData::Data::dir() / "sight" / "image" / "bitmap" / filename;
    reader->setFile(filepath);

    const std::string backend_name =
        [&]
        {
            switch(backend)
            {
                case Backend::LIBTIFF:
                    return "libTIFF";

                case Backend::LIBPNG:
                    return "libPNG";

                case Backend::NVJPEG:
                    return "nvJPEG";

                case Backend::LIBJPEG:
                    return "libjpeg";

                case Backend::NVJPEG2K:
                    return "nvJPEG2K";

                case Backend::OPENJPEG:
                    return "openJPEG";

                default:
                    SIGHT_THROW("Unknown backend: '" << std::uint8_t(backend) << "'");
            }
        }();

    // Now profile reading
    SIGHT_PROFILE_FUNC(
        [&]
            (std::size_t)
        {
            CPPUNIT_ASSERT_NO_THROW(reader->read(backend));
        },
        loop,
        backend_name,
        0.1
    );

    // Now profile openCV reading to compare
    if(!utest::Filter::ignoreSlowTests() && loop > 1)
    {
        const auto& opencv_label = "(openCV): " + filepath.extension().string();
        SIGHT_PROFILE_FUNC(
            [&]
                (std::size_t)
            {
                CPPUNIT_ASSERT_NO_THROW(readImage(filepath));
            },
            loop,
            opencv_label,
            0.1
        );
    }
}

//------------------------------------------------------------------------------

void ReaderTest::setUp()
{
    std::string jasper("OPENCV_IO_ENABLE_JASPER=1");
    putenv(jasper.data());
}

//------------------------------------------------------------------------------

void ReaderTest::tearDown()
{
}

//------------------------------------------------------------------------------

void ReaderTest::basicTest()
{
    auto reader = Reader::New();

    CPPUNIT_ASSERT_EQUAL(io::bitmap::extensions(Backend::LIBTIFF).front(), reader->extension());
}

//------------------------------------------------------------------------------

void ReaderTest::extensionsTest()
{
    std::vector<data::sequenced_set<std::string> > extensions {
        {".jpg", ".jpeg"},
        {".jp2"},
        {".j2k"},
        {".tiff", ".tif"},
        {".png"}
    };

    std::vector<Backend> backends {
        io::bitmap::nvJPEG() ? Backend::NVJPEG : Backend::LIBJPEG,
        io::bitmap::nvJPEG2K() ? Backend::NVJPEG2K : Backend::OPENJPEG,
        io::bitmap::nvJPEG2K() ? Backend::NVJPEG2K_J2K : Backend::OPENJPEG_J2K,
        Backend::LIBTIFF,
        Backend::LIBPNG
    };

    for(std::size_t i = 0 ; i < extensions.size() ; ++i)
    {
        const auto& extension_set        = extensions[i];
        const auto& backend              = backends[i];
        const auto& actual_extension_set = io::bitmap::extensions(backend);

        CPPUNIT_ASSERT_EQUAL(extension_set.size(), actual_extension_set.size());

        for(std::size_t j = 0 ; j < extension_set.size() ; ++j)
        {
            CPPUNIT_ASSERT_EQUAL(extension_set[j], actual_extension_set[j]);
        }
    }
}

//------------------------------------------------------------------------------

void ReaderTest::wildcardTest()
{
    std::vector<Backend> backends {
        io::bitmap::nvJPEG() ? Backend::NVJPEG : Backend::LIBJPEG,
        io::bitmap::nvJPEG2K() ? Backend::NVJPEG2K : Backend::OPENJPEG,
        io::bitmap::nvJPEG2K() ? Backend::NVJPEG2K_J2K : Backend::OPENJPEG_J2K,
        Backend::LIBTIFF,
        Backend::LIBPNG
    };

    static constexpr auto JPEG_LABEL {"JPEG image"};
    static constexpr auto TIFF_LABEL {"TIFF image"};
    static constexpr auto PNG_LABEL {"PNG image"};
    static constexpr auto J2K_LABEL {"JPEG2000 image"};

    std::vector<std::string> labels {
        JPEG_LABEL,
        J2K_LABEL,
        J2K_LABEL,
        TIFF_LABEL,
        PNG_LABEL
    };

    static constexpr auto JPEG_EXT {".jpeg"};
    static constexpr auto JPG_EXT {".jpg"};
    static constexpr auto TIF_EXT {".tif"};
    static constexpr auto TIFF_EXT {".tiff"};
    static constexpr auto PNG_EXT {".png"};
    static constexpr auto JP2_EXT {".jp2"};
    static constexpr auto J2K_EXT {".j2k"};

    std::vector<std::string> wildcards {
        std::string("*") + JPG_EXT + " *" + JPEG_EXT,
        std::string("*") + JP2_EXT,
        std::string("*") + J2K_EXT,
        std::string("*") + TIF_EXT + " *" + TIFF_EXT,
        std::string("*") + PNG_EXT
    };

    for(std::size_t index = 0 ; const auto& backend : backends)
    {
        const auto& [label, wildcard] = io::bitmap::wildcardFilter(backend);

        CPPUNIT_ASSERT_EQUAL(labels[index], label);
        CPPUNIT_ASSERT_EQUAL(wildcards[index], wildcard);
        ++index;
    }
}

//------------------------------------------------------------------------------

void ReaderTest::nvJPEGTest()
{
    if(!io::bitmap::nvJPEG())
    {
        return;
    }

    testBackend("nvJPEG.jpg", Backend::NVJPEG, getSyntheticImage(0));
}

//------------------------------------------------------------------------------

void ReaderTest::nvJPEG2KTest()
{
    if(!io::bitmap::nvJPEG2K())
    {
        return;
    }

    testBackend(
        // Pure synthetic images cannot be read/written correctly with JASPER and openCV.
        // For that reason, we use a real image.
        utestData::Data::dir() / "sight" / "image" / "bitmap" / "wild.jp2",
        Backend::NVJPEG2K
    );
}

//------------------------------------------------------------------------------

void ReaderTest::libPNGTest()
{
    testBackend("libPNG_RGB_UINT8.png", Backend::LIBPNG, getSyntheticImage(0));

    testBackend(
        "libPNG_RGBA_UINT8.png",
        Backend::LIBPNG,
        getSyntheticImage(1, core::Type::UINT8, data::Image::PixelFormat::RGBA)
    );
}

//------------------------------------------------------------------------------

void ReaderTest::libJPEGTest()
{
    testBackend("libJPEG.jpg", Backend::LIBJPEG, getSyntheticImage(0));
}

//------------------------------------------------------------------------------

void ReaderTest::openJPEGTest()
{
    testBackend(
        // Pure synthetic images cannot be read/written correctly with JASPER and openCV.
        // For that reason, we use a real image.
        utestData::Data::dir() / "sight" / "image" / "bitmap" / "wild.jp2",
        Backend::OPENJPEG
    );
}

//------------------------------------------------------------------------------

void ReaderTest::libTIFFTest()
{
    testBackend("libTIFF_RGB_UINT8.tiff", Backend::LIBTIFF, getSyntheticImage(0));

    testBackend(
        "libTIFF_RGBA_UINT16.tiff",
        Backend::LIBTIFF,
        getSyntheticImage(1, core::Type::UINT16, data::Image::PixelFormat::RGBA)
    );

    testBackend(
        "libTIFF_GRAYSCALE_DOUBLE.tiff",
        Backend::LIBTIFF,
        getSyntheticImage(2, core::Type::DOUBLE, data::Image::PixelFormat::GRAY_SCALE)
    );
}

//------------------------------------------------------------------------------

void ReaderTest::profilingTest()
{
    // Check how many loop to perform
    static const char* const env_loop   = std::getenv("PROFILETEST_LOOP");
    static const std::size_t LOOP_COUNT =
        env_loop != nullptr
        ? std::stoull(env_loop)
        : 1;

    SIGHT_INFO("Loop: " << LOOP_COUNT);

    profileReader(LOOP_COUNT, Backend::LIBJPEG);

    if(io::bitmap::nvJPEG())
    {
        profileReader(LOOP_COUNT, Backend::NVJPEG);
    }

    profileReader(LOOP_COUNT, Backend::OPENJPEG);

    if(io::bitmap::nvJPEG2K())
    {
        profileReader(LOOP_COUNT, Backend::NVJPEG2K);
    }

    profileReader(LOOP_COUNT, Backend::LIBPNG);
    profileReader(LOOP_COUNT, Backend::LIBTIFF);
}

} // namespace sight::io::bitmap::ut
