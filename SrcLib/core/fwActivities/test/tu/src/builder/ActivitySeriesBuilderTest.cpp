/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2013.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <fwRuntime/EConfigurationElement.hpp>

#include <fwData/Image.hpp>
#include <fwData/Composite.hpp>

#include <fwActivities/ActivitySeries.hpp>
#include <fwActivities/IBuilder.hpp>

#include "builder/ActivitySeriesBuilderTest.hpp"


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
}

//------------------------------------------------------------------------------

void ActivitySeriesBuilderTest::tearDown()
{
    // Clean up after the test run.
}

//------------------------------------------------------------------------------

void ActivitySeriesBuilderTest::builDataTest()
{
    const std::string defaultActivitySeriesBuilder = "::fwActivities::builder::ActivitySeries";
    ::fwActivities::ActivitySeries::sptr actSeries;
    ::fwActivities::IBuilder::sptr builder;
    builder = ::fwActivities::builder::factory::New(defaultActivitySeriesBuilder);

    CPPUNIT_ASSERT_MESSAGE(defaultActivitySeriesBuilder + " instantiation failed", builder);

    const std::string configID = "Visu2D";
    ::fwData::Composite::sptr composite = ::fwData::Composite::New();
    (*composite)["image"] = ::fwData::Image::New();

    ::fwRuntime::EConfigurationElement::sptr config = ::fwRuntime::EConfigurationElement::New("requirements");

    builder->setConfiguration(config);
    actSeries = builder->buildData(composite, configID);

    CPPUNIT_ASSERT_MESSAGE("ActivitySeries instantiation failed", actSeries);
}

//------------------------------------------------------------------------------

} //namespace ut
} //namespace fwActivities
