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

#include "reader_test.hpp"

#include <core/tools/system.hpp>
#include <core/tools/uuid.hpp>

#include <data/image.hpp>

#include <io/__/service/reader.hpp>
#include <io/bitmap/backend.hpp>
#include <io/bitmap/reader.hpp>

#include <service/op.hpp>

#include <utest_data/data.hpp>
#include <utest_data/generator/image.hpp>

// cspell:ignore nvjpeg sreader

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION(sight::module::io::bitmap::ut::reader_test);

namespace sight::module::io::bitmap::ut
{

//------------------------------------------------------------------------------

inline static void runreader(
    const boost::property_tree::ptree& _config,
    sight::data::image::sptr _image,
    bool _should_fail = false
)
{
    service::base::sptr sreader = service::add("sight::module::io::bitmap::reader");
    CPPUNIT_ASSERT_MESSAGE(std::string("Failed to create service 'sight::module::io::bitmap::reader'"), sreader);
    sreader->set_inout(_image, "data");

    CPPUNIT_ASSERT_NO_THROW(sreader->set_config(_config));
    CPPUNIT_ASSERT_NO_THROW(sreader->configure());
    CPPUNIT_ASSERT_NO_THROW(sreader->start().wait());
    CPPUNIT_ASSERT_NO_THROW(sreader->update().wait());
    CPPUNIT_ASSERT_NO_THROW(sreader->stop().wait());
    service::remove(sreader);

    // Check the result...
    CPPUNIT_ASSERT_EQUAL(
        _should_fail,
        std::dynamic_pointer_cast<sight::io::service::reader>(sreader)->has_failed()
    );
}

//------------------------------------------------------------------------------

inline static void test_enable(
    data::image::sptr _actual_image,
    const std::vector<sight::io::bitmap::backend>& _backends,
    const std::string _enabled
)
{
    for(const auto& backend : _backends)
    {
        const auto& filename = "wild" + sight::io::bitmap::extensions(backend).front();
        const auto& filepath = utest_data::dir() / "sight" / "image" / "bitmap" / filename;

        // Add file
        service::config_t config;
        config.add("file", filepath.string());

        boost::property_tree::ptree backends_tree;
        backends_tree.put("<xmlattr>.enable", _enabled);
        config.add_child("backends", backends_tree);

        // Run the service
        runreader(config, _actual_image);

        // Only test if the image exists. Conformance tests are already done in the reader
        CPPUNIT_ASSERT(_actual_image);

        const auto& sizes = _actual_image->size();
        CPPUNIT_ASSERT(sizes[0] > 0 && sizes[1] > 0 && sizes[2] == 0);
    }
}

//------------------------------------------------------------------------------

void reader_test::setUp()
{
}

//------------------------------------------------------------------------------

void reader_test::tearDown()
{
}

//------------------------------------------------------------------------------

void reader_test::basic_test()
{
    const auto& filename = "wild" + sight::io::bitmap::extensions(sight::io::bitmap::backend::libtiff).front();
    const auto& filepath = utest_data::dir() / "sight" / "image" / "bitmap" / filename;

    service::config_t config;
    config.add("file", filepath.string());

    auto actual_image = std::make_shared<sight::data::image>();
    runreader(config, actual_image);

    // Only test if the image exists. Conformance tests are already done in the reader
    CPPUNIT_ASSERT(actual_image);

    const auto& sizes = actual_image->size();
    CPPUNIT_ASSERT(sizes[0] > 0 && sizes[1] > 0 && sizes[2] == 0);
}

//------------------------------------------------------------------------------

void reader_test::config_test()
{
    // Build backend list
    std::vector backends {
        sight::io::bitmap::backend::libpng,
        sight::io::bitmap::backend::libtiff
    };

    if(sight::io::bitmap::nv_jpeg())
    {
        backends.push_back(sight::io::bitmap::backend::nvjpeg);
    }

    backends.push_back(sight::io::bitmap::backend::libjpeg);

    if(sight::io::bitmap::nv_jpeg_2k())
    {
        backends.push_back(sight::io::bitmap::backend::nvjpeg2k);
    }

    backends.push_back(sight::io::bitmap::backend::openjpeg);

    // Test enable="all"
    {
        // For each backend and each mode ("all" means ".jpeg, .tiff, .png, .jp2")
        auto actual_image = std::make_shared<sight::data::image>();
        test_enable(actual_image, backends, "all");
    }

    // Test enable="cpu"
    {
        // For each backend and each mode ("cpu" means ".jpeg, .tiff, .png, .jp2")
        auto actual_image = std::make_shared<sight::data::image>();
        test_enable(actual_image, backends, "cpu");
    }

    // Test enable="gpu"
    if(sight::io::bitmap::nv_jpeg()
       || sight::io::bitmap::nv_jpeg_2k())
    {
        std::vector<sight::io::bitmap::backend> gpu_backend;

        if(sight::io::bitmap::nv_jpeg())
        {
            gpu_backend.push_back(sight::io::bitmap::backend::nvjpeg);
        }

        if(sight::io::bitmap::nv_jpeg_2k())
        {
            gpu_backend.push_back(sight::io::bitmap::backend::nvjpeg2k);
        }

        // For each backend and each mode ("cpu" means ".jpeg, .jp2")
        auto actual_image = std::make_shared<sight::data::image>();
        test_enable(actual_image, gpu_backend, "gpu");
    }
}

//------------------------------------------------------------------------------

} // namespace sight::module::io::bitmap::ut
