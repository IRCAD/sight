/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2013.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <boost/assign/std/vector.hpp>

#include <fwDataCamp/Version.hpp>
#include <fwMedData/Study.hpp>

#include "fwMedDataCamp/MedDataCampHelper.hpp"
#include "fwMedDataCamp/StudyTest.hpp"

using namespace ::boost::assign;

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION( ::fwMedDataCamp::ut::StudyTest );

namespace fwMedDataCamp
{
namespace ut
{

void StudyTest::setUp()
{
    // Set up context before running a test.
    //Force link with fwDataCamp
    const int version = ::fwDataCamp::Version::s_CURRENT_VERSION;
}

void StudyTest::tearDown()
{
    // Clean up after the test run.
}

//------------------------------------------------------------------------------

void StudyTest::propertiesTest()
{
    const std::string instance_uid = "123456789";
    const std::string date = "20130214";
    const std::string time = "143328";
    const std::string referring_physician_name = "John Doe";
    const std::string description = "description";
    const std::string patient_age = "42";
    const MedDataCampHelper::PropertiesNameType dataProperties = list_of("fields")
                                                                        ("instance_uid")
                                                                        ("date")
                                                                        ("time")
                                                                        ("referring_physician_name")
                                                                        ("description")
                                                                        ("patient_age");

    ::fwMedData::Study::sptr obj = ::fwMedData::Study::New();
    obj->setInstanceUID(instance_uid);
    obj->setDate(date);
    obj->setTime(time);
    obj->setReferringPhysicianName(referring_physician_name);
    obj->setDescription(description);
    obj->setPatientAge(patient_age);

    MedDataCampHelper::visitProperties(obj->getClassname(), dataProperties);
    MedDataCampHelper::compareSimplePropertyValue(obj, "@instance_uid", instance_uid);
    MedDataCampHelper::compareSimplePropertyValue(obj, "@date", date);
    MedDataCampHelper::compareSimplePropertyValue(obj, "@time", time);
    MedDataCampHelper::compareSimplePropertyValue(obj, "@referring_physician_name", referring_physician_name);
    MedDataCampHelper::compareSimplePropertyValue(obj, "@description", description);
    MedDataCampHelper::compareSimplePropertyValue(obj, "@patient_age", patient_age);
}

//------------------------------------------------------------------------------


} //namespace ut
} //namespace fwMedDataCamp
