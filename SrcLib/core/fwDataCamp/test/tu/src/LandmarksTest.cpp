/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "LandmarksTest.hpp"

#include "DataCampHelper.hpp"

#include <fwDataCamp/Version.hpp>

#include <fwData/Landmarks.hpp>
#include <fwData/String.hpp>

#include <boost/assign/std/vector.hpp>

using namespace ::boost::assign;

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION( ::fwMedDataCamp::ut::LandmarksTest );

namespace fwMedDataCamp
{
namespace ut
{

//------------------------------------------------------------------------------

void LandmarksTest::setUp()
{
    // Set up context before running a test.
    //Force link with fwMedDataCamp
    const int version = ::fwDataCamp::Version::s_CURRENT_VERSION;
    FwCoreNotUsedMacro(version);
}

//------------------------------------------------------------------------------

void LandmarksTest::tearDown()
{
    // Clean up after the test run.
}

//------------------------------------------------------------------------------

void LandmarksTest::propertiesTest()
{
    const std::string VALUE_NAME = "myValueName";
    const std::string VALUE      = "myValue";
    ::DataCampHelper::PropertiesNameType dataProperties = list_of("fields");

    ::fwData::Landmarks::sptr obj = ::fwData::Landmarks::New();
    ::fwData::String::sptr str    = ::fwData::String::New(VALUE);
    obj->setField(VALUE_NAME, str);

    ::DataCampHelper::visitProperties(obj->getClassname(), dataProperties);
    ::DataCampHelper::compareSimplePropertyValue(obj, "@fields.myValueName", VALUE);
}

//------------------------------------------------------------------------------

} //namespace ut
} //namespace fwMedDataCamp
