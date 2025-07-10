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

#include "matrix_to_point_test.hpp"

#include <core/runtime/runtime.hpp>

#include <data/matrix4.hpp>
#include <data/point.hpp>
#include <data/tools/picking_info.hpp>

#include <service/op.hpp>

#include <boost/property_tree/xml_parser.hpp>

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION(sight::module::geometry::ut::matrix_to_point_test);

namespace sight::module::geometry::ut
{

//------------------------------------------------------------------------------

void matrix_to_point_test::setUp()
{
}

//------------------------------------------------------------------------------

void matrix_to_point_test::tearDown()
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
        CPPUNIT_ASSERT_NO_THROW(srv = sight::service::add("sight::module::geometry::matrix_to_point"));
        CPPUNIT_ASSERT(srv != nullptr);
        CPPUNIT_ASSERT(srv->is_a("sight::module::geometry::matrix_to_point"));

        point = std::make_shared<sight::data::point>();
        srv->set_inout(point, "point");
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
    sight::data::point::sptr point;
};

} // namespace

//------------------------------------------------------------------------------

void matrix_to_point_test::update_test()
{
    context service_tester;
    auto matrix = std::make_shared<sight::data::matrix4>();
    service_tester.srv->set_input(matrix, "matrix");
    service_tester.set_config("");

    service_tester.srv->update().get();

    const auto& point = service_tester.point;
    CPPUNIT_ASSERT_EQUAL(0., (*point)[0]);
    CPPUNIT_ASSERT_EQUAL(0., (*point)[1]);
    CPPUNIT_ASSERT_EQUAL(0., (*point)[2]);

    data::matrix4::container_t expected;
    expected[3]  = 14.0;
    expected[7]  = 15.0;
    expected[11] = -112.5;
    *matrix      = expected;

    service_tester.srv->update().get();

    CPPUNIT_ASSERT_EQUAL(expected[3], (*point)[0]);
    CPPUNIT_ASSERT_EQUAL(expected[7], (*point)[1]);
    CPPUNIT_ASSERT_EQUAL(expected[11], (*point)[2]);
}

} // namespace sight::module::geometry::ut
