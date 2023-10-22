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
#include <core/os/temp_path.hpp>

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

inline static void test_backend(
    const std::filesystem::path& _file,
    Backend _backend,
    data::image::sptr _expected_image = data::image::sptr()
)
{
    core::os::temp_dir temp_dir;
    std::filesystem::path filepath;

    if(_expected_image)
    {
        filepath = temp_dir / _file;

        // Write a expected image to disk
        const auto& mat = image_to_mat(_expected_image);
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
    else if(std::filesystem::exists(_file) && std::filesystem::is_regular_file(_file))
    {
        filepath        = _file;
        _expected_image = read_image(filepath);
    }
    else
    {
        CPPUNIT_FAIL("File not found: " + _file.string());
    }

    auto actual_image = std::make_shared<data::image>();

    // Read the image from disk
    {
        auto reader = std::make_shared<Reader>();
        reader->set_object(actual_image);
        reader->set_file(filepath);

        CPPUNIT_ASSERT_NO_THROW(reader->read(_backend));
    }

    const auto& expected_sizes = _expected_image->size();
    const auto& actual_sizes   = actual_image->size();

    CPPUNIT_ASSERT_EQUAL(expected_sizes[0], actual_sizes[0]);
    CPPUNIT_ASSERT_EQUAL(expected_sizes[1], actual_sizes[1]);
    CPPUNIT_ASSERT_EQUAL(expected_sizes[2], actual_sizes[2]);
    CPPUNIT_ASSERT_EQUAL(_expected_image->getPixelFormat(), actual_image->getPixelFormat());
    CPPUNIT_ASSERT_EQUAL(_expected_image->getType(), actual_image->getType());

    if(_backend == Backend::LIBJPEG || _backend == Backend::NVJPEG)
    {
        CPPUNIT_ASSERT(compute_psnr(_expected_image, actual_image) > 20.0);
    }
    else
    {
        CPPUNIT_ASSERT(*_expected_image == *actual_image);
    }

    // test non existant file
    {
        constexpr auto path = "<<[{:}]>>";

        auto reader = std::make_shared<Reader>();
        reader->set_object(actual_image);
        reader->set_file(path);

        CPPUNIT_ASSERT_THROW(reader->read(_backend), core::exception);
    }

    // test existing corrupted file
    {
        core::os::temp_file corrupted_file(std::ios_base::out);
        corrupted_file << "This is a corrupted file";
        corrupted_file.stream().close();

        auto reader = std::make_shared<Reader>();
        reader->set_object(actual_image);
        reader->set_file(corrupted_file);

        CPPUNIT_ASSERT_THROW(reader->read(_backend), core::exception);
    }

    // test file with bad extension
    if(std::filesystem::exists(filepath))
    {
        auto reader = std::make_shared<Reader>();
        reader->set_object(actual_image);
        reader->set_file(filepath.replace_extension(".bad"));

        CPPUNIT_ASSERT_THROW(reader->read(_backend), core::exception);
    }
}

//------------------------------------------------------------------------------

inline static void profile_reader(
    std::size_t _loop,
    Backend _backend
)
{
    auto reader = std::make_shared<Reader>();

    auto actual_image = std::make_shared<data::image>();
    reader->set_object(actual_image);

    const auto& filename = "wild" + extensions(_backend).front();
    const auto& filepath = utest_data::Data::dir() / "sight" / "image" / "bitmap" / filename;
    reader->set_file(filepath);

    const std::string backend_name =
        [&]
        {
            switch(_backend)
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
                    return "nv_jpeg_2k";

                case Backend::OPENJPEG:
                    return "openJPEG";

                default:
                    SIGHT_THROW("Unknown backend: '" << std::uint8_t(_backend) << "'");
            }
        }();

    // Now profile reading
    SIGHT_PROFILE_FUNC(
        [&]
            (std::size_t)
        {
            CPPUNIT_ASSERT_NO_THROW(reader->read(_backend));
        },
        _loop,
        backend_name,
        0.1
    );

    // Now profile openCV reading to compare
    if(!utest::Filter::ignoreSlowTests() && _loop > 1)
    {
        const auto& opencv_label = "(openCV): " + filepath.extension().string();
        SIGHT_PROFILE_FUNC(
            [&]
                (std::size_t)
            {
                CPPUNIT_ASSERT_NO_THROW(read_image(filepath));
            },
            _loop,
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
    auto reader = std::make_shared<Reader>();

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
        io::bitmap::nv_jpeg() ? Backend::NVJPEG : Backend::LIBJPEG,
        io::bitmap::nv_jpeg_2k() ? Backend::NVJPEG2K : Backend::OPENJPEG,
        io::bitmap::nv_jpeg_2k() ? Backend::NVJPEG2K_J2K : Backend::OPENJPEG_J2K,
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
        io::bitmap::nv_jpeg() ? Backend::NVJPEG : Backend::LIBJPEG,
        io::bitmap::nv_jpeg_2k() ? Backend::NVJPEG2K : Backend::OPENJPEG,
        io::bitmap::nv_jpeg_2k() ? Backend::NVJPEG2K_J2K : Backend::OPENJPEG_J2K,
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
        const auto& [label, wildcard] = io::bitmap::wildcard_filter(backend);

        CPPUNIT_ASSERT_EQUAL(labels[index], label);
        CPPUNIT_ASSERT_EQUAL(wildcards[index], wildcard);
        ++index;
    }
}

//------------------------------------------------------------------------------

void ReaderTest::nvJPEGTest()
{
    if(!io::bitmap::nv_jpeg())
    {
        return;
    }

    test_backend("nvJPEG.jpg", Backend::NVJPEG, get_synthetic_image(0));
}

//------------------------------------------------------------------------------

void ReaderTest::nvJPEG2KTest()
{
    if(!io::bitmap::nv_jpeg_2k())
    {
        return;
    }

    test_backend(
        // Pure synthetic images cannot be read/written correctly with JASPER and openCV.
        // For that reason, we use a real image.
        utest_data::Data::dir() / "sight" / "image" / "bitmap" / "wild.jp2",
        Backend::NVJPEG2K
    );
}

//------------------------------------------------------------------------------

void ReaderTest::libPNGTest()
{
    test_backend("libPNG_RGB_UINT8.png", Backend::LIBPNG, get_synthetic_image(0));

    test_backend(
        "libPNG_RGBA_UINT8.png",
        Backend::LIBPNG,
        get_synthetic_image(1, core::type::UINT8, data::image::PixelFormat::RGBA)
    );
}

//------------------------------------------------------------------------------

void ReaderTest::libJPEGTest()
{
    test_backend("libJPEG.jpg", Backend::LIBJPEG, get_synthetic_image(0));
}

//------------------------------------------------------------------------------

void ReaderTest::openJPEGTest()
{
    test_backend(
        // Pure synthetic images cannot be read/written correctly with JASPER and openCV.
        // For that reason, we use a real image.
        utest_data::Data::dir() / "sight" / "image" / "bitmap" / "wild.jp2",
        Backend::OPENJPEG
    );
}

//------------------------------------------------------------------------------

void ReaderTest::libTIFFTest()
{
    test_backend("libTIFF_RGB_UINT8.tiff", Backend::LIBTIFF, get_synthetic_image(0));

    test_backend(
        "libTIFF_RGBA_UINT16.tiff",
        Backend::LIBTIFF,
        get_synthetic_image(1, core::type::UINT16, data::image::PixelFormat::RGBA)
    );

    test_backend(
        "libTIFF_GRAYSCALE_DOUBLE.tiff",
        Backend::LIBTIFF,
        get_synthetic_image(2, core::type::DOUBLE, data::image::PixelFormat::GRAY_SCALE)
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

    profile_reader(LOOP_COUNT, Backend::LIBJPEG);

    if(io::bitmap::nv_jpeg())
    {
        profile_reader(LOOP_COUNT, Backend::NVJPEG);
    }

    profile_reader(LOOP_COUNT, Backend::OPENJPEG);

    if(io::bitmap::nv_jpeg_2k())
    {
        profile_reader(LOOP_COUNT, Backend::NVJPEG2K);
    }

    profile_reader(LOOP_COUNT, Backend::LIBPNG);
    profile_reader(LOOP_COUNT, Backend::LIBTIFF);
}

} // namespace sight::io::bitmap::ut
