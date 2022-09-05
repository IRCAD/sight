/************************************************************************
 *
 * Copyright (C) 2009-2022 IRCAD France
 * Copyright (C) 2012-2015 IHU Strasbourg
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

#include "SeriesTest.hpp"

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION(sight::data::ut::SeriesTest);

namespace sight::data::ut
{

//------------------------------------------------------------------------------

void SeriesTest::setUp()
{
    // Set up context before running a test.
    m_series.push_back(data::ImageSeries::New());
    m_series.push_back(data::ModelSeries::New());
}

//------------------------------------------------------------------------------

void SeriesTest::tearDown()
{
    // Clean up after the test run.
    m_series.clear();
}

//------------------------------------------------------------------------------

void SeriesTest::attrPatientTest()
{
    for(const auto& series : m_series)
    {
        CPPUNIT_ASSERT(series->getPatient());
    }
}

//------------------------------------------------------------------------------

void SeriesTest::attrStudyTest()
{
    for(const auto& series : m_series)
    {
        CPPUNIT_ASSERT(series->getStudy());
    }
}

//------------------------------------------------------------------------------

void SeriesTest::attrEquipmentTest()
{
    for(const auto& series : m_series)
    {
        CPPUNIT_ASSERT(series->getEquipment());
    }
}

//------------------------------------------------------------------------------

void SeriesTest::attrInstanceUIDTest()
{
    const std::string instanceUID = "1337.1664.42";

    for(const auto& series : m_series)
    {
        series->setInstanceUID(instanceUID);

        CPPUNIT_ASSERT_EQUAL(instanceUID, series->getInstanceUID());
    }
}

//------------------------------------------------------------------------------

void SeriesTest::attrModalityTest()
{
    const std::string modality = "MR";

    for(const auto& series : m_series)
    {
        series->setModality(modality);

        CPPUNIT_ASSERT_EQUAL(modality, series->getModality());
    }
}

//------------------------------------------------------------------------------

void SeriesTest::attrDateTest()
{
    const std::string date = "02-14-2015";

    for(const auto& series : m_series)
    {
        series->setDate(date);

        CPPUNIT_ASSERT_EQUAL(date, series->getDate());
    }
}

//------------------------------------------------------------------------------

void SeriesTest::attrTimeTest()
{
    const std::string time = "14:02";

    for(const auto& series : m_series)
    {
        series->setTime(time);

        CPPUNIT_ASSERT_EQUAL(time, series->getTime());
    }
}

//------------------------------------------------------------------------------

void SeriesTest::attrPerformingPhysiciansNameTest()
{
    data::DicomValuesType performingPhysiciansName;

    performingPhysiciansName.push_back("Dr Jekyl");
    performingPhysiciansName.push_back("Dr House");
    performingPhysiciansName.push_back("Dr Einstein");

    for(const auto& series : m_series)
    {
        series->setPerformingPhysiciansName(performingPhysiciansName);

        CPPUNIT_ASSERT(performingPhysiciansName == series->getPerformingPhysiciansName());
    }
}

//------------------------------------------------------------------------------

void SeriesTest::attrDescriptionTest()
{
    const std::string description = "Series";

    for(const auto& series : m_series)
    {
        series->setDescription(description);

        CPPUNIT_ASSERT_EQUAL(description, series->getDescription());
    }
}

//------------------------------------------------------------------------------

void SeriesTest::equalityTest()
{
    auto series1 = data::Series::New();
    auto series2 = data::Series::New();

    CPPUNIT_ASSERT(*series1 == *series2);

    series1->setModality("1");
    CPPUNIT_ASSERT(*series1 != *series2);
    series2->setModality(series1->getModality());
    CPPUNIT_ASSERT(*series1 == *series2);

    series1->setInstanceUID("2");
    CPPUNIT_ASSERT(*series1 != *series2);
    series2->setInstanceUID(series1->getInstanceUID());
    CPPUNIT_ASSERT(*series1 == *series2);

    series1->setNumber("3");
    CPPUNIT_ASSERT(*series1 != *series2);
    series2->setNumber(series1->getNumber());
    CPPUNIT_ASSERT(*series1 == *series2);

    series1->setLaterality("4");
    CPPUNIT_ASSERT(*series1 != *series2);
    series2->setLaterality(series1->getLaterality());
    CPPUNIT_ASSERT(*series1 == *series2);

    series1->setDate("5");
    CPPUNIT_ASSERT(*series1 != *series2);
    series2->setDate(series1->getDate());
    CPPUNIT_ASSERT(*series1 == *series2);

    series1->setTime("6");
    CPPUNIT_ASSERT(*series1 != *series2);
    series2->setTime(series1->getTime());
    CPPUNIT_ASSERT(*series1 == *series2);

    series1->setPerformingPhysiciansName({"7", "8", "9"});
    CPPUNIT_ASSERT(*series1 != *series2);
    series2->setPerformingPhysiciansName(series1->getPerformingPhysiciansName());
    CPPUNIT_ASSERT(*series1 == *series2);

    series1->setProtocolName("10");
    CPPUNIT_ASSERT(*series1 != *series2);
    series2->setProtocolName(series1->getProtocolName());
    CPPUNIT_ASSERT(*series1 == *series2);

    series1->setDescription("11");
    CPPUNIT_ASSERT(*series1 != *series2);
    series2->setDescription(series1->getDescription());
    CPPUNIT_ASSERT(*series1 == *series2);

    series1->setBodyPartExamined("12");
    CPPUNIT_ASSERT(*series1 != *series2);
    series2->setBodyPartExamined(series1->getBodyPartExamined());
    CPPUNIT_ASSERT(*series1 == *series2);

    series1->setPatientPosition("13");
    CPPUNIT_ASSERT(*series1 != *series2);
    series2->setPatientPosition(series1->getPatientPosition());
    CPPUNIT_ASSERT(*series1 == *series2);

    series1->setAnatomicalOrientationType("14");
    CPPUNIT_ASSERT(*series1 != *series2);
    series2->setAnatomicalOrientationType(series1->getAnatomicalOrientationType());
    CPPUNIT_ASSERT(*series1 == *series2);

    series1->setPerformedProcedureStepID("15");
    CPPUNIT_ASSERT(*series1 != *series2);
    series2->setPerformedProcedureStepID(series1->getPerformedProcedureStepID());
    CPPUNIT_ASSERT(*series1 == *series2);

    series1->setPerformedProcedureStepStartDate("16");
    CPPUNIT_ASSERT(*series1 != *series2);
    series2->setPerformedProcedureStepStartDate(series1->getPerformedProcedureStepStartDate());
    CPPUNIT_ASSERT(*series1 == *series2);

    series1->setPerformedProcedureStepStartTime("17");
    CPPUNIT_ASSERT(*series1 != *series2);
    series2->setPerformedProcedureStepStartTime(series1->getPerformedProcedureStepStartTime());
    CPPUNIT_ASSERT(*series1 == *series2);

    series1->setPerformedProcedureStepEndDate("18");
    CPPUNIT_ASSERT(*series1 != *series2);
    series2->setPerformedProcedureStepEndDate(series1->getPerformedProcedureStepEndDate());
    CPPUNIT_ASSERT(*series1 == *series2);

    series1->setPerformedProcedureStepEndTime("19");
    CPPUNIT_ASSERT(*series1 != *series2);
    series2->setPerformedProcedureStepEndTime(series1->getPerformedProcedureStepEndTime());
    CPPUNIT_ASSERT(*series1 == *series2);

    series1->setPerformedProcedureStepDescription("20");
    CPPUNIT_ASSERT(*series1 != *series2);
    series2->setPerformedProcedureStepDescription(series1->getPerformedProcedureStepDescription());
    CPPUNIT_ASSERT(*series1 == *series2);

    series1->setPerformedProcedureComments("21");
    CPPUNIT_ASSERT(*series1 != *series2);
    series2->setPerformedProcedureComments(series1->getPerformedProcedureComments());
    CPPUNIT_ASSERT(*series1 == *series2);

    auto patient = data::Patient::New();
    patient->setPatientId("22");
    series1->setPatient(patient);
    CPPUNIT_ASSERT(*series1 != *series2);
    series2->setPatient(series1->getPatient());
    CPPUNIT_ASSERT(*series1 == *series2);

    auto study = data::Study::New();
    study->setInstanceUID("23");
    series1->setStudy(study);
    CPPUNIT_ASSERT(*series1 != *series2);
    series2->setStudy(series1->getStudy());
    CPPUNIT_ASSERT(*series1 == *series2);

    auto equipment = data::Equipment::New();
    equipment->setInstitutionName("24");
    series1->setEquipment(equipment);
    CPPUNIT_ASSERT(*series1 != *series2);
    series2->setEquipment(series1->getEquipment());
    CPPUNIT_ASSERT(*series1 == *series2);

    // Test also deepcopy, just for fun
    auto series3 = data::Series::New();
    series3->deepCopy(series1);
    CPPUNIT_ASSERT(*series1 == *series3);
}

} // namespace sight::data::ut
