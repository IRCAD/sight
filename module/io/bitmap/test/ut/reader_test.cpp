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

#include <doctest/doctest.h>

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
    CHECK_MESSAGE(sreader, std::string("Failed to create service 'sight::module::io::bitmap::reader'"));
    sreader->set_inout(_image, "data");

    CHECK_NOTHROW(sreader->set_config(_config));
    CHECK_NOTHROW(sreader->configure());
    CHECK_NOTHROW(sreader->start().get());
    CHECK_NOTHROW(sreader->update().get());
    CHECK_NOTHROW(sreader->stop().get());
    service::remove(sreader);

    // Check the result...
    CHECK_EQ(
        _should_fail,
        std::dynamic_pointer_cast<sight::io::service::reader>(sreader)->has_failed()
    );
}

//------------------------------------------------------------------------------

inline static void test_enable(data::image::sptr _actual_image, bool _gpu_required)
{
    const std::array<std::string, 4> extensions = {".jp2", ".jpg", ".png", ".tiff"};

    for(const auto& ext : extensions)
    {
        const auto filename = "wild" + ext;
        const auto filepath = utest_data::dir() / "sight" / "image" / "bitmap" / filename;

        // Add file
        service::config_t config;
        config.add("file", filepath.string());
        config.add("gpu_required", _gpu_required);

        // Run the service
        runreader(config, _actual_image);

        // Only test if the image exists. Conformance tests are already done in the reader
        CHECK(_actual_image);

        const auto& sizes = _actual_image->size();
        CHECK(sizes[0] > 0);
        CHECK(sizes[1] > 0);
        CHECK(sizes[2] == 0);
    }
}

TEST_SUITE("sight::module::io::bitmap::reader")
{
    TEST_CASE("basic_test")
    {
        const auto& filename = "wild" + sight::io::bitmap::extensions(sight::io::bitmap::backend::libtiff).front();
        const auto& filepath = utest_data::dir() / "sight" / "image" / "bitmap" / filename;

        service::config_t config;
        config.add("file", filepath.string());

        auto actual_image = std::make_shared<sight::data::image>();
        runreader(config, actual_image);

        // Only test if the image exists. Conformance tests are already done in the reader
        CHECK(actual_image);

        const auto& sizes = actual_image->size();
        CHECK(sizes[0] > 0);
        CHECK(sizes[1] > 0);
        CHECK(sizes[2] == 0);
    }

    TEST_CASE("config_test")
    {
        {
            auto actual_image = std::make_shared<sight::data::image>();
            test_enable(actual_image, false);
        }

        if(sight::io::bitmap::nvjpeg())
        {
            auto actual_image = std::make_shared<sight::data::image>();
            test_enable(actual_image, true);
        }
    }
}

//------------------------------------------------------------------------------

} // namespace sight::module::io::bitmap::ut
