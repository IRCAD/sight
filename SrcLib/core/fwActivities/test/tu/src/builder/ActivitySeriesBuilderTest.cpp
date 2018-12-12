/************************************************************************
 *
 * Copyright (C) 2009-2018 IRCAD France
 * Copyright (C) 2012-2018 IHU Strasbourg
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

#include <fwData/Composite.hpp>
#include <fwData/Vector.hpp>

#include <fwMedData/ActivitySeries.hpp>
#include <fwMedData/ImageSeries.hpp>
#include <fwMedData/ModelSeries.hpp>

#include <fwRuntime/Bundle.hpp>
#include <fwRuntime/Extension.hpp>
#include <fwRuntime/io/BundleDescriptorReader.hpp>
#include <fwRuntime/operations.hpp>

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION( ::fwActivities::ut::ActivitySeriesBuilderTest );

namespace fwActivities
{
namespace ut
{

//------------------------------------------------------------------------------

void ActivitySeriesBuilderTest::setUp()
{
    // Set up context before running a test.
    ::boost::filesystem::path plugin = ::fwRuntime::getResourceFilePath("tu_exec_fwActivities-0.0/tu_builder");
    m_bundle                         = ::fwRuntime::io::BundleDescriptorReader::createBundle(plugin);

    m_activities = ::fwActivities::registry::Activities::New();

    ::fwRuntime::Bundle::ExtensionContainer extensionsSet( m_bundle->extensionsBegin(), m_bundle->extensionsEnd());
    std::vector< SPTR( ::fwRuntime::Extension ) > extensions(extensionsSet.begin(), extensionsSet.end());
    m_activities->parseBundleInformation(extensions);

    CPPUNIT_ASSERT_EQUAL( size_t(1), extensions.size());
}

//------------------------------------------------------------------------------

void ActivitySeriesBuilderTest::tearDown()
{
    // Clean up after the test run.
    m_bundle.reset();
}

//------------------------------------------------------------------------------

void ActivitySeriesBuilderTest::builDataTest()
{
    ::fwData::Vector::sptr selection                   = ::fwData::Vector::New();
    ::fwMedData::ImageSeries::sptr imgSeriesSelected   = ::fwMedData::ImageSeries::New();
    ::fwMedData::ModelSeries::sptr modelSeriesSelected = ::fwMedData::ModelSeries::New();
    selection->getContainer().push_back(imgSeriesSelected);
    selection->getContainer().push_back(modelSeriesSelected);

    ::fwActivities::registry::Activities::ActivitiesType activities;
    activities = m_activities->getInfos(selection);
    CPPUNIT_ASSERT_EQUAL(size_t(1), activities.size());

    ::fwActivities::registry::ActivityInfo activityInfo = activities[0];
    ::fwMedData::ActivitySeries::sptr actSeries;
    ::fwActivities::IBuilder::sptr builder;
    builder = ::fwActivities::builder::factory::New(activityInfo.builderImpl);
    CPPUNIT_ASSERT_MESSAGE("<" + activityInfo.builderImpl + "> instantiation failed", builder);
    actSeries = builder->buildData(activityInfo, selection);

    CPPUNIT_ASSERT_MESSAGE("ActivitySeries instantiation failed", actSeries);

    CPPUNIT_ASSERT_EQUAL(activityInfo.id, actSeries->getActivityConfigId());

    ::fwData::Composite::sptr dataActivity = actSeries->getData();
    CPPUNIT_ASSERT_EQUAL(size_t(2), dataActivity->size());

    const std::string imageKey = "imageSeries";
    const std::string modelKey = "modelSeries";
    CPPUNIT_ASSERT_MESSAGE(imageKey + " key is missing", dataActivity->find(imageKey) != dataActivity->end());
    CPPUNIT_ASSERT_MESSAGE(modelKey + " key is missing", dataActivity->find(modelKey) != dataActivity->end());

    //single param [1;1]
    ::fwData::Object::sptr obj = (*dataActivity)[imageKey];
    CPPUNIT_ASSERT(obj == imgSeriesSelected);

    //set of param [0;2]
    obj                                 = (*dataActivity)[modelKey];
    ::fwData::Composite::sptr composite = ::fwData::Composite::dynamicCast(obj);
    CPPUNIT_ASSERT_MESSAGE(modelKey + " param dynamicCast to fwData::Composite failed", composite);
    CPPUNIT_ASSERT_EQUAL(size_t(1), composite->size());
    CPPUNIT_ASSERT(modelSeriesSelected == (*composite)["key1"]);
}

//------------------------------------------------------------------------------

} //namespace ut
} //namespace fwActivities
