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

#include <utest/filter.hpp>
#include <utest/profiling.hpp>

#include <doctest/doctest.h>

// cspell:ignore hicpp nvjpeg LIBJPEG LIBTIFF LUMA Acuson IMWRITE IMREAD ANYDEPTH ANYCOLOR OPENCV stoull
namespace sight::io::bitmap::ut
{

//------------------------------------------------------------------------------

inline static void test_backend_api(
    const std::filesystem::path& _file,
    backend _backend,
    data::image::sptr _expected_image,
    bool _buffer_api
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
        const std::string msg = "File not found: " + _file.string();
        CHECK_MESSAGE(false, msg);
    }

    auto actual_image = std::make_shared<data::image>();

    // Read the image from disk
    {
        auto reader = std::make_shared<io::bitmap::reader>();

        if(_buffer_api)
        {
            // Fill the buffer with the file content
            std::ifstream file_stream(filepath, std::ios::in | std::ios::binary);
            CHECK(file_stream.is_open());
            std::stringstream string_stream;
            string_stream << file_stream.rdbuf();
            std::string data = std::move(string_stream).str();

            // In full buffer API, the image must be already allocated
            const auto dump_lock = actual_image->dump_lock();
            actual_image->resize(_expected_image->size(), _expected_image->type(), _expected_image->pixel_format());

            CHECK_NOTHROW(
                reader->read(
                    reinterpret_cast<const std::uint8_t*>(data.data()),
                    data.size(),
                    _backend,
                    reinterpret_cast<std::uint8_t*>(actual_image->buffer())
                )
            );

            sight::data::helper::medical_image::check_image_slice_index(*actual_image);
        }
        else
        {
            reader->set_file(filepath);
            reader->set_object(actual_image);
            CHECK_NOTHROW(reader->read(_backend));
        }
    }

    const auto& expected_sizes = _expected_image->size();
    const auto& actual_sizes   = actual_image->size();

    CHECK_EQ(expected_sizes[0], actual_sizes[0]);
    CHECK_EQ(expected_sizes[1], actual_sizes[1]);
    CHECK_EQ(expected_sizes[2], actual_sizes[2]);
    CHECK_EQ(_expected_image->pixel_format(), actual_image->pixel_format());
    CHECK_EQ(_expected_image->type(), actual_image->type());

    if(_backend == backend::libjpeg || _backend == backend::nvjpeg)
    {
        CHECK(compute_psnr(_expected_image, actual_image) > 20.0);
    }
    else
    {
        CHECK(*_expected_image == *actual_image);
    }

    // test non existant file
    {
        constexpr auto path = "<<[{:}]>>";

        auto reader = std::make_shared<io::bitmap::reader>();
        reader->set_object(actual_image);
        reader->set_file(path);

        CHECK_THROWS(reader->read(_backend));
    }

    // test existing corrupted file
    {
        core::os::temp_file corrupted_file(std::ios_base::out);
        corrupted_file << "This is a corrupted file";
        corrupted_file.stream().close();

        auto reader = std::make_shared<io::bitmap::reader>();
        reader->set_object(actual_image);
        reader->set_file(corrupted_file);

        CHECK_THROWS(reader->read(_backend));
    }

    // test file with bad extension
    if(std::filesystem::exists(filepath))
    {
        auto reader = std::make_shared<io::bitmap::reader>();
        reader->set_object(actual_image);
        reader->set_file(filepath.replace_extension(".bad"));

        CHECK_THROWS(reader->read(_backend));
    }
}

//------------------------------------------------------------------------------

inline static void test_backend(
    const std::filesystem::path& _file,
    backend _backend,
    data::image::sptr _expected_image = data::image::sptr()
)
{
    test_backend_api(_file, _backend, _expected_image, false);
    test_backend_api(_file, _backend, _expected_image, true);
}

//------------------------------------------------------------------------------

inline static void profile_reader(
    std::size_t _loop,
    backend _backend
)
{
    auto reader = std::make_shared<io::bitmap::reader>();

    auto actual_image = std::make_shared<data::image>();
    reader->set_object(actual_image);

    const auto& filename = "wild" + extensions(_backend).front();
    const auto& filepath = utest_data::dir() / "sight" / "image" / "bitmap" / filename;
    reader->set_file(filepath);

    const std::string backend_name =
        [&]
        {
            switch(_backend)
            {
                case backend::libtiff:
                    return "libTIFF";

                case backend::libpng:
                    return "libPNG";

                case backend::nvjpeg:
                    return "nvJPEG";

                case backend::libjpeg:
                    return "libjpeg";

                case backend::nvjpeg2k:
                    return "nvjpeg2k";

                case backend::openjpeg:
                    return "openJPEG";

                default:
                    SIGHT_THROW("Unknown backend: '" << static_cast<unsigned int>(std::uint8_t(_backend)) << "'");
            }
        }();

    // Now profile reading
    SIGHT_PROFILE_FUNC(
        [&]
        (std::size_t)
        {
            CHECK_NOTHROW(reader->read(_backend));
        },
        _loop,
        backend_name,
        0.1
    );

    // Now profile openCV reading to compare
    if(!utest::filter::ignore_slow_tests() && _loop > 1)
    {
        const auto& opencv_label = "(openCV): " + filepath.extension().string();
        SIGHT_PROFILE_FUNC(
            [&]
            (std::size_t)
            {
                CHECK_NOTHROW(read_image(filepath));
            },
            _loop,
            opencv_label,
            0.1
        );
    }
}

//------------------------------------------------------------------------------

TEST_SUITE("sight::io::bitmap::reader")
{
    TEST_CASE("basic")
    {
        auto reader = std::make_shared<io::bitmap::reader>();

        CHECK_EQ(io::bitmap::extensions(backend::libtiff).front(), reader->extension());
    }

    TEST_CASE("extensions")
    {
        std::vector<data::sequenced_set<std::string> > extensions {
            {".jpg", ".jpeg"},
            {".jp2"},
            {".j2k"},
            {".tiff", ".tif"},
            {".png"}
        };

        std::vector<backend> backends {
            io::bitmap::nvjpeg() ? backend::nvjpeg : backend::libjpeg,
            io::bitmap::nvjpeg2k() ? backend::nvjpeg2k : backend::openjpeg,
            io::bitmap::nvjpeg2k() ? backend::nvjpeg2k_j2k : backend::openjpeg_j2k,
            backend::libtiff,
            backend::libpng
        };

        for(std::size_t i = 0 ; i < extensions.size() ; ++i)
        {
            const auto& extension_set        = extensions[i];
            const auto& backend              = backends[i];
            const auto& actual_extension_set = io::bitmap::extensions(backend);

            CHECK_EQ(extension_set.size(), actual_extension_set.size());

            for(std::size_t j = 0 ; j < extension_set.size() ; ++j)
            {
                CHECK_EQ(extension_set[j], actual_extension_set[j]);
            }
        }
    }

    TEST_CASE("wildcard")
    {
        std::vector<backend> backends {
            io::bitmap::nvjpeg() ? backend::nvjpeg : backend::libjpeg,
            io::bitmap::nvjpeg2k() ? backend::nvjpeg2k : backend::openjpeg,
            io::bitmap::nvjpeg2k() ? backend::nvjpeg2k_j2k : backend::openjpeg_j2k,
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

            CHECK_EQ(labels[index], label);
            CHECK_EQ(wildcards[index], wildcard);
            ++index;
        }
    }

    TEST_CASE("nvjpeg")
    {
        if(!io::bitmap::nvjpeg())
        {
            return;
        }

        test_backend("nvJPEG.jpg", backend::nvjpeg, get_synthetic_image(0));
    }

    TEST_CASE("nvjpeg2k")
    {
        if(!io::bitmap::nvjpeg2k())
        {
            return;
        }

        test_backend(
            // Pure synthetic images cannot be read/written correctly with JASPER and openCV.
            // For that reason, we use a real image.
            utest_data::dir() / "sight" / "image" / "bitmap" / "wild.jp2",
            backend::nvjpeg2k
        );
    }

    TEST_CASE("libpng")
    {
        test_backend("libPNG_RGB_UINT8.png", backend::libpng, get_synthetic_image(0));

        test_backend(
            "libPNG_RGBA_UINT8.png",
            backend::libpng,
            get_synthetic_image(1, core::type::UINT8, data::image::pixel_format_t::rgba)
        );
    }

    TEST_CASE("libjpeg")
    {
        test_backend("libJPEG.jpg", backend::libjpeg, get_synthetic_image(0));
    }

    TEST_CASE("openjpeg")
    {
        test_backend(
            // Pure synthetic images cannot be read/written correctly with JASPER and openCV.
            // For that reason, we use a real image.
            utest_data::dir() / "sight" / "image" / "bitmap" / "wild.jp2",
            backend::openjpeg
        );
    }

    TEST_CASE("libtiff")
    {
        test_backend("libTIFF_RGB_UINT8.tiff", backend::libtiff, get_synthetic_image(0));

        test_backend(
            "libTIFF_RGBA_UINT16.tiff",
            backend::libtiff,
            get_synthetic_image(1, core::type::UINT16, data::image::pixel_format_t::rgba)
        );

        test_backend(
            "libTIFF_GRAYSCALE_DOUBLE.tiff",
            backend::libtiff,
            get_synthetic_image(2, core::type::FLOAT64, data::image::pixel_format_t::gray_scale)
        );
    }

    TEST_CASE("profiling")
    {
        // Check how many loop to perform
        static const char* const s_ENV_LOOP   = std::getenv("PROFILETEST_LOOP");
        static const std::size_t s_LOOP_COUNT =
            s_ENV_LOOP != nullptr
            ? std::stoull(s_ENV_LOOP)
            : 1;

        SIGHT_INFO("Loop: " << s_LOOP_COUNT);

        profile_reader(s_LOOP_COUNT, backend::libjpeg);

        if(io::bitmap::nvjpeg())
        {
            profile_reader(s_LOOP_COUNT, backend::nvjpeg);
        }

        profile_reader(s_LOOP_COUNT, backend::openjpeg);

        if(io::bitmap::nvjpeg2k())
        {
            profile_reader(s_LOOP_COUNT, backend::nvjpeg2k);
        }

        profile_reader(s_LOOP_COUNT, backend::libpng);
        profile_reader(s_LOOP_COUNT, backend::libtiff);
    }
}

} // namespace sight::io::bitmap::ut
