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

#include <utest/service_fixture.hpp>

namespace
{

class service_fixture : public sight::utest::service_fixture
{
public:

    service_fixture() :
        sight::utest::service_fixture("sight::module::data::get_point")
    {
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
        const std::string config =
            "<inout group='points'>"
            "<key group='test_group_2' index='1' uid='output_point0'/>"
            "<key group='test_group_1' index='0' uid='output_point1'/>"
            "<key group='test_group_1' index='1' uid='output_point2'/>"
            "</inout>";

        m_service->set_config(config);
        m_service->set_input(image, "image");
        m_service->set_inout(point0, "points", false, false, 0);
        m_service->set_inout(point1, "points", false, false, 1);
        m_service->set_inout(point2, "points", false, false, 2);
        m_service->configure();
        m_service->start().get();
        CHECK_NOTHROW(m_service->update().get());

        CHECK((sight::data::point({8.0, 9.0, -20.0}) == *point0));
        CHECK((sight::data::point({1.0, 80.0, 2.0}) == *point1));
        CHECK((sight::data::point({-1.0, 5.0, -2.0}) == *point2));
    }

//------------------------------------------------------------------------------

    TEST_CASE_FIXTURE(service_fixture, "extracts_point_with_index_out_of_bound")
    {
        const std::string config =
            "<inout group='points'>"
            "<key group='test_group_1' index='1' uid='output_point0'/>"
            "<key group='test_group_1' index='122' uid='output_point1'/>"
            "<key group='test_group_1' index='0' uid='output_point2'/>"
            "</inout>";

        m_service->set_config(config);
        m_service->set_input(image, "image");
        m_service->set_inout(point0, "points", false, false, 0);
        m_service->set_inout(point1, "points", false, false, 1);
        m_service->set_inout(point2, "points", false, false, 2);
        m_service->configure();
        m_service->start().get();
        CHECK_THROWS_AS(m_service->update().get(), sight::core::exception);

        CHECK((sight::data::point({-1.0, 5.0, -2.0}) == *point0));
        CHECK((sight::data::point({0.0, 0.0, 0.0}) == *point1)); // Failed starting from there
        CHECK((sight::data::point({0.0, 0.0, 0.0}) == *point2));
    }

//------------------------------------------------------------------------------

    TEST_CASE_FIXTURE(service_fixture, "extracts_point_with_unknown_group")
    {
        const std::string config =
            "<inout group='points'>"
            "<key group='test_group_1' index='1' uid='output_point0'/>"
            "<key group='unknown' index='2' uid='output_point1'/>"
            "<key group='test_group_1' index='0' uid='output_point2'/>"
            "</inout>";

        m_service->set_config(config);
        m_service->set_input(image, "image");
        m_service->set_inout(point0, "points", false, false, 0);
        m_service->set_inout(point1, "points", false, false, 1);
        m_service->set_inout(point2, "points", false, false, 2);
        m_service->configure();
        m_service->start().get();
        CHECK_THROWS_AS(m_service->update().get(), sight::core::exception);

        CHECK((sight::data::point({-1.0, 5.0, -2.0}) == *point0));
        CHECK((sight::data::point({0.0, 0.0, 0.0}) == *point1)); // Failed starting from there
        CHECK((sight::data::point({0.0, 0.0, 0.0}) == *point2));
    }

//------------------------------------------------------------------------------
} // TEST_SUITE("sight::module::data::get_point")
