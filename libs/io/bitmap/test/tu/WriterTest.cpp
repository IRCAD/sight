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

#include <core/Profiling.hpp>
#include <core/os/TempPath.hpp>
#include <core/tools/UUID.hpp>

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

inline static std::vector<data::Image::sptr> readDicomImages(std::size_t count = 3)
{
    static const std::vector<data::Image::sptr> result =
        []
        {
            std::vector<data::Image::sptr> images;
            auto seriesSet = data::SeriesSet::New();
            auto reader    = io::dicom::Reader::New();

            reader->setObject(seriesSet);

            // Read a DICOM image "us/Ultrasound Image Storage/GE, lossy JPEG"
            reader->setFolder(utestData::Data::dir() / "us/Ultrasound Image Storage/GE, lossy JPEG");
            CPPUNIT_ASSERT_NO_THROW(reader->read());

            // Just to be sure we read the good data
            CPPUNIT_ASSERT_EQUAL(std::size_t(1), seriesSet->size());

            auto imageSeries = data::ImageSeries::dynamicCast(seriesSet->at(0));
            CPPUNIT_ASSERT(imageSeries);

            auto size = imageSeries->getSize();
            CPPUNIT_ASSERT_EQUAL(std::size_t(636), size[0]);
            CPPUNIT_ASSERT_EQUAL(std::size_t(434), size[1]);
            CPPUNIT_ASSERT_EQUAL(std::size_t(1), size[2]);

            images.push_back(imageSeries);
            seriesSet->clear();

            // Read next image "us/Ultrasound Image Storage/Siemens Acuson 500"
            reader->setFolder(utestData::Data::dir() / "us/Ultrasound Image Storage/Siemens Acuson 500");
            CPPUNIT_ASSERT_NO_THROW(reader->read());

            // Just to be sure we read the good data
            CPPUNIT_ASSERT_EQUAL(std::size_t(1), seriesSet->size());

            imageSeries = data::ImageSeries::dynamicCast(seriesSet->at(0));
            CPPUNIT_ASSERT(imageSeries);

            size = imageSeries->getSize();
            CPPUNIT_ASSERT_EQUAL(std::size_t(800), size[0]);
            CPPUNIT_ASSERT_EQUAL(std::size_t(600), size[1]);
            CPPUNIT_ASSERT_EQUAL(std::size_t(1), size[2]);

            images.push_back(imageSeries);
            seriesSet->clear();

            // Read next image "us/Ultrasound Multi-frame Image Storage/Siemens Acuson 500"
            reader->setFolder(utestData::Data::dir() / "us/Ultrasound Multi-frame Image Storage/Siemens Acuson 500");
            CPPUNIT_ASSERT_NO_THROW(reader->read());

            // Just to be sure we read the good data
            CPPUNIT_ASSERT_EQUAL(std::size_t(1), seriesSet->size());

            imageSeries = data::ImageSeries::dynamicCast(seriesSet->at(0));
            CPPUNIT_ASSERT(imageSeries);

            size = imageSeries->getSize();
            CPPUNIT_ASSERT_EQUAL(std::size_t(800), size[0]);
            CPPUNIT_ASSERT_EQUAL(std::size_t(600), size[1]);
            CPPUNIT_ASSERT_EQUAL(std::size_t(60), size[2]);

            images.push_back(imageSeries);
            return images;
        }();

    auto copy = result;
    copy.resize(count);
    return copy;
}

//------------------------------------------------------------------------------

inline static void profileWriter(
    const std::vector<data::Image::sptr>& images,
    const std::filesystem::path& tmp_folder,
    std::size_t loop,
    Backend backend,
    Writer::Mode mode,
    std::vector<std::future<void> >& tasks
)
{
    auto writer = Writer::New();

    const auto [BACKEND, EXT] = backendToString(backend);
    const std::string MODE = modeToString(mode);

    const std::string FILE_SUFFIX = "_" + BACKEND + "_" + MODE + EXT;
    const std::string LABEL       = BACKEND + " (" + MODE + ")";

    // Get size, psnr, ...
    for(std::size_t i = 0 ; const auto& image : images)
    {
        // Write image
        writer->setObject(image);
        const auto& tmp_path = tmp_folder / (std::to_string(i) + FILE_SUFFIX);
        writer->setFile(tmp_path);
        CPPUNIT_ASSERT_NO_THROW(writer->write(backend, mode));

        SIGHT_INFO(LABEL << " size: " << std::filesystem::file_size(tmp_path));

        // PSNR is only relevant with lossy format...
        if(EXT.ends_with("jpg") || EXT.ends_with(".jpeg"))
        {
            SIGHT_INFO(LABEL << " PSNR: " << computePSNR(image, readImage(tmp_path)));
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
            for(std::size_t j = 0 ; const auto& image : images)
            {
                writer->setObject(image);
                const auto& tmp_path = tmp_folder / (std::to_string(i) + "_" + std::to_string(j++) + FILE_SUFFIX);
                writer->setFile(tmp_path);

                CPPUNIT_ASSERT_NO_THROW(writer->write(backend, mode));

                // Schedule cleanup
                tasks.emplace_back(std::async(std::launch::deferred, [ = ]{std::filesystem::remove_all(tmp_path);}));
            }
        },
        loop,
        LABEL,
        0.1
    );

    // Wait for all file delete task to finish
    while(!tasks.empty())
    {
        tasks.back().wait();
        tasks.pop_back();
    }
}

//------------------------------------------------------------------------------

inline static void profileOpenCVWriter(
    const std::vector<data::Image::sptr>& images,
    const std::filesystem::path& tmp_folder,
    std::size_t loop,
    std::string ext,
    Writer::Mode mode,
    std::vector<std::future<void> >& tasks
)
{
    const std::string MODE = modeToString(mode);

    const std::string FILE_SUFFIX = "_OPENCV_" + MODE + "." + ext;
    const std::string LABEL       = "OpenCV (" + ext + " - " + MODE + ")";

    // Get size, psnr, ...
    for(std::size_t i = 0 ; const auto& image : images)
    {
        const auto& tmp_path = tmp_folder / (std::to_string(i++) + FILE_SUFFIX);

        // Convert Image to OpenCV Mat
        const cv::Mat& mat = imageToMat(image);

        // Write image
        if(mode == Writer::Mode::BEST)
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

        SIGHT_INFO(LABEL << " size: " << std::filesystem::file_size(tmp_path));

        if(ext.ends_with("jpg") || ext.ends_with(".jpeg"))
        {
            SIGHT_INFO(LABEL << " PSNR: " << computePSNR(image, readImage(tmp_path)));
        }

        // Cleanup
        std::filesystem::remove_all(tmp_path);
    }

    // Now profile writing
    SIGHT_PROFILE_FUNC(
        [&]
            (std::size_t i)
        {
            for(std::size_t j = 0 ; const auto& image : images)
            {
                const auto& tmp_path = tmp_folder / (std::to_string(i) + "_" + std::to_string(j++) + FILE_SUFFIX);

                // Convert Image to OpenCV Mat
                const cv::Mat& mat = imageToMat(image);

                // Write image
                if(mode == Writer::Mode::BEST)
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
                tasks.emplace_back(std::async(std::launch::deferred, [ = ]{std::filesystem::remove_all(tmp_path);}));
            }
        },
        loop,
        LABEL,
        0.1
    );

    // Wait for all file delete task to finish
    while(!tasks.empty())
    {
        tasks.back().wait();
        tasks.pop_back();
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
    auto writer = Writer::New();

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

void WriterTest::wildcardTest()
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

inline static void conformance(
    const std::vector<Backend>& supported,
    const std::vector<Backend>& unsupported,
    core::Type type,
    data::Image::PixelFormat format
)
{
    // Create a temporary directory
    core::os::TempDir tmp_dir;

    // Create the synthetic image
    const auto& expected_image = getSyntheticImage(0, type, format);

    // Create the writer
    auto writer = Writer::New();
    writer->setObject(expected_image);

    // Build mode list
    const std::vector modes {
        Writer::Mode::BEST,
        Writer::Mode::FAST
    };

    // For each backend and each mode
    for(const auto& backend : supported)
    {
        const std::string& backend_string = backendToString(backend).first;

        for(const auto& mode : modes)
        {
            // Test write
            const auto& file_path = tmp_dir / ("conformance" + fileSuffix(backend, mode));
            CPPUNIT_ASSERT_NO_THROW(writer->setFile(file_path));
            CPPUNIT_ASSERT_NO_THROW(writer->write(backend, mode));
            CPPUNIT_ASSERT_MESSAGE(file_path.string() + " doesn't exist.", std::filesystem::exists(file_path));

            const auto& actual_image = readImage(file_path.string());
            CPPUNIT_ASSERT(actual_image);

            if(backend == Backend::OPENJPEG || backend == Backend::NVJPEG2K)
            {
                // Because of bad encoder or decoder implementation, JPEG2000 is not always *perfectly* lossless.
                // Indeed, it is mathematically, but the implementation can suffer from some corner floating point
                // rounding errors. This seems to be the case with openCV/Jasper plugin, at least, in this test
                // which uses synthetic data. One component value differ 0x254 instead of 0x253
                // As a workaround, we try to encode two times, and we consider success if multiple pass doesn't
                // degrade the situation more.
                writer->setObject(actual_image);
                const auto& copy_file_path = tmp_dir / ("conformance_copy" + fileSuffix(backend, mode));
                CPPUNIT_ASSERT_NO_THROW(writer->setFile(copy_file_path));
                CPPUNIT_ASSERT_NO_THROW(writer->write(backend, mode));
                CPPUNIT_ASSERT_MESSAGE(
                    copy_file_path.string() + " doesn't exist.",
                    std::filesystem::exists(copy_file_path)
                );

                const auto& actual_image_copy = readImage(copy_file_path.string());
                CPPUNIT_ASSERT(actual_image_copy);

                CPPUNIT_ASSERT_MESSAGE(
                    "The image are not equal for backend '" + backend_string + "', mode '" + modeToString(mode)
                    + "', format '" + pixelFormatToString(format) + "', type '" + type.name() + "'",
                    *expected_image == *actual_image
                );

                // Restore back the original source
                writer->setObject(expected_image);
            }
            // Compare pixels only for lossless backend
            else if(backend != Backend::LIBJPEG && backend != Backend::NVJPEG)
            {
                CPPUNIT_ASSERT_MESSAGE(
                    "The image are not equal for backend '" + backend_string + "', mode '" + modeToString(mode)
                    + "', format '" + pixelFormatToString(format) + "', type '" + type.name() + "'",
                    *expected_image == *actual_image
                );
            }
            else
            {
                // Compare at least sizes...
                const auto& expected_size = expected_image->getSize();
                const auto& actual_size   = actual_image->getSize();
                CPPUNIT_ASSERT_EQUAL(expected_size[0], actual_size[0]);
                CPPUNIT_ASSERT_EQUAL(expected_size[1], actual_size[1]);
                CPPUNIT_ASSERT_EQUAL(expected_size[2], actual_size[2]);
                CPPUNIT_ASSERT_EQUAL(expected_image->getPixelFormat(), actual_image->getPixelFormat());
                CPPUNIT_ASSERT_EQUAL(expected_image->getType(), actual_image->getType());

                // Ensure that psnr is at least > 20
                const double psnr = computePSNR(expected_image, actual_image);
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
    for(const auto& backend : unsupported)
    {
        for(const auto& mode : modes)
        {
            // Test write
            const auto& file_path = tmp_dir / ("conformance" + fileSuffix(backend, mode));
            CPPUNIT_ASSERT_NO_THROW(writer->setFile(file_path));
            CPPUNIT_ASSERT_THROW(writer->write(backend, mode), core::Exception);
        }
    }
}

//------------------------------------------------------------------------------

void WriterTest::conformanceTest()
{
    // UINT8 RGB
    {
        if(io::bitmap::nvJPEG2K())
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
                core::Type::UINT8,
                data::Image::PixelFormat::RGB
            );
        }
        else if(io::bitmap::nvJPEG())
        {
            conformance(
                {Backend::LIBJPEG, Backend::LIBPNG, Backend::LIBTIFF, Backend::OPENJPEG, Backend::NVJPEG},
                {Backend::NVJPEG2K},
                core::Type::UINT8,
                data::Image::PixelFormat::RGB
            );
        }
        else
        {
            conformance(
                {Backend::LIBJPEG, Backend::LIBPNG, Backend::LIBTIFF, Backend::OPENJPEG},
                {Backend::NVJPEG2K, Backend::NVJPEG},
                core::Type::UINT8,
                data::Image::PixelFormat::RGB
            );
        }
    }

    // UINT8 GRAYSCALE
    {
        if(io::bitmap::nvJPEG2K())
        {
            conformance(
                {Backend::LIBJPEG, Backend::LIBPNG, Backend::LIBTIFF, Backend::OPENJPEG, Backend::NVJPEG2K},
                {Backend::NVJPEG},
                core::Type::UINT8,
                data::Image::PixelFormat::GRAY_SCALE
            );
        }
        else
        {
            conformance(
                {Backend::LIBJPEG, Backend::LIBPNG, Backend::LIBTIFF, Backend::OPENJPEG},
                {Backend::NVJPEG2K, Backend::NVJPEG},
                core::Type::UINT8,
                data::Image::PixelFormat::GRAY_SCALE
            );
        }
    }

    // UINT16 RGB
    {
        if(io::bitmap::nvJPEG2K())
        {
            conformance(
                {Backend::LIBTIFF, Backend::OPENJPEG, Backend::NVJPEG2K},
                {Backend::LIBJPEG, Backend::NVJPEG},
                core::Type::UINT16,
                data::Image::PixelFormat::RGB
            );
        }
        else
        {
            conformance(
                {Backend::LIBPNG, Backend::LIBTIFF, Backend::OPENJPEG},
                {Backend::LIBJPEG, Backend::NVJPEG2K, Backend::NVJPEG},
                core::Type::UINT16,
                data::Image::PixelFormat::RGB
            );
        }
    }

    // UINT16 GRAYSCALE
    {
        if(io::bitmap::nvJPEG2K())
        {
            conformance(
                {Backend::LIBPNG, Backend::LIBTIFF, Backend::OPENJPEG, Backend::NVJPEG2K},
                {Backend::LIBJPEG, Backend::NVJPEG},
                core::Type::UINT16,
                data::Image::PixelFormat::GRAY_SCALE
            );
        }
        else
        {
            conformance(
                {Backend::LIBPNG, Backend::LIBTIFF, Backend::OPENJPEG},
                {Backend::LIBJPEG, Backend::NVJPEG2K, Backend::NVJPEG},
                core::Type::UINT16,
                data::Image::PixelFormat::GRAY_SCALE
            );
        }
    }
}

//------------------------------------------------------------------------------

void WriterTest::emptyImageTest()
{
    // Create a temporary directory
    core::os::TempDir tmp_dir;

    const auto& empty_image = data::Image::New();
    auto writer             = Writer::New();
    writer->setObject(empty_image);

    std::vector<std::string> extensions {".jpeg", ".tiff", ".png", ".jp2", ".j2k"};

    for(const auto& ext : extensions)
    {
        const auto& tmp_path = tmp_dir / ("empty" + ext);
        CPPUNIT_ASSERT_NO_THROW(writer->setFile(tmp_path));
        CPPUNIT_ASSERT_THROW(writer->write(), core::Exception);
        CPPUNIT_ASSERT_MESSAGE(tmp_path.string() + " exists.", !std::filesystem::exists(tmp_path));
    }
}

//------------------------------------------------------------------------------

void WriterTest::wrongPathTest()
{
    // Create a temporary directory, with a non existing leaf directory
    core::os::TempDir tmp_dir;
    const auto tmp_folder = tmp_dir / UUID::generateUUID();
    std::filesystem::remove_all(tmp_folder);

    const auto& imageSeries = readDicomImages(1);
    auto writer             = Writer::New();
    writer->setObject(imageSeries.front());

    std::vector<std::string> extensions {".jpeg", ".tiff", ".png", ".jp2", ".j2k"};

    for(const auto& ext : extensions)
    {
        const auto& tmp_path = tmp_folder / ("wrong_path" + ext);
        CPPUNIT_ASSERT_NO_THROW(writer->setFile(tmp_path));
        CPPUNIT_ASSERT_NO_THROW(writer->write());
        CPPUNIT_ASSERT_MESSAGE(tmp_path.string() + " doesn't exist.", std::filesystem::exists(tmp_path));
    }
}

//------------------------------------------------------------------------------

void WriterTest::fromDicomTest()
{
    const auto& imageSeries = readDicomImages();

    // Create a temporary directory
    core::os::TempDir tmp_dir;

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

    auto writer = Writer::New();

    const auto& write =
        [&](std::size_t i, const std::string& ext)
        {
            const auto& tmp_path = tmp_dir / (std::to_string(i) + "_from_dicom" + ext);
            CPPUNIT_ASSERT_NO_THROW(writer->setFile(tmp_path));
            CPPUNIT_ASSERT_NO_THROW(writer->write());
            CPPUNIT_ASSERT(std::filesystem::exists(tmp_path));

            return std::int64_t(std::filesystem::file_size(tmp_path));
        };

    for(std::size_t i = 0 ; const auto& image : imageSeries)
    {
        writer->setObject(image);

        // Test .jpg with nvJPEG (if available)
        if(io::bitmap::nvJPEG())
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

        // Test .jp2 with nvJPEG2K (if available)
        if(io::bitmap::nvJPEG2K())
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

        // Test .j2k with nvJPEG2K (if available)
        if(io::bitmap::nvJPEG2K())
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
    auto images = readDicomImages();
    images.push_back(getSyntheticImage(0));

    // Create a temporary directory
    core::os::TempDir tmp_dir;

    // Check how many loop to perform
    static const char* const env_loop   = std::getenv("PROFILETEST_LOOP");
    static const std::size_t LOOP_COUNT =
        env_loop != nullptr
        ? std::stoull(env_loop)
        : 1;

    SIGHT_INFO("Loop: " << LOOP_COUNT);

    std::vector<std::future<void> > tasks;

    // nvJPEG
    if(io::bitmap::nvJPEG())
    {
        profileWriter(
            images,
            tmp_dir,
            LOOP_COUNT,
            Backend::NVJPEG,
            Writer::Mode::FAST,
            tasks
        );

        profileWriter(
            images,
            tmp_dir,
            LOOP_COUNT,
            Backend::NVJPEG,
            Writer::Mode::BEST,
            tasks
        );
    }

    // nvJPEG2K
    if(io::bitmap::nvJPEG2K())
    {
        profileWriter(
            images,
            tmp_dir,
            LOOP_COUNT,
            Backend::NVJPEG2K,
            Writer::Mode::FAST,
            tasks
        );

        profileWriter(
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
        profileWriter(
            images,
            tmp_dir,
            LOOP_COUNT,
            Backend::LIBJPEG,
            Writer::Mode::FAST,
            tasks
        );

        profileWriter(
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
        profileWriter(
            images,
            tmp_dir,
            LOOP_COUNT,
            Backend::LIBTIFF,
            Writer::Mode::FAST,
            tasks
        );

        profileWriter(
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
        profileWriter(
            images,
            tmp_dir,
            LOOP_COUNT,
            Backend::LIBPNG,
            Writer::Mode::FAST,
            tasks
        );

        profileWriter(
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
        profileWriter(
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
        profileOpenCVWriter(
            images,
            tmp_dir,
            LOOP_COUNT,
            "jpg",
            Writer::Mode::FAST,
            tasks
        );

        profileOpenCVWriter(
            images,
            tmp_dir,
            LOOP_COUNT,
            "jpg",
            Writer::Mode::BEST,
            tasks
        );

        // Use OPENCV TIFF
        profileOpenCVWriter(
            images,
            tmp_dir,
            LOOP_COUNT,
            "tiff",
            Writer::Mode::FAST,
            tasks
        );

        // Use OPENCV PNG
        profileOpenCVWriter(
            images,
            tmp_dir,
            LOOP_COUNT,
            "png",
            Writer::Mode::FAST,
            tasks
        );

        profileOpenCVWriter(
            images,
            tmp_dir,
            LOOP_COUNT,
            "png",
            Writer::Mode::BEST,
            tasks
        );

        // Use OPENCV WEBP
        profileOpenCVWriter(
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
