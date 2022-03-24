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

#include "ImageSeriesTest.hpp"

#include <core/Type.hpp>

#include <utestData/generator/Image.hpp>

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION(sight::data::ut::ImageSeriesTest);

namespace sight::data
{

namespace ut
{

//------------------------------------------------------------------------------

void ImageSeriesTest::setUp()
{
    // Set up context before running a test.
    m_series = data::ImageSeries::New();
}

//------------------------------------------------------------------------------

void ImageSeriesTest::tearDown()
{
    // Clean up after the test run.
    m_series.reset();
}

//------------------------------------------------------------------------------

void ImageSeriesTest::imageTest()
{
    CPPUNIT_ASSERT(m_series);

    auto img = data::Image::New();
    utestData::generator::Image::generateRandomImage(img, core::Type::FLOAT);
    CPPUNIT_ASSERT(img);

    m_series->setImage(img);
    CPPUNIT_ASSERT_EQUAL(img, m_series->getImage());

    auto series2 = data::ImageSeries::New();
    CPPUNIT_ASSERT(*series2 != *m_series);

    series2->setImage(img);
    CPPUNIT_ASSERT(*series2 == *m_series);
}

//------------------------------------------------------------------------------

void ImageSeriesTest::equalityTest()
{
    auto series1 = data::ImageSeries::New();
    auto series2 = data::ImageSeries::New();

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

    auto image = data::Image::New();
    utestData::generator::Image::generateRandomImage(image, core::Type::FLOAT);
    series1->setImage(image);
    CPPUNIT_ASSERT(*series1 != *series2);
    series2->setImage(image);
    CPPUNIT_ASSERT(*series1 == *series2);

    auto dicomSeries = data::DicomSeries::New();
    dicomSeries->setInstanceUID("25");
    dicomSeries->setNumberOfInstances(1);
    series1->setDicomReference(dicomSeries);
    CPPUNIT_ASSERT(*series1 != *series2);
    series2->setDicomReference(dicomSeries);
    CPPUNIT_ASSERT(*series1 == *series2);

    series1->setContrastAgent("26");
    CPPUNIT_ASSERT(*series1 != *series2);
    series2->setContrastAgent(series1->getContrastAgent());
    CPPUNIT_ASSERT(*series1 == *series2);

    series1->setContrastRoute("27");
    CPPUNIT_ASSERT(*series1 != *series2);
    series2->setContrastRoute(series1->getContrastRoute());
    CPPUNIT_ASSERT(*series1 == *series2);

    series1->setContrastVolume("28");
    CPPUNIT_ASSERT(*series1 != *series2);
    series2->setContrastVolume(series1->getContrastVolume());
    CPPUNIT_ASSERT(*series1 == *series2);

    series1->setContrastStartTime("29");
    CPPUNIT_ASSERT(*series1 != *series2);
    series2->setContrastStartTime(series1->getContrastStartTime());
    CPPUNIT_ASSERT(*series1 == *series2);

    series1->setContrastStopTime("30");
    CPPUNIT_ASSERT(*series1 != *series2);
    series2->setContrastStopTime(series1->getContrastStopTime());
    CPPUNIT_ASSERT(*series1 == *series2);

    series1->setContrastTotalDose("31");
    CPPUNIT_ASSERT(*series1 != *series2);
    series2->setContrastTotalDose(series1->getContrastTotalDose());
    CPPUNIT_ASSERT(*series1 == *series2);

    series1->setContrastFlowRate("32");
    CPPUNIT_ASSERT(*series1 != *series2);
    series2->setContrastFlowRate(series1->getContrastFlowRate());
    CPPUNIT_ASSERT(*series1 == *series2);

    series1->setContrastFlowDuration("33");
    CPPUNIT_ASSERT(*series1 != *series2);
    series2->setContrastFlowDuration(series1->getContrastFlowDuration());
    CPPUNIT_ASSERT(*series1 == *series2);

    series1->setContrastIngredient("34");
    CPPUNIT_ASSERT(*series1 != *series2);
    series2->setContrastIngredient(series1->getContrastIngredient());
    CPPUNIT_ASSERT(*series1 == *series2);

    series1->setContrastIngredientConcentration("35");
    CPPUNIT_ASSERT(*series1 != *series2);
    series2->setContrastIngredientConcentration(series1->getContrastIngredientConcentration());
    CPPUNIT_ASSERT(*series1 == *series2);

    series1->setAcquisitionDate("36");
    CPPUNIT_ASSERT(*series1 != *series2);
    series2->setAcquisitionDate(series1->getAcquisitionDate());
    CPPUNIT_ASSERT(*series1 == *series2);

    series1->setAcquisitionTime("37");
    CPPUNIT_ASSERT(*series1 != *series2);
    series2->setAcquisitionTime(series1->getAcquisitionTime());
    CPPUNIT_ASSERT(*series1 == *series2);

    // Test also deepcopy, just for fun
    auto series3 = data::ImageSeries::New();
    series3->deepCopy(series1);
    CPPUNIT_ASSERT(*series1 == *series3);
}

//------------------------------------------------------------------------------

} //namespace ut

} //namespace sight::data
