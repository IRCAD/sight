/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "EquipmentTest.hpp"

#include "DataCampHelper.hpp"

#include <fwMedDataCamp/Version.hpp>

#include <fwMedData/Equipment.hpp>

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION( ::fwMedDataCamp::ut::EquipmentTest );

namespace fwMedDataCamp
{
namespace ut
{

void EquipmentTest::setUp()
{
    // Set up context before running a test.
    //Force link with fwMedDataCamp
    const int version = ::fwMedDataCamp::Version::s_CURRENT_VERSION;
    FwCoreNotUsedMacro(version);
}

void EquipmentTest::tearDown()
{
    // Clean up after the test run.
}

//------------------------------------------------------------------------------

void EquipmentTest::propertiesTest()
{
    const std::string institution_name                        = "IHU Strasbourg / IRCAD";
    const ::DataCampHelper::PropertiesNameType dataProperties = {"fields", "institution_name"};

    ::fwMedData::Equipment::sptr obj = ::fwMedData::Equipment::New();
    obj->setInstitutionName(institution_name);

    ::DataCampHelper::visitProperties(obj->getClassname(), dataProperties);
    ::DataCampHelper::compareSimplePropertyValue(obj, "@institution_name", institution_name);

}

//------------------------------------------------------------------------------

} //namespace ut
} //namespace fwMedDataCamp
