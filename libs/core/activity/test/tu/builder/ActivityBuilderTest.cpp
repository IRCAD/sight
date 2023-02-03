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

#include "ActivityBuilderTest.hpp"

#include <activity/extension/Activity.hpp>
#include <activity/IBuilder.hpp>

#include <core/runtime/path.hpp>
#include <core/runtime/runtime.hpp>

#include <data/Activity.hpp>
#include <data/Composite.hpp>
#include <data/ImageSeries.hpp>
#include <data/ModelSeries.hpp>
#include <data/String.hpp>
#include <data/Vector.hpp>

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION(sight::activity::ut::ActivityBuilderTest);

namespace sight::activity::ut
{

//------------------------------------------------------------------------------

void ActivityBuilderTest::setUp()
{
    m_activity = activity::extension::Activity::New();

    // Set up context before running a test.
    core::runtime::addModules(core::runtime::getResourceFilePath("tu_exec_activity"));
    core::runtime::loadModule("tu_builder");

    m_activity->parseBundleInformation();
}

//------------------------------------------------------------------------------

void ActivityBuilderTest::tearDown()
{
    activity::extension::Activity::getDefault()->clearRegistry();
}

//------------------------------------------------------------------------------

void ActivityBuilderTest::buildDataTest()
{
    data::Vector::sptr selection                = data::Vector::New();
    data::ImageSeries::sptr imgSeriesSelected   = data::ImageSeries::New();
    data::ModelSeries::sptr modelSeriesSelected = data::ModelSeries::New();
    selection->push_back(imgSeriesSelected);
    selection->push_back(modelSeriesSelected);

    activity::extension::Activity::ActivitiesType activities;
    activities = m_activity->getInfos(selection);
    CPPUNIT_ASSERT_EQUAL(std::size_t(2), activities.size());

    activity::extension::ActivityInfo activityInfo = activities[0];
    activity::IBuilder::sptr builder               = activity::builder::factory::New(activityInfo.builderImpl);
    CPPUNIT_ASSERT_MESSAGE("<" + activityInfo.builderImpl + "> instantiation failed", builder);
    data::Activity::sptr activity = builder->buildData(activityInfo, selection);

    CPPUNIT_ASSERT_MESSAGE("Activity instantiation failed", activity);

    CPPUNIT_ASSERT_EQUAL(activityInfo.id, activity->getActivityConfigId());
    CPPUNIT_ASSERT_EQUAL(std::size_t(2), activity->size());

    const std::string imageKey = "imageSeries";
    const std::string modelKey = "modelSeries";
    CPPUNIT_ASSERT_MESSAGE(imageKey + " key is missing", activity->find(imageKey) != activity->end());
    CPPUNIT_ASSERT_MESSAGE(modelKey + " key is missing", activity->find(modelKey) != activity->end());

    //single param [1;1]
    data::Object::sptr obj = (*activity)[imageKey];
    CPPUNIT_ASSERT(obj == imgSeriesSelected);

    //set of param [0;2]
    obj = (*activity)[modelKey];
    data::Composite::sptr composite = data::Composite::dynamicCast(obj);
    CPPUNIT_ASSERT_MESSAGE(modelKey + " param dynamicCast to data::Composite failed", composite);
    CPPUNIT_ASSERT_EQUAL(std::size_t(1), composite->size());
    CPPUNIT_ASSERT(modelSeriesSelected == (*composite)["key1"]);
}

//------------------------------------------------------------------------------

void ActivityBuilderTest::objectParserTest()
{
    core::runtime::addModules(core::runtime::getResourceFilePath("module_service"));
    auto module = core::runtime::loadModule("sight::module::service");
    CPPUNIT_ASSERT_MESSAGE("Cannot load 'service' module.", module);

    data::Vector::sptr selection     = data::Vector::New();
    auto activityInfo                = m_activity->getInfo("TestBuilderObjectParser");
    activity::IBuilder::sptr builder = activity::builder::factory::New(activityInfo.builderImpl);
    CPPUNIT_ASSERT_MESSAGE("<" + activityInfo.builderImpl + "> instantiation failed", builder);
    data::Activity::sptr activity = builder->buildData(activityInfo, selection);

    CPPUNIT_ASSERT_MESSAGE("Activity instantiation failed", activity);

    CPPUNIT_ASSERT_EQUAL(activityInfo.id, activity->getActivityConfigId());
    CPPUNIT_ASSERT_EQUAL(std::size_t(1), activity->size());

    const std::string stringKey = "string";
    CPPUNIT_ASSERT_MESSAGE(stringKey + " key is missing", activity->find(stringKey) != activity->end());

    // Check that the string parser is well executed
    // This implicitly test the function sight::activity::detail::data::create()
    const auto obj            = (*activity)[stringKey];
    data::String::sptr string = data::String::dynamicCast(obj);
    CPPUNIT_ASSERT_MESSAGE(stringKey + " param dynamicCast to data::Composite failed", string);
    CPPUNIT_ASSERT_EQUAL(std::string("dummy string"), string->value());

    core::runtime::unloadModule("sight::module::service");
}

} //namespace sight::activity::ut
