/************************************************************************
 *
 * Copyright (C) 2009-2023 IRCAD France
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

#include "activity_builder_test.hpp"

#include <activity/builder/base.hpp>
#include <activity/extension/activity.hpp>

#include <core/runtime/path.hpp>
#include <core/runtime/runtime.hpp>

#include <data/activity.hpp>
#include <data/composite.hpp>
#include <data/image_series.hpp>
#include <data/model_series.hpp>
#include <data/string.hpp>
#include <data/vector.hpp>

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION(sight::activity::ut::activity_builder_test);

namespace sight::activity::ut
{

//------------------------------------------------------------------------------

void activity_builder_test::setUp()
{
    m_activity = std::make_shared<activity::extension::activity>();

    // Set up context before running a test.
    core::runtime::add_modules(core::runtime::get_resource_file_path("tu_exec_activity"));
    core::runtime::load_module("tu_builder");

    m_activity->parse_plugin_infos();
}

//------------------------------------------------------------------------------

void activity_builder_test::tearDown()
{
    activity::extension::activity::getDefault()->clear_registry();
}

//------------------------------------------------------------------------------

void activity_builder_test::buildDataTest()
{
    data::vector::sptr selection                   = std::make_shared<data::vector>();
    data::image_series::sptr img_series_selected   = std::make_shared<data::image_series>();
    data::model_series::sptr model_series_selected = std::make_shared<data::model_series>();
    selection->push_back(img_series_selected);
    selection->push_back(model_series_selected);

    activity::extension::activity::infos_t activities;
    activities = m_activity->getInfos(selection);
    CPPUNIT_ASSERT_EQUAL(std::size_t(2), activities.size());

    activity::extension::activity_info activity_info = activities[0];
    auto builder                                     =
        sight::activity::builder::factory::make(activity_info.builderImpl);
    CPPUNIT_ASSERT_MESSAGE("<" + activity_info.builderImpl + "> instantiation failed", builder);
    data::activity::sptr activity = builder->buildData(activity_info, selection);

    CPPUNIT_ASSERT_MESSAGE("Activity instantiation failed", activity);

    CPPUNIT_ASSERT_EQUAL(activity_info.id, activity->getActivityConfigId());
    CPPUNIT_ASSERT_EQUAL(std::size_t(2), activity->size());

    const std::string image_key = "imageSeries";
    const std::string model_key = "modelSeries";
    CPPUNIT_ASSERT_MESSAGE(image_key + " key is missing", activity->find(image_key) != activity->end());
    CPPUNIT_ASSERT_MESSAGE(model_key + " key is missing", activity->find(model_key) != activity->end());

    //single param [1;1]
    data::object::sptr obj = (*activity)[image_key];
    CPPUNIT_ASSERT(obj == img_series_selected);

    //set of param [0;2]
    obj = (*activity)[model_key];
    data::composite::sptr composite = std::dynamic_pointer_cast<data::composite>(obj);
    CPPUNIT_ASSERT_MESSAGE(model_key + " param dynamicCast to data::composite failed", composite);
    CPPUNIT_ASSERT_EQUAL(std::size_t(1), composite->size());
    CPPUNIT_ASSERT(model_series_selected == (*composite)["key1"]);
}

//------------------------------------------------------------------------------

void activity_builder_test::objectParserTest()
{
    core::runtime::add_modules(core::runtime::get_resource_file_path("module_service"));
    auto module = core::runtime::load_module("sight::module::service");
    core::runtime::add_modules(core::runtime::get_resource_file_path("module_app"));
    module = core::runtime::load_module("sight::module::app");
    CPPUNIT_ASSERT_MESSAGE("Cannot load 'service' module.", module);

    data::vector::sptr selection = std::make_shared<data::vector>();
    auto activity_info           = m_activity->getInfo("TestBuilderObjectParser");
    auto builder                 = sight::activity::builder::factory::make(activity_info.builderImpl);
    CPPUNIT_ASSERT_MESSAGE("<" + activity_info.builderImpl + "> instantiation failed", builder);
    data::activity::sptr activity = builder->buildData(activity_info, selection);

    CPPUNIT_ASSERT_MESSAGE("Activity instantiation failed", activity);

    CPPUNIT_ASSERT_EQUAL(activity_info.id, activity->getActivityConfigId());
    CPPUNIT_ASSERT_EQUAL(std::size_t(1), activity->size());

    const std::string string_key = "string";
    CPPUNIT_ASSERT_MESSAGE(string_key + " key is missing", activity->find(string_key) != activity->end());

    // Check that the string parser is well executed
    // This implicitly test the function sight::activity::detail::data::create()
    const auto obj            = (*activity)[string_key];
    data::string::sptr string = std::dynamic_pointer_cast<data::string>(obj);
    CPPUNIT_ASSERT_MESSAGE(string_key + " param dynamicCast to data::composite failed", string);
    CPPUNIT_ASSERT_EQUAL(std::string("dummy string"), string->value());

    core::runtime::unload_module("sight::module::app");
    core::runtime::unload_module("sight::module::service");
}

} //namespace sight::activity::ut
