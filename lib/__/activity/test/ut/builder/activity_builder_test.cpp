/************************************************************************
 *
 * Copyright (C) 2009-2026 IRCAD France
 * Copyright (C) 2012-2020 IHU Strasbourg
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

#include <activity/builder/base.hpp>
#include <activity/extension/activity.hpp>

#include <core/runtime/path.hpp>
#include <core/runtime/runtime.hpp>

#include <data/activity.hpp>
#include <data/image_series.hpp>
#include <data/map.hpp>
#include <data/model_series.hpp>
#include <data/string.hpp>
#include <data/vector.hpp>

#include <doctest/doctest.h>

namespace
{

struct activity_builder_fixture
{
    sight::activity::extension::activity::sptr m_activity;

    activity_builder_fixture()
    {
        m_activity = std::make_shared<sight::activity::extension::activity>();

        // Set up context before running a test.
        sight::core::runtime::add_modules(sight::core::runtime::get_resource_file_path("activity_ut"));
        sight::core::runtime::load_module("ut_registry");
        sight::core::runtime::load_module("ut_builder");

        m_activity->parse_plugin_infos();
    }

    ~activity_builder_fixture()
    {
        sight::activity::extension::activity::get_default()->clear_registry();
    }
};

} // namespace
TEST_SUITE("sight::activity::activity_builder")
{
    TEST_CASE_FIXTURE(activity_builder_fixture, "build_data_test")
    {
        sight::data::vector::sptr selection                   = std::make_shared<sight::data::vector>();
        sight::data::image_series::sptr img_series_selected   = std::make_shared<sight::data::image_series>();
        sight::data::model_series::sptr model_series_selected = std::make_shared<sight::data::model_series>();
        selection->push_back(img_series_selected);
        selection->push_back(model_series_selected);

        sight::activity::extension::activity::infos_t activities;
        activities = m_activity->get_infos(selection);
        CHECK_EQ(std::size_t(1), activities.size());

        sight::activity::extension::activity_info activity_info = activities[0];
        auto builder                                            =
            sight::activity::builder::factory::make(activity_info.builder_impl);
        CHECK_MESSAGE(builder, "<", activity_info.builder_impl, "> instantiation failed");
        sight::data::activity::sptr activity = builder->build_data(activity_info, selection);

        CHECK_MESSAGE(activity, "Activity instantiation failed");

        CHECK_EQ(activity_info.id, activity->get_activity_config_id());
        CHECK_EQ(std::size_t(2), activity->size());

        const std::string image_key = "imageSeries";
        const std::string model_key = "modelSeries";
        CHECK_MESSAGE(activity->find(image_key) != activity->end(), image_key, " key is missing");
        CHECK_MESSAGE(activity->find(model_key) != activity->end(), model_key, " key is missing");

        //single param [1;1]
        sight::data::object::sptr obj = (*activity)[image_key];
        CHECK(obj == img_series_selected);

        //set of param [0;2]
        obj = (*activity)[model_key];
        sight::data::map::sptr map = std::dynamic_pointer_cast<sight::data::map>(obj);
        CHECK_MESSAGE(map, model_key, " param dynamicCast to data::map failed");
        CHECK_EQ(std::size_t(1), map->size());
        CHECK(model_series_selected == (*map)["key1"]);
    }

    TEST_CASE_FIXTURE(activity_builder_fixture, "object_parser_test")
    {
        sight::core::runtime::add_modules(sight::core::runtime::get_resource_file_path("module_service"));
        auto module = sight::core::runtime::load_module("sight::module::service");
        sight::core::runtime::add_modules(sight::core::runtime::get_resource_file_path("module_app"));
        module = sight::core::runtime::load_module("sight::module::app");
        CHECK_MESSAGE(module, "Cannot load 'app' module.");

        sight::data::vector::sptr selection = std::make_shared<sight::data::vector>();
        auto activity_info                  = m_activity->get_info("TestBuilderObjectParser");
        auto builder                        = sight::activity::builder::factory::make(activity_info.builder_impl);
        CHECK_MESSAGE(builder, "<", activity_info.builder_impl, "> instantiation failed");
        sight::data::activity::sptr activity = builder->build_data(activity_info, selection);

        CHECK_MESSAGE(activity, "Activity instantiation failed");

        CHECK_EQ(activity_info.id, activity->get_activity_config_id());
        CHECK_EQ(std::size_t(1), activity->size());

        const std::string string_key = "string";
        CHECK_MESSAGE(activity->find(string_key) != activity->end(), string_key, " key is missing");

        // Check that the string parser is well executed
        // This implicitly test the function sight::activity::detail::data::create()
        const auto obj                   = (*activity)[string_key];
        sight::data::string::sptr string = std::dynamic_pointer_cast<sight::data::string>(obj);
        CHECK_MESSAGE(string, string_key, " param dynamicCast to data::string failed");
        CHECK_EQ(std::string("dummy string"), string->value());

        sight::core::runtime::unload_module("sight::module::app");
        sight::core::runtime::unload_module("sight::module::service");
    }
}
