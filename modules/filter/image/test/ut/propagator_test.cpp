/************************************************************************
 *
 * Copyright (C) 2024 IRCAD France
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

#include "propagator_test.hpp"

#include <core/runtime/runtime.hpp>

#include <data/helper/medical_image.hpp>
#include <data/image.hpp>
#include <data/point_list.hpp>
#include <data/string.hpp>

#include <service/op.hpp>

#include <utest_data/generator/image.hpp>

#include <boost/property_tree/xml_parser.hpp>

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION(sight::module::filter::mesh::ut::propagator_test);

namespace sight::module::filter::mesh::ut
{

//------------------------------------------------------------------------------

void propagator_test::setUp()
{
    // Set up context before running a test.
    core::runtime::init();
    auto module = core::runtime::load_module(std::string("sight::module::filter::image"));
}

//------------------------------------------------------------------------------

void propagator_test::tearDown()
{
    // Clean up after the test run.
}

//------------------------------------------------------------------------------

void propagator_test::propagate()
{
    auto image           = std::make_shared<sight::data::image>();
    const auto dump_lock = image->dump_lock();

    const data::image::size_t size       = {10, 20, 90};
    const data::image::spacing_t spacing = {1.4, 3.5, 0.5};
    const data::image::origin_t origin   = {100., -208.2, 14.};

    utest_data::generator::image::generate_image(
        image,
        size,
        spacing,
        origin,
        core::type::get<std::int16_t>(),
        data::image::pixel_format::gray_scale
    );

    for(std::size_t x = 3 ; x < 6 ; ++x)
    {
        for(std::size_t y = 0 ; y < 10 ; ++y)
        {
            for(std::size_t z = 40 ; z < 70 ; ++z)
            {
                const auto index = x + y * size[0] + z * size[0] * size[1];

                std::int16_t value = 2554;
                image->set_pixel(index, reinterpret_cast<data::image::buffer_t*>(&value));
            }

            for(std::size_t z = 80 ; z < 90 ; ++z)
            {
                const auto index = x + y * size[0] + z * size[0] * size[1];

                std::int16_t value = 456;
                image->set_pixel(index, reinterpret_cast<data::image::buffer_t*>(&value));
            }
        }
    }

    // Create service
    sight::service::base::sptr srv = sight::service::add("sight::module::filter::image::propagator");
    CPPUNIT_ASSERT(srv->is_a("sight::module::filter::image::propagator"));
    CPPUNIT_ASSERT(srv);

    service::config_t config;
    std::stringstream config_string;
    config_string << R"(<properties value="50"/>)";

    auto point_list = std::make_shared<sight::data::point_list>();
    auto mask       = std::make_shared<sight::data::image>();
    auto mode       = std::make_shared<sight::data::string>();
    mode->set_value("minmax");
    boost::property_tree::read_xml(config_string, config);

    srv->set_worker(sight::core::thread::get_default_worker());
    srv->set_config(config);
    srv->set_input(image, "image_in");
    srv->set_input(point_list, "seeds");
    srv->set_inout(mask, "image_out");
    srv->set_inout(mode, "mode");
    srv->configure();
    srv->start().wait();
    srv->update().wait();

    // No seed, no change expected
    {
        const auto mask_lock = mask->dump_lock();
        for(std::size_t x = 0 ; x < 10 ; ++x)
        {
            for(std::size_t y = 0 ; y < 20 ; ++y)
            {
                for(std::size_t z = 0 ; z < 90 ; ++z)
                {
                    const auto index = x + y * size[0] + z * size[0] * size[1];

                    const std::uint8_t value_out = mask->at<std::uint8_t>(index);

                    CPPUNIT_ASSERT_EQUAL(static_cast<std::uint8_t>(0), value_out);
                }
            }
        }
    }

    // One seed in the '2554' region, propagation expected
    point_list->push_back(std::make_shared<sight::data::point>(105., -190., 40.));

    srv->update().wait();
    {
        const auto mask_lock = mask->dump_lock();
        for(std::size_t x = 0 ; x < 10 ; ++x)
        {
            for(std::size_t y = 0 ; y < 20 ; ++y)
            {
                for(std::size_t z = 0 ; z < 90 ; ++z)
                {
                    const auto index             = x + y * size[0] + z * size[0] * size[1];
                    const std::uint8_t value_out = mask->at<std::uint8_t>(index);

                    if(x >= 3 && x < 6 && y < 10 && z >= 40 && z < 70)
                    {
                        CPPUNIT_ASSERT_EQUAL(static_cast<std::uint8_t>(50), value_out);
                    }
                    else
                    {
                        CPPUNIT_ASSERT_EQUAL(static_cast<std::uint8_t>(0), value_out);
                    }
                }
            }
        }
    }

    // Reset the output image
    {
        const auto lock = mask->dump_lock();
        std::fill(mask->begin(), mask->end(), std::uint8_t(0));
    }

    // One seed in the '2554' region and one seed in the '1200' region, propagation expected in both regions
    point_list->push_back(std::make_shared<sight::data::point>(105., -190., 55.));
    auto samples_out = std::make_shared<sight::data::image>();
    srv->set_inout(samples_out, "samples_out");

    srv->update().wait();
    {
        const auto mask_lock = mask->dump_lock();
        for(std::size_t x = 0 ; x < 10 ; ++x)
        {
            for(std::size_t y = 0 ; y < 20 ; ++y)
            {
                for(std::size_t z = 0 ; z < 90 ; ++z)
                {
                    const auto index             = x + y * size[0] + z * size[0] * size[1];
                    const std::uint8_t value_out = mask->at<std::uint8_t>(index);

                    if(x >= 3 && x < 6 && y < 10 && ((z >= 40 && z < 70) || (z >= 80 && z < 90)))
                    {
                        CPPUNIT_ASSERT_EQUAL(static_cast<std::uint8_t>(50), value_out);
                    }
                    else
                    {
                        CPPUNIT_ASSERT_EQUAL(static_cast<std::uint8_t>(0), value_out);
                    }
                }
            }
        }

        const auto voxels_lock = samples_out->dump_lock();
        CPPUNIT_ASSERT_EQUAL(std::size_t(1200), samples_out->size()[0]);

        std::size_t count_1 = 0;
        std::size_t count_2 = 0;
        for(std::size_t i = 0 ; i < 1200 ; ++i)
        {
            const std::int16_t voxel = samples_out->at<std::int16_t>(i);
            CPPUNIT_ASSERT(static_cast<std::int16_t>(2554) == voxel || static_cast<std::int16_t>(456) == voxel);
            count_1 = (static_cast<std::int16_t>(2554) == voxel) ? count_1 + 1 : count_1;
            count_2 = (static_cast<std::int16_t>(456) == voxel) ? count_2 + 1 : count_2;
        }

        CPPUNIT_ASSERT_EQUAL(static_cast<std::size_t>(900), count_1);
        CPPUNIT_ASSERT_EQUAL(static_cast<std::size_t>(300), count_2);
    }

    // Clear the output image
    {
        srv->slot("clear")->run();

        const auto mask_lock = mask->dump_lock();
        for(std::size_t x = 0 ; x < 10 ; ++x)
        {
            for(std::size_t y = 0 ; y < 20 ; ++y)
            {
                for(std::size_t z = 0 ; z < 90 ; ++z)
                {
                    const auto index = x + y * size[0] + z * size[0] * size[1];

                    const std::uint8_t value_out = mask->at<std::uint8_t>(index);

                    CPPUNIT_ASSERT_EQUAL(static_cast<std::uint8_t>(0), value_out);
                }
            }
        }
    }

    // Switch to standard deviation mode
    mode->set_value("stddev");
    auto sig = mode->signal<sight::data::object::modified_signal_t>(sight::data::object::MODIFIED_SIG);
    sig->emit();

    point_list->push_back(std::make_shared<sight::data::point>(105., -190., 52.));

    // Add a new region around the mean of the seed values so that it will exclude the '2554' and '456'
    for(std::size_t x = 3 ; x < 6 ; ++x)
    {
        for(std::size_t y = 0 ; y < 10 ; ++y)
        {
            for(std::size_t z = 70 ; z < 80 ; ++z)
            {
                const auto index = x + y * size[0] + z * size[0] * size[1];

                std::int16_t value = 1400;
                image->set_pixel(index, reinterpret_cast<data::image::buffer_t*>(&value));
            }
        }
    }

    srv->update().wait();

    {
        const auto mask_lock = mask->dump_lock();
        for(std::size_t x = 0 ; x < 10 ; ++x)
        {
            for(std::size_t y = 0 ; y < 20 ; ++y)
            {
                for(std::size_t z = 0 ; z < 90 ; ++z)
                {
                    const auto index             = x + y * size[0] + z * size[0] * size[1];
                    const std::uint8_t value_out = mask->at<std::uint8_t>(index);

                    if((x == 4 && y == 5 && z == 52)                       // first seed, not propagated
                       || (x == 4 && y == 5 && z == 82)                    // second seed, not propagated
                       || (x >= 3 && x < 6 && y < 10 && z >= 70 && z < 80) // third seed, propagated
                    )
                    {
                        CPPUNIT_ASSERT_EQUAL(static_cast<std::uint8_t>(50), value_out);
                    }
                    else
                    {
                        std::stringstream ss;
                        ss << x << " " << y << " " << z;
                        CPPUNIT_ASSERT_EQUAL_MESSAGE(ss.str(), static_cast<std::uint8_t>(0), value_out);
                    }
                }
            }
        }

        const auto voxels_lock = samples_out->dump_lock();
        CPPUNIT_ASSERT_EQUAL(std::size_t(302), samples_out->size()[0]);

        std::size_t count = 0;
        for(std::size_t i = 0 ; i < 302 ; ++i)
        {
            const std::int16_t voxel = samples_out->at<std::int16_t>(i);
            count = (static_cast<std::int16_t>(1400) == voxel) ? count + 1 : count;
        }

        CPPUNIT_ASSERT_EQUAL(static_cast<std::size_t>(300), count);
    }

    srv->stop().wait();
    sight::service::remove(srv);
}

} // namespace sight::module::filter::mesh::ut
