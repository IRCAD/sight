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

#include "writer_test.hpp"

#include <core/os/temp_path.hpp>

#include <data/image.hpp>

#include <io/__/service/writer.hpp>
#include <io/bitmap/Writer.hpp>

#include <service/op.hpp>

#include <utest_data/Data.hpp>
#include <utest_data/generator/image.hpp>

// cspell:ignore nvjpeg

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION(sight::module::io::bitmap::ut::writer_test);

namespace sight::module::io::bitmap::ut
{

//------------------------------------------------------------------------------

inline static void runwriter(
    const boost::property_tree::ptree& _config,
    const sight::data::image::csptr _image,
    bool _should_fail = false
)
{
    service::base::sptr swriter = service::add("sight::module::io::bitmap::writer");
    CPPUNIT_ASSERT_MESSAGE(std::string("Failed to create service 'sight::module::io::bitmap::writer'"), swriter);
    swriter->set_input(_image, "data");

    CPPUNIT_ASSERT_NO_THROW(swriter->set_config(_config));
    CPPUNIT_ASSERT_NO_THROW(swriter->configure());
    CPPUNIT_ASSERT_NO_THROW(swriter->start().wait());
    CPPUNIT_ASSERT_NO_THROW(swriter->update().wait());
    CPPUNIT_ASSERT_NO_THROW(swriter->stop().wait());
    service::remove(swriter);

    // Check the result...
    CPPUNIT_ASSERT_EQUAL(
        _should_fail,
        std::dynamic_pointer_cast<sight::io::service::writer>(swriter)->hasFailed()
    );
}

//------------------------------------------------------------------------------

inline static data::image::csptr get_synthetic_image()
{
    static const data::image::csptr generated =
        []
        {
            auto image           = std::make_shared<data::image>();
            const auto dump_lock = image->dump_lock();
            image->resize({800, 600, 0}, core::type::UINT8, data::image::RGB);

            auto it        = image->begin<data::iterator::rgb>();
            const auto end = image->end<data::iterator::rgb>();

            std::uint8_t r = 0;
            std::uint8_t g = 0;
            std::uint8_t b = 0;
            std::for_each(
                it,
                end,
                [&](auto& _x)
            {
                _x.r = r;
                _x.g = g;
                _x.b = b;

                r += 1;

                if(r > 254)
                {
                    r  = 0;
                    g += 1;
                }

                if(g > 254)
                {
                    g  = 0;
                    b += 1;
                }

                if(b > 254)
                {
                    r = 0;
                    g = 0;
                    b = 0;
                }
            });

            return image;
        }();

    return generated;
}

//------------------------------------------------------------------------------

inline static void test_enable(
    const std::filesystem::path& _temp_folder,
    const data::image::csptr& _expected_image,
    const std::vector<sight::io::bitmap::Backend>& _backends,
    const std::vector<sight::io::bitmap::Writer::Mode>& _modes,
    const std::string _enabled
)
{
    for(const auto& backend : _backends)
    {
        std::map<sight::io::bitmap::Writer::Mode, std::size_t> sizes;

        for(const auto& mode : _modes)
        {
            const std::string mode_string(mode == sight::io::bitmap::Writer::Mode::BEST ? "best" : "fast");
            const auto& file_path = _temp_folder / (
                "config_" + mode_string + sight::io::bitmap::extensions(backend).front()
            );

            // Add file
            service::config_t config;
            config.add("file", file_path.string());

            boost::property_tree::ptree backends_tree;
            backends_tree.put("<xmlattr>.enable", _enabled);
            backends_tree.put("<xmlattr>.mode", mode_string);
            config.add_child("backends", backends_tree);

            // Run the service
            runwriter(config, _expected_image);

            // Only test if the file exists. Conformance tests are already done in the writer
            CPPUNIT_ASSERT_MESSAGE(
                "File '" + file_path.string() + "' doesn't exist.",
                std::filesystem::exists(file_path) && std::filesystem::is_regular_file(file_path)
            );

            sizes.insert_or_assign(mode, std::filesystem::file_size(file_path));
        }

        // Sizes should be bigger than 0..
        CPPUNIT_ASSERT_GREATER(
            std::size_t(0),
            sizes[sight::io::bitmap::Writer::Mode::BEST]
        );

        CPPUNIT_ASSERT_GREATER(
            std::size_t(0),
            sizes[sight::io::bitmap::Writer::Mode::FAST]
        );

        // openJPEG have no "BEST" or "FAST" mode and nvJPEG2000 can be equal
        if(backend == sight::io::bitmap::Backend::OPENJPEG
           || backend == sight::io::bitmap::Backend::NVJPEG2K)
        {
            CPPUNIT_ASSERT_LESSEQUAL(
                sizes[sight::io::bitmap::Writer::Mode::FAST],
                sizes[sight::io::bitmap::Writer::Mode::BEST]
            );
        }
        else
        {
            // Best should compress better
            CPPUNIT_ASSERT_LESSEQUAL(
                sizes[sight::io::bitmap::Writer::Mode::FAST],
                sizes[sight::io::bitmap::Writer::Mode::BEST]
            );
        }
    }
}

//------------------------------------------------------------------------------

void writer_test::setUp()
{
}

//------------------------------------------------------------------------------

void writer_test::tearDown()
{
}

//------------------------------------------------------------------------------

void writer_test::basicTest()
{
    core::os::temp_dir tmp_dir;
    const auto& file_path = tmp_dir / "basic.tiff";

    service::config_t config;
    config.add("file", file_path.string());

    const auto& expected_image = get_synthetic_image();
    runwriter(config, expected_image);

    // Only test if the file exists. Conformance tests are already done in the writer
    CPPUNIT_ASSERT_MESSAGE(
        "File '" + file_path.string() + "' doesn't exist.",
        std::filesystem::exists(file_path)
        && std::filesystem::is_regular_file(file_path)
        && std::filesystem::file_size(file_path) > 0
    );
}

//------------------------------------------------------------------------------

void writer_test::configTest()
{
    core::os::temp_dir tmp_dir;

    // Build backend list
    std::vector backends {
        sight::io::bitmap::Backend::LIBPNG,
        sight::io::bitmap::Backend::LIBTIFF
    };

    if(sight::io::bitmap::nv_jpeg())
    {
        backends.push_back(sight::io::bitmap::Backend::NVJPEG);
    }

    backends.push_back(sight::io::bitmap::Backend::LIBJPEG);

    if(sight::io::bitmap::nv_jpeg_2k())
    {
        backends.push_back(sight::io::bitmap::Backend::NVJPEG2K);
    }

    backends.push_back(sight::io::bitmap::Backend::OPENJPEG);

    // Build mode list
    const std::vector modes {
        sight::io::bitmap::Writer::Mode::BEST,
        sight::io::bitmap::Writer::Mode::FAST
    };

    const auto& expected_image = get_synthetic_image();

    // Test enable="all"
    {
        // For each backend and each mode ("all" means ".jpeg, .tiff, .png, .jp2")
        test_enable(tmp_dir, expected_image, backends, modes, "all");
    }

    // Test enable="cpu"
    {
        // For each backend and each mode ("cpu" means ".jpeg, .tiff, .png, .jp2")
        test_enable(tmp_dir, expected_image, backends, modes, "cpu");
    }

    // Test enable="gpu"
    if(sight::io::bitmap::nv_jpeg()
       || sight::io::bitmap::nv_jpeg_2k())
    {
        std::vector<sight::io::bitmap::Backend> gpu_backend;

        if(sight::io::bitmap::nv_jpeg())
        {
            gpu_backend.push_back(sight::io::bitmap::Backend::NVJPEG);
        }

        if(sight::io::bitmap::nv_jpeg_2k())
        {
            gpu_backend.push_back(sight::io::bitmap::Backend::NVJPEG2K);
        }

        // For each backend and each mode ("cpu" means ".jpeg, .jp2")
        test_enable(tmp_dir, expected_image, gpu_backend, modes, "gpu");
    }

    // Test custom backend choice
    {
        for(const auto& backend : backends)
        {
            std::map<sight::io::bitmap::Writer::Mode, std::size_t> sizes;

            for(const auto& mode : modes)
            {
                const std::string mode_string(mode == sight::io::bitmap::Writer::Mode::BEST ? "best" : "fast");
                const auto& file_path = tmp_dir / (
                    "config_" + mode_string + sight::io::bitmap::extensions(backend).front()
                );

                // Add file
                service::config_t config;
                config.add("file", file_path.string());

                // Add tiff and png backend
                boost::property_tree::ptree tiff_backend_tree;
                tiff_backend_tree.put("<xmlattr>.mode", mode_string);

                boost::property_tree::ptree png_backend_tree;
                png_backend_tree.put("<xmlattr>.mode", mode_string);

                boost::property_tree::ptree backends_tree;
                backends_tree.add_child("libtiff", tiff_backend_tree);
                backends_tree.add_child("libpng", png_backend_tree);
                config.add_child("backends", backends_tree);

                if(backend == sight::io::bitmap::Backend::LIBTIFF
                   || backend == sight::io::bitmap::Backend::LIBPNG)
                {
                    // Run the service
                    runwriter(config, expected_image);

                    // Only test if the file exists. Conformance tests are already done in the writer
                    CPPUNIT_ASSERT_MESSAGE(
                        "File '" + file_path.string() + "' doesn't exist.",
                        std::filesystem::exists(file_path) && std::filesystem::is_regular_file(file_path)
                    );

                    sizes.insert_or_assign(mode, std::filesystem::file_size(file_path));
                }
                else
                {
                    // Run the service -> Write should work, but the extension should be changed
                    runwriter(config, expected_image);

                    auto tiff_file_path = file_path;
                    tiff_file_path.replace_extension(
                        sight::io::bitmap::extensions(sight::io::bitmap::Backend::LIBTIFF).front()
                    );

                    CPPUNIT_ASSERT_MESSAGE(
                        "File '" + tiff_file_path.string() + "' doesn't exist.",
                        std::filesystem::exists(tiff_file_path) && std::filesystem::is_regular_file(tiff_file_path)
                    );
                }
            }

            if(backend == sight::io::bitmap::Backend::LIBTIFF
               || backend == sight::io::bitmap::Backend::LIBPNG)
            {
                // Sizes should be bigger than 0..
                CPPUNIT_ASSERT_GREATER(
                    std::size_t(0),
                    sizes[sight::io::bitmap::Writer::Mode::BEST]
                );

                CPPUNIT_ASSERT_GREATER(
                    std::size_t(0),
                    sizes[sight::io::bitmap::Writer::Mode::FAST]
                );

                // Best should compress better
                CPPUNIT_ASSERT_LESSEQUAL(
                    sizes[sight::io::bitmap::Writer::Mode::FAST],
                    sizes[sight::io::bitmap::Writer::Mode::BEST]
                );
            }
        }
    }
}

//------------------------------------------------------------------------------

} // namespace sight::module::io::bitmap::ut
