/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2013.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <boost/assign/std/vector.hpp>

#include <fwDataCamp/Version.hpp>
#include <fwMedData/Equipment.hpp>
#include <fwData/GenericFieldBase.hpp>

#include "fwMedDataCamp/EquipmentTest.hpp"
#include "fwMedDataCamp/MedDataCampHelper.hpp"

using namespace ::boost::assign;

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION( ::fwMedDataCamp::ut::EquipmentTest );

namespace fwMedDataCamp
{
namespace ut
{

void EquipmentTest::setUp()
{
    // Set up context before running a test.
    //Force link with fwDataCamp
    const int version = ::fwDataCamp::Version::s_CURRENT_VERSION;
}

void EquipmentTest::tearDown()
{
    // Clean up after the test run.
}

//------------------------------------------------------------------------------

void EquipmentTest::propertiesTest()
{
    const std::string institution_name = "IHU Strasbourg / IRCAD";
    const MedDataCampHelper::PropertiesNameType dataProperties = list_of("fields")
                                                                        ("institution_name");

    ::fwMedData::Equipment::sptr obj = ::fwMedData::Equipment::New();
    obj->setInstitutionName(institution_name);

    MedDataCampHelper::visitProperties(obj->getClassname(), dataProperties);
    MedDataCampHelper::compareSimplePropertyValue(obj, "@institution_name", institution_name);

}

//------------------------------------------------------------------------------

} //namespace ut
} //namespace fwMedDataCamp
