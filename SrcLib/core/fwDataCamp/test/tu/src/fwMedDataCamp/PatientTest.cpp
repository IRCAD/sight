/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2013.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <boost/assign/std/vector.hpp>

#include <fwData/GenericFieldBase.hpp>

#include <fwDataCamp/Version.hpp>
#include <fwMedData/Patient.hpp>

#include "fwMedDataCamp/MedDataCampHelper.hpp"
#include "fwMedDataCamp/PatientTest.hpp"

using namespace ::boost::assign;

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION( ::fwMedDataCamp::ut::PatientTest );

namespace fwMedDataCamp
{
namespace ut
{

void PatientTest::setUp()
{
    // Set up context before running a test.
    //Force link with fwDataCamp
    const int version = ::fwDataCamp::Version::s_CURRENT_VERSION;
}

void PatientTest::tearDown()
{
    // Clean up after the test run.
}

//------------------------------------------------------------------------------

void PatientTest::propertiesTest()
{
    const std::string name = "Patient Name";
    const std::string id = "42";
    const std::string birth_date = "19830214";
    const std::string sex = "M";
    const MedDataCampHelper::PropertiesNameType dataProperties = list_of("fields")
                                                                        ("name")
                                                                        ("id")
                                                                        ("birth_date")
                                                                        ("sex");

    ::fwMedData::Patient::sptr obj = ::fwMedData::Patient::New();
    obj->setName(name);
    obj->setId(id);
    obj->setBirthdate(birth_date);
    obj->setSex(sex);

    MedDataCampHelper::visitProperties(obj->getClassname(), dataProperties);
    MedDataCampHelper::compareSimplePropertyValue(obj, "@name", name);
    MedDataCampHelper::compareSimplePropertyValue(obj, "@id", id);
    MedDataCampHelper::compareSimplePropertyValue(obj, "@birth_date", birth_date);
    MedDataCampHelper::compareSimplePropertyValue(obj, "@sex", sex);
}

//------------------------------------------------------------------------------

} //namespace ut
} //namespace fwMedDataCamp
