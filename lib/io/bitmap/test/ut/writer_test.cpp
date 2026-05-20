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
#include <core/profiling.hpp>
#include <core/progress/observer.hpp>
#include <core/tools/uuid.hpp>

#include <io/bitmap/writer.hpp>
#include <io/dicom/reader/file.hpp>

#include <utest/filter.hpp>
#include <utest/profiling.hpp>

#include <doctest/doctest.h>

// cspell:ignore nvjpeg Acuson LUMA

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
            {
                auto observer = std::make_shared<core::progress::observer>("Reading DICOM images");
                reader->read(observer);
            }

            // Just to be sure we read the good data
            if(series_set->size() != std::size_t(1))
            {
                throw std::runtime_error("Expected 1 series, got " + std::to_string(series_set->size()));
            }

            auto image_series = std::dynamic_pointer_cast<data::image_series>(series_set->at(0));
            if(!image_series)
            {
                throw std::runtime_error("Failed to cast to image_series");
            }

            auto size = image_series->size();
            if(size[0] != std::size_t(636) || size[1] != std::size_t(434) || size[2] != std::size_t(1))
            {
                throw std::runtime_error("Image size mismatch");
            }

            images.push_back(image_series);
            series_set->clear();

            // Read next image "us/Ultrasound Image Storage/Siemens Acuson 500"
            reader->set_folder(utest_data::dir() / "us/Ultrasound Image Storage/Siemens Acuson 500");
            {
                auto observer = std::make_shared<core::progress::observer>("Reading DICOM images");
                reader->read(observer);
            }

            // Just to be sure we read the good data
            if(series_set->size() != std::size_t(1))
            {
                throw std::runtime_error("Expected 1 series at second read");
            }

            image_series = std::dynamic_pointer_cast<data::image_series>(series_set->at(0));
            if(!image_series)
            {
                throw std::runtime_error("Failed to cast to image_series at second read");
            }

            size = image_series->size();
            if(size[0] != std::size_t(800) || size[1] != std::size_t(600) || size[2] != std::size_t(1))
            {
                throw std::runtime_error("Image size mismatch at second read");
            }

            images.push_back(image_series);
            series_set->clear();

            // Read next image "us/Ultrasound Multi-frame Image Storage/Siemens Acuson 500"
            reader->set_folder(utest_data::dir() / "us/Ultrasound Multi-frame Image Storage/Siemens Acuson 500");
            {
                auto observer = std::make_shared<core::progress::observer>("Reading DICOM images");
                reader->read(observer);
            }

            // Just to be sure we read the good data
            if(series_set->size() != std::size_t(1))
            {
                throw std::runtime_error("Expected 1 series at third read");
            }

            image_series = std::dynamic_pointer_cast<data::image_series>(series_set->at(0));
            if(!image_series)
            {
                throw std::runtime_error("Failed to cast to image_series at third read");
            }

            size = image_series->size();
            if(size[0] != std::size_t(800) || size[1] != std::size_t(600) || size[2] != std::size_t(60))
            {
                throw std::runtime_error("Image size mismatch at third read");
            }

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
        CHECK_NOTHROW(writer->write(_backend, _mode));

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
        (std::size_t _i)
        {
            for(std::size_t j = 0 ; const auto& image : _images)
            {
                writer->set_object(image);
                const auto& tmp_path = _tmp_folder / (std::to_string(_i) + "_" + std::to_string(j++) + file_suffix);
                writer->set_file(tmp_path);

                CHECK_NOTHROW(writer->write(_backend, _mode));

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
            CHECK(
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
            CHECK(cv::imwrite(tmp_path.string(), mat));
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
        (std::size_t _i)
        {
            for(std::size_t j = 0 ; const auto& image : _images)
            {
                const auto& tmp_path = _tmp_folder / (std::to_string(_i) + "_" + std::to_string(j++) + file_suffix);

                // Convert Image to OpenCV Mat
                const cv::Mat& mat = image_to_mat(image);

                // Write image
                if(_mode == writer::mode::best)
                {
                    CHECK(
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
                    CHECK(cv::imwrite(tmp_path.string(), mat));
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

TEST_SUITE("sight::io::bitmap::writer")
{
    TEST_CASE("basic")
    {
        auto writer = std::make_shared<io::bitmap::writer>();
        CHECK_EQ(io::bitmap::extensions(backend::libtiff).front(), writer->extension());
    }

    //------------------------------------------------------------------------------

    inline static void conformance(
        const std::vector<backend>& _supported,
        const std::vector<backend>& _unsupported,
        core::type _type,
        enum data::image::pixel_format_t _format
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
                CHECK_NOTHROW(writer->set_file(file_path));
                CHECK_NOTHROW(writer->write(backend, mode));
                CHECK_MESSAGE(std::filesystem::exists(file_path), (file_path.string() + " doesn't exist."));

                const auto& actual_image = read_image(file_path.string());
                CHECK(actual_image);

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
                    CHECK_NOTHROW(writer->set_file(copy_file_path));
                    CHECK_NOTHROW(writer->write(backend, mode));
                    CHECK_MESSAGE(
                        std::filesystem::exists(copy_file_path),
                        (copy_file_path.string() + " doesn't exist.")
                    );

                    const auto& actual_image_copy = read_image(copy_file_path.string());
                    CHECK(actual_image_copy);

                    CHECK_MESSAGE(
                        *expected_image == *actual_image,
                        ("The image are not equal for backend '" + backend_string + "', mode '" + mode_to_string(mode)
                         + "', format '" + pixel_format_to_string(_format) + "', type '" + _type.name() + "'")
                    );

                    // Restore back the original source
                    writer->set_object(expected_image);
                }
                // Compare pixels only for lossless backend
                else if(backend != backend::libjpeg && backend != backend::nvjpeg)
                {
                    CHECK_MESSAGE(
                        *expected_image == *actual_image,
                        ("The image are not equal for backend '" + backend_string + "', mode '" + mode_to_string(mode)
                         + "', format '" + pixel_format_to_string(_format) + "', type '" + _type.name() + "'")
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
                         + backend_string
                         + "', PSNR="
                         + std::to_string(psnr)
                         + "dB")
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
                CHECK_NOTHROW(writer->set_file(file_path));
                CHECK_THROWS(writer->write(backend, mode));
            }
        }
    }

//------------------------------------------------------------------------------

    TEST_CASE("conformance")
    {
        // UINT8 RGB
        {
            if(io::bitmap::nvjpeg2k())
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
                    data::image::pixel_format_t::rgb
                );
            }
            else if(io::bitmap::nvjpeg())
            {
                conformance(
                    {backend::libjpeg, backend::libpng, backend::libtiff, backend::openjpeg, backend::nvjpeg},
                    {backend::nvjpeg2k},
                    core::type::UINT8,
                    data::image::pixel_format_t::rgb
                );
            }
            else
            {
                conformance(
                    {backend::libjpeg, backend::libpng, backend::libtiff, backend::openjpeg},
                    {backend::nvjpeg2k, backend::nvjpeg},
                    core::type::UINT8,
                    data::image::pixel_format_t::rgb
                );
            }
        }

        // UINT8 GRAYSCALE
        {
            if(io::bitmap::nvjpeg2k())
            {
                conformance(
                    {backend::libjpeg, backend::libpng, backend::libtiff, backend::openjpeg, backend::nvjpeg2k},
                    {backend::nvjpeg},
                    core::type::UINT8,
                    data::image::pixel_format_t::gray_scale
                );
            }
            else
            {
                conformance(
                    {backend::libjpeg, backend::libpng, backend::libtiff, backend::openjpeg},
                    {backend::nvjpeg2k, backend::nvjpeg},
                    core::type::UINT8,
                    data::image::pixel_format_t::gray_scale
                );
            }
        }

        // UINT16 RGB
        {
            if(io::bitmap::nvjpeg2k())
            {
                conformance(
                    {backend::libtiff, backend::openjpeg, backend::nvjpeg2k},
                    {backend::libjpeg, backend::nvjpeg},
                    core::type::UINT16,
                    data::image::pixel_format_t::rgb
                );
            }
            else
            {
                conformance(
                    {backend::libpng, backend::libtiff, backend::openjpeg},
                    {backend::libjpeg, backend::nvjpeg2k, backend::nvjpeg},
                    core::type::UINT16,
                    data::image::pixel_format_t::rgb
                );
            }
        }

        // UINT16 GRAYSCALE
        {
            if(io::bitmap::nvjpeg2k())
            {
                conformance(
                    {backend::libpng, backend::libtiff, backend::openjpeg, backend::nvjpeg2k},
                    {backend::libjpeg, backend::nvjpeg},
                    core::type::UINT16,
                    data::image::pixel_format_t::gray_scale
                );
            }
            else
            {
                conformance(
                    {backend::libpng, backend::libtiff, backend::openjpeg},
                    {backend::libjpeg, backend::nvjpeg2k, backend::nvjpeg},
                    core::type::UINT16,
                    data::image::pixel_format_t::gray_scale
                );
            }
        }
    }

//------------------------------------------------------------------------------

    TEST_CASE("empty_image")
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
            CHECK_NOTHROW(writer->set_file(tmp_path));
            auto observer = std::make_shared<core::progress::observer>("Writing empty image... ");
            CHECK_THROWS(writer->write(observer));
            CHECK_MESSAGE(!std::filesystem::exists(tmp_path), (tmp_path.string() + " exists."));
        }
    }

//------------------------------------------------------------------------------

    TEST_CASE("wrong_path")
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
            CHECK_NOTHROW(writer->set_file(tmp_path));
            auto observer = std::make_shared<core::progress::observer>("Writing wrong path image... ");
            CHECK_NOTHROW(writer->write(observer));
            CHECK_MESSAGE(std::filesystem::exists(tmp_path), (tmp_path.string() + " doesn't exist."));
        }
    }

//------------------------------------------------------------------------------

    TEST_CASE("from_dicom")
    {
        const auto& image_series = read_dicom_images();

        // Create a temporary directory
        core::os::temp_dir tmp_dir;

        // Use a big epsilon since size can vary between platforms and libraries version, especially for nvJPEG2000
        static constexpr std::int64_t s_EPSILON = 50000;

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
                CHECK_NOTHROW(writer->set_file(tmp_path));
                auto observer = std::make_shared<core::progress::observer>("Writing from DICOM image... ");
                CHECK_NOTHROW(writer->write(observer));
                CHECK(std::filesystem::exists(tmp_path));

                return std::int64_t(std::filesystem::file_size(tmp_path));
            };

        for(std::size_t i = 0 ; const auto& image : image_series)
        {
            writer->set_object(image);

            // Test .jpg with nvJPEG (if available)
            if(io::bitmap::nvjpeg())
            {
                const auto file_size = write(i, ".jpg");
                CHECK(std::abs(s_NVJPEG_SIZE[i] - file_size) <= s_EPSILON);
            }
            else
            {
                // Use libJPEG as fallback
                const auto file_size = write(i, ".jpg");
                CHECK(std::abs(s_LIBJPEG_SIZE[i] - file_size) <= s_EPSILON);
            }

            // Test .jp2 with nvjpeg2k (if available)
            if(io::bitmap::nvjpeg2k())
            {
                const auto file_size = write(i, ".jp2");
                CHECK(std::abs(s_NVJPEG2K_JP2_SIZE[i] - file_size) <= s_EPSILON);
            }
            else
            {
                // Use openJPEG as fallback
                const auto file_size = write(i, ".jp2");
                CHECK(std::abs(s_OPENJPEG_JP2_SIZE[i] - file_size) <= s_EPSILON);
            }

            // Test .j2k with nvjpeg2k (if available)
            if(io::bitmap::nvjpeg2k())
            {
                const auto file_size = write(i, ".j2k");
                CHECK(std::abs(s_NVJPEG2K_JK2_SIZE[i] - file_size) <= s_EPSILON);
            }
            else
            {
                // Use openJPEG as fallback
                const auto file_size = write(i, ".j2k");
                CHECK(std::abs(s_OPENJPEG_JK2_SIZE[i] - file_size) <= s_EPSILON);
            }

            // test .tiff with libTIFF
            {
                const auto file_size = write(i, ".tiff");
                CHECK(std::abs(s_TIFF_SIZE[i] - file_size) <= s_EPSILON);
            }

            // test .png with libPNG
            {
                const auto file_size = write(i, ".png");
                CHECK(std::abs(s_PNG_SIZE[i] - file_size) <= s_EPSILON);
            }

            ++i;
        }
    }

//------------------------------------------------------------------------------

    TEST_CASE("profiling")
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
        if(io::bitmap::nvjpeg())
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

        // nvjpeg2k
        if(io::bitmap::nvjpeg2k())
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
} // TEST_SUITE

} // namespace sight::io::bitmap::ut
