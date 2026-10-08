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

#include <data/activity_set.hpp>
#include <data/image_series.hpp>
#include <data/model_series.hpp>

#include <service/op.hpp>

#include <doctest/doctest.h>

TEST_SUITE("sight::module::data::get_activity_object")
{
//------------------------------------------------------------------------------

    TEST_CASE("extract_objects")
    {
        auto activity_set = std::make_shared<sight::data::activity_set>();

        sight::service::base::sptr srv = sight::service::add("sight::module::data::get_activity_object");
        CHECK(srv);
        CHECK(srv->is_a("sight::module::data::get_activity_object"));

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
        CHECK(activity1->empty());
        CHECK(activity2->empty());

        (*activity1)["req1"] = series1;
        (*activity1)["req2"] = series2;
        (*activity1)["req3"] = series3;

        (*activity2)["req1"] = series4;
        (*activity2)["req2"] = series5;
        (*activity2)["req3"] = series6;

        CHECK(activity_set->empty());

        activity_set->push_back(activity1);
        activity_set->push_back(activity2);

        const std::string config =
            "<id>config1</id>"
            "<out group='objects'>"
            "<key name='req1' uid='object1'/>"
            "<key name='req2' uid='object2'/>"
            "</out>";
        srv->set_config(config);
        srv->set_input(activity_set, "activity_set");
        srv->configure();
        srv->start().get();
        srv->update().get();

        CHECK(*srv->output("objects", 0).lock() == *series1);
        CHECK(*srv->output("objects", 1).lock() == *series2);

        activity1->erase("req2");

        CHECK_NOTHROW(srv->update().get());

        CHECK(*srv->output("objects", 0).lock() == *series1);
        CHECK(srv->output("objects", 1).lock() == nullptr);

        activity_set->clear();
        CHECK_THROWS_AS(srv->update().get(), sight::data::exception);

        CHECK(srv->output("objects", 0).lock() == nullptr);
        CHECK(srv->output("objects", 1).lock() == nullptr);

        srv->stop().get();
        sight::service::remove(srv);
    }

//------------------------------------------------------------------------------

    TEST_CASE("invalid_activity")
    {
        auto activity_set = std::make_shared<sight::data::activity_set>();

        sight::service::base::sptr srv = sight::service::add("sight::module::data::get_activity_object");
        CHECK(srv);
        CHECK(srv->is_a("sight::module::data::get_activity_object"));

        // create different series
        sight::data::series::sptr series1     = std::make_shared<sight::data::model_series>();
        sight::data::activity::sptr activity1 = std::make_shared<sight::data::activity>();
        activity1->set_activity_config_id("config1");
        (*activity1)["req1"] = series1;
        activity_set->push_back(activity1);

        const std::string config =
            "<id>config_unknown</id>"
            "<out group='objects'>"
            "<key name='req1' uid='object1'/>"
            "<key name='req2' uid='object2'/>"
            "</out>";
        srv->set_config(config);
        srv->set_input(activity_set, "activity_set");
        srv->configure();
        srv->start().get();
        CHECK_THROWS_AS(srv->update().get(), sight::data::exception);

        srv->stop().get();
        sight::service::remove(srv);
    }

//------------------------------------------------------------------------------
} // TEST_SUITE("sight::module::data::get_activity_object")
