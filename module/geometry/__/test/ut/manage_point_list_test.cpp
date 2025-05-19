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

#include "manage_point_list_test.hpp"

#include <core/runtime/runtime.hpp>

#include <data/matrix4.hpp>
#include <data/point_list.hpp>
#include <data/tools/picking_info.hpp>

#include <service/op.hpp>

#include <boost/property_tree/xml_parser.hpp>

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION(sight::module::geometry::ut::manage_point_list_test);

namespace sight::module::geometry::ut
{

//------------------------------------------------------------------------------

void manage_point_list_test::setUp()
{
}

//------------------------------------------------------------------------------

void manage_point_list_test::tearDown()
{
}

//------------------------------------------------------------------------------

namespace
{

class context final
{
public:

    context()
    {
        CPPUNIT_ASSERT_NO_THROW(srv = sight::service::add("sight::module::geometry::manage_point_list"));
        CPPUNIT_ASSERT(srv != nullptr);
        CPPUNIT_ASSERT(srv->is_a("sight::module::geometry::manage_point_list"));

        point_list = std::make_shared<sight::data::point_list>();
        srv->set_inout(point_list, "point_list");
    }

    //------------------------------------------------------------------------------

    ~context()
    {
        if(srv->started())
        {
            CPPUNIT_ASSERT_NO_THROW(srv->stop().wait());
        }

        sight::service::remove(srv);
    }

    //------------------------------------------------------------------------------

    void set_config(const std::string& _config)
    {
        sight::service::base::config_t config;
        std::stringstream stream(_config);
        boost::property_tree::read_xml(stream, config);

        srv->set_config(config);

        srv->configure();
        srv->start().wait();
    }

    sight::service::base::sptr srv;
    sight::data::point_list::sptr point_list;
};

} // namespace

//------------------------------------------------------------------------------

void manage_point_list_test::update_test()
{
    context service_tester;
    auto input_point = std::make_shared<sight::data::matrix4>();
    service_tester.srv->set_input(input_point, "position");

    service_tester.set_config("");

    service_tester.srv->update().get();

    const auto& point_list = service_tester.point_list->get_points();
    CPPUNIT_ASSERT_EQUAL(std::size_t(1), point_list.size());
    CPPUNIT_ASSERT_EQUAL((*input_point)[3], (*point_list[0])[0]);
    CPPUNIT_ASSERT_EQUAL((*input_point)[7], (*point_list[0])[1]);
    CPPUNIT_ASSERT_EQUAL((*input_point)[11], (*point_list[0])[2]);

    data::matrix4::container_t expected;
    expected[3]  = 14.0;
    expected[7]  = 15.0;
    expected[11] = -112.5;
    *input_point = expected;

    service_tester.srv->update().get();

    CPPUNIT_ASSERT_EQUAL(std::size_t(2), point_list.size());
    CPPUNIT_ASSERT_EQUAL(0., (*point_list[0])[0]);
    CPPUNIT_ASSERT_EQUAL(0., (*point_list[0])[1]);
    CPPUNIT_ASSERT_EQUAL(0., (*point_list[0])[2]);

    CPPUNIT_ASSERT_EQUAL(expected[3], (*point_list[1])[0]);
    CPPUNIT_ASSERT_EQUAL(expected[7], (*point_list[1])[1]);
    CPPUNIT_ASSERT_EQUAL(expected[11], (*point_list[1])[2]);

    service_tester.srv->slot("clear")->run();
    CPPUNIT_ASSERT_EQUAL(std::size_t(0), point_list.size());

    service_tester.srv->update().get();
    CPPUNIT_ASSERT_EQUAL(std::size_t(1), point_list.size());
    CPPUNIT_ASSERT_EQUAL(expected[3], (*point_list[0])[0]);
    CPPUNIT_ASSERT_EQUAL(expected[7], (*point_list[0])[1]);
    CPPUNIT_ASSERT_EQUAL(expected[11], (*point_list[0])[2]);
}

//------------------------------------------------------------------------------
void manage_point_list_test::pick_test()
{
    context service_tester;
    service_tester.set_config(R"(<config max="0" />)");

    const auto& point_list = service_tester.point_list->get_points();

    sight::data::tools::picking_info info;
    service_tester.srv->slot("pick")->run(info);

    CPPUNIT_ASSERT_EQUAL(std::size_t(1), point_list.size());
    CPPUNIT_ASSERT_EQUAL(0., (*point_list[0])[0]);
    CPPUNIT_ASSERT_EQUAL(0., (*point_list[0])[1]);
    CPPUNIT_ASSERT_EQUAL(0., (*point_list[0])[2]);

    info.m_world_pos[0] = 14.;
    info.m_world_pos[1] = -18.;
    info.m_world_pos[2] = 19.;
    service_tester.srv->slot("pick")->run(info);

    CPPUNIT_ASSERT_EQUAL(std::size_t(2), point_list.size());
    CPPUNIT_ASSERT_EQUAL(0., (*point_list[0])[0]);
    CPPUNIT_ASSERT_EQUAL(0., (*point_list[0])[1]);
    CPPUNIT_ASSERT_EQUAL(0., (*point_list[0])[2]);
    CPPUNIT_ASSERT_EQUAL(info.m_world_pos[0], (*point_list[1])[0]);
    CPPUNIT_ASSERT_EQUAL(info.m_world_pos[1], (*point_list[1])[1]);
    CPPUNIT_ASSERT_EQUAL(info.m_world_pos[2], (*point_list[1])[2]);
}

//------------------------------------------------------------------------------
void manage_point_list_test::max_test()
{
    context service_tester;
    service_tester.set_config(R"(<config max="2" />)");

    const auto& point_list = service_tester.point_list->get_points();

    sight::data::tools::picking_info info;
    service_tester.srv->slot("pick")->run(info);

    CPPUNIT_ASSERT_EQUAL(std::size_t(1), point_list.size());
    CPPUNIT_ASSERT_EQUAL(0., (*point_list[0])[0]);
    CPPUNIT_ASSERT_EQUAL(0., (*point_list[0])[1]);
    CPPUNIT_ASSERT_EQUAL(0., (*point_list[0])[2]);

    info.m_world_pos[0] = 14.;
    info.m_world_pos[1] = -18.;
    info.m_world_pos[2] = 19.;
    service_tester.srv->slot("pick")->run(info);

    CPPUNIT_ASSERT_EQUAL(std::size_t(2), point_list.size());
    CPPUNIT_ASSERT_EQUAL(0., (*point_list[0])[0]);
    CPPUNIT_ASSERT_EQUAL(0., (*point_list[0])[1]);
    CPPUNIT_ASSERT_EQUAL(0., (*point_list[0])[2]);
    CPPUNIT_ASSERT_EQUAL(14., (*point_list[1])[0]);
    CPPUNIT_ASSERT_EQUAL(-18., (*point_list[1])[1]);
    CPPUNIT_ASSERT_EQUAL(19., (*point_list[1])[2]);

    info.m_world_pos[0] = 89.;
    info.m_world_pos[1] = 84.;
    info.m_world_pos[2] = -19.;
    service_tester.srv->slot("pick")->run(info);

    CPPUNIT_ASSERT_EQUAL(std::size_t(2), point_list.size());
    CPPUNIT_ASSERT_EQUAL(14., (*point_list[0])[0]);
    CPPUNIT_ASSERT_EQUAL(-18., (*point_list[0])[1]);
    CPPUNIT_ASSERT_EQUAL(19., (*point_list[0])[2]);
    CPPUNIT_ASSERT_EQUAL(info.m_world_pos[0], (*point_list[1])[0]);
    CPPUNIT_ASSERT_EQUAL(info.m_world_pos[1], (*point_list[1])[1]);
    CPPUNIT_ASSERT_EQUAL(info.m_world_pos[2], (*point_list[1])[2]);
}

} // namespace sight::module::geometry::ut
