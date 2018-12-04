/************************************************************************
 *
 * Copyright (C) 2009-2016 IRCAD France
 * Copyright (C) 2012-2016 IHU Strasbourg
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

#include "StudyTest.hpp"

#include "DataCampHelper.hpp"

#include <fwMedDataCamp/Version.hpp>

#include <fwMedData/Study.hpp>

#include <boost/assign/std/vector.hpp>

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
    //Force link with fwMedDataCamp
    const int version = ::fwMedDataCamp::Version::s_CURRENT_VERSION;
    FwCoreNotUsedMacro(version);
}

void StudyTest::tearDown()
{
    // Clean up after the test run.
}

//------------------------------------------------------------------------------

void StudyTest::propertiesTest()
{
    const std::string instance_uid                            = "123456789";
    const std::string date                                    = "20130214";
    const std::string time                                    = "143328";
    const std::string referring_physician_name                = "John Doe";
    const std::string description                             = "description";
    const std::string patient_age                             = "42";
    const ::DataCampHelper::PropertiesNameType dataProperties = list_of("fields")
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

    ::DataCampHelper::visitProperties(obj->getClassname(), dataProperties);
    ::DataCampHelper::compareSimplePropertyValue(obj, "@instance_uid", instance_uid);
    ::DataCampHelper::compareSimplePropertyValue(obj, "@date", date);
    ::DataCampHelper::compareSimplePropertyValue(obj, "@time", time);
    ::DataCampHelper::compareSimplePropertyValue(obj, "@referring_physician_name", referring_physician_name);
    ::DataCampHelper::compareSimplePropertyValue(obj, "@description", description);
    ::DataCampHelper::compareSimplePropertyValue(obj, "@patient_age", patient_age);
}

//------------------------------------------------------------------------------


} //namespace ut
} //namespace fwMedDataCamp
