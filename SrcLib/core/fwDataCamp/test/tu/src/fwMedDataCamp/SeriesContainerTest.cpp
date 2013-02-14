/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2013.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <boost/assign/std/vector.hpp>

#include <fwDataCamp/Version.hpp>
#include <fwMedData/ModelSeries.hpp>
#include <fwMedData/ImageSeries.hpp>
#include <fwMedData/SeriesContainer.hpp>

#include "fwMedDataCamp/SeriesContainerTest.hpp"
#include "fwMedDataCamp/MedDataCampHelper.hpp"

using namespace ::boost::assign;

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION( ::fwMedDataCamp::ut::SeriesContainerTest );

namespace fwMedDataCamp
{
namespace ut
{

void SeriesContainerTest::setUp()
{
    // Set up context before running a test.
    //Force link with fwDataCamp
    const int version = ::fwDataCamp::Version::s_CURRENT_VERSION;
}

void SeriesContainerTest::tearDown()
{
    // Clean up after the test run.
}

//------------------------------------------------------------------------------

void SeriesContainerTest::propertiesTest()
{
    const MedDataCampHelper::PropertiesNameType dataProperties = list_of("fields")
                                                                        ("values");

    ::fwMedData::SeriesContainer::sptr obj = ::fwMedData::SeriesContainer::New();
    ::fwMedData::SeriesContainer::ContainerType vectSeries;
    vectSeries.push_back(::fwMedData::ImageSeries::New());
    vectSeries.push_back(::fwMedData::ImageSeries::New());
    vectSeries.push_back(::fwMedData::ModelSeries::New());
    obj->setContainer(vectSeries);

    MedDataCampHelper::visitProperties(obj->getClassname(), dataProperties);
    MedDataCampHelper::compareObjectPropertyValue(obj, "@values.0", vectSeries[0]);
    MedDataCampHelper::compareObjectPropertyValue(obj, "@values.1", vectSeries[1]);
    MedDataCampHelper::compareObjectPropertyValue(obj, "@values.2", vectSeries[2]);
}

//------------------------------------------------------------------------------

} //namespace ut
} //namespace fwMedDataCamp
