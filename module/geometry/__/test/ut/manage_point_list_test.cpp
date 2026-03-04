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

#include <data/matrix4.hpp>
#include <data/point_list.hpp>
#include <data/tools/picking_info.hpp>

#include <service/op.hpp>

#include <boost/property_tree/xml_parser.hpp>

#include <doctest/doctest.h>

namespace
{

class context
{
public:

    context()
    {
        CHECK_NOTHROW(srv = sight::service::add("sight::module::geometry::manage_point_list"));
        CHECK(srv != nullptr);
        CHECK(srv->is_a("sight::module::geometry::manage_point_list"));

        point_list = std::make_shared<sight::data::point_list>();
        srv->set_inout(point_list, "point_list");
    }

    //------------------------------------------------------------------------------

    ~context()
    {
        if(srv->started())
        {
            CHECK_NOTHROW(srv->stop().wait());
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

TEST_SUITE("sight::module::geometry::manage_point_list")
{
    TEST_CASE_FIXTURE(context, "update_test")
    {
        auto input_point = std::make_shared<sight::data::matrix4>();
        srv->set_input(input_point, "position");

        set_config("");

        srv->update().get();

        CHECK_EQ(std::size_t(1), point_list->size());
        CHECK_EQ((*input_point)[3], (*(*point_list)[0])[0]);
        CHECK_EQ((*input_point)[7], (*(*point_list)[0])[1]);
        CHECK_EQ((*input_point)[11], (*(*point_list)[0])[2]);

        sight::data::matrix4::container_t expected;
        expected[3]  = 14.0;
        expected[7]  = 15.0;
        expected[11] = -112.5;
        *input_point = expected;

        srv->update().get();

        CHECK_EQ(std::size_t(2), point_list->size());
        CHECK_EQ(0., (*(*point_list)[0])[0]);
        CHECK_EQ(0., (*(*point_list)[0])[1]);
        CHECK_EQ(0., (*(*point_list)[0])[2]);

        CHECK_EQ(expected[3], (*(*point_list)[1])[0]);
        CHECK_EQ(expected[7], (*(*point_list)[1])[1]);
        CHECK_EQ(expected[11], (*(*point_list)[1])[2]);

        srv->slot("clear")->run();
        CHECK_EQ(std::size_t(0), point_list->size());

        srv->update().get();
        CHECK_EQ(std::size_t(1), point_list->size());
        CHECK_EQ(expected[3], (*(*point_list)[0])[0]);
        CHECK_EQ(expected[7], (*(*point_list)[0])[1]);
        CHECK_EQ(expected[11], (*(*point_list)[0])[2]);
    }

//------------------------------------------------------------------------------

    TEST_CASE_FIXTURE(context, "pick_test")
    {
        set_config(R"(<config max="0" />)");

        sight::data::tools::picking_info info;
        srv->slot("pick")->run(info);

        CHECK_EQ(std::size_t(1), point_list->size());
        CHECK_EQ(0., (*(*point_list)[0])[0]);
        CHECK_EQ(0., (*(*point_list)[0])[1]);
        CHECK_EQ(0., (*(*point_list)[0])[2]);

        info.m_world_pos[0] = 14.;
        info.m_world_pos[1] = -18.;
        info.m_world_pos[2] = 19.;
        srv->slot("pick")->run(info);

        CHECK_EQ(std::size_t(2), point_list->size());
        CHECK_EQ(0., (*(*point_list)[0])[0]);
        CHECK_EQ(0., (*(*point_list)[0])[1]);
        CHECK_EQ(0., (*(*point_list)[0])[2]);
        CHECK_EQ(info.m_world_pos[0], (*(*point_list)[1])[0]);
        CHECK_EQ(info.m_world_pos[1], (*(*point_list)[1])[1]);
        CHECK_EQ(info.m_world_pos[2], (*(*point_list)[1])[2]);
    }

//------------------------------------------------------------------------------

    TEST_CASE_FIXTURE(context, "max_test")
    {
        set_config(R"(<config max="2" />)");

        sight::data::tools::picking_info info;
        srv->slot("pick")->run(info);

        CHECK_EQ(std::size_t(1), point_list->size());
        CHECK_EQ(0., (*(*point_list)[0])[0]);
        CHECK_EQ(0., (*(*point_list)[0])[1]);
        CHECK_EQ(0., (*(*point_list)[0])[2]);

        info.m_world_pos[0] = 14.;
        info.m_world_pos[1] = -18.;
        info.m_world_pos[2] = 19.;
        srv->slot("pick")->run(info);

        CHECK_EQ(std::size_t(2), point_list->size());
        CHECK_EQ(0., (*(*point_list)[0])[0]);
        CHECK_EQ(0., (*(*point_list)[0])[1]);
        CHECK_EQ(0., (*(*point_list)[0])[2]);
        CHECK_EQ(14., (*(*point_list)[1])[0]);
        CHECK_EQ(-18., (*(*point_list)[1])[1]);
        CHECK_EQ(19., (*(*point_list)[1])[2]);

        info.m_world_pos[0] = 89.;
        info.m_world_pos[1] = 84.;
        info.m_world_pos[2] = -19.;
        srv->slot("pick")->run(info);

        CHECK_EQ(std::size_t(2), point_list->size());
        CHECK_EQ(14., (*(*point_list)[0])[0]);
        CHECK_EQ(-18., (*(*point_list)[0])[1]);
        CHECK_EQ(19., (*(*point_list)[0])[2]);
        CHECK_EQ(info.m_world_pos[0], (*(*point_list)[1])[0]);
        CHECK_EQ(info.m_world_pos[1], (*(*point_list)[1])[1]);
        CHECK_EQ(info.m_world_pos[2], (*(*point_list)[1])[2]);
    }
} // TEST_SUITE
