/************************************************************************
 *
 * Copyright (C) 2009-2022 IRCAD France
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
    // Clean up after the test run.
    m_module.reset();
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
    data::Activity::sptr activity;
    activity::IBuilder::sptr builder;
    builder = activity::builder::factory::New(activityInfo.builderImpl);
    CPPUNIT_ASSERT_MESSAGE("<" + activityInfo.builderImpl + "> instantiation failed", builder);
    activity = builder->buildData(activityInfo, selection);

    CPPUNIT_ASSERT_MESSAGE("Activity instantiation failed", activity);

    CPPUNIT_ASSERT_EQUAL(activityInfo.id, activity->getActivityConfigId());

    data::Composite::sptr dataActivity = activity->getData();
    CPPUNIT_ASSERT_EQUAL(std::size_t(2), dataActivity->size());

    const std::string imageKey = "imageSeries";
    const std::string modelKey = "modelSeries";
    CPPUNIT_ASSERT_MESSAGE(imageKey + " key is missing", dataActivity->find(imageKey) != dataActivity->end());
    CPPUNIT_ASSERT_MESSAGE(modelKey + " key is missing", dataActivity->find(modelKey) != dataActivity->end());

    //single param [1;1]
    data::Object::sptr obj = (*dataActivity)[imageKey];
    CPPUNIT_ASSERT(obj == imgSeriesSelected);

    //set of param [0;2]
    obj = (*dataActivity)[modelKey];
    data::Composite::sptr composite = data::Composite::dynamicCast(obj);
    CPPUNIT_ASSERT_MESSAGE(modelKey + " param dynamicCast to data::Composite failed", composite);
    CPPUNIT_ASSERT_EQUAL(std::size_t(1), composite->size());
    CPPUNIT_ASSERT(modelSeriesSelected == (*composite)["key1"]);
}

} //namespace sight::activity::ut
