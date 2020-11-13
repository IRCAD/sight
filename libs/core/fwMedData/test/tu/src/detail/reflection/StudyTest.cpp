/************************************************************************
 *
 * Copyright (C) 2009-2020 IRCAD France
 * Copyright (C) 2012-2020 IHU Strasbourg
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

#include "detail/reflection/StudyTest.hpp"

#include "detail/reflection/DataCampHelper.hpp"

#include <fwMedData/Study.hpp>

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION( ::fwMedData::detail::reflection::ut::StudyTest );

namespace fwMedData
{
namespace detail
{
namespace reflection
{
namespace ut
{

//------------------------------------------------------------------------------

void StudyTest::setUp()
{
}

//------------------------------------------------------------------------------

void StudyTest::tearDown()
{
    // Clean up after the test run.
}

//------------------------------------------------------------------------------

void StudyTest::propertiesTest()
{
    const std::string instanceUID             = "123456789";
    const std::string studyID                 = "2";
    const std::string date                    = "20130214";
    const std::string time                    = "143328";
    const std::string referringPhysicianMame  = "John Doe";
    const std::string consultingPhysicianMame = "John Doe 2";
    const std::string description             = "description";
    const std::string patientAge              = "42";
    const std::string patientSize             = "175";
    const std::string patientWeight           = "70";
    const std::string patientBodyMassIndex    = "2";

    const ::DataCampHelper::PropertiesNameType dataProperties = { { "fields" },
                                                                  { "instance_uid" },
                                                                  { "study_id" },
                                                                  { "date" },
                                                                  { "time" },
                                                                  { "referring_physician_name" },
                                                                  { "consulting_physician_name" },
                                                                  { "description" },
                                                                  { "patient_age" },
                                                                  { "patient_size" },
                                                                  { "patient_weight" },
                                                                  { "patient_body_mass_index" }};

    ::fwMedData::Study::sptr obj = ::fwMedData::Study::New();
    obj->setInstanceUID(instanceUID);
    obj->setStudyID(studyID);
    obj->setDate(date);
    obj->setTime(time);
    obj->setReferringPhysicianName(referringPhysicianMame);
    obj->setConsultingPhysicianName(consultingPhysicianMame);
    obj->setDescription(description);
    obj->setPatientAge(patientAge);
    obj->setPatientSize(patientSize);
    obj->setPatientWeight(patientWeight);
    obj->setPatientBodyMassIndex(patientBodyMassIndex);

    ::DataCampHelper::visitProperties(obj->getClassname(), dataProperties);
    ::DataCampHelper::compareSimplePropertyValue(obj, "@instance_uid", instanceUID);
    ::DataCampHelper::compareSimplePropertyValue(obj, "@study_id", studyID);
    ::DataCampHelper::compareSimplePropertyValue(obj, "@date", date);
    ::DataCampHelper::compareSimplePropertyValue(obj, "@time", time);
    ::DataCampHelper::compareSimplePropertyValue(obj, "@referring_physician_name", referringPhysicianMame);
    ::DataCampHelper::compareSimplePropertyValue(obj, "@consulting_physician_name", consultingPhysicianMame);
    ::DataCampHelper::compareSimplePropertyValue(obj, "@description", description);
    ::DataCampHelper::compareSimplePropertyValue(obj, "@patient_age", patientAge);
    ::DataCampHelper::compareSimplePropertyValue(obj, "@patient_size", patientSize);
    ::DataCampHelper::compareSimplePropertyValue(obj, "@patient_weight", patientWeight);
    ::DataCampHelper::compareSimplePropertyValue(obj, "@patient_body_mass_index", patientBodyMassIndex);
}

//------------------------------------------------------------------------------

} //namespace ut
} //namespace reflection
} //namespace detail
} //namespace fwMedData
