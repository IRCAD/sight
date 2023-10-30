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
#include <io/dicom/reader/file.hpp>

#include <utest/filter.hpp>
#include <utest/profiling.hpp>

#include <cstdlib>
#include <future>

// This is for putenv() which is part of <cstdlib>
// cspell:ignore hicpp nvjpeg LIBJPEG LIBTIFF LUMA Acuson IMWRITE IMREAD ANYDEPTH ANYCOLOR OPENCV stoull
// NOLINTNEXTLINE(hicpp-deprecated-headers,modernize-deprecated-headers)
#include <stdlib.h>

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION(sight::io::bitmap::ut::writer_test);

namespace sight::io::bitmap::ut
{

//------------------------------------------------------------------------------

inline static std::vector<data::image::sptr> read_dicom_images(std::size_t _count = 3)
{
    static const std::vector<data::image::sptr> s_RESULT =
        []
        {
            std::vector<data::image::sptr> images;
            auto series_set = std::make_shared<data::series_set>();
            auto reader     = std::make_shared<io::dicom::reader::file>();

            reader->set_object(series_set);

            // Read a DICOM image "us/Ultrasound Image Storage/GE, lossy JPEG"
            reader->set_folder(utest_data::dir() / "us/Ultrasound Image Storage/GE, lossy JPEG");
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
            reader->set_folder(utest_data::dir() / "us/Ultrasound Image Storage/Siemens Acuson 500");
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
            reader->set_folder(utest_data::dir() / "us/Ultrasound Multi-frame Image Storage/Siemens Acuson 500");
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

    auto copy = s_RESULT;
    copy.resize(_count);
    return copy;
}

//------------------------------------------------------------------------------

inline static void profile_writer(
    const std::vector<data::image::sptr>& _images,
    const std::filesystem::path& _tmp_folder,
    std::size_t _loop,
    backend _backend,
    writer::mode _mode,
    std::vector<std::future<void> >& _tasks
)
{
    auto writer = std::make_shared<io::bitmap::writer>();

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
    writer::mode _mode,
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
        if(_mode == writer::mode::best)
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
                if(_mode == writer::mode::best)
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

void writer_test::setUp()
{
    std::string jasper("OPENCV_IO_ENABLE_JASPER=1");
    putenv(jasper.data());
}

//------------------------------------------------------------------------------

void writer_test::tearDown()
{
}

//------------------------------------------------------------------------------

void writer_test::basic_test()
{
    auto writer = std::make_shared<io::bitmap::writer>();

    CPPUNIT_ASSERT_EQUAL(io::bitmap::extensions(backend::libtiff).front(), writer->extension());
}

//------------------------------------------------------------------------------

void writer_test::extensions_test()
{
    std::vector<data::sequenced_set<std::string> > extensions {
        {".jpg", ".jpeg"},
        {".jp2"},
        {".j2k"},
        {".tiff", ".tif"},
        {".png"}
    };

    std::vector<backend> backends {
        io::bitmap::nv_jpeg() ? backend::nvjpeg : backend::libjpeg,
        io::bitmap::nv_jpeg_2k() ? backend::nvjpeg2k : backend::openjpeg,
        io::bitmap::nv_jpeg_2k() ? backend::nvjpeg2k_j2k : backend::openjpeg_j2_k,
        backend::libtiff,
        backend::libpng
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

void writer_test::wildcard_test()
{
    std::vector<backend> backends {
        io::bitmap::nv_jpeg() ? backend::nvjpeg : backend::libjpeg,
        io::bitmap::nv_jpeg_2k() ? backend::nvjpeg2k : backend::openjpeg,
        io::bitmap::nv_jpeg_2k() ? backend::nvjpeg2k_j2k : backend::openjpeg_j2_k,
        backend::libtiff,
        backend::libpng
    };

    static constexpr auto s_JPEG_LABEL {"JPEG image"};
    static constexpr auto s_TIFF_LABEL {"TIFF image"};
    static constexpr auto s_PNG_LABEL {"PNG image"};
    static constexpr auto s_J2_K_LABEL {"JPEG2000 image"};

    std::vector<std::string> labels {
        s_JPEG_LABEL,
        s_J2_K_LABEL,
        s_J2_K_LABEL,
        s_TIFF_LABEL,
        s_PNG_LABEL
    };

    static constexpr auto s_JPEG_EXT {".jpeg"};
    static constexpr auto s_JPG_EXT {".jpg"};
    static constexpr auto s_TIF_EXT {".tif"};
    static constexpr auto s_TIFF_EXT {".tiff"};
    static constexpr auto s_PNG_EXT {".png"};
    static constexpr auto s_J_P2_EXT {".jp2"};
    static constexpr auto s_J2_K_EXT {".j2k"};

    std::vector<std::string> wildcards {
        std::string("*") + s_JPG_EXT + " *" + s_JPEG_EXT,
        std::string("*") + s_J_P2_EXT,
        std::string("*") + s_J2_K_EXT,
        std::string("*") + s_TIF_EXT + " *" + s_TIFF_EXT,
        std::string("*") + s_PNG_EXT
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
    const std::vector<backend>& _supported,
    const std::vector<backend>& _unsupported,
    core::type _type,
    enum data::image::pixel_format _format
)
{
    // Create a temporary directory
    core::os::temp_dir tmp_dir;

    // Create the synthetic image
    const auto& expected_image = get_synthetic_image(0, _type, _format);

    // Create the writer
    auto writer = std::make_shared<io::bitmap::writer>();
    writer->set_object(expected_image);

    // Build mode list
    const std::vector modes {
        writer::mode::best,
        writer::mode::fast
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

            if(backend == backend::openjpeg || backend == backend::nvjpeg2k)
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
            else if(backend != backend::libjpeg && backend != backend::nvjpeg)
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
                CPPUNIT_ASSERT_EQUAL(expected_image->pixel_format(), actual_image->pixel_format());
                CPPUNIT_ASSERT_EQUAL(expected_image->type(), actual_image->type());

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

void writer_test::conformance_test()
{
    // UINT8 RGB
    {
        if(io::bitmap::nv_jpeg_2k())
        {
            conformance(
                {
                    backend::libjpeg,
                    backend::libpng,
                    backend::libtiff,
                    backend::openjpeg,
                    backend::nvjpeg2k,
                    backend::nvjpeg
                },
                {},
                core::type::UINT8,
                data::image::pixel_format::rgb
            );
        }
        else if(io::bitmap::nv_jpeg())
        {
            conformance(
                {backend::libjpeg, backend::libpng, backend::libtiff, backend::openjpeg, backend::nvjpeg},
                {backend::nvjpeg2k},
                core::type::UINT8,
                data::image::pixel_format::rgb
            );
        }
        else
        {
            conformance(
                {backend::libjpeg, backend::libpng, backend::libtiff, backend::openjpeg},
                {backend::nvjpeg2k, backend::nvjpeg},
                core::type::UINT8,
                data::image::pixel_format::rgb
            );
        }
    }

    // UINT8 GRAYSCALE
    {
        if(io::bitmap::nv_jpeg_2k())
        {
            conformance(
                {backend::libjpeg, backend::libpng, backend::libtiff, backend::openjpeg, backend::nvjpeg2k},
                {backend::nvjpeg},
                core::type::UINT8,
                data::image::pixel_format::gray_scale
            );
        }
        else
        {
            conformance(
                {backend::libjpeg, backend::libpng, backend::libtiff, backend::openjpeg},
                {backend::nvjpeg2k, backend::nvjpeg},
                core::type::UINT8,
                data::image::pixel_format::gray_scale
            );
        }
    }

    // UINT16 RGB
    {
        if(io::bitmap::nv_jpeg_2k())
        {
            conformance(
                {backend::libtiff, backend::openjpeg, backend::nvjpeg2k},
                {backend::libjpeg, backend::nvjpeg},
                core::type::UINT16,
                data::image::pixel_format::rgb
            );
        }
        else
        {
            conformance(
                {backend::libpng, backend::libtiff, backend::openjpeg},
                {backend::libjpeg, backend::nvjpeg2k, backend::nvjpeg},
                core::type::UINT16,
                data::image::pixel_format::rgb
            );
        }
    }

    // UINT16 GRAYSCALE
    {
        if(io::bitmap::nv_jpeg_2k())
        {
            conformance(
                {backend::libpng, backend::libtiff, backend::openjpeg, backend::nvjpeg2k},
                {backend::libjpeg, backend::nvjpeg},
                core::type::UINT16,
                data::image::pixel_format::gray_scale
            );
        }
        else
        {
            conformance(
                {backend::libpng, backend::libtiff, backend::openjpeg},
                {backend::libjpeg, backend::nvjpeg2k, backend::nvjpeg},
                core::type::UINT16,
                data::image::pixel_format::gray_scale
            );
        }
    }
}

//------------------------------------------------------------------------------

void writer_test::empty_image_test()
{
    // Create a temporary directory
    core::os::temp_dir tmp_dir;

    const auto& empty_image = std::make_shared<data::image>();
    auto writer             = std::make_shared<io::bitmap::writer>();
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

void writer_test::wrong_path_test()
{
    // Create a temporary directory, with a non existing leaf directory
    core::os::temp_dir tmp_dir;
    const auto tmp_folder = tmp_dir / uuid::generate();
    std::filesystem::remove_all(tmp_folder);

    const auto& image_series = read_dicom_images(1);
    auto writer              = std::make_shared<io::bitmap::writer>();
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

void writer_test::from_dicom_test()
{
    const auto& image_series = read_dicom_images();

    // Create a temporary directory
    core::os::temp_dir tmp_dir;

    // Use a big epsilon since size can vary between platforms and libraries version, especially for nvJPEG2000
    static constexpr std::int64_t s_EPSILON = 5000;

    static constexpr std::array<std::int64_t, 3> s_NVJPEG_SIZE       = {278295, 174839, 254339};
    static constexpr std::array<std::int64_t, 3> s_LIBJPEG_SIZE      = {176726, 140868, 165298};
    static constexpr std::array<std::int64_t, 3> s_NVJPEG2K_JP2_SIZE = {282577, 144367, 276889};
    static constexpr std::array<std::int64_t, 3> s_NVJPEG2K_JK2_SIZE = {279052, 141242, 272350};
    static constexpr std::array<std::int64_t, 3> s_TIFF_SIZE         = {493572, 597782, 693950};
    static constexpr std::array<std::int64_t, 3> s_PNG_SIZE          = {313130, 220007, 352335};
    static constexpr std::array<std::int64_t, 3> s_OPENJPEG_JP2_SIZE = {279111, 141282, 272407};
    static constexpr std::array<std::int64_t, 3> s_OPENJPEG_JK2_SIZE = {279111, 141282, 272407};

    auto writer = std::make_shared<io::bitmap::writer>();

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
                "expected[" + std::to_string(i) + "] (" + std::to_string(s_NVJPEG_SIZE[i])
                + ") != actual (" + std::to_string(file_size) + ")",
                std::abs(s_NVJPEG_SIZE[i] - file_size) <= s_EPSILON
            );
        }
        else
        {
            // Use libJPEG as fallback
            const auto file_size = write(i, ".jpg");

            CPPUNIT_ASSERT_MESSAGE(
                "expected[" + std::to_string(i) + "] (" + std::to_string(s_LIBJPEG_SIZE[i])
                + ") != actual (" + std::to_string(file_size) + ")",
                std::abs(s_LIBJPEG_SIZE[i] - file_size) <= s_EPSILON
            );
        }

        // Test .jp2 with nv_jpeg_2k (if available)
        if(io::bitmap::nv_jpeg_2k())
        {
            const auto file_size = write(i, ".jp2");

            CPPUNIT_ASSERT_MESSAGE(
                "expected[" + std::to_string(i) + "] (" + std::to_string(s_NVJPEG2K_JP2_SIZE[i])
                + ") != actual (" + std::to_string(file_size) + ")",
                std::abs(s_NVJPEG2K_JP2_SIZE[i] - file_size) <= s_EPSILON
            );
        }
        else
        {
            // Use openJPEG as fallback
            const auto file_size = write(i, ".jp2");

            CPPUNIT_ASSERT_MESSAGE(
                "expected[" + std::to_string(i) + "] (" + std::to_string(s_OPENJPEG_JP2_SIZE[i])
                + ") != actual (" + std::to_string(file_size) + ")",
                std::abs(s_OPENJPEG_JP2_SIZE[i] - file_size) <= s_EPSILON
            );
        }

        // Test .j2k with nv_jpeg_2k (if available)
        if(io::bitmap::nv_jpeg_2k())
        {
            const auto file_size = write(i, ".j2k");

            CPPUNIT_ASSERT_MESSAGE(
                "expected[" + std::to_string(i) + "] (" + std::to_string(s_NVJPEG2K_JK2_SIZE[i])
                + ") != actual (" + std::to_string(file_size) + ")",
                std::abs(s_NVJPEG2K_JK2_SIZE[i] - file_size) <= s_EPSILON
            );
        }
        else
        {
            // Use openJPEG as fallback
            const auto file_size = write(i, ".j2k");

            CPPUNIT_ASSERT_MESSAGE(
                "expected[" + std::to_string(i) + "] (" + std::to_string(s_OPENJPEG_JK2_SIZE[i])
                + ") != actual (" + std::to_string(file_size) + ")",
                std::abs(s_OPENJPEG_JK2_SIZE[i] - file_size) <= s_EPSILON
            );
        }

        // test .tiff with libTIFF
        {
            const auto file_size = write(i, ".tiff");

            CPPUNIT_ASSERT_MESSAGE(
                "expected[" + std::to_string(i) + "] (" + std::to_string(s_TIFF_SIZE[i])
                + ") != actual (" + std::to_string(file_size) + ")",
                std::abs(s_TIFF_SIZE[i] - file_size) <= s_EPSILON
            );
        }

        // test .png with libPNG
        {
            const auto file_size = write(i, ".png");

            CPPUNIT_ASSERT_MESSAGE(
                "expected[" + std::to_string(i) + "] (" + std::to_string(s_PNG_SIZE[i])
                + ") != actual (" + std::to_string(file_size) + ")",
                std::abs(s_PNG_SIZE[i] - file_size) <= s_EPSILON
            );
        }

        ++i;
    }
}

//------------------------------------------------------------------------------

void writer_test::profiling_test()
{
    auto images = read_dicom_images();
    images.push_back(get_synthetic_image(0));

    // Create a temporary directory
    core::os::temp_dir tmp_dir;

    // Check how many loop to perform
    static const char* const s_ENV_LOOP   = std::getenv("PROFILETEST_LOOP");
    static const std::size_t s_LOOP_COUNT =
        s_ENV_LOOP != nullptr
        ? std::stoull(s_ENV_LOOP)
        : 1;

    SIGHT_INFO("Loop: " << s_LOOP_COUNT);

    std::vector<std::future<void> > tasks;

    // nvJPEG
    if(io::bitmap::nv_jpeg())
    {
        profile_writer(
            images,
            tmp_dir,
            s_LOOP_COUNT,
            backend::nvjpeg,
            writer::mode::fast,
            tasks
        );

        profile_writer(
            images,
            tmp_dir,
            s_LOOP_COUNT,
            backend::nvjpeg,
            writer::mode::best,
            tasks
        );
    }

    // nv_jpeg_2k
    if(io::bitmap::nv_jpeg_2k())
    {
        profile_writer(
            images,
            tmp_dir,
            s_LOOP_COUNT,
            backend::nvjpeg2k,
            writer::mode::fast,
            tasks
        );

        profile_writer(
            images,
            tmp_dir,
            s_LOOP_COUNT,
            backend::nvjpeg2k,
            writer::mode::best,
            tasks
        );
    }

    // libJPEG
    {
        profile_writer(
            images,
            tmp_dir,
            s_LOOP_COUNT,
            backend::libjpeg,
            writer::mode::fast,
            tasks
        );

        profile_writer(
            images,
            tmp_dir,
            s_LOOP_COUNT,
            backend::libjpeg,
            writer::mode::best,
            tasks
        );
    }

    // libTIFF
    {
        profile_writer(
            images,
            tmp_dir,
            s_LOOP_COUNT,
            backend::libtiff,
            writer::mode::fast,
            tasks
        );

        profile_writer(
            images,
            tmp_dir,
            s_LOOP_COUNT,
            backend::libtiff,
            writer::mode::best,
            tasks
        );
    }

    // libPNG
    {
        profile_writer(
            images,
            tmp_dir,
            s_LOOP_COUNT,
            backend::libpng,
            writer::mode::fast,
            tasks
        );

        profile_writer(
            images,
            tmp_dir,
            s_LOOP_COUNT,
            backend::libpng,
            writer::mode::best,
            tasks
        );
    }

    // openjpeg
    {
        profile_writer(
            images,
            tmp_dir,
            s_LOOP_COUNT,
            backend::openjpeg,
            writer::mode::fast,
            tasks
        );
    }

    if(!utest::filter::ignore_slow_tests() && s_ENV_LOOP != nullptr)
    {
        // Use OPENCV JPEG
        profile_open_cv_writer(
            images,
            tmp_dir,
            s_LOOP_COUNT,
            "jpg",
            writer::mode::fast,
            tasks
        );

        profile_open_cv_writer(
            images,
            tmp_dir,
            s_LOOP_COUNT,
            "jpg",
            writer::mode::best,
            tasks
        );

        // Use OPENCV TIFF
        profile_open_cv_writer(
            images,
            tmp_dir,
            s_LOOP_COUNT,
            "tiff",
            writer::mode::fast,
            tasks
        );

        // Use OPENCV PNG
        profile_open_cv_writer(
            images,
            tmp_dir,
            s_LOOP_COUNT,
            "png",
            writer::mode::fast,
            tasks
        );

        profile_open_cv_writer(
            images,
            tmp_dir,
            s_LOOP_COUNT,
            "png",
            writer::mode::best,
            tasks
        );

        // Use OPENCV WEBP
        profile_open_cv_writer(
            images,
            tmp_dir,
            s_LOOP_COUNT,
            "webp",
            writer::mode::fast,
            tasks
        );
    }

    for(const auto& task : tasks)
    {
        task.wait();
    }
}

} // namespace sight::io::bitmap::ut
