/************************************************************************
 *
 * Copyright (C) 2009-2021 IRCAD France
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

#include "builder/ActivitySeriesBuilderTest.hpp"

#include <fwActivities/IBuilder.hpp>
#include <fwActivities/registry/Activities.hpp>

#include <core/runtime/Extension.hpp>
#include <core/runtime/Module.hpp>
#include <core/runtime/operations.hpp>

#include <data/ActivitySeries.hpp>
#include <data/Composite.hpp>
#include <data/ImageSeries.hpp>
#include <data/ModelSeries.hpp>
#include <data/Vector.hpp>

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION( ::fwActivities::ut::ActivitySeriesBuilderTest );

namespace fwActivities
{
namespace ut
{

//------------------------------------------------------------------------------

void ActivitySeriesBuilderTest::setUp()
{
    m_activities = ::fwActivities::registry::Activities::New();

    // Set up context before running a test.
    core::runtime::addModules(core::runtime::getResourceFilePath("tu_exec_fwActivities-0.0"));
    core::runtime::loadModule("tu_builder");

    m_activities->parseBundleInformation();
}

//------------------------------------------------------------------------------

void ActivitySeriesBuilderTest::tearDown()
{
    ::fwActivities::registry::Activities::getDefault()->clearRegistry();

    // Clean up after the test run.
    m_module.reset();
}

//------------------------------------------------------------------------------

void ActivitySeriesBuilderTest::buildDataTest()
{
    data::Vector::sptr selection                = data::Vector::New();
    data::ImageSeries::sptr imgSeriesSelected   = data::ImageSeries::New();
    data::ModelSeries::sptr modelSeriesSelected = data::ModelSeries::New();
    selection->getContainer().push_back(imgSeriesSelected);
    selection->getContainer().push_back(modelSeriesSelected);

    ::fwActivities::registry::Activities::ActivitiesType activities;
    activities = m_activities->getInfos(selection);
    CPPUNIT_ASSERT_EQUAL(size_t(1), activities.size());

    ::fwActivities::registry::ActivityInfo activityInfo = activities[0];
    data::ActivitySeries::sptr actSeries;
    ::fwActivities::IBuilder::sptr builder;
    builder = ::fwActivities::builder::factory::New(activityInfo.builderImpl);
    CPPUNIT_ASSERT_MESSAGE("<" + activityInfo.builderImpl + "> instantiation failed", builder);
    actSeries = builder->buildData(activityInfo, selection);

    CPPUNIT_ASSERT_MESSAGE("ActivitySeries instantiation failed", actSeries);

    CPPUNIT_ASSERT_EQUAL(activityInfo.id, actSeries->getActivityConfigId());

    data::Composite::sptr dataActivity = actSeries->getData();
    CPPUNIT_ASSERT_EQUAL(size_t(2), dataActivity->size());

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
    CPPUNIT_ASSERT_EQUAL(size_t(1), composite->size());
    CPPUNIT_ASSERT(modelSeriesSelected == (*composite)["key1"]);
}

//------------------------------------------------------------------------------

} //namespace ut
} //namespace fwActivities
