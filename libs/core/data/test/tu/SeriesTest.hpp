/************************************************************************
 *
 * Copyright (C) 2022-2023 IRCAD France
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

#pragma once

#include <data/ImageSeries.hpp>
#include <data/ModelSeries.hpp>

#include <cppunit/extensions/HelperMacros.h>

namespace sight::data::ut
{

class SeriesTest : public CPPUNIT_NS::TestFixture
{
CPPUNIT_TEST_SUITE(SeriesTest);

CPPUNIT_TEST(privateTagTest);
CPPUNIT_TEST(multiFramePrivateTagTest);
CPPUNIT_TEST(attrPatientTest);
CPPUNIT_TEST(attrStudyTest);
CPPUNIT_TEST(attrEquipmentTest);
CPPUNIT_TEST(attrInstanceUIDTest);
CPPUNIT_TEST(attrModalityTest);
CPPUNIT_TEST(attrDateTest);
CPPUNIT_TEST(attrTimeTest);
CPPUNIT_TEST(attrPerformingPhysicianNameTest);
CPPUNIT_TEST(attrDescriptionTest);

CPPUNIT_TEST(equalityTest);

CPPUNIT_TEST(sopClassUIDTest);
CPPUNIT_TEST(sopInstanceUIDTest);
CPPUNIT_TEST(specificCharacterSetTest);
CPPUNIT_TEST(encodingTest);
CPPUNIT_TEST(seriesDateTest);
CPPUNIT_TEST(seriesTimeTest);
CPPUNIT_TEST(modalityTest);
CPPUNIT_TEST(seriesDescriptionTest);
CPPUNIT_TEST(performingPhysicianNameTest);
CPPUNIT_TEST(anatomicalOrientationTypeTest);
CPPUNIT_TEST(bodyPartExaminedTest);
CPPUNIT_TEST(protocolNameTest);
CPPUNIT_TEST(patientPositionTest);
CPPUNIT_TEST(seriesInstanceUIDTest);
CPPUNIT_TEST(seriesNumberTest);
CPPUNIT_TEST(lateralityTest);
CPPUNIT_TEST(performedProcedureStepStartDateTest);
CPPUNIT_TEST(performedProcedureStepStartTimeTest);
CPPUNIT_TEST(performedProcedureStepEndDateTest);
CPPUNIT_TEST(performedProcedureStepEndTimeTest);
CPPUNIT_TEST(performedProcedureStepIDTest);
CPPUNIT_TEST(performedProcedureStepDescriptionTest);
CPPUNIT_TEST(commentsOnThePerformedProcedureStepTest);
CPPUNIT_TEST(institutionNameTest);
CPPUNIT_TEST(patientNameTest);
CPPUNIT_TEST(patientIDTest);
CPPUNIT_TEST(patientBirthDateTest);
CPPUNIT_TEST(patientSexTest);
CPPUNIT_TEST(studyDateTest);
CPPUNIT_TEST(studyTimeTest);
CPPUNIT_TEST(referringPhysicianNameTest);
CPPUNIT_TEST(studyDescriptionTest);
CPPUNIT_TEST(studyInstanceUIDTest);
CPPUNIT_TEST(studyIDTest);
CPPUNIT_TEST(patientAgeTest);
CPPUNIT_TEST(patientSizeTest);
CPPUNIT_TEST(patientWeightTest);
CPPUNIT_TEST(acquisitionDateTest);
CPPUNIT_TEST(acquisitionTimeTest);
CPPUNIT_TEST(acquisitionNumberTest);
CPPUNIT_TEST(instanceNumberTest);
CPPUNIT_TEST(contentTimeTest);
CPPUNIT_TEST(contrastBolusAgentTest);
CPPUNIT_TEST(contrastBolusRouteTest);
CPPUNIT_TEST(contrastBolusVolumeTest);
CPPUNIT_TEST(contrastBolusStartTimeTest);
CPPUNIT_TEST(contrastBolusStopTimeTest);
CPPUNIT_TEST(contrastBolusTotalDoseTest);
CPPUNIT_TEST(contrastFlowRateTest);
CPPUNIT_TEST(contrastFlowDurationTest);
CPPUNIT_TEST(contrastBolusIngredientTest);
CPPUNIT_TEST(contrastBolusIngredientConcentrationTest);
CPPUNIT_TEST(rowsTest);
CPPUNIT_TEST(columnsTest);
CPPUNIT_TEST(windowCenterTest);
CPPUNIT_TEST(windowWidthTest);
CPPUNIT_TEST(imagePositionPatientTest);
CPPUNIT_TEST(imageOrientationPatientTest);
CPPUNIT_TEST(imageTransformPatientTest);
CPPUNIT_TEST(frameAcquisitionDateTimeTest);
CPPUNIT_TEST(frameAcquisitionTimePointTest);
CPPUNIT_TEST(frameCommentsTest);
CPPUNIT_TEST(frameLabelTest);
CPPUNIT_TEST(stringConversionTest);

CPPUNIT_TEST(copyPatientModuleTest);
CPPUNIT_TEST(copyGeneralStudyModuleTest);
CPPUNIT_TEST(copyPatientStudyModuleTest);
CPPUNIT_TEST(copyGeneralSeriesModuleTest);
CPPUNIT_TEST(copyGeneralEquipmentModuleTest);
CPPUNIT_TEST(copySOPCommonModuleTest);
CPPUNIT_TEST(copyGeneralImageModuleTest);

CPPUNIT_TEST(getPatientPositionStringTest);

CPPUNIT_TEST_SUITE_END();

public:

    // interface
    void setUp() override;
    void tearDown() override;

    static void privateTagTest();
    static void multiFramePrivateTagTest();

    void attrPatientTest();
    void attrStudyTest();
    void attrEquipmentTest();
    void attrInstanceUIDTest();
    void attrModalityTest();
    void attrDateTest();
    void attrTimeTest();
    void attrPerformingPhysicianNameTest();
    void attrDescriptionTest();

    static void equalityTest();

    static void sopClassUIDTest();
    static void sopInstanceUIDTest();
    static void specificCharacterSetTest();
    static void encodingTest();
    static void seriesDateTest();
    static void seriesTimeTest();
    static void modalityTest();
    static void seriesDescriptionTest();
    static void performingPhysicianNameTest();
    static void anatomicalOrientationTypeTest();
    static void bodyPartExaminedTest();
    static void protocolNameTest();
    static void patientPositionTest();
    static void seriesInstanceUIDTest();
    static void seriesNumberTest();
    static void lateralityTest();
    static void performedProcedureStepStartDateTest();
    static void performedProcedureStepStartTimeTest();
    static void performedProcedureStepEndDateTest();
    static void performedProcedureStepEndTimeTest();
    static void performedProcedureStepIDTest();
    static void performedProcedureStepDescriptionTest();
    static void commentsOnThePerformedProcedureStepTest();
    static void institutionNameTest();
    static void patientNameTest();
    static void patientIDTest();
    static void patientBirthDateTest();
    static void patientSexTest();
    static void studyDateTest();
    static void studyTimeTest();
    static void referringPhysicianNameTest();
    static void studyDescriptionTest();
    static void studyInstanceUIDTest();
    static void studyIDTest();
    static void patientAgeTest();
    static void patientSizeTest();
    static void patientWeightTest();
    static void acquisitionDateTest();
    static void acquisitionTimeTest();
    static void acquisitionNumberTest();
    static void instanceNumberTest();
    static void contentTimeTest();
    static void contrastBolusAgentTest();
    static void contrastBolusRouteTest();
    static void contrastBolusVolumeTest();
    static void contrastBolusStartTimeTest();
    static void contrastBolusStopTimeTest();
    static void contrastBolusTotalDoseTest();
    static void contrastFlowRateTest();
    static void contrastFlowDurationTest();
    static void contrastBolusIngredientTest();
    static void contrastBolusIngredientConcentrationTest();
    static void rowsTest();
    static void columnsTest();
    static void windowCenterTest();
    static void windowWidthTest();
    static void imagePositionPatientTest();
    static void imageOrientationPatientTest();
    static void imageTransformPatientTest();
    static void frameAcquisitionDateTimeTest();
    static void frameAcquisitionTimePointTest();
    static void frameCommentsTest();
    static void frameLabelTest();

    static void stringConversionTest();

    static void copyPatientModuleTest();
    static void copyGeneralStudyModuleTest();
    static void copyPatientStudyModuleTest();
    static void copyGeneralSeriesModuleTest();
    static void copyGeneralEquipmentModuleTest();
    static void copySOPCommonModuleTest();
    static void copyGeneralImageModuleTest();

    static void getPatientPositionStringTest();

protected:

    typedef std::vector<std::shared_ptr<Series> > SeriesVectorType;

    SeriesVectorType m_series;
};

} // namespace sight::data::ut
