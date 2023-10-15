/************************************************************************
 *
 * Copyright (C) 2009-2023 IRCAD France
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

#include "image_series_test.hpp"

#include <core/type.hpp>

#include <utest_data/generator/image.hpp>

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION(sight::data::ut::image_seriesTest);

namespace sight::data::ut
{

//------------------------------------------------------------------------------

void image_seriesTest::setUp()
{
    // Set up context before running a test.
    m_series = std::make_shared<data::image_series>();
}

//------------------------------------------------------------------------------

void image_seriesTest::tearDown()
{
    // Clean up after the test run.
    m_series.reset();
}

//------------------------------------------------------------------------------

void image_seriesTest::imageTest()
{
    CPPUNIT_ASSERT(m_series);

    utest_data::generator::image::generateRandomImage(m_series, core::type::FLOAT);

    auto series2 = std::make_shared<data::image_series>();
    CPPUNIT_ASSERT(*series2 != *m_series);

    series2->shallow_copy(m_series);
    CPPUNIT_ASSERT(*series2 == *m_series);
}

//------------------------------------------------------------------------------

void image_seriesTest::equalityTest()
{
    auto series1 = std::make_shared<data::image_series>();
    auto series2 = std::make_shared<data::image_series>();

    CPPUNIT_ASSERT(*series1 == *series2 && !(*series1 != *series2));

    series1->setModality("1");
    CPPUNIT_ASSERT(*series1 != *series2 && !(*series1 == *series2));
    series2->setModality(series1->getModality());
    CPPUNIT_ASSERT(*series1 == *series2 && !(*series1 != *series2));

    series1->setSeriesInstanceUID("2");
    CPPUNIT_ASSERT(*series1 != *series2 && !(*series1 == *series2));
    series2->setSeriesInstanceUID(series1->getSeriesInstanceUID());
    CPPUNIT_ASSERT(*series1 == *series2 && !(*series1 != *series2));

    series1->setSeriesNumber(3);
    CPPUNIT_ASSERT(*series1 != *series2 && !(*series1 == *series2));
    series2->setSeriesNumber(series1->getSeriesNumber());
    CPPUNIT_ASSERT(*series1 == *series2 && !(*series1 != *series2));

    series1->setLaterality("4");
    CPPUNIT_ASSERT(*series1 != *series2 && !(*series1 == *series2));
    series2->setLaterality(series1->getLaterality());
    CPPUNIT_ASSERT(*series1 == *series2 && !(*series1 != *series2));

    series1->setSeriesDate("5");
    CPPUNIT_ASSERT(*series1 != *series2 && !(*series1 == *series2));
    series2->setSeriesDate(series1->getSeriesDate());
    CPPUNIT_ASSERT(*series1 == *series2 && !(*series1 != *series2));

    series1->setSeriesTime("6");
    CPPUNIT_ASSERT(*series1 != *series2 && !(*series1 == *series2));
    series2->setSeriesTime(series1->getSeriesTime());
    CPPUNIT_ASSERT(*series1 == *series2 && !(*series1 != *series2));

    series1->setPerformingPhysicianName("7\\8\\9");
    CPPUNIT_ASSERT(*series1 != *series2 && !(*series1 == *series2));
    series2->setPerformingPhysicianName(series1->getPerformingPhysicianName());
    CPPUNIT_ASSERT(*series1 == *series2 && !(*series1 != *series2));

    series1->setProtocolName("10");
    CPPUNIT_ASSERT(*series1 != *series2 && !(*series1 == *series2));
    series2->setProtocolName(series1->getProtocolName());
    CPPUNIT_ASSERT(*series1 == *series2 && !(*series1 != *series2));

    series1->setSeriesDescription("11");
    CPPUNIT_ASSERT(*series1 != *series2 && !(*series1 == *series2));
    series2->setSeriesDescription(series1->getSeriesDescription());
    CPPUNIT_ASSERT(*series1 == *series2 && !(*series1 != *series2));

    series1->setBodyPartExamined("12");
    CPPUNIT_ASSERT(*series1 != *series2 && !(*series1 == *series2));
    series2->setBodyPartExamined(series1->getBodyPartExamined());
    CPPUNIT_ASSERT(*series1 == *series2 && !(*series1 != *series2));

    series1->setPatientPosition("13");
    CPPUNIT_ASSERT(*series1 != *series2 && !(*series1 == *series2));
    series2->setPatientPosition(series1->getPatientPosition());
    CPPUNIT_ASSERT(*series1 == *series2 && !(*series1 != *series2));

    series1->setAnatomicalOrientationType("14");
    CPPUNIT_ASSERT(*series1 != *series2 && !(*series1 == *series2));
    series2->setAnatomicalOrientationType(series1->getAnatomicalOrientationType());
    CPPUNIT_ASSERT(*series1 == *series2 && !(*series1 != *series2));

    series1->setPerformedProcedureStepID("15");
    CPPUNIT_ASSERT(*series1 != *series2 && !(*series1 == *series2));
    series2->setPerformedProcedureStepID(series1->getPerformedProcedureStepID());
    CPPUNIT_ASSERT(*series1 == *series2 && !(*series1 != *series2));

    series1->setPerformedProcedureStepStartDate("16");
    CPPUNIT_ASSERT(*series1 != *series2 && !(*series1 == *series2));
    series2->setPerformedProcedureStepStartDate(series1->getPerformedProcedureStepStartDate());
    CPPUNIT_ASSERT(*series1 == *series2 && !(*series1 != *series2));

    series1->setPerformedProcedureStepStartTime("17");
    CPPUNIT_ASSERT(*series1 != *series2 && !(*series1 == *series2));
    series2->setPerformedProcedureStepStartTime(series1->getPerformedProcedureStepStartTime());
    CPPUNIT_ASSERT(*series1 == *series2 && !(*series1 != *series2));

    series1->setPerformedProcedureStepEndDate("18");
    CPPUNIT_ASSERT(*series1 != *series2 && !(*series1 == *series2));
    series2->setPerformedProcedureStepEndDate(series1->getPerformedProcedureStepEndDate());
    CPPUNIT_ASSERT(*series1 == *series2 && !(*series1 != *series2));

    series1->setPerformedProcedureStepEndTime("19");
    CPPUNIT_ASSERT(*series1 != *series2 && !(*series1 == *series2));
    series2->setPerformedProcedureStepEndTime(series1->getPerformedProcedureStepEndTime());
    CPPUNIT_ASSERT(*series1 == *series2 && !(*series1 != *series2));

    series1->setPerformedProcedureStepDescription("20");
    CPPUNIT_ASSERT(*series1 != *series2 && !(*series1 == *series2));
    series2->setPerformedProcedureStepDescription(series1->getPerformedProcedureStepDescription());
    CPPUNIT_ASSERT(*series1 == *series2 && !(*series1 != *series2));

    series1->setCommentsOnThePerformedProcedureStep("21");
    CPPUNIT_ASSERT(*series1 != *series2 && !(*series1 == *series2));
    series2->setCommentsOnThePerformedProcedureStep(series1->getCommentsOnThePerformedProcedureStep());
    CPPUNIT_ASSERT(*series1 == *series2 && !(*series1 != *series2));

    series1->setPatientID("22");
    CPPUNIT_ASSERT(*series1 != *series2 && !(*series1 == *series2));
    series2->setPatientID(series1->getPatientID());
    CPPUNIT_ASSERT(*series1 == *series2 && !(*series1 != *series2));

    series1->setStudyInstanceUID("23");
    CPPUNIT_ASSERT(*series1 != *series2 && !(*series1 == *series2));
    series2->setStudyInstanceUID(series1->getStudyInstanceUID());
    CPPUNIT_ASSERT(*series1 == *series2 && !(*series1 != *series2));

    series1->setInstitutionName("24");
    CPPUNIT_ASSERT(*series1 != *series2 && !(*series1 == *series2));
    series2->setInstitutionName(series1->getInstitutionName());
    CPPUNIT_ASSERT(*series1 == *series2 && !(*series1 != *series2));

    utest_data::generator::image::generateRandomImage(series1, core::type::FLOAT);
    CPPUNIT_ASSERT(*series1 != *series2 && !(*series1 == *series2));
    series2->shallow_copy(series1);
    CPPUNIT_ASSERT(*series1 == *series2 && !(*series1 != *series2));

    auto dicomSeries = std::make_shared<data::dicom_series>();
    dicomSeries->setSeriesInstanceUID("25");
    dicomSeries->setNumberOfInstances(1);
    series1->setDicomReference(dicomSeries);
    CPPUNIT_ASSERT(*series1 != *series2 && !(*series1 == *series2));
    series2->setDicomReference(dicomSeries);
    CPPUNIT_ASSERT(*series1 == *series2 && !(*series1 != *series2));

    series1->setContrastBolusAgent("26");
    CPPUNIT_ASSERT(*series1 != *series2 && !(*series1 == *series2));
    series2->setContrastBolusAgent(series1->getContrastBolusAgent());
    CPPUNIT_ASSERT(*series1 == *series2 && !(*series1 != *series2));

    series1->setContrastBolusRoute("27");
    CPPUNIT_ASSERT(*series1 != *series2 && !(*series1 == *series2));
    series2->setContrastBolusRoute(series1->getContrastBolusRoute());
    CPPUNIT_ASSERT(*series1 == *series2 && !(*series1 != *series2));

    series1->setContrastBolusVolume(28.0);
    CPPUNIT_ASSERT(*series1 != *series2 && !(*series1 == *series2));
    series2->setContrastBolusVolume(series1->getContrastBolusVolume());
    CPPUNIT_ASSERT(*series1 == *series2 && !(*series1 != *series2));

    series1->setContrastBolusStartTime("29");
    CPPUNIT_ASSERT(*series1 != *series2 && !(*series1 == *series2));
    series2->setContrastBolusStartTime(series1->getContrastBolusStartTime());
    CPPUNIT_ASSERT(*series1 == *series2 && !(*series1 != *series2));

    series1->setContrastBolusStopTime("30");
    CPPUNIT_ASSERT(*series1 != *series2 && !(*series1 == *series2));
    series2->setContrastBolusStopTime(series1->getContrastBolusStopTime());
    CPPUNIT_ASSERT(*series1 == *series2 && !(*series1 != *series2));

    series1->setContrastBolusTotalDose(31.0);
    CPPUNIT_ASSERT(*series1 != *series2 && !(*series1 == *series2));
    series2->setContrastBolusTotalDose(series1->getContrastBolusTotalDose());
    CPPUNIT_ASSERT(*series1 == *series2 && !(*series1 != *series2));

    series1->setContrastFlowRate("32.0\\32.1\\32.2");
    CPPUNIT_ASSERT(*series1 != *series2 && !(*series1 == *series2));
    series2->setContrastFlowRate(series1->getContrastFlowRate());
    CPPUNIT_ASSERT(*series1 == *series2 && !(*series1 != *series2));

    series1->setContrastFlowDuration("33.0\\33.1\\33.2");
    CPPUNIT_ASSERT(*series1 != *series2 && !(*series1 == *series2));
    series2->setContrastFlowDuration(series1->getContrastFlowDuration());
    CPPUNIT_ASSERT(*series1 == *series2 && !(*series1 != *series2));

    series1->setContrastBolusIngredient("34");
    CPPUNIT_ASSERT(*series1 != *series2 && !(*series1 == *series2));
    series2->setContrastBolusIngredient(series1->getContrastBolusIngredient());
    CPPUNIT_ASSERT(*series1 == *series2 && !(*series1 != *series2));

    series1->setContrastBolusIngredientConcentration(35.0);
    CPPUNIT_ASSERT(*series1 != *series2 && !(*series1 == *series2));
    series2->setContrastBolusIngredientConcentration(series1->getContrastBolusIngredientConcentration());
    CPPUNIT_ASSERT(*series1 == *series2 && !(*series1 != *series2));

    series1->setAcquisitionDate("36");
    CPPUNIT_ASSERT(*series1 != *series2 && !(*series1 == *series2));
    series2->setAcquisitionDate(series1->getAcquisitionDate());
    CPPUNIT_ASSERT(*series1 == *series2 && !(*series1 != *series2));

    series1->setAcquisitionTime("37");
    CPPUNIT_ASSERT(*series1 != *series2 && !(*series1 == *series2));
    series2->setAcquisitionTime(series1->getAcquisitionTime());
    CPPUNIT_ASSERT(*series1 == *series2 && !(*series1 != *series2));

    series1->setWindowCenter({38, 39});
    CPPUNIT_ASSERT(*series1 != *series2 && !(*series1 == *series2));
    series2->setWindowCenter(series1->getWindowCenter());
    CPPUNIT_ASSERT(*series1 == *series2 && !(*series1 != *series2));

    series1->setWindowWidth({40, 41});
    CPPUNIT_ASSERT(*series1 != *series2 && !(*series1 == *series2));
    series2->setWindowWidth(series1->getWindowWidth());
    CPPUNIT_ASSERT(*series1 == *series2 && !(*series1 != *series2));

    series1->setRows(std::uint16_t(42));
    CPPUNIT_ASSERT(*series1 != *series2 && !(*series1 == *series2));
    series2->setRows(series1->getRows());
    CPPUNIT_ASSERT(*series1 == *series2 && !(*series1 != *series2));

    series1->setColumns(std::uint16_t(43));
    CPPUNIT_ASSERT(*series1 != *series2 && !(*series1 == *series2));
    series2->setColumns(series1->getColumns());
    CPPUNIT_ASSERT(*series1 == *series2 && !(*series1 != *series2));

    // Test also deepcopy, just for fun
    auto series3 = std::make_shared<data::image_series>();
    series3->deep_copy(series1);
    CPPUNIT_ASSERT(*series1 == *series3 && !(*series1 != *series3));
}

//------------------------------------------------------------------------------

void image_seriesTest::resizeTest()
{
    auto series1 = std::make_shared<data::image_series>();
    series1->setSOPKeyword(data::dicom::sop::Keyword::EnhancedUSVolumeStorage);
    do
    {
        utest_data::generator::image::generateRandomImage(series1, core::type::INT8);
    }
    while(series1->size()[2] <= 2);

    auto size = series1->size();

    for(std::size_t i = 0, end = size[2] ; i < end ; ++i)
    {
        series1->setImagePositionPatient({0.1 * double(i), 0.2 * double(i), 0.3 * double(i)}, i);
        series1->setFrameComments("Comments" + std::to_string(i), i);
        series1->setFrameLabel("Label" + std::to_string(i), i);
    }

    auto series2 = std::make_shared<data::image_series>();
    CPPUNIT_ASSERT(*series1 != *series2 && !(*series1 == *series2));

    series2->deep_copy(series1);
    CPPUNIT_ASSERT(*series1 == *series2 && !(*series1 != *series2));

    // Resize to 2 slices
    size[2] = 2;
    series1->resize(size, series1->getType(), series1->getPixelFormat());

    CPPUNIT_ASSERT(*series1 != *series2 && !(*series1 == *series2));

    CPPUNIT_ASSERT(series2->getImagePositionPatient(0) == series1->getImagePositionPatient(0));
    CPPUNIT_ASSERT(series2->getImagePositionPatient(1) == series1->getImagePositionPatient(1));
    CPPUNIT_ASSERT(
        series2->getImagePositionPatient(2) != series1->getImagePositionPatient(2)
        && series1->getImagePositionPatient(2).empty()
    );

    CPPUNIT_ASSERT_EQUAL(*series2->getFrameComments(0), *series1->getFrameComments(0));
    CPPUNIT_ASSERT_EQUAL(*series2->getFrameComments(1), *series1->getFrameComments(1));
    CPPUNIT_ASSERT(series2->getFrameComments(2) && !series1->getFrameComments(2));

    CPPUNIT_ASSERT_EQUAL(*series2->getFrameLabel(0), *series1->getFrameLabel(0));
    CPPUNIT_ASSERT_EQUAL(*series2->getFrameLabel(1), *series1->getFrameLabel(1));
    CPPUNIT_ASSERT(series2->getFrameLabel(2) && !series1->getFrameLabel(2));

    series2->deep_copy(series1);
    CPPUNIT_ASSERT(*series1 == *series2 && !(*series1 != *series2));
}

//------------------------------------------------------------------------------

} // namespace sight::data::ut
