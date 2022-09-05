/************************************************************************
 *
 * Copyright (C) 2009-2022 IRCAD France
 * Copyright (C) 2012-2018 IHU Strasbourg
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

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION(sight::data::ut::StudyTest);

namespace sight::data::ut
{

//------------------------------------------------------------------------------

void StudyTest::setUp()
{
    // Set up context before running a test.
    m_study = data::Study::New();
}

//------------------------------------------------------------------------------

void StudyTest::tearDown()
{
    // Clean up after the test run.
    m_study.reset();
}

//------------------------------------------------------------------------------

void StudyTest::instanceUIDTest()
{
    const std::string instanceUID = "1346357.1664.643101.421337.4123403";
    CPPUNIT_ASSERT(m_study);
    m_study->setInstanceUID(instanceUID);
    CPPUNIT_ASSERT_EQUAL(instanceUID, m_study->getInstanceUID());
}

//------------------------------------------------------------------------------

void StudyTest::dateTest()
{
    const std::string date = "02-14-2015";
    CPPUNIT_ASSERT(m_study);
    m_study->setDate(date);
    CPPUNIT_ASSERT_EQUAL(date, m_study->getDate());
}

//------------------------------------------------------------------------------

void StudyTest::timeTest()
{
    const std::string time = "11:59";
    CPPUNIT_ASSERT(m_study);
    m_study->setTime(time);
    CPPUNIT_ASSERT_EQUAL(time, m_study->getTime());
}

//------------------------------------------------------------------------------

void StudyTest::referringPhysicianNameTest()
{
    const std::string rpn = "Dr Jekyl";
    CPPUNIT_ASSERT(m_study);
    m_study->setReferringPhysicianName(rpn);
    CPPUNIT_ASSERT_EQUAL(rpn, m_study->getReferringPhysicianName());
}

//------------------------------------------------------------------------------

void StudyTest::descriptionTest()
{
    const std::string desc = "Say 33.";
    CPPUNIT_ASSERT(m_study);
    m_study->setDescription(desc);
    CPPUNIT_ASSERT_EQUAL(desc, m_study->getDescription());
}

//------------------------------------------------------------------------------

void StudyTest::patientAgeTest()
{
    const std::string age = "42";
    CPPUNIT_ASSERT(m_study);
    m_study->setPatientAge(age);
    CPPUNIT_ASSERT_EQUAL(age, m_study->getPatientAge());
}

//------------------------------------------------------------------------------

void StudyTest::equalityTest()
{
    auto study1 = data::Study::New();
    auto study2 = data::Study::New();

    CPPUNIT_ASSERT(*study1 == *study2);

    study1->setConsultingPhysicianName("1");
    CPPUNIT_ASSERT(*study1 != *study2);
    study2->setConsultingPhysicianName(study1->getConsultingPhysicianName());
    CPPUNIT_ASSERT(*study1 == *study2);

    study1->setDate("2");
    CPPUNIT_ASSERT(*study1 != *study2);
    study2->setDate(study1->getDate());
    CPPUNIT_ASSERT(*study1 == *study2);

    study1->setDescription("3");
    CPPUNIT_ASSERT(*study1 != *study2);
    study2->setDescription(study1->getDescription());
    CPPUNIT_ASSERT(*study1 == *study2);

    study1->setInstanceUID("4");
    CPPUNIT_ASSERT(*study1 != *study2);
    study2->setInstanceUID(study1->getInstanceUID());
    CPPUNIT_ASSERT(*study1 == *study2);

    study1->setPatientAge("5");
    CPPUNIT_ASSERT(*study1 != *study2);
    study2->setPatientAge(study1->getPatientAge());
    CPPUNIT_ASSERT(*study1 == *study2);

    study1->setPatientBodyMassIndex("6");
    CPPUNIT_ASSERT(*study1 != *study2);
    study2->setPatientBodyMassIndex(study1->getPatientBodyMassIndex());
    CPPUNIT_ASSERT(*study1 == *study2);

    study1->setPatientSize("7");
    CPPUNIT_ASSERT(*study1 != *study2);
    study2->setPatientSize(study1->getPatientSize());
    CPPUNIT_ASSERT(*study1 == *study2);

    study1->setPatientWeight("8");
    CPPUNIT_ASSERT(*study1 != *study2);
    study2->setPatientWeight(study1->getPatientWeight());
    CPPUNIT_ASSERT(*study1 == *study2);

    study1->setReferringPhysicianName("9");
    CPPUNIT_ASSERT(*study1 != *study2);
    study2->setReferringPhysicianName(study1->getReferringPhysicianName());
    CPPUNIT_ASSERT(*study1 == *study2);

    study1->setReferringPhysicianName("10");
    CPPUNIT_ASSERT(*study1 != *study2);
    study2->setReferringPhysicianName(study1->getReferringPhysicianName());
    CPPUNIT_ASSERT(*study1 == *study2);

    study1->setStudyID("11");
    CPPUNIT_ASSERT(*study1 != *study2);
    study2->setStudyID(study1->getStudyID());
    CPPUNIT_ASSERT(*study1 == *study2);

    study1->setTime("12");
    CPPUNIT_ASSERT(*study1 != *study2);
    study2->setTime(study1->getTime());
    CPPUNIT_ASSERT(*study1 == *study2);

    // Test also deepcopy, just for fun
    auto Patient3 = data::Study::New();
    Patient3->deepCopy(study1);
    CPPUNIT_ASSERT(*study1 == *Patient3);
}

//------------------------------------------------------------------------------

} // namespace sight::data::ut
