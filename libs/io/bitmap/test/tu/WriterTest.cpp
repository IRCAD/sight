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

#include "WriterTest.hpp"
#include "helper.hxx"

#include <core/profiling.hpp>
#include <core/os/temp_path.hpp>
#include <core/tools/uuid.hpp>

#include <io/bitmap/Writer.hpp>
#include <io/dicom/Reader.hpp>

#include <utest/Filter.hpp>
#include <utest/profiling.hpp>

#include <cstdlib>
#include <future>

// This is for putenv() which is part of <cstdlib>
// cspell:ignore hicpp nvjpeg LIBJPEG LIBTIFF LUMA Acuson IMWRITE IMREAD ANYDEPTH ANYCOLOR OPENCV stoull
// NOLINTNEXTLINE(hicpp-deprecated-headers,modernize-deprecated-headers)
#include <stdlib.h>

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION(sight::io::bitmap::ut::WriterTest);

namespace sight::io::bitmap::ut
{

//------------------------------------------------------------------------------

inline static std::vector<data::image::sptr> read_dicom_images(std::size_t _count = 3)
{
    static const std::vector<data::image::sptr> result =
        []
        {
            std::vector<data::image::sptr> images;
            auto series_set = std::make_shared<data::series_set>();
            auto reader     = std::make_shared<io::dicom::Reader>();

            reader->set_object(series_set);

            // Read a DICOM image "us/Ultrasound Image Storage/GE, lossy JPEG"
            reader->set_folder(utest_data::Data::dir() / "us/Ultrasound Image Storage/GE, lossy JPEG");
            CPPUNIT_ASSERT_NO_THROW(reader->read());

            // Just to be sure we read the good data
            CPPUNIT_ASSERT_EQUAL(std::size_t(1), series_set->size());

            auto image_series = std::dynamic_pointer_cast<data::image_series>(series_set->at(0));
            CPPUNIT_ASSERT(image_series);

            auto size = image_series->size();
            CPPUNIT_ASSERT_EQUAL(std::size_t(636), size[0]);
            CPPUNIT_ASSERT_EQUAL(std::size_t(434), size[1]);
            CPPUNIT_ASSERT_EQUAL(std::size_t(1), size[2]);

            images.push_back(image_series);
            series_set->clear();

            // Read next image "us/Ultrasound Image Storage/Siemens Acuson 500"
            reader->set_folder(utest_data::Data::dir() / "us/Ultrasound Image Storage/Siemens Acuson 500");
            CPPUNIT_ASSERT_NO_THROW(reader->read());

            // Just to be sure we read the good data
            CPPUNIT_ASSERT_EQUAL(std::size_t(1), series_set->size());

            image_series = std::dynamic_pointer_cast<data::image_series>(series_set->at(0));
            CPPUNIT_ASSERT(image_series);

            size = image_series->size();
            CPPUNIT_ASSERT_EQUAL(std::size_t(800), size[0]);
            CPPUNIT_ASSERT_EQUAL(std::size_t(600), size[1]);
            CPPUNIT_ASSERT_EQUAL(std::size_t(1), size[2]);

            images.push_back(image_series);
            series_set->clear();

            // Read next image "us/Ultrasound Multi-frame Image Storage/Siemens Acuson 500"
            reader->set_folder(utest_data::Data::dir() / "us/Ultrasound Multi-frame Image Storage/Siemens Acuson 500");
            CPPUNIT_ASSERT_NO_THROW(reader->read());

            // Just to be sure we read the good data
            CPPUNIT_ASSERT_EQUAL(std::size_t(1), series_set->size());

            image_series = std::dynamic_pointer_cast<data::image_series>(series_set->at(0));
            CPPUNIT_ASSERT(image_series);

            size = image_series->size();
            CPPUNIT_ASSERT_EQUAL(std::size_t(800), size[0]);
            CPPUNIT_ASSERT_EQUAL(std::size_t(600), size[1]);
            CPPUNIT_ASSERT_EQUAL(std::size_t(60), size[2]);

            images.push_back(image_series);
            return images;
        }();

    auto copy = result;
    copy.resize(_count);
    return copy;
}

//------------------------------------------------------------------------------

inline static void profile_writer(
    const std::vector<data::image::sptr>& _images,
    const std::filesystem::path& _tmp_folder,
    std::size_t _loop,
    Backend _backend,
    Writer::Mode _mode,
    std::vector<std::future<void> >& _tasks
)
{
    auto writer = std::make_shared<Writer>();

    const auto [BACKEND, EXT] = backend_to_string(_backend);
    const std::string mode = mode_to_string(_mode);

    const std::string file_suffix = "_" + BACKEND + "_" + mode + EXT;
    const std::string label       = BACKEND + " (" + mode + ")";

    // Get size, psnr, ...
    for(std::size_t i = 0 ; const auto& image : _images)
    {
        // Write image
        writer->set_object(image);
        const auto& tmp_path = _tmp_folder / (std::to_string(i) + file_suffix);
        writer->set_file(tmp_path);
        CPPUNIT_ASSERT_NO_THROW(writer->write(_backend, _mode));

        SIGHT_INFO(label << " size: " << std::filesystem::file_size(tmp_path));

        // PSNR is only relevant with lossy format...
        if(EXT.ends_with("jpg") || EXT.ends_with(".jpeg"))
        {
            SIGHT_INFO(label << " PSNR: " << compute_psnr(image, read_image(tmp_path)));
        }

        // Cleanup
        std::filesystem::remove_all(tmp_path);
        ++i;
    }

    // Now profile writing
    SIGHT_PROFILE_FUNC(
        [&]
            (std::size_t i)
        {
            for(std::size_t j = 0 ; const auto& image : _images)
            {
                writer->set_object(image);
                const auto& tmp_path = _tmp_folder / (std::to_string(i) + "_" + std::to_string(j++) + file_suffix);
                writer->set_file(tmp_path);

                CPPUNIT_ASSERT_NO_THROW(writer->write(_backend, _mode));

                // Schedule cleanup
                _tasks.emplace_back(std::async(std::launch::deferred, [ = ]{std::filesystem::remove_all(tmp_path);}));
            }
        },
        _loop,
        label,
        0.1
    );

    // Wait for all file delete task to finish
    while(!_tasks.empty())
    {
        _tasks.back().wait();
        _tasks.pop_back();
    }
}

//------------------------------------------------------------------------------

inline static void profile_open_cv_writer(
    const std::vector<data::image::sptr>& _images,
    const std::filesystem::path& _tmp_folder,
    std::size_t _loop,
    std::string _ext,
    Writer::Mode _mode,
    std::vector<std::future<void> >& _tasks
)
{
    const std::string mode = mode_to_string(_mode);

    const std::string file_suffix = "_OPENCV_" + mode + "." + _ext;
    const std::string label       = "OpenCV (" + _ext + " - " + mode + ")";

    // Get size, psnr, ...
    for(std::size_t i = 0 ; const auto& image : _images)
    {
        const auto& tmp_path = _tmp_folder / (std::to_string(i++) + file_suffix);

        // Convert Image to OpenCV Mat
        const cv::Mat& mat = image_to_mat(image);

        // Write image
        if(_mode == Writer::Mode::BEST)
        {
            CPPUNIT_ASSERT(
                cv::imwrite(
                    tmp_path.string(),
                    mat,
                {
                    cv::IMWRITE_JPEG_QUALITY, 100,
                    cv::IMWRITE_JPEG_CHROMA_QUALITY, 100,
                    cv::IMWRITE_JPEG_LUMA_QUALITY, 100,
                    cv::IMWRITE_JPEG_OPTIMIZE, 1,
                    cv::IMWRITE_PNG_COMPRESSION, 9,
                    cv::IMWRITE_PNG_STRATEGY, cv::IMWRITE_PNG_STRATEGY_DEFAULT
                })
            );
        }
        else
        {
            CPPUNIT_ASSERT(cv::imwrite(tmp_path.string(), mat));
        }

        SIGHT_INFO(label << " size: " << std::filesystem::file_size(tmp_path));

        if(_ext.ends_with("jpg") || _ext.ends_with(".jpeg"))
        {
            SIGHT_INFO(label << " PSNR: " << compute_psnr(image, read_image(tmp_path)));
        }

        // Cleanup
        std::filesystem::remove_all(tmp_path);
    }

    // Now profile writing
    SIGHT_PROFILE_FUNC(
        [&]
            (std::size_t i)
        {
            for(std::size_t j = 0 ; const auto& image : _images)
            {
                const auto& tmp_path = _tmp_folder / (std::to_string(i) + "_" + std::to_string(j++) + file_suffix);

                // Convert Image to OpenCV Mat
                const cv::Mat& mat = image_to_mat(image);

                // Write image
                if(_mode == Writer::Mode::BEST)
                {
                    CPPUNIT_ASSERT(
                        cv::imwrite(
                            tmp_path.string(),
                            mat,
                    {
                        cv::IMWRITE_JPEG_QUALITY, 100,
                        cv::IMWRITE_JPEG_CHROMA_QUALITY, 100,
                        cv::IMWRITE_JPEG_LUMA_QUALITY, 100,
                        cv::IMWRITE_JPEG_OPTIMIZE, 1,
                        cv::IMWRITE_PNG_COMPRESSION, 9,
                        cv::IMWRITE_PNG_STRATEGY, cv::IMWRITE_PNG_STRATEGY_DEFAULT
                    })
                    );
                }
                else
                {
                    CPPUNIT_ASSERT(cv::imwrite(tmp_path.string(), mat));
                }

                // Schedule cleanup
                _tasks.emplace_back(std::async(std::launch::deferred, [ = ]{std::filesystem::remove_all(tmp_path);}));
            }
        },
        _loop,
        label,
        0.1
    );

    // Wait for all file delete task to finish
    while(!_tasks.empty())
    {
        _tasks.back().wait();
        _tasks.pop_back();
    }
}

//------------------------------------------------------------------------------

void WriterTest::setUp()
{
    std::string jasper("OPENCV_IO_ENABLE_JASPER=1");
    putenv(jasper.data());
}

//------------------------------------------------------------------------------

void WriterTest::tearDown()
{
}

//------------------------------------------------------------------------------

void WriterTest::basicTest()
{
    auto writer = std::make_shared<Writer>();

    CPPUNIT_ASSERT_EQUAL(io::bitmap::extensions(Backend::LIBTIFF).front(), writer->extension());
}

//------------------------------------------------------------------------------

void WriterTest::extensionsTest()
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

void WriterTest::wildcardTest()
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

inline static void conformance(
    const std::vector<Backend>& _supported,
    const std::vector<Backend>& _unsupported,
    core::type _type,
    data::image::PixelFormat _format
)
{
    // Create a temporary directory
    core::os::temp_dir tmp_dir;

    // Create the synthetic image
    const auto& expected_image = get_synthetic_image(0, _type, _format);

    // Create the writer
    auto writer = std::make_shared<Writer>();
    writer->set_object(expected_image);

    // Build mode list
    const std::vector modes {
        Writer::Mode::BEST,
        Writer::Mode::FAST
    };

    // For each backend and each mode
    for(const auto& backend : _supported)
    {
        const std::string& backend_string = backend_to_string(backend).first;

        for(const auto& mode : modes)
        {
            // Test write
            const auto& file_path = tmp_dir / ("conformance" + file_suffix(backend, mode));
            CPPUNIT_ASSERT_NO_THROW(writer->set_file(file_path));
            CPPUNIT_ASSERT_NO_THROW(writer->write(backend, mode));
            CPPUNIT_ASSERT_MESSAGE(file_path.string() + " doesn't exist.", std::filesystem::exists(file_path));

            const auto& actual_image = read_image(file_path.string());
            CPPUNIT_ASSERT(actual_image);

            if(backend == Backend::OPENJPEG || backend == Backend::NVJPEG2K)
            {
                // Because of bad encoder or decoder implementation, JPEG2000 is not always *perfectly* lossless.
                // Indeed, it is mathematically, but the implementation can suffer from some corner floating point
                // rounding errors. This seems to be the case with openCV/Jasper plugin, at least, in this test
                // which uses synthetic data. One component value differ 0x254 instead of 0x253
                // As a workaround, we try to encode two times, and we consider success if multiple pass doesn't
                // degrade the situation more.
                writer->set_object(actual_image);
                const auto& copy_file_path = tmp_dir / ("conformance_copy" + file_suffix(backend, mode));
                CPPUNIT_ASSERT_NO_THROW(writer->set_file(copy_file_path));
                CPPUNIT_ASSERT_NO_THROW(writer->write(backend, mode));
                CPPUNIT_ASSERT_MESSAGE(
                    copy_file_path.string() + " doesn't exist.",
                    std::filesystem::exists(copy_file_path)
                );

                const auto& actual_image_copy = read_image(copy_file_path.string());
                CPPUNIT_ASSERT(actual_image_copy);

                CPPUNIT_ASSERT_MESSAGE(
                    "The image are not equal for backend '" + backend_string + "', mode '" + mode_to_string(mode)
                    + "', format '" + pixel_format_to_string(_format) + "', type '" + _type.name() + "'",
                    *expected_image == *actual_image
                );

                // Restore back the original source
                writer->set_object(expected_image);
            }
            // Compare pixels only for lossless backend
            else if(backend != Backend::LIBJPEG && backend != Backend::NVJPEG)
            {
                CPPUNIT_ASSERT_MESSAGE(
                    "The image are not equal for backend '" + backend_string + "', mode '" + mode_to_string(mode)
                    + "', format '" + pixel_format_to_string(_format) + "', type '" + _type.name() + "'",
                    *expected_image == *actual_image
                );
            }
            else
            {
                // Compare at least sizes...
                const auto& expected_size = expected_image->size();
                const auto& actual_size   = actual_image->size();
                CPPUNIT_ASSERT_EQUAL(expected_size[0], actual_size[0]);
                CPPUNIT_ASSERT_EQUAL(expected_size[1], actual_size[1]);
                CPPUNIT_ASSERT_EQUAL(expected_size[2], actual_size[2]);
                CPPUNIT_ASSERT_EQUAL(expected_image->getPixelFormat(), actual_image->getPixelFormat());
                CPPUNIT_ASSERT_EQUAL(expected_image->getType(), actual_image->getType());

                // Ensure that psnr is at least > 20
                const double psnr = compute_psnr(expected_image, actual_image);
                CPPUNIT_ASSERT_MESSAGE(
                    "The image seems to be different with backend '"
                    + backend_string
                    + "', PSNR="
                    + std::to_string(psnr)
                    + "dB",
                    psnr > 20
                );
            }
        }
    }

    // For each backend and each mode
    for(const auto& backend : _unsupported)
    {
        for(const auto& mode : modes)
        {
            // Test write
            const auto& file_path = tmp_dir / ("conformance" + file_suffix(backend, mode));
            CPPUNIT_ASSERT_NO_THROW(writer->set_file(file_path));
            CPPUNIT_ASSERT_THROW(writer->write(backend, mode), core::exception);
        }
    }
}

//------------------------------------------------------------------------------

void WriterTest::conformanceTest()
{
    // UINT8 RGB
    {
        if(io::bitmap::nv_jpeg_2k())
        {
            conformance(
                {
                    Backend::LIBJPEG,
                    Backend::LIBPNG,
                    Backend::LIBTIFF,
                    Backend::OPENJPEG,
                    Backend::NVJPEG2K,
                    Backend::NVJPEG
                },
                {},
                core::type::UINT8,
                data::image::PixelFormat::RGB
            );
        }
        else if(io::bitmap::nv_jpeg())
        {
            conformance(
                {Backend::LIBJPEG, Backend::LIBPNG, Backend::LIBTIFF, Backend::OPENJPEG, Backend::NVJPEG},
                {Backend::NVJPEG2K},
                core::type::UINT8,
                data::image::PixelFormat::RGB
            );
        }
        else
        {
            conformance(
                {Backend::LIBJPEG, Backend::LIBPNG, Backend::LIBTIFF, Backend::OPENJPEG},
                {Backend::NVJPEG2K, Backend::NVJPEG},
                core::type::UINT8,
                data::image::PixelFormat::RGB
            );
        }
    }

    // UINT8 GRAYSCALE
    {
        if(io::bitmap::nv_jpeg_2k())
        {
            conformance(
                {Backend::LIBJPEG, Backend::LIBPNG, Backend::LIBTIFF, Backend::OPENJPEG, Backend::NVJPEG2K},
                {Backend::NVJPEG},
                core::type::UINT8,
                data::image::PixelFormat::GRAY_SCALE
            );
        }
        else
        {
            conformance(
                {Backend::LIBJPEG, Backend::LIBPNG, Backend::LIBTIFF, Backend::OPENJPEG},
                {Backend::NVJPEG2K, Backend::NVJPEG},
                core::type::UINT8,
                data::image::PixelFormat::GRAY_SCALE
            );
        }
    }

    // UINT16 RGB
    {
        if(io::bitmap::nv_jpeg_2k())
        {
            conformance(
                {Backend::LIBTIFF, Backend::OPENJPEG, Backend::NVJPEG2K},
                {Backend::LIBJPEG, Backend::NVJPEG},
                core::type::UINT16,
                data::image::PixelFormat::RGB
            );
        }
        else
        {
            conformance(
                {Backend::LIBPNG, Backend::LIBTIFF, Backend::OPENJPEG},
                {Backend::LIBJPEG, Backend::NVJPEG2K, Backend::NVJPEG},
                core::type::UINT16,
                data::image::PixelFormat::RGB
            );
        }
    }

    // UINT16 GRAYSCALE
    {
        if(io::bitmap::nv_jpeg_2k())
        {
            conformance(
                {Backend::LIBPNG, Backend::LIBTIFF, Backend::OPENJPEG, Backend::NVJPEG2K},
                {Backend::LIBJPEG, Backend::NVJPEG},
                core::type::UINT16,
                data::image::PixelFormat::GRAY_SCALE
            );
        }
        else
        {
            conformance(
                {Backend::LIBPNG, Backend::LIBTIFF, Backend::OPENJPEG},
                {Backend::LIBJPEG, Backend::NVJPEG2K, Backend::NVJPEG},
                core::type::UINT16,
                data::image::PixelFormat::GRAY_SCALE
            );
        }
    }
}

//------------------------------------------------------------------------------

void WriterTest::emptyImageTest()
{
    // Create a temporary directory
    core::os::temp_dir tmp_dir;

    const auto& empty_image = std::make_shared<data::image>();
    auto writer             = std::make_shared<Writer>();
    writer->set_object(empty_image);

    std::vector<std::string> extensions {".jpeg", ".tiff", ".png", ".jp2", ".j2k"};

    for(const auto& ext : extensions)
    {
        const auto& tmp_path = tmp_dir / ("empty" + ext);
        CPPUNIT_ASSERT_NO_THROW(writer->set_file(tmp_path));
        CPPUNIT_ASSERT_THROW(writer->write(), core::exception);
        CPPUNIT_ASSERT_MESSAGE(tmp_path.string() + " exists.", !std::filesystem::exists(tmp_path));
    }
}

//------------------------------------------------------------------------------

void WriterTest::wrongPathTest()
{
    // Create a temporary directory, with a non existing leaf directory
    core::os::temp_dir tmp_dir;
    const auto tmp_folder = tmp_dir / UUID::generate();
    std::filesystem::remove_all(tmp_folder);

    const auto& image_series = read_dicom_images(1);
    auto writer              = std::make_shared<Writer>();
    writer->set_object(image_series.front());

    std::vector<std::string> extensions {".jpeg", ".tiff", ".png", ".jp2", ".j2k"};

    for(const auto& ext : extensions)
    {
        const auto& tmp_path = tmp_folder / ("wrong_path" + ext);
        CPPUNIT_ASSERT_NO_THROW(writer->set_file(tmp_path));
        CPPUNIT_ASSERT_NO_THROW(writer->write());
        CPPUNIT_ASSERT_MESSAGE(tmp_path.string() + " doesn't exist.", std::filesystem::exists(tmp_path));
    }
}

//------------------------------------------------------------------------------

void WriterTest::fromDicomTest()
{
    const auto& image_series = read_dicom_images();

    // Create a temporary directory
    core::os::temp_dir tmp_dir;

    // Use a big epsilon since size can vary between platforms and libraries version, especially for nvJPEG2000
    static constexpr std::int64_t EPSILON = 5000;

    static constexpr std::array<std::int64_t, 3> nvjpeg_size       = {278295, 174839, 254339};
    static constexpr std::array<std::int64_t, 3> libjpeg_size      = {176726, 140868, 165298};
    static constexpr std::array<std::int64_t, 3> nvjpeg2k_jp2_size = {282577, 144367, 276889};
    static constexpr std::array<std::int64_t, 3> nvjpeg2k_jk2_size = {279052, 141242, 272350};
    static constexpr std::array<std::int64_t, 3> tiff_size         = {493572, 597782, 693950};
    static constexpr std::array<std::int64_t, 3> png_size          = {313130, 220007, 352335};
    static constexpr std::array<std::int64_t, 3> openjpeg_jp2_size = {279111, 141282, 272407};
    static constexpr std::array<std::int64_t, 3> openjpeg_jk2_size = {279111, 141282, 272407};

    auto writer = std::make_shared<Writer>();

    const auto& write =
        [&](std::size_t _i, const std::string& _ext)
        {
            const auto& tmp_path = tmp_dir / (std::to_string(_i) + "_from_dicom" + _ext);
            CPPUNIT_ASSERT_NO_THROW(writer->set_file(tmp_path));
            CPPUNIT_ASSERT_NO_THROW(writer->write());
            CPPUNIT_ASSERT(std::filesystem::exists(tmp_path));

            return std::int64_t(std::filesystem::file_size(tmp_path));
        };

    for(std::size_t i = 0 ; const auto& image : image_series)
    {
        writer->set_object(image);

        // Test .jpg with nvJPEG (if available)
        if(io::bitmap::nv_jpeg())
        {
            const auto file_size = write(i, ".jpg");

            CPPUNIT_ASSERT_MESSAGE(
                "expected[" + std::to_string(i) + "] (" + std::to_string(nvjpeg_size[i])
                + ") != actual (" + std::to_string(file_size) + ")",
                std::abs(nvjpeg_size[i] - file_size) <= EPSILON
            );
        }
        else
        {
            // Use libJPEG as fallback
            const auto file_size = write(i, ".jpg");

            CPPUNIT_ASSERT_MESSAGE(
                "expected[" + std::to_string(i) + "] (" + std::to_string(libjpeg_size[i])
                + ") != actual (" + std::to_string(file_size) + ")",
                std::abs(libjpeg_size[i] - file_size) <= EPSILON
            );
        }

        // Test .jp2 with nv_jpeg_2k (if available)
        if(io::bitmap::nv_jpeg_2k())
        {
            const auto file_size = write(i, ".jp2");

            CPPUNIT_ASSERT_MESSAGE(
                "expected[" + std::to_string(i) + "] (" + std::to_string(nvjpeg2k_jp2_size[i])
                + ") != actual (" + std::to_string(file_size) + ")",
                std::abs(nvjpeg2k_jp2_size[i] - file_size) <= EPSILON
            );
        }
        else
        {
            // Use openJPEG as fallback
            const auto file_size = write(i, ".jp2");

            CPPUNIT_ASSERT_MESSAGE(
                "expected[" + std::to_string(i) + "] (" + std::to_string(openjpeg_jp2_size[i])
                + ") != actual (" + std::to_string(file_size) + ")",
                std::abs(openjpeg_jp2_size[i] - file_size) <= EPSILON
            );
        }

        // Test .j2k with nv_jpeg_2k (if available)
        if(io::bitmap::nv_jpeg_2k())
        {
            const auto file_size = write(i, ".j2k");

            CPPUNIT_ASSERT_MESSAGE(
                "expected[" + std::to_string(i) + "] (" + std::to_string(nvjpeg2k_jk2_size[i])
                + ") != actual (" + std::to_string(file_size) + ")",
                std::abs(nvjpeg2k_jk2_size[i] - file_size) <= EPSILON
            );
        }
        else
        {
            // Use openJPEG as fallback
            const auto file_size = write(i, ".j2k");

            CPPUNIT_ASSERT_MESSAGE(
                "expected[" + std::to_string(i) + "] (" + std::to_string(openjpeg_jk2_size[i])
                + ") != actual (" + std::to_string(file_size) + ")",
                std::abs(openjpeg_jk2_size[i] - file_size) <= EPSILON
            );
        }

        // test .tiff with libTIFF
        {
            const auto file_size = write(i, ".tiff");

            CPPUNIT_ASSERT_MESSAGE(
                "expected[" + std::to_string(i) + "] (" + std::to_string(tiff_size[i])
                + ") != actual (" + std::to_string(file_size) + ")",
                std::abs(tiff_size[i] - file_size) <= EPSILON
            );
        }

        // test .png with libPNG
        {
            const auto file_size = write(i, ".png");

            CPPUNIT_ASSERT_MESSAGE(
                "expected[" + std::to_string(i) + "] (" + std::to_string(png_size[i])
                + ") != actual (" + std::to_string(file_size) + ")",
                std::abs(png_size[i] - file_size) <= EPSILON
            );
        }

        ++i;
    }
}

//------------------------------------------------------------------------------

void WriterTest::profilingTest()
{
    auto images = read_dicom_images();
    images.push_back(get_synthetic_image(0));

    // Create a temporary directory
    core::os::temp_dir tmp_dir;

    // Check how many loop to perform
    static const char* const env_loop   = std::getenv("PROFILETEST_LOOP");
    static const std::size_t LOOP_COUNT =
        env_loop != nullptr
        ? std::stoull(env_loop)
        : 1;

    SIGHT_INFO("Loop: " << LOOP_COUNT);

    std::vector<std::future<void> > tasks;

    // nvJPEG
    if(io::bitmap::nv_jpeg())
    {
        profile_writer(
            images,
            tmp_dir,
            LOOP_COUNT,
            Backend::NVJPEG,
            Writer::Mode::FAST,
            tasks
        );

        profile_writer(
            images,
            tmp_dir,
            LOOP_COUNT,
            Backend::NVJPEG,
            Writer::Mode::BEST,
            tasks
        );
    }

    // nv_jpeg_2k
    if(io::bitmap::nv_jpeg_2k())
    {
        profile_writer(
            images,
            tmp_dir,
            LOOP_COUNT,
            Backend::NVJPEG2K,
            Writer::Mode::FAST,
            tasks
        );

        profile_writer(
            images,
            tmp_dir,
            LOOP_COUNT,
            Backend::NVJPEG2K,
            Writer::Mode::BEST,
            tasks
        );
    }

    // libJPEG
    {
        profile_writer(
            images,
            tmp_dir,
            LOOP_COUNT,
            Backend::LIBJPEG,
            Writer::Mode::FAST,
            tasks
        );

        profile_writer(
            images,
            tmp_dir,
            LOOP_COUNT,
            Backend::LIBJPEG,
            Writer::Mode::BEST,
            tasks
        );
    }

    // libTIFF
    {
        profile_writer(
            images,
            tmp_dir,
            LOOP_COUNT,
            Backend::LIBTIFF,
            Writer::Mode::FAST,
            tasks
        );

        profile_writer(
            images,
            tmp_dir,
            LOOP_COUNT,
            Backend::LIBTIFF,
            Writer::Mode::BEST,
            tasks
        );
    }

    // libPNG
    {
        profile_writer(
            images,
            tmp_dir,
            LOOP_COUNT,
            Backend::LIBPNG,
            Writer::Mode::FAST,
            tasks
        );

        profile_writer(
            images,
            tmp_dir,
            LOOP_COUNT,
            Backend::LIBPNG,
            Writer::Mode::BEST,
            tasks
        );
    }

    // openjpeg
    {
        profile_writer(
            images,
            tmp_dir,
            LOOP_COUNT,
            Backend::OPENJPEG,
            Writer::Mode::FAST,
            tasks
        );
    }

    if(!utest::Filter::ignoreSlowTests() && env_loop != nullptr)
    {
        // Use OPENCV JPEG
        profile_open_cv_writer(
            images,
            tmp_dir,
            LOOP_COUNT,
            "jpg",
            Writer::Mode::FAST,
            tasks
        );

        profile_open_cv_writer(
            images,
            tmp_dir,
            LOOP_COUNT,
            "jpg",
            Writer::Mode::BEST,
            tasks
        );

        // Use OPENCV TIFF
        profile_open_cv_writer(
            images,
            tmp_dir,
            LOOP_COUNT,
            "tiff",
            Writer::Mode::FAST,
            tasks
        );

        // Use OPENCV PNG
        profile_open_cv_writer(
            images,
            tmp_dir,
            LOOP_COUNT,
            "png",
            Writer::Mode::FAST,
            tasks
        );

        profile_open_cv_writer(
            images,
            tmp_dir,
            LOOP_COUNT,
            "png",
            Writer::Mode::BEST,
            tasks
        );

        // Use OPENCV WEBP
        profile_open_cv_writer(
            images,
            tmp_dir,
            LOOP_COUNT,
            "webp",
            Writer::Mode::FAST,
            tasks
        );
    }

    for(const auto& task : tasks)
    {
        task.wait();
    }
}

} // namespace sight::io::bitmap::ut
