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

#include "get_activity_object_test.hpp"

#include <core/runtime/runtime.hpp>

#include <data/activity_set.hpp>
#include <data/image_series.hpp>
#include <data/model_series.hpp>

#include <service/op.hpp>

#include <boost/property_tree/xml_parser.hpp>

#include <sstream>

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION(sight::module::data::ut::get_activity_object_test);

namespace sight::module::data::ut
{

//------------------------------------------------------------------------------

void get_activity_object_test::setUp()
{
}

//------------------------------------------------------------------------------

void get_activity_object_test::tearDown()
{
}

//------------------------------------------------------------------------------

void get_activity_object_test::extract_objects()
{
    auto activity_set = std::make_shared<sight::data::activity_set>();

    sight::service::base::sptr srv = sight::service::add("sight::module::data::get_activity_object");
    CPPUNIT_ASSERT(srv);
    CPPUNIT_ASSERT(srv->is_a("sight::module::data::get_activity_object"));

    // create different series
    sight::data::series::sptr series1 = std::make_shared<sight::data::model_series>();
    sight::data::series::sptr series2 = std::make_shared<sight::data::image_series>();
    sight::data::series::sptr series3 = std::make_shared<sight::data::model_series>();
    sight::data::series::sptr series4 = std::make_shared<sight::data::image_series>();
    sight::data::series::sptr series5 = std::make_shared<sight::data::model_series>();
    sight::data::series::sptr series6 = std::make_shared<sight::data::model_series>();

    sight::data::activity::sptr activity1 = std::make_shared<sight::data::activity>();
    sight::data::activity::sptr activity2 = std::make_shared<sight::data::activity>();

    activity1->set_activity_config_id("config1");
    activity2->set_activity_config_id("config2");
    CPPUNIT_ASSERT(activity1->empty());
    CPPUNIT_ASSERT(activity2->empty());

    (*activity1)["req1"] = series1;
    (*activity1)["req2"] = series2;
    (*activity1)["req3"] = series3;

    (*activity2)["req1"] = series4;
    (*activity2)["req2"] = series5;
    (*activity2)["req3"] = series6;

    CPPUNIT_ASSERT(activity_set->empty());

    activity_set->push_back(activity1);
    activity_set->push_back(activity2);

    service::config_t config;
    std::stringstream config_string;
    config_string
    << R"(<id>config1</id>)"
       R"(<out group="objects">)"
       R"(<key name="req1" uid="object1"/>)"
       R"(<key name="req2" uid="object2"/>)"
       R"(</out>)";
    boost::property_tree::read_xml(config_string, config);
    srv->set_config(config);
    srv->set_input(activity_set, "activity_set");
    srv->configure();
    srv->start().wait();
    srv->update().wait();

    CPPUNIT_ASSERT(*srv->output("objects", 0).lock() == *series1);
    CPPUNIT_ASSERT(*srv->output("objects", 1).lock() == *series2);

    activity1->erase("req2");

    CPPUNIT_ASSERT_NO_THROW(srv->update().get());

    CPPUNIT_ASSERT(*srv->output("objects", 0).lock() == *series1);
    CPPUNIT_ASSERT(srv->output("objects", 1).lock() == nullptr);

    activity_set->clear();
    CPPUNIT_ASSERT_THROW(srv->update().get(), sight::data::exception);

    CPPUNIT_ASSERT(srv->output("objects", 0).lock() == nullptr);
    CPPUNIT_ASSERT(srv->output("objects", 1).lock() == nullptr);

    srv->stop().wait();
    sight::service::remove(srv);
}

//------------------------------------------------------------------------------

void get_activity_object_test::invalid_activity()
{
    auto activity_set = std::make_shared<sight::data::activity_set>();

    sight::service::base::sptr srv = sight::service::add("sight::module::data::get_activity_object");
    CPPUNIT_ASSERT(srv);
    CPPUNIT_ASSERT(srv->is_a("sight::module::data::get_activity_object"));

    // create different series
    sight::data::series::sptr series1     = std::make_shared<sight::data::model_series>();
    sight::data::activity::sptr activity1 = std::make_shared<sight::data::activity>();
    activity1->set_activity_config_id("config1");
    (*activity1)["req1"] = series1;
    activity_set->push_back(activity1);

    service::config_t config;
    std::stringstream config_string;
    config_string
    << R"(<id>config_unknown</id>)"
       R"(<out group="objects">)"
       R"(<key name="req1" uid="object1"/>)"
       R"(<key name="req2" uid="object2"/>)"
       R"(</out>)";
    boost::property_tree::read_xml(config_string, config);
    srv->set_config(config);
    srv->set_input(activity_set, "activity_set");
    srv->configure();
    srv->start().wait();
    CPPUNIT_ASSERT_THROW(srv->update().get(), sight::data::exception);

    srv->stop().wait();
    sight::service::remove(srv);
}

//------------------------------------------------------------------------------

} // namespace sight::module::data::ut
