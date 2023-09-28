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

#include "SWriterTest.hpp"

#include <core/os/temp_path.hpp>

#include <data/Image.hpp>

#include <io/__/service/writer.hpp>
#include <io/bitmap/Writer.hpp>

#include <service/op.hpp>

#include <utestData/Data.hpp>
#include <utestData/generator/Image.hpp>

// cspell:ignore nvjpeg

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION(sight::module::io::bitmap::ut::SWriterTest);

namespace sight::module::io::bitmap::ut
{

//------------------------------------------------------------------------------

inline static void runSWriter(
    const boost::property_tree::ptree& config,
    const sight::data::Image::csptr image,
    bool should_fail = false
)
{
    service::base::sptr swriter = service::add("sight::module::io::bitmap::SWriter");
    CPPUNIT_ASSERT_MESSAGE(std::string("Failed to create service 'sight::module::io::bitmap::SWriter'"), swriter);
    swriter->setInput(image, "data");

    CPPUNIT_ASSERT_NO_THROW(swriter->setConfiguration(config));
    CPPUNIT_ASSERT_NO_THROW(swriter->configure());
    CPPUNIT_ASSERT_NO_THROW(swriter->start().wait());
    CPPUNIT_ASSERT_NO_THROW(swriter->update().wait());
    CPPUNIT_ASSERT_NO_THROW(swriter->stop().wait());
    service::remove(swriter);

    // Check the result...
    CPPUNIT_ASSERT_EQUAL(
        should_fail,
        std::dynamic_pointer_cast<sight::io::service::writer>(swriter)->hasFailed()
    );
}

//------------------------------------------------------------------------------

inline static data::Image::csptr getSyntheticImage()
{
    static const data::Image::csptr generated =
        []
        {
            auto image           = std::make_shared<data::Image>();
            const auto dump_lock = image->dump_lock();
            image->resize({800, 600, 0}, core::type::UINT8, data::Image::RGB);

            auto it        = image->begin<data::iterator::rgb>();
            const auto end = image->end<data::iterator::rgb>();

            std::uint8_t r = 0;
            std::uint8_t g = 0;
            std::uint8_t b = 0;
            std::for_each(
                it,
                end,
                [&](auto& x)
            {
                x.r = r;
                x.g = g;
                x.b = b;

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

inline static void testEnable(
    const std::filesystem::path& tempFolder,
    const data::Image::csptr& expected_image,
    const std::vector<sight::io::bitmap::Backend>& backends,
    const std::vector<sight::io::bitmap::Writer::Mode>& modes,
    const std::string enabled
)
{
    for(const auto& backend : backends)
    {
        std::map<sight::io::bitmap::Writer::Mode, std::size_t> sizes;

        for(const auto& mode : modes)
        {
            const std::string mode_string(mode == sight::io::bitmap::Writer::Mode::BEST ? "best" : "fast");
            const auto& filePath = tempFolder / (
                "config_" + mode_string + sight::io::bitmap::extensions(backend).front()
            );

            // Add file
            service::config_t config;
            config.add("file", filePath.string());

            boost::property_tree::ptree backends_tree;
            backends_tree.put("<xmlattr>.enable", enabled);
            backends_tree.put("<xmlattr>.mode", mode_string);
            config.add_child("backends", backends_tree);

            // Run the service
            runSWriter(config, expected_image);

            // Only test if the file exists. Conformance tests are already done in the writer
            CPPUNIT_ASSERT_MESSAGE(
                "File '" + filePath.string() + "' doesn't exist.",
                std::filesystem::exists(filePath) && std::filesystem::is_regular_file(filePath)
            );

            sizes.insert_or_assign(mode, std::filesystem::file_size(filePath));
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

void SWriterTest::setUp()
{
}

//------------------------------------------------------------------------------

void SWriterTest::tearDown()
{
}

//------------------------------------------------------------------------------

void SWriterTest::basicTest()
{
    core::os::temp_dir tmpDir;
    const auto& filePath = tmpDir / "basic.tiff";

    service::config_t config;
    config.add("file", filePath.string());

    const auto& expected_image = getSyntheticImage();
    runSWriter(config, expected_image);

    // Only test if the file exists. Conformance tests are already done in the writer
    CPPUNIT_ASSERT_MESSAGE(
        "File '" + filePath.string() + "' doesn't exist.",
        std::filesystem::exists(filePath)
        && std::filesystem::is_regular_file(filePath)
        && std::filesystem::file_size(filePath) > 0
    );
}

//------------------------------------------------------------------------------

void SWriterTest::configTest()
{
    core::os::temp_dir tmpDir;

    // Build backend list
    std::vector backends {
        sight::io::bitmap::Backend::LIBPNG,
        sight::io::bitmap::Backend::LIBTIFF
    };

    if(sight::io::bitmap::nvJPEG())
    {
        backends.push_back(sight::io::bitmap::Backend::NVJPEG);
    }

    backends.push_back(sight::io::bitmap::Backend::LIBJPEG);

    if(sight::io::bitmap::nvJPEG2K())
    {
        backends.push_back(sight::io::bitmap::Backend::NVJPEG2K);
    }

    backends.push_back(sight::io::bitmap::Backend::OPENJPEG);

    // Build mode list
    const std::vector modes {
        sight::io::bitmap::Writer::Mode::BEST,
        sight::io::bitmap::Writer::Mode::FAST
    };

    const auto& expected_image = getSyntheticImage();

    // Test enable="all"
    {
        // For each backend and each mode ("all" means ".jpeg, .tiff, .png, .jp2")
        testEnable(tmpDir, expected_image, backends, modes, "all");
    }

    // Test enable="cpu"
    {
        // For each backend and each mode ("cpu" means ".jpeg, .tiff, .png, .jp2")
        testEnable(tmpDir, expected_image, backends, modes, "cpu");
    }

    // Test enable="gpu"
    if(sight::io::bitmap::nvJPEG()
       || sight::io::bitmap::nvJPEG2K())
    {
        std::vector<sight::io::bitmap::Backend> gpu_backend;

        if(sight::io::bitmap::nvJPEG())
        {
            gpu_backend.push_back(sight::io::bitmap::Backend::NVJPEG);
        }

        if(sight::io::bitmap::nvJPEG2K())
        {
            gpu_backend.push_back(sight::io::bitmap::Backend::NVJPEG2K);
        }

        // For each backend and each mode ("cpu" means ".jpeg, .jp2")
        testEnable(tmpDir, expected_image, gpu_backend, modes, "gpu");
    }

    // Test custom backend choice
    {
        for(const auto& backend : backends)
        {
            std::map<sight::io::bitmap::Writer::Mode, std::size_t> sizes;

            for(const auto& mode : modes)
            {
                const std::string mode_string(mode == sight::io::bitmap::Writer::Mode::BEST ? "best" : "fast");
                const auto& filePath = tmpDir / (
                    "config_" + mode_string + sight::io::bitmap::extensions(backend).front()
                );

                // Add file
                service::config_t config;
                config.add("file", filePath.string());

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
                    runSWriter(config, expected_image);

                    // Only test if the file exists. Conformance tests are already done in the writer
                    CPPUNIT_ASSERT_MESSAGE(
                        "File '" + filePath.string() + "' doesn't exist.",
                        std::filesystem::exists(filePath) && std::filesystem::is_regular_file(filePath)
                    );

                    sizes.insert_or_assign(mode, std::filesystem::file_size(filePath));
                }
                else
                {
                    // Run the service -> Write should work, but the extension should be changed
                    runSWriter(config, expected_image);

                    auto tiffFilePath = filePath;
                    tiffFilePath.replace_extension(
                        sight::io::bitmap::extensions(sight::io::bitmap::Backend::LIBTIFF).front()
                    );

                    CPPUNIT_ASSERT_MESSAGE(
                        "File '" + tiffFilePath.string() + "' doesn't exist.",
                        std::filesystem::exists(tiffFilePath) && std::filesystem::is_regular_file(tiffFilePath)
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
