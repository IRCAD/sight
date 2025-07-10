/************************************************************************
 *
 * Copyright (C) 2025 IRCAD France
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

#include "get_point_test.hpp"

#include <core/runtime/runtime.hpp>

#include <data/image_series.hpp>
#include <data/point.hpp>

#include <service/op.hpp>

#include <boost/property_tree/xml_parser.hpp>

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION(sight::module::data::ut::get_point_test);

namespace sight::module::data::ut
{

class tester final
{
public:

    tester()
    {
        // Create service
        srv = sight::service::add("sight::module::data::get_point");
        CPPUNIT_ASSERT(srv);
        CPPUNIT_ASSERT(srv->is_a("sight::module::data::get_point"));

        // Create data
        image = std::make_shared<sight::data::image_series>();
        CPPUNIT_ASSERT(image);

        auto fiducials = image->get_fiducials();
        fiducials->add_group("test_group_1", {1.0, 0.0, 1.0, 1.0}, 3.F);
        fiducials->add_point("test_group_1", {1.0, 80.0, 2.0});
        fiducials->add_point("test_group_1", {-1.0, 5.0, -2.0});

        fiducials->add_group("test_group_2", {1.0, 0.0, 1.0, 1.0}, 3.F);
        fiducials->add_point("test_group_2", {6.0, 4.0, 7.0});
        fiducials->add_point("test_group_2", {8.0, 9.0, -20.0});

        point0 = std::make_shared<sight::data::point>();
        CPPUNIT_ASSERT(point0);
        point1 = std::make_shared<sight::data::point>();
        CPPUNIT_ASSERT(point1);
        point2 = std::make_shared<sight::data::point>();
        CPPUNIT_ASSERT(point2);
    }

    //------------------------------------------------------------------------------

    ~tester()
    {
        if(srv->started())
        {
            srv->stop().get();
        }

        sight::service::remove(srv);
    }

    //------------------------------------------------------------------------------

    sight::service::base::sptr srv;
    sight::data::image_series::sptr image;
    sight::data::point::sptr point0;
    sight::data::point::sptr point1;
    sight::data::point::sptr point2;
};

//------------------------------------------------------------------------------

void get_point_test::setUp()
{
    // Set up context before running a test.
    core::runtime::init();
    auto module = core::runtime::load_module(std::string("sight::module::data"));
}

//------------------------------------------------------------------------------

void get_point_test::tearDown()
{
}

//------------------------------------------------------------------------------

void get_point_test::extracts_point_by_index()
{
    tester srv_tester;
    service::config_t config;
    std::stringstream config_string;
    config_string
    << R"(<inout group="points">)"
       R"(<key group="test_group_2" index="1" uid="output_point0"/>)"
       R"(<key group="test_group_1" index="0" uid="output_point1"/>)"
       R"(<key group="test_group_1" index="1" uid="output_point2"/>)"
       R"(</inout>)";
    boost::property_tree::read_xml(config_string, config);

    srv_tester.srv->set_config(config);
    srv_tester.srv->set_input(srv_tester.image, "image");
    srv_tester.srv->set_inout(srv_tester.point0, "points", false, false, 0);
    srv_tester.srv->set_inout(srv_tester.point1, "points", false, false, 1);
    srv_tester.srv->set_inout(srv_tester.point2, "points", false, false, 2);
    srv_tester.srv->configure();
    srv_tester.srv->start().get();
    CPPUNIT_ASSERT_NO_THROW(srv_tester.srv->update().get());

    CPPUNIT_ASSERT(sight::data::point({8.0, 9.0, -20.0}) == *srv_tester.point0);
    CPPUNIT_ASSERT(sight::data::point({1.0, 80.0, 2.0}) == *srv_tester.point1);
    CPPUNIT_ASSERT(sight::data::point({-1.0, 5.0, -2.0}) == *srv_tester.point2);
}

//------------------------------------------------------------------------------

void get_point_test::extracts_point_with_index_out_of_bound()
{
    tester srv_tester;
    service::config_t config;
    std::stringstream config_string;
    config_string
    << R"(<inout group="points">)"
       R"(<key group="test_group_1" index="1" uid="output_point0"/>)"
       R"(<key group="test_group_1" index="122" uid="output_point1"/>)"
       R"(<key group="test_group_1" index="0" uid="output_point2"/>)"
       R"(</inout>)";
    boost::property_tree::read_xml(config_string, config);

    srv_tester.srv->set_config(config);
    srv_tester.srv->set_input(srv_tester.image, "image");
    srv_tester.srv->set_inout(srv_tester.point0, "points", false, false, 0);
    srv_tester.srv->set_inout(srv_tester.point1, "points", false, false, 1);
    srv_tester.srv->set_inout(srv_tester.point2, "points", false, false, 2);
    srv_tester.srv->configure();
    srv_tester.srv->start().get();
    CPPUNIT_ASSERT_THROW(srv_tester.srv->update().get(), sight::core::exception);

    CPPUNIT_ASSERT(sight::data::point({-1.0, 5.0, -2.0}) == *srv_tester.point0);
    CPPUNIT_ASSERT(sight::data::point({0.0, 0.0, 0.0}) == *srv_tester.point1); // Failed starting from there
    CPPUNIT_ASSERT(sight::data::point({0.0, 0.0, 0.0}) == *srv_tester.point2);
}

//------------------------------------------------------------------------------

void get_point_test::extracts_point_with_unknown_group()
{
    tester srv_tester;
    service::config_t config;
    std::stringstream config_string;
    config_string
    << R"(<inout group="points">)"
       R"(<key group="test_group_1" index="1" uid="output_point0"/>)"
       R"(<key group="unknown" index="2" uid="output_point1"/>)"
       R"(<key group="test_group_1" index="0" uid="output_point2"/>)"
       R"(</inout>)";
    boost::property_tree::read_xml(config_string, config);

    srv_tester.srv->set_config(config);
    srv_tester.srv->set_input(srv_tester.image, "image");
    srv_tester.srv->set_inout(srv_tester.point0, "points", false, false, 0);
    srv_tester.srv->set_inout(srv_tester.point1, "points", false, false, 1);
    srv_tester.srv->set_inout(srv_tester.point2, "points", false, false, 2);
    srv_tester.srv->configure();
    srv_tester.srv->start().get();
    CPPUNIT_ASSERT_THROW(srv_tester.srv->update().get(), sight::core::exception);

    CPPUNIT_ASSERT(sight::data::point({-1.0, 5.0, -2.0}) == *srv_tester.point0);
    CPPUNIT_ASSERT(sight::data::point({0.0, 0.0, 0.0}) == *srv_tester.point1); // Failed starting from there
    CPPUNIT_ASSERT(sight::data::point({0.0, 0.0, 0.0}) == *srv_tester.point2);
}

//------------------------------------------------------------------------------

} // namespace sight::module::data::ut
