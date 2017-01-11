/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "PatientTest.hpp"

#include "DataCampHelper.hpp"

#include <fwMedDataCamp/Version.hpp>

#include <fwMedData/Patient.hpp>

#include <boost/assign/std/vector.hpp>

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
    //Force link with fwMedDataCamp
    const int version = ::fwMedDataCamp::Version::s_CURRENT_VERSION;
    FwCoreNotUsedMacro(version);
}

void PatientTest::tearDown()
{
    // Clean up after the test run.
}

//------------------------------------------------------------------------------

void PatientTest::propertiesTest()
{
    const std::string name                                    = "Patient Name";
    const std::string patient_id                              = "42";
    const std::string birth_date                              = "19830214";
    const std::string sex                                     = "M";
    const ::DataCampHelper::PropertiesNameType dataProperties = list_of("fields")
                                                                    ("name")
                                                                    ("patient_id")
                                                                    ("birth_date")
                                                                    ("sex");

    ::fwMedData::Patient::sptr obj = ::fwMedData::Patient::New();
    obj->setName(name);
    obj->setPatientId(patient_id);
    obj->setBirthdate(birth_date);
    obj->setSex(sex);

    ::DataCampHelper::visitProperties(obj->getClassname(), dataProperties);
    ::DataCampHelper::compareSimplePropertyValue(obj, "@name", name);
    ::DataCampHelper::compareSimplePropertyValue(obj, "@patient_id", patient_id);
    ::DataCampHelper::compareSimplePropertyValue(obj, "@birth_date", birth_date);
    ::DataCampHelper::compareSimplePropertyValue(obj, "@sex", sex);
}

//------------------------------------------------------------------------------

} //namespace ut
} //namespace fwMedDataCamp
