/************************************************************************
 *
 * Copyright (C) 2025-2026 IRCAD France
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

#include <core/runtime/runtime.hpp>

#include <data/image_series.hpp>
#include <data/point.hpp>

#include <service/op.hpp>

#include <boost/property_tree/xml_parser.hpp>

#include <doctest/doctest.h>

namespace
{

class service_fixture
{
public:

    service_fixture()
    {
        // Create service
        srv = sight::service::add("sight::module::data::get_point");
        CHECK(srv);
        CHECK(srv->is_a("sight::module::data::get_point"));

        // Create data
        image = std::make_shared<sight::data::image_series>();
        CHECK(image);

        auto fiducials = image->get_fiducials();
        fiducials->add_group("test_group_1", {1.0, 0.0, 1.0, 1.0}, 3.F);
        fiducials->add_point("test_group_1", {1.0, 80.0, 2.0});
        fiducials->add_point("test_group_1", {-1.0, 5.0, -2.0});

        fiducials->add_group("test_group_2", {1.0, 0.0, 1.0, 1.0}, 3.F);
        fiducials->add_point("test_group_2", {6.0, 4.0, 7.0});
        fiducials->add_point("test_group_2", {8.0, 9.0, -20.0});

        point0 = std::make_shared<sight::data::point>();
        CHECK(point0);
        point1 = std::make_shared<sight::data::point>();
        CHECK(point1);
        point2 = std::make_shared<sight::data::point>();
        CHECK(point2);
    }

    //------------------------------------------------------------------------------

    ~service_fixture()
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

} // namespace

TEST_SUITE("sight::module::data::get_point")
{
//------------------------------------------------------------------------------

    TEST_CASE_FIXTURE(service_fixture, "extracts_point_by_index")
    {
        sight::service::config_t config;
        std::stringstream config_string;
        config_string
        << R"(<inout group="points">)"
           R"(<key group="test_group_2" index="1" uid="output_point0"/>)"
           R"(<key group="test_group_1" index="0" uid="output_point1"/>)"
           R"(<key group="test_group_1" index="1" uid="output_point2"/>)"
           R"(</inout>)";
        boost::property_tree::read_xml(config_string, config);

        srv->set_config(config);
        srv->set_input(image, "image");
        srv->set_inout(point0, "points", false, false, 0);
        srv->set_inout(point1, "points", false, false, 1);
        srv->set_inout(point2, "points", false, false, 2);
        srv->configure();
        srv->start().get();
        CHECK_NOTHROW(srv->update().get());

        CHECK((sight::data::point({8.0, 9.0, -20.0}) == *point0));
        CHECK((sight::data::point({1.0, 80.0, 2.0}) == *point1));
        CHECK((sight::data::point({-1.0, 5.0, -2.0}) == *point2));
    }

//------------------------------------------------------------------------------

    TEST_CASE_FIXTURE(service_fixture, "extracts_point_with_index_out_of_bound")
    {
        sight::service::config_t config;
        std::stringstream config_string;
        config_string
        << R"(<inout group="points">)"
           R"(<key group="test_group_1" index="1" uid="output_point0"/>)"
           R"(<key group="test_group_1" index="122" uid="output_point1"/>)"
           R"(<key group="test_group_1" index="0" uid="output_point2"/>)"
           R"(</inout>)";
        boost::property_tree::read_xml(config_string, config);

        srv->set_config(config);
        srv->set_input(image, "image");
        srv->set_inout(point0, "points", false, false, 0);
        srv->set_inout(point1, "points", false, false, 1);
        srv->set_inout(point2, "points", false, false, 2);
        srv->configure();
        srv->start().get();
        CHECK_THROWS_AS(srv->update().get(), sight::core::exception);

        CHECK((sight::data::point({-1.0, 5.0, -2.0}) == *point0));
        CHECK((sight::data::point({0.0, 0.0, 0.0}) == *point1)); // Failed starting from there
        CHECK((sight::data::point({0.0, 0.0, 0.0}) == *point2));
    }

//------------------------------------------------------------------------------

    TEST_CASE_FIXTURE(service_fixture, "extracts_point_with_unknown_group")
    {
        sight::service::config_t config;
        std::stringstream config_string;
        config_string
        << R"(<inout group="points">)"
           R"(<key group="test_group_1" index="1" uid="output_point0"/>)"
           R"(<key group="unknown" index="2" uid="output_point1"/>)"
           R"(<key group="test_group_1" index="0" uid="output_point2"/>)"
           R"(</inout>)";
        boost::property_tree::read_xml(config_string, config);

        srv->set_config(config);
        srv->set_input(image, "image");
        srv->set_inout(point0, "points", false, false, 0);
        srv->set_inout(point1, "points", false, false, 1);
        srv->set_inout(point2, "points", false, false, 2);
        srv->configure();
        srv->start().get();
        CHECK_THROWS_AS(srv->update().get(), sight::core::exception);

        CHECK((sight::data::point({-1.0, 5.0, -2.0}) == *point0));
        CHECK((sight::data::point({0.0, 0.0, 0.0}) == *point1)); // Failed starting from there
        CHECK((sight::data::point({0.0, 0.0, 0.0}) == *point2));
    }

//------------------------------------------------------------------------------
} // TEST_SUITE("sight::module::data::get_point")
