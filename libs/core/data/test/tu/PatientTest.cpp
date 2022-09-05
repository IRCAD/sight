/************************************************************************
 *
 * Copyright (C) 2009-2022 IRCAD France
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

#include "PatientTest.hpp"

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION(sight::data::ut::PatientTest);

namespace sight::data::ut
{

//------------------------------------------------------------------------------

void PatientTest::setUp()
{
    // Set up context before running a test.
    m_patient = data::Patient::New();
}

//------------------------------------------------------------------------------

void PatientTest::tearDown()
{
    // Clean up after the test run.
    m_patient.reset();
}

//------------------------------------------------------------------------------

void PatientTest::nameTest()
{
    const std::string name = "John DOE";
    CPPUNIT_ASSERT(m_patient);
    m_patient->setName(name);
    CPPUNIT_ASSERT_EQUAL(name, m_patient->getName());
}

//------------------------------------------------------------------------------

void PatientTest::idTest()
{
    const std::string patientId = "42";
    CPPUNIT_ASSERT(m_patient);
    m_patient->setPatientId(patientId);
    CPPUNIT_ASSERT_EQUAL(patientId, m_patient->getPatientId());
}

//------------------------------------------------------------------------------

void PatientTest::birthdateTest()
{
    const std::string birthdate = "02-14-2015 11:28";
    CPPUNIT_ASSERT(m_patient);
    m_patient->setBirthdate(birthdate);
    CPPUNIT_ASSERT_EQUAL(birthdate, m_patient->getBirthdate());
}

//------------------------------------------------------------------------------

void PatientTest::sexTest()
{
    const std::string sex = "M";
    CPPUNIT_ASSERT(m_patient);
    m_patient->setSex(sex);
    CPPUNIT_ASSERT_EQUAL(sex, m_patient->getSex());
}

//------------------------------------------------------------------------------

void PatientTest::equalityTest()
{
    auto patient1 = data::Patient::New();
    auto patient2 = data::Patient::New();

    CPPUNIT_ASSERT(*patient1 == *patient2);

    patient1->setBirthdate("1");
    CPPUNIT_ASSERT(*patient1 != *patient2);
    patient2->setBirthdate(patient1->getBirthdate());
    CPPUNIT_ASSERT(*patient1 == *patient2);

    patient1->setName("2");
    CPPUNIT_ASSERT(*patient1 != *patient2);
    patient2->setName(patient1->getName());
    CPPUNIT_ASSERT(*patient1 == *patient2);

    patient1->setPatientId("3");
    CPPUNIT_ASSERT(*patient1 != *patient2);
    patient2->setPatientId(patient1->getPatientId());
    CPPUNIT_ASSERT(*patient1 == *patient2);

    patient1->setSex("4");
    CPPUNIT_ASSERT(*patient1 != *patient2);
    patient2->setSex(patient1->getSex());
    CPPUNIT_ASSERT(*patient1 == *patient2);

    // Test also deepcopy, just for fun
    auto Patient3 = data::Patient::New();
    Patient3->deepCopy(patient1);
    CPPUNIT_ASSERT(*patient1 == *Patient3);
}

} // namespace sight::data::ut
