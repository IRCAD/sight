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

#include "SReaderTest.hpp"

#include <core/tools/System.hpp>
#include <core/tools/UUID.hpp>

#include <data/Image.hpp>

#include <io/base/service/IReader.hpp>
#include <io/bitmap/backend.hpp>
#include <io/bitmap/Reader.hpp>

#include <service/base.hpp>

#include <utestData/Data.hpp>
#include <utestData/generator/Image.hpp>

// cspell:ignore nvjpeg sreader

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION(sight::module::io::bitmap::ut::SReaderTest);

namespace sight::module::io::bitmap::ut
{

//------------------------------------------------------------------------------

inline static void runSReader(
    const boost::property_tree::ptree& config,
    sight::data::Image::sptr image,
    bool should_fail = false
)
{
    service::IService::sptr sreader = service::add("sight::module::io::bitmap::SReader");
    CPPUNIT_ASSERT_MESSAGE(std::string("Failed to create service 'sight::module::io::bitmap::SReader'"), sreader);
    sreader->setInOut(image, "data");

    CPPUNIT_ASSERT_NO_THROW(sreader->setConfiguration(config));
    CPPUNIT_ASSERT_NO_THROW(sreader->configure());
    CPPUNIT_ASSERT_NO_THROW(sreader->start().wait());
    CPPUNIT_ASSERT_NO_THROW(sreader->update().wait());
    CPPUNIT_ASSERT_NO_THROW(sreader->stop().wait());
    service::remove(sreader);

    // Check the result...
    CPPUNIT_ASSERT_EQUAL(should_fail, sight::io::base::service::IReader::dynamicCast(sreader)->hasFailed());
}

//------------------------------------------------------------------------------

inline static void testEnable(
    data::Image::sptr actual_image,
    const std::vector<sight::io::bitmap::Backend>& backends,
    const std::string enabled
)
{
    for(const auto& backend : backends)
    {
        const auto& filename = "wild" + sight::io::bitmap::extensions(backend).front();
        const auto& filepath = utestData::Data::dir() / "sight" / "image" / "bitmap" / filename;

        // Add file
        service::IService::ConfigType config;
        config.add("file", filepath.string());

        boost::property_tree::ptree backends_tree;
        backends_tree.put("<xmlattr>.enable", enabled);
        config.add_child("backends", backends_tree);

        // Run the service
        runSReader(config, actual_image);

        // Only test if the image exists. Conformance tests are already done in the reader
        CPPUNIT_ASSERT(actual_image);

        const auto& sizes = actual_image->getSize();
        CPPUNIT_ASSERT(sizes[0] > 0 && sizes[1] > 0 && sizes[2] == 0);
    }
}

//------------------------------------------------------------------------------

void SReaderTest::setUp()
{
}

//------------------------------------------------------------------------------

void SReaderTest::tearDown()
{
}

//------------------------------------------------------------------------------

void SReaderTest::basicTest()
{
    const auto& filename = "wild" + sight::io::bitmap::extensions(sight::io::bitmap::Backend::LIBTIFF).front();
    const auto& filepath = utestData::Data::dir() / "sight" / "image" / "bitmap" / filename;

    service::IService::ConfigType config;
    config.add("file", filepath.string());

    auto actual_image = sight::data::Image::New();
    runSReader(config, actual_image);

    // Only test if the image exists. Conformance tests are already done in the reader
    CPPUNIT_ASSERT(actual_image);

    const auto& sizes = actual_image->getSize();
    CPPUNIT_ASSERT(sizes[0] > 0 && sizes[1] > 0 && sizes[2] == 0);
}

//------------------------------------------------------------------------------

void SReaderTest::configTest()
{
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

    // Test enable="all"
    {
        // For each backend and each mode ("all" means ".jpeg, .tiff, .png, .jp2")
        auto actual_image = sight::data::Image::New();
        testEnable(actual_image, backends, "all");
    }

    // Test enable="cpu"
    {
        // For each backend and each mode ("cpu" means ".jpeg, .tiff, .png, .jp2")
        auto actual_image = sight::data::Image::New();
        testEnable(actual_image, backends, "cpu");
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
        auto actual_image = sight::data::Image::New();
        testEnable(actual_image, gpu_backend, "gpu");
    }
}

//------------------------------------------------------------------------------

} // namespace sight::module::io::bitmap::ut
