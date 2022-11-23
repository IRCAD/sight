/************************************************************************
 *
 * Copyright (C) 2022 IRCAD France
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

#include <core/tools/compare.hpp>
#include <core/tools/UUID.hpp>

#include <boost/algorithm/string/classification.hpp>
#include <boost/algorithm/string/join.hpp>
#include <boost/algorithm/string/split.hpp>

#include <algorithm>

using UUID = sight::core::tools::UUID;

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
    const std::string patientName      = "John DOE";
    const std::string patientId        = "42";
    const std::string patientBirthDate = "02-14-2015 11:28";
    const std::string patientSex       = "M";

    for(const auto& series : m_series)
    {
        series->setPatientName(patientName);
        CPPUNIT_ASSERT(series->getPatientName() == patientName);

        series->setPatientID(patientId);
        CPPUNIT_ASSERT(series->getPatientID() == patientId);

        series->setPatientBirthDate(patientBirthDate);
        CPPUNIT_ASSERT(series->getPatientBirthDate() == patientBirthDate);

        series->setPatientSex(patientSex);
        CPPUNIT_ASSERT(series->getPatientSex() == patientSex);
    }
}

//------------------------------------------------------------------------------

void SeriesTest::attrStudyTest()
{
    const std::string instanceUID = "1346357.1664.643101.421337.4123403";
    const std::string date        = "02-14-2015";
    const std::string time        = "11:59";
    const std::string rpn         = "Dr^Jekyl";
    const std::string desc        = "Say 33.";
    const std::string age         = "42";

    for(const auto& series : m_series)
    {
        series->setStudyInstanceUID(instanceUID);
        CPPUNIT_ASSERT(series->getStudyInstanceUID() == instanceUID);

        series->setStudyDate(date);
        CPPUNIT_ASSERT(series->getStudyDate() == date);

        series->setStudyTime(time);
        CPPUNIT_ASSERT(series->getStudyTime() == time);

        series->setReferringPhysicianName(rpn);
        CPPUNIT_ASSERT(series->getReferringPhysicianName() == rpn);

        series->setStudyDescription(desc);
        CPPUNIT_ASSERT(series->getStudyDescription() == desc);

        series->setPatientAge(age);
        CPPUNIT_ASSERT(series->getPatientAge() == age);
    }
}

//------------------------------------------------------------------------------

void SeriesTest::attrEquipmentTest()
{
    constexpr auto institution_name = "IRCAD";
    for(const auto& series : m_series)
    {
        series->setInstitutionName(institution_name);
        CPPUNIT_ASSERT(series->getInstitutionName() == institution_name);
    }
}

//------------------------------------------------------------------------------

void SeriesTest::attrInstanceUIDTest()
{
    const std::string instanceUID = "1337.1664.42";

    for(const auto& series : m_series)
    {
        series->setSeriesInstanceUID(instanceUID);
        CPPUNIT_ASSERT_EQUAL(instanceUID, series->getSeriesInstanceUID());
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
        series->setSeriesDate(date);
        CPPUNIT_ASSERT_EQUAL(date, series->getSeriesDate());
    }
}

//------------------------------------------------------------------------------

void SeriesTest::attrTimeTest()
{
    const std::string time = "14:02";

    for(const auto& series : m_series)
    {
        series->setSeriesTime(time);
        CPPUNIT_ASSERT_EQUAL(time, series->getSeriesTime());
    }
}

//------------------------------------------------------------------------------

void SeriesTest::attrPerformingPhysicianNameTest()
{
    const std::string performingPhysicianName =
        "Adams^John Robert Quincy^^Rev.^B.A. M.Div.\\Morrison-Jones^Susan^^^Ph.D., Chief Executive Officer\\Doe^John";

    for(const auto& series : m_series)
    {
        series->setPerformingPhysicianName(performingPhysicianName);
        CPPUNIT_ASSERT(performingPhysicianName == series->getPerformingPhysicianName());
    }
}

//------------------------------------------------------------------------------

void SeriesTest::attrDescriptionTest()
{
    const std::string description = "Series";

    for(const auto& series : m_series)
    {
        series->setSeriesDescription(description);
        CPPUNIT_ASSERT_EQUAL(description, series->getSeriesDescription());
    }
}

//------------------------------------------------------------------------------

void SeriesTest::equalityTest()
{
    auto series1 = data::Series::New();
    auto series2 = data::Series::New();

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

    series1->setPatientBirthDate("22");
    CPPUNIT_ASSERT(*series1 != *series2 && !(*series1 == *series2));
    series2->setPatientBirthDate(series1->getPatientBirthDate());
    CPPUNIT_ASSERT(*series1 == *series2 && !(*series1 != *series2));

    series1->setPatientName("23");
    CPPUNIT_ASSERT(*series1 != *series2 && !(*series1 == *series2));
    series2->setPatientName(series1->getPatientName());
    CPPUNIT_ASSERT(*series1 == *series2 && !(*series1 != *series2));

    series1->setPatientSex("24");
    CPPUNIT_ASSERT(*series1 != *series2 && !(*series1 == *series2));
    series2->setPatientSex(series1->getPatientSex());
    CPPUNIT_ASSERT(*series1 == *series2 && !(*series1 != *series2));

    series1->setPatientID("25");
    CPPUNIT_ASSERT(*series1 != *series2 && !(*series1 == *series2));
    series2->setPatientID(series1->getPatientID());
    CPPUNIT_ASSERT(*series1 == *series2 && !(*series1 != *series2));

    series1->setStudyInstanceUID("26");
    CPPUNIT_ASSERT(*series1 != *series2 && !(*series1 == *series2));
    series2->setStudyInstanceUID(series1->getStudyInstanceUID());
    CPPUNIT_ASSERT(*series1 == *series2 && !(*series1 != *series2));

    series1->setStudyDate("28");
    CPPUNIT_ASSERT(*series1 != *series2 && !(*series1 == *series2));
    series2->setStudyDate(series1->getStudyDate());
    CPPUNIT_ASSERT(*series1 == *series2 && !(*series1 != *series2));

    series1->setStudyDescription("29");
    CPPUNIT_ASSERT(*series1 != *series2 && !(*series1 == *series2));
    series2->setStudyDescription(series1->getStudyDescription());
    CPPUNIT_ASSERT(*series1 == *series2 && !(*series1 != *series2));

    series1->setPatientAge("30");
    CPPUNIT_ASSERT(*series1 != *series2 && !(*series1 == *series2));
    series2->setPatientAge(series1->getPatientAge());
    CPPUNIT_ASSERT(*series1 == *series2 && !(*series1 != *series2));

    series1->setPatientSize(32.0);
    CPPUNIT_ASSERT(*series1 != *series2 && !(*series1 == *series2));
    series2->setPatientSize(series1->getPatientSize());
    CPPUNIT_ASSERT(*series1 == *series2 && !(*series1 != *series2));

    series1->setPatientWeight(33.0);
    CPPUNIT_ASSERT(*series1 != *series2 && !(*series1 == *series2));
    series2->setPatientWeight(series1->getPatientWeight());
    CPPUNIT_ASSERT(*series1 == *series2 && !(*series1 != *series2));

    series1->setReferringPhysicianName("34");
    CPPUNIT_ASSERT(*series1 != *series2 && !(*series1 == *series2));
    series2->setReferringPhysicianName(series1->getReferringPhysicianName());
    CPPUNIT_ASSERT(*series1 == *series2 && !(*series1 != *series2));

    series1->setStudyID("36");
    CPPUNIT_ASSERT(*series1 != *series2 && !(*series1 == *series2));
    series2->setStudyID(series1->getStudyID());
    CPPUNIT_ASSERT(*series1 == *series2 && !(*series1 != *series2));

    series1->setStudyTime("37");
    CPPUNIT_ASSERT(*series1 != *series2 && !(*series1 == *series2));
    series2->setStudyTime(series1->getStudyTime());
    CPPUNIT_ASSERT(*series1 == *series2 && !(*series1 != *series2));

    series1->setInstitutionName("38");
    CPPUNIT_ASSERT(*series1 != *series2 && !(*series1 == *series2));
    series2->setInstitutionName(series1->getInstitutionName());
    CPPUNIT_ASSERT(*series1 == *series2 && !(*series1 != *series2));

    series1->setSOPClassUID("39");
    CPPUNIT_ASSERT(*series1 != *series2 && !(*series1 == *series2));
    series2->setSOPClassUID(series1->getSOPClassUID());
    CPPUNIT_ASSERT(*series1 == *series2 && !(*series1 != *series2));

    series1->setSOPInstanceUID("40");
    CPPUNIT_ASSERT(*series1 != *series2 && !(*series1 == *series2));
    series2->setSOPInstanceUID(series1->getSOPInstanceUID());
    CPPUNIT_ASSERT(*series1 == *series2 && !(*series1 != *series2));

    series1->setSpecificCharacterSet("41");
    CPPUNIT_ASSERT(*series1 != *series2 && !(*series1 == *series2));
    series2->setSpecificCharacterSet(series1->getSpecificCharacterSet());
    CPPUNIT_ASSERT(*series1 == *series2 && !(*series1 != *series2));

    series1->setContentTime("42");
    CPPUNIT_ASSERT(*series1 != *series2 && !(*series1 == *series2));
    series2->setContentTime(series1->getContentTime());
    CPPUNIT_ASSERT(*series1 == *series2 && !(*series1 != *series2));

    series1->setInstanceNumber(43);
    CPPUNIT_ASSERT(*series1 != *series2 && !(*series1 == *series2));
    series2->setInstanceNumber(series1->getInstanceNumber());
    CPPUNIT_ASSERT(*series1 == *series2 && !(*series1 != *series2));

    series1->setAcquisitionNumber(44);
    CPPUNIT_ASSERT(*series1 != *series2 && !(*series1 == *series2));
    series2->setAcquisitionNumber(series1->getAcquisitionNumber());
    CPPUNIT_ASSERT(*series1 == *series2 && !(*series1 != *series2));

    series1->setRescaleIntercept(45);
    CPPUNIT_ASSERT(*series1 != *series2 && !(*series1 == *series2));
    series2->setRescaleIntercept(series1->getRescaleIntercept());
    CPPUNIT_ASSERT(*series1 == *series2 && !(*series1 != *series2));

    series1->setRescaleSlope(46);
    CPPUNIT_ASSERT(*series1 != *series2 && !(*series1 == *series2));
    series2->setRescaleSlope(series1->getRescaleSlope());
    CPPUNIT_ASSERT(*series1 == *series2 && !(*series1 != *series2));

    series1->setImagePositionPatient({47, 48, 49});
    CPPUNIT_ASSERT(*series1 != *series2 && !(*series1 == *series2));
    series2->setImagePositionPatient(series1->getImagePositionPatient());
    CPPUNIT_ASSERT(*series1 == *series2 && !(*series1 != *series2));

    series1->setImageOrientationPatient({50, 51, 52, 53, 54, 55});
    CPPUNIT_ASSERT(*series1 != *series2 && !(*series1 == *series2));
    series2->setImageOrientationPatient(series1->getImageOrientationPatient());
    CPPUNIT_ASSERT(*series1 == *series2 && !(*series1 != *series2));

    series1->setSliceThickness(56);
    CPPUNIT_ASSERT(*series1 != *series2 && !(*series1 == *series2));
    series2->setSliceThickness(series1->getSliceThickness());
    CPPUNIT_ASSERT(*series1 == *series2 && !(*series1 != *series2));

    // Test also deepcopy, just for fun
    auto series3 = data::Series::New();
    series3->deepCopy(series1);
    CPPUNIT_ASSERT(*series1 == *series3 && !(*series1 != *series3));
}

//------------------------------------------------------------------------------

void SeriesTest::sopClassUIDTest()
{
    static const std::string sopClassUID(UUID::generateUUID());

    {
        auto series = data::Series::New();
        series->setSOPClassUID(sopClassUID);
        CPPUNIT_ASSERT_EQUAL(sopClassUID, series->getSOPClassUID());
        CPPUNIT_ASSERT_EQUAL(sopClassUID, series->getByteValue(0x0008, 0x0016));
    }

    {
        auto series = data::Series::New();
        series->setByteValue(0x0008, 0x0016, sopClassUID);
        CPPUNIT_ASSERT_EQUAL(sopClassUID, series->getSOPClassUID());
        CPPUNIT_ASSERT_EQUAL(sopClassUID, series->getByteValue(0x0008, 0x0016));
    }
}

//------------------------------------------------------------------------------

void SeriesTest::sopClassNameTest()
{
    // This can be found in DICOM PS 3.3 (or gdcmSOPClassUIDToIOD.cxx)
    static constexpr auto SOPClassUID  = "1.2.840.10008.5.1.4.1.1.2";
    static constexpr auto SOPClassName = "CT Image Storage";

    auto series = data::Series::New();
    series->setSOPClassUID(SOPClassUID);
    CPPUNIT_ASSERT_EQUAL(std::string(SOPClassName), series->getSOPClassName());
}

//------------------------------------------------------------------------------

void SeriesTest::sopInstanceUIDTest()
{
    static const std::string sopInstanceUID(UUID::generateUUID());

    {
        auto series = data::Series::New();
        series->setSOPInstanceUID(sopInstanceUID);
        CPPUNIT_ASSERT_EQUAL(sopInstanceUID, series->getSOPInstanceUID());
        CPPUNIT_ASSERT_EQUAL(sopInstanceUID, series->getByteValue(0x0008, 0x0018));
    }

    {
        auto series = data::Series::New();
        series->setByteValue(0x0008, 0x0018, sopInstanceUID);
        CPPUNIT_ASSERT_EQUAL(sopInstanceUID, series->getSOPInstanceUID());
        CPPUNIT_ASSERT_EQUAL(sopInstanceUID, series->getByteValue(0x0008, 0x0018));
    }
}

//------------------------------------------------------------------------------

void SeriesTest::specificCharacterSetTest()
{
    static const std::string specificCharacterSet("ISO 2022\\IR 100");

    {
        auto series = data::Series::New();
        series->setSpecificCharacterSet(specificCharacterSet);
        CPPUNIT_ASSERT_EQUAL(specificCharacterSet, series->getSpecificCharacterSet());
        CPPUNIT_ASSERT_EQUAL(specificCharacterSet, series->getByteValue(0x0008, 0x0005));
    }

    {
        auto series = data::Series::New();
        series->setByteValue(0x0008, 0x0005, specificCharacterSet);
        CPPUNIT_ASSERT_EQUAL(specificCharacterSet, series->getSpecificCharacterSet());
        CPPUNIT_ASSERT_EQUAL(specificCharacterSet, series->getByteValue(0x0008, 0x0005));
    }
}

//------------------------------------------------------------------------------

void SeriesTest::encodingTest()
{
    static const std::array<std::pair<std::string, std::string>, 19> CHARACTER_SET {{
        {"GB18030", "GB18030"},
        {"GBK", "GBK"},
        {"ISO 2022\\IR 6", "UTF-8"},
        {"ISO 2022\\IR 100", "ISO-8859-1"},
        {"ISO 2022\\IR 101", "ISO-8859-2"},
        {"ISO 2022\\IR 109", "ISO-8859-3"},
        {"ISO 2022\\IR 110", "ISO-8859-4"},
        {"ISO 2022\\IR 144", "ISO-8859-5"},
        {"ISO 2022\\IR 127", "ISO-8859-6"},
        {"ISO 2022\\IR 126", "ISO-8859-7"},
        {"ISO 2022\\IR 138", "ISO-8859-8"},
        {"ISO 2022\\IR 148", "ISO-8859-9"},
        {"ISO 2022\\IR 13", "Shift_JIS"},
        {"ISO 2022\\IR 166", "TIS-620"},
        {"ISO 2022\\IR 192", "UTF-8"},
        {"ISO 2022\\IR 87", "ISO-2022-JP-1"},
        {"ISO 2022\\IR 159", "ISO-2022-JP-2"},
        {"ISO 2022\\IR 149", "ISO-2022-KR"},
        {"ISO 2022\\IR 58", "ISO-2022-CN"}
    }
    };

    {
        auto series = data::Series::New();

        for(const auto& character_set : CHARACTER_SET)
        {
            series->setSpecificCharacterSet(character_set.first);
            CPPUNIT_ASSERT_EQUAL(character_set.second, series->getEncoding());
        }
    }

    {
        // By default, UTF-8 should be used...
        auto series = data::Series::New();
        CPPUNIT_ASSERT_EQUAL(std::string("UTF-8"), series->getEncoding());
    }
}

//------------------------------------------------------------------------------

void SeriesTest::seriesDateTest()
{
    static const std::string seriesDate("20180201");

    {
        auto series = data::Series::New();
        series->setSeriesDate(seriesDate);
        CPPUNIT_ASSERT_EQUAL(seriesDate, series->getSeriesDate());
        CPPUNIT_ASSERT_EQUAL(seriesDate, series->getByteValue(0x0008, 0x0021));
    }

    {
        auto series = data::Series::New();
        series->setByteValue(0x0008, 0x0021, seriesDate);
        CPPUNIT_ASSERT_EQUAL(seriesDate, series->getSeriesDate());
        CPPUNIT_ASSERT_EQUAL(seriesDate, series->getByteValue(0x0008, 0x0021));
    }
}

//------------------------------------------------------------------------------

void SeriesTest::seriesTimeTest()
{
    static const std::string seriesTime("165649.123456");

    {
        auto series = data::Series::New();
        series->setSeriesTime(seriesTime);
        CPPUNIT_ASSERT_EQUAL(seriesTime, series->getSeriesTime());
        CPPUNIT_ASSERT_EQUAL(seriesTime, series->getByteValue(0x0008, 0x0031));
    }

    {
        auto series = data::Series::New();
        series->setByteValue(0x0008, 0x0031, seriesTime);
        CPPUNIT_ASSERT_EQUAL(seriesTime, series->getSeriesTime());
        CPPUNIT_ASSERT_EQUAL(seriesTime, series->getByteValue(0x0008, 0x0031));
    }
}

//------------------------------------------------------------------------------

void SeriesTest::modalityTest()
{
    static const std::string modality("US");

    {
        auto series = data::Series::New();
        series->setModality(modality);
        CPPUNIT_ASSERT_EQUAL(modality, series->getModality());
        CPPUNIT_ASSERT_EQUAL(modality, series->getByteValue(0x0008, 0x0060));
    }

    {
        auto series = data::Series::New();
        series->setByteValue(0x0008, 0x0060, modality);
        CPPUNIT_ASSERT_EQUAL(modality, series->getModality());
        CPPUNIT_ASSERT_EQUAL(modality, series->getByteValue(0x0008, 0x0060));
    }
}

//------------------------------------------------------------------------------

void SeriesTest::seriesDescriptionTest()
{
    static const std::string seriesDescription(UUID::generateUUID());

    {
        auto series = data::Series::New();
        series->setSeriesDescription(seriesDescription);
        CPPUNIT_ASSERT_EQUAL(seriesDescription, series->getSeriesDescription());
        CPPUNIT_ASSERT_EQUAL(seriesDescription, series->getByteValue(0x0008, 0x103e));
    }

    {
        auto series = data::Series::New();
        series->setByteValue(0x0008, 0x103e, seriesDescription);
        CPPUNIT_ASSERT_EQUAL(seriesDescription, series->getSeriesDescription());
        CPPUNIT_ASSERT_EQUAL(seriesDescription, series->getByteValue(0x0008, 0x103e));
    }
}

//------------------------------------------------------------------------------

void SeriesTest::performingPhysicianNameTest()
{
    static const std::vector<std::string> performingPhysicianNames = {
        UUID::generateUUID(),
        UUID::generateUUID(),
        UUID::generateUUID()
    };

    static const std::string performingPhysicianName = boost::join(performingPhysicianNames, "\\");

    {
        auto series = data::Series::New();
        series->setPerformingPhysicianNames(performingPhysicianNames);
        CPPUNIT_ASSERT(performingPhysicianNames == series->getPerformingPhysicianNames());
        CPPUNIT_ASSERT(performingPhysicianNames == series->getByteValues(0x0008, 0x1050));

        CPPUNIT_ASSERT_EQUAL(performingPhysicianName, series->getPerformingPhysicianName());
        CPPUNIT_ASSERT_EQUAL(performingPhysicianName, series->getByteValue(0x0008, 0x1050));
    }

    {
        auto series = data::Series::New();
        series->setByteValues(0x0008, 0x1050, performingPhysicianNames);
        CPPUNIT_ASSERT(performingPhysicianNames == series->getPerformingPhysicianNames());
        CPPUNIT_ASSERT(performingPhysicianNames == series->getByteValues(0x0008, 0x1050));

        CPPUNIT_ASSERT_EQUAL(performingPhysicianName, series->getPerformingPhysicianName());
        CPPUNIT_ASSERT_EQUAL(performingPhysicianName, series->getByteValue(0x0008, 0x1050));
    }

    {
        auto series = data::Series::New();
        series->setPerformingPhysicianName(performingPhysicianName);
        CPPUNIT_ASSERT(performingPhysicianNames == series->getPerformingPhysicianNames());
        CPPUNIT_ASSERT(performingPhysicianNames == series->getByteValues(0x0008, 0x1050));

        CPPUNIT_ASSERT_EQUAL(performingPhysicianName, series->getPerformingPhysicianName());
        CPPUNIT_ASSERT_EQUAL(performingPhysicianName, series->getByteValue(0x0008, 0x1050));
    }

    {
        auto series = data::Series::New();
        series->setByteValue(0x0008, 0x1050, performingPhysicianName);
        CPPUNIT_ASSERT(performingPhysicianNames == series->getPerformingPhysicianNames());
        CPPUNIT_ASSERT(performingPhysicianNames == series->getByteValues(0x0008, 0x1050));

        CPPUNIT_ASSERT_EQUAL(performingPhysicianName, series->getPerformingPhysicianName());
        CPPUNIT_ASSERT_EQUAL(performingPhysicianName, series->getByteValue(0x0008, 0x1050));
    }
}

//------------------------------------------------------------------------------

void SeriesTest::anatomicalOrientationTypeTest()
{
    static const std::string anatomicalOrientationType("QUADRUPED");

    {
        auto series = data::Series::New();
        series->setAnatomicalOrientationType(anatomicalOrientationType);
        CPPUNIT_ASSERT_EQUAL(anatomicalOrientationType, series->getAnatomicalOrientationType());
        CPPUNIT_ASSERT_EQUAL(anatomicalOrientationType, series->getByteValue(0x0010, 0x2210));
    }

    {
        auto series = data::Series::New();
        series->setByteValue(0x0010, 0x2210, anatomicalOrientationType);
        CPPUNIT_ASSERT_EQUAL(anatomicalOrientationType, series->getAnatomicalOrientationType());
        CPPUNIT_ASSERT_EQUAL(anatomicalOrientationType, series->getByteValue(0x0010, 0x2210));
    }
}

//------------------------------------------------------------------------------

void SeriesTest::bodyPartExaminedTest()
{
    static const std::string bodyPartExamined("ABDOMEN");

    {
        auto series = data::Series::New();
        series->setBodyPartExamined(bodyPartExamined);
        CPPUNIT_ASSERT_EQUAL(bodyPartExamined, series->getBodyPartExamined());
        CPPUNIT_ASSERT_EQUAL(bodyPartExamined, series->getByteValue(0x0018, 0x0015));
    }

    {
        auto series = data::Series::New();
        series->setByteValue(0x0018, 0x0015, bodyPartExamined);
        CPPUNIT_ASSERT_EQUAL(bodyPartExamined, series->getBodyPartExamined());
        CPPUNIT_ASSERT_EQUAL(bodyPartExamined, series->getByteValue(0x0018, 0x0015));
    }
}

//------------------------------------------------------------------------------

void SeriesTest::protocolNameTest()
{
    static const std::string protocolName(UUID::generateUUID());

    {
        auto series = data::Series::New();
        series->setProtocolName(protocolName);
        CPPUNIT_ASSERT_EQUAL(protocolName, series->getProtocolName());
        CPPUNIT_ASSERT_EQUAL(protocolName, series->getByteValue(0x0018, 0x1030));
    }

    {
        auto series = data::Series::New();
        series->setByteValue(0x0018, 0x1030, protocolName);
        CPPUNIT_ASSERT_EQUAL(protocolName, series->getProtocolName());
        CPPUNIT_ASSERT_EQUAL(protocolName, series->getByteValue(0x0018, 0x1030));
    }
}

//------------------------------------------------------------------------------

void SeriesTest::patientPositionTest()
{
    static const std::string patientPosition("FFP");

    {
        auto series = data::Series::New();
        series->setPatientPosition(patientPosition);
        CPPUNIT_ASSERT_EQUAL(patientPosition, series->getPatientPosition());
        CPPUNIT_ASSERT_EQUAL(patientPosition, series->getByteValue(0x0018, 0x5100));
    }

    {
        auto series = data::Series::New();
        series->setByteValue(0x0018, 0x5100, patientPosition);
        CPPUNIT_ASSERT_EQUAL(patientPosition, series->getPatientPosition());
        CPPUNIT_ASSERT_EQUAL(patientPosition, series->getByteValue(0x0018, 0x5100));
    }
}

//------------------------------------------------------------------------------

void SeriesTest::seriesInstanceUIDTest()
{
    static const std::string seriesInstanceUID(UUID::generateUUID());

    {
        auto series = data::Series::New();
        series->setSeriesInstanceUID(seriesInstanceUID);
        CPPUNIT_ASSERT_EQUAL(seriesInstanceUID, series->getSeriesInstanceUID());
        CPPUNIT_ASSERT_EQUAL(seriesInstanceUID, series->getByteValue(0x0020, 0x000e));
    }

    {
        auto series = data::Series::New();
        series->setByteValue(0x0020, 0x000e, seriesInstanceUID);
        CPPUNIT_ASSERT_EQUAL(seriesInstanceUID, series->getSeriesInstanceUID());
        CPPUNIT_ASSERT_EQUAL(seriesInstanceUID, series->getByteValue(0x0020, 0x000e));
    }
}

//------------------------------------------------------------------------------

void SeriesTest::seriesNumberTest()
{
    static const std::int32_t seriesNumber = 1;

    {
        auto series = data::Series::New();
        series->setSeriesNumber(seriesNumber);
        CPPUNIT_ASSERT_EQUAL(seriesNumber, *series->getSeriesNumber());
        CPPUNIT_ASSERT_EQUAL(std::to_string(seriesNumber), series->getByteValue(0x0020, 0x0011));
    }

    {
        auto series = data::Series::New();
        series->setByteValue(0x0020, 0x0011, std::to_string(seriesNumber));
        CPPUNIT_ASSERT_EQUAL(seriesNumber, *series->getSeriesNumber());
        CPPUNIT_ASSERT_EQUAL(std::to_string(seriesNumber), series->getByteValue(0x0020, 0x0011));
    }

    {
        // Test emtpy value
        auto series = data::Series::New();
        series->setSeriesNumber();

        CPPUNIT_ASSERT(!series->getSeriesNumber());
        CPPUNIT_ASSERT_EQUAL(std::string(), series->getByteValue(0x0020, 0x0011));
    }
}

//------------------------------------------------------------------------------

void SeriesTest::lateralityTest()
{
    static const std::string laterality("R");

    {
        auto series = data::Series::New();
        series->setLaterality(laterality);
        CPPUNIT_ASSERT_EQUAL(laterality, series->getLaterality());
        CPPUNIT_ASSERT_EQUAL(laterality, series->getByteValue(0x0020, 0x0060));
    }

    {
        auto series = data::Series::New();
        series->setByteValue(0x0020, 0x0060, laterality);
        CPPUNIT_ASSERT_EQUAL(laterality, series->getLaterality());
        CPPUNIT_ASSERT_EQUAL(laterality, series->getByteValue(0x0020, 0x0060));
    }
}

//------------------------------------------------------------------------------

void SeriesTest::performedProcedureStepStartDateTest()
{
    static const std::string performedProcedureStepStartDate("20000101");

    {
        auto series = data::Series::New();
        series->setPerformedProcedureStepStartDate(performedProcedureStepStartDate);
        CPPUNIT_ASSERT_EQUAL(performedProcedureStepStartDate, series->getPerformedProcedureStepStartDate());
        CPPUNIT_ASSERT_EQUAL(performedProcedureStepStartDate, series->getByteValue(0x0040, 0x0244));
    }

    {
        auto series = data::Series::New();
        series->setByteValue(0x0040, 0x0244, performedProcedureStepStartDate);
        CPPUNIT_ASSERT_EQUAL(performedProcedureStepStartDate, series->getPerformedProcedureStepStartDate());
        CPPUNIT_ASSERT_EQUAL(performedProcedureStepStartDate, series->getByteValue(0x0040, 0x0244));
    }
}

//------------------------------------------------------------------------------

void SeriesTest::performedProcedureStepStartTimeTest()
{
    static const std::string performedProcedureStepStartTime("165649.123456");

    {
        auto series = data::Series::New();
        series->setPerformedProcedureStepStartTime(performedProcedureStepStartTime);
        CPPUNIT_ASSERT_EQUAL(performedProcedureStepStartTime, series->getPerformedProcedureStepStartTime());
        CPPUNIT_ASSERT_EQUAL(performedProcedureStepStartTime, series->getByteValue(0x0040, 0x0245));
    }

    {
        auto series = data::Series::New();
        series->setByteValue(0x0040, 0x0245, performedProcedureStepStartTime);
        CPPUNIT_ASSERT_EQUAL(performedProcedureStepStartTime, series->getPerformedProcedureStepStartTime());
        CPPUNIT_ASSERT_EQUAL(performedProcedureStepStartTime, series->getByteValue(0x0040, 0x0245));
    }
}

//------------------------------------------------------------------------------

void SeriesTest::performedProcedureStepEndDateTest()
{
    static const std::string performedProcedureStepEndDate("20000101");

    {
        auto series = data::Series::New();
        series->setPerformedProcedureStepEndDate(performedProcedureStepEndDate);
        CPPUNIT_ASSERT_EQUAL(performedProcedureStepEndDate, series->getPerformedProcedureStepEndDate());
        CPPUNIT_ASSERT_EQUAL(performedProcedureStepEndDate, series->getByteValue(0x0040, 0x0250));
    }

    {
        auto series = data::Series::New();
        series->setByteValue(0x0040, 0x0250, performedProcedureStepEndDate);
        CPPUNIT_ASSERT_EQUAL(performedProcedureStepEndDate, series->getPerformedProcedureStepEndDate());
        CPPUNIT_ASSERT_EQUAL(performedProcedureStepEndDate, series->getByteValue(0x0040, 0x0250));
    }
}

//------------------------------------------------------------------------------

void SeriesTest::performedProcedureStepEndTimeTest()
{
    static const std::string performedProcedureStepEndTime("165649.123456");

    {
        auto series = data::Series::New();
        series->setPerformedProcedureStepEndTime(performedProcedureStepEndTime);
        CPPUNIT_ASSERT_EQUAL(performedProcedureStepEndTime, series->getPerformedProcedureStepEndTime());
        CPPUNIT_ASSERT_EQUAL(performedProcedureStepEndTime, series->getByteValue(0x0040, 0x0251));
    }

    {
        auto series = data::Series::New();
        series->setByteValue(0x0040, 0x0251, performedProcedureStepEndTime);
        CPPUNIT_ASSERT_EQUAL(performedProcedureStepEndTime, series->getPerformedProcedureStepEndTime());
        CPPUNIT_ASSERT_EQUAL(performedProcedureStepEndTime, series->getByteValue(0x0040, 0x0251));
    }
}

//------------------------------------------------------------------------------

void SeriesTest::performedProcedureStepIDTest()
{
    static const std::string performedProcedureStepID("B");

    {
        auto series = data::Series::New();
        series->setPerformedProcedureStepID(performedProcedureStepID);
        CPPUNIT_ASSERT_EQUAL(performedProcedureStepID, series->getPerformedProcedureStepID());
        CPPUNIT_ASSERT_EQUAL(performedProcedureStepID, series->getByteValue(0x0040, 0x0253));
    }

    {
        auto series = data::Series::New();
        series->setByteValue(0x0040, 0x0253, performedProcedureStepID);
        CPPUNIT_ASSERT_EQUAL(performedProcedureStepID, series->getPerformedProcedureStepID());
        CPPUNIT_ASSERT_EQUAL(performedProcedureStepID, series->getByteValue(0x0040, 0x0253));
    }
}

//------------------------------------------------------------------------------

void SeriesTest::performedProcedureStepDescriptionTest()
{
    static const std::string performedProcedureStepDescription(UUID::generateUUID());

    {
        auto series = data::Series::New();
        series->setPerformedProcedureStepDescription(performedProcedureStepDescription);
        CPPUNIT_ASSERT_EQUAL(performedProcedureStepDescription, series->getPerformedProcedureStepDescription());
        CPPUNIT_ASSERT_EQUAL(performedProcedureStepDescription, series->getByteValue(0x0040, 0x0254));
    }

    {
        auto series = data::Series::New();
        series->setByteValue(0x0040, 0x0254, performedProcedureStepDescription);
        CPPUNIT_ASSERT_EQUAL(performedProcedureStepDescription, series->getPerformedProcedureStepDescription());
        CPPUNIT_ASSERT_EQUAL(performedProcedureStepDescription, series->getByteValue(0x0040, 0x0254));
    }
}

//------------------------------------------------------------------------------

void SeriesTest::commentsOnThePerformedProcedureStepTest()
{
    static const std::string commentsOnThePerformedProcedureStep(UUID::generateUUID());

    {
        auto series = data::Series::New();
        series->setCommentsOnThePerformedProcedureStep(commentsOnThePerformedProcedureStep);
        CPPUNIT_ASSERT_EQUAL(commentsOnThePerformedProcedureStep, series->getCommentsOnThePerformedProcedureStep());
        CPPUNIT_ASSERT_EQUAL(commentsOnThePerformedProcedureStep, series->getByteValue(0x0040, 0x0280));
    }

    {
        auto series = data::Series::New();
        series->setByteValue(0x0040, 0x0280, commentsOnThePerformedProcedureStep);
        CPPUNIT_ASSERT_EQUAL(commentsOnThePerformedProcedureStep, series->getCommentsOnThePerformedProcedureStep());
        CPPUNIT_ASSERT_EQUAL(commentsOnThePerformedProcedureStep, series->getByteValue(0x0040, 0x0280));
    }
}

//------------------------------------------------------------------------------

void SeriesTest::institutionNameTest()
{
    static const std::string institutionName(UUID::generateUUID());

    {
        auto series = data::Series::New();
        series->setInstitutionName(institutionName);
        CPPUNIT_ASSERT_EQUAL(institutionName, series->getInstitutionName());
        CPPUNIT_ASSERT_EQUAL(institutionName, series->getByteValue(0x0008, 0x0080));
    }

    {
        auto series = data::Series::New();
        series->setByteValue(0x0008, 0x0080, institutionName);
        CPPUNIT_ASSERT_EQUAL(institutionName, series->getInstitutionName());
        CPPUNIT_ASSERT_EQUAL(institutionName, series->getByteValue(0x0008, 0x0080));
    }
}

//------------------------------------------------------------------------------

void SeriesTest::patientNameTest()
{
    static const std::string patientName(UUID::generateUUID());

    {
        auto series = data::Series::New();
        series->setPatientName(patientName);
        CPPUNIT_ASSERT_EQUAL(patientName, series->getPatientName());
        CPPUNIT_ASSERT_EQUAL(patientName, series->getByteValue(0x0010, 0x0010));
    }

    {
        auto series = data::Series::New();
        series->setByteValue(0x0010, 0x0010, patientName);
        CPPUNIT_ASSERT_EQUAL(patientName, series->getPatientName());
        CPPUNIT_ASSERT_EQUAL(patientName, series->getByteValue(0x0010, 0x0010));
    }
}

//------------------------------------------------------------------------------

void SeriesTest::patientIDTest()
{
    static const std::string patientID(UUID::generateUUID());

    {
        auto series = data::Series::New();
        series->setPatientID(patientID);
        CPPUNIT_ASSERT_EQUAL(patientID, series->getPatientID());
        CPPUNIT_ASSERT_EQUAL(patientID, series->getByteValue(0x0010, 0x0020));
    }

    {
        auto series = data::Series::New();
        series->setByteValue(0x0010, 0x0020, patientID);
        CPPUNIT_ASSERT_EQUAL(patientID, series->getPatientID());
        CPPUNIT_ASSERT_EQUAL(patientID, series->getByteValue(0x0010, 0x0020));
    }
}

//------------------------------------------------------------------------------

void SeriesTest::patientBirthDateTest()
{
    static const std::string patientBirthDate("20000101");

    {
        auto series = data::Series::New();
        series->setPatientBirthDate(patientBirthDate);
        CPPUNIT_ASSERT_EQUAL(patientBirthDate, series->getPatientBirthDate());
        CPPUNIT_ASSERT_EQUAL(patientBirthDate, series->getByteValue(0x0010, 0x0030));
    }

    {
        auto series = data::Series::New();
        series->setByteValue(0x0010, 0x0030, patientBirthDate);
        CPPUNIT_ASSERT_EQUAL(patientBirthDate, series->getPatientBirthDate());
        CPPUNIT_ASSERT_EQUAL(patientBirthDate, series->getByteValue(0x0010, 0x0030));
    }
}

//------------------------------------------------------------------------------

void SeriesTest::patientSexTest()
{
    static const std::string patientSex("M");

    {
        auto series = data::Series::New();
        series->setPatientSex(patientSex);
        CPPUNIT_ASSERT_EQUAL(patientSex, series->getPatientSex());
        CPPUNIT_ASSERT_EQUAL(patientSex, series->getByteValue(0x0010, 0x0040));
    }

    {
        auto series = data::Series::New();
        series->setByteValue(0x0010, 0x0040, patientSex);
        CPPUNIT_ASSERT_EQUAL(patientSex, series->getPatientSex());
        CPPUNIT_ASSERT_EQUAL(patientSex, series->getByteValue(0x0010, 0x0040));
    }
}

//------------------------------------------------------------------------------

void SeriesTest::studyDateTest()
{
    static const std::string studyDate("20011012");

    {
        auto series = data::Series::New();
        series->setStudyDate(studyDate);
        CPPUNIT_ASSERT_EQUAL(studyDate, series->getStudyDate());
        CPPUNIT_ASSERT_EQUAL(studyDate, series->getByteValue(0x0008, 0x0020));
    }

    {
        auto series = data::Series::New();
        series->setByteValue(0x0008, 0x0020, studyDate);
        CPPUNIT_ASSERT_EQUAL(studyDate, series->getStudyDate());
        CPPUNIT_ASSERT_EQUAL(studyDate, series->getByteValue(0x0008, 0x0020));
    }
}

//------------------------------------------------------------------------------

void SeriesTest::studyTimeTest()
{
    static const std::string studyTime("224513.123456");

    {
        auto series = data::Series::New();
        series->setStudyTime(studyTime);
        CPPUNIT_ASSERT_EQUAL(studyTime, series->getStudyTime());
        CPPUNIT_ASSERT_EQUAL(studyTime, series->getByteValue(0x0008, 0x0030));
    }

    {
        auto series = data::Series::New();
        series->setByteValue(0x0008, 0x0030, studyTime);
        CPPUNIT_ASSERT_EQUAL(studyTime, series->getStudyTime());
        CPPUNIT_ASSERT_EQUAL(studyTime, series->getByteValue(0x0008, 0x0030));
    }
}

//------------------------------------------------------------------------------

void SeriesTest::referringPhysicianNameTest()
{
    static const std::string referringPhysicianName(UUID::generateUUID());

    {
        auto series = data::Series::New();
        series->setReferringPhysicianName(referringPhysicianName);
        CPPUNIT_ASSERT_EQUAL(referringPhysicianName, series->getReferringPhysicianName());
        CPPUNIT_ASSERT_EQUAL(referringPhysicianName, series->getByteValue(0x0008, 0x0090));
    }

    {
        auto series = data::Series::New();
        series->setByteValue(0x0008, 0x0090, referringPhysicianName);
        CPPUNIT_ASSERT_EQUAL(referringPhysicianName, series->getReferringPhysicianName());
        CPPUNIT_ASSERT_EQUAL(referringPhysicianName, series->getByteValue(0x0008, 0x0090));
    }
}

//------------------------------------------------------------------------------

void SeriesTest::studyDescriptionTest()
{
    static const std::string studyDescription(UUID::generateUUID());

    {
        auto series = data::Series::New();
        series->setStudyDescription(studyDescription);
        CPPUNIT_ASSERT_EQUAL(studyDescription, series->getStudyDescription());
        CPPUNIT_ASSERT_EQUAL(studyDescription, series->getByteValue(0x0008, 0x1030));
    }

    {
        auto series = data::Series::New();
        series->setByteValue(0x0008, 0x1030, studyDescription);
        CPPUNIT_ASSERT_EQUAL(studyDescription, series->getStudyDescription());
        CPPUNIT_ASSERT_EQUAL(studyDescription, series->getByteValue(0x0008, 0x1030));
    }
}

//------------------------------------------------------------------------------

void SeriesTest::studyInstanceUIDTest()
{
    static const std::string studyInstanceUID(UUID::generateUUID());

    {
        auto series = data::Series::New();
        series->setStudyInstanceUID(studyInstanceUID);
        CPPUNIT_ASSERT_EQUAL(studyInstanceUID, series->getStudyInstanceUID());
        CPPUNIT_ASSERT_EQUAL(studyInstanceUID, series->getByteValue(0x0020, 0x000d));
    }

    {
        auto series = data::Series::New();
        series->setByteValue(0x0020, 0x000d, studyInstanceUID);
        CPPUNIT_ASSERT_EQUAL(studyInstanceUID, series->getStudyInstanceUID());
        CPPUNIT_ASSERT_EQUAL(studyInstanceUID, series->getByteValue(0x0020, 0x000d));
    }
}

//------------------------------------------------------------------------------

void SeriesTest::studyIDTest()
{
    static const std::string studyID("A");

    {
        auto series = data::Series::New();
        series->setStudyID(studyID);
        CPPUNIT_ASSERT_EQUAL(studyID, series->getStudyID());
        CPPUNIT_ASSERT_EQUAL(studyID, series->getByteValue(0x0020, 0x0010));
    }

    {
        auto series = data::Series::New();
        series->setByteValue(0x0020, 0x0010, studyID);
        CPPUNIT_ASSERT_EQUAL(studyID, series->getStudyID());
        CPPUNIT_ASSERT_EQUAL(studyID, series->getByteValue(0x0020, 0x0010));
    }
}

//------------------------------------------------------------------------------

void SeriesTest::patientAgeTest()
{
    static const std::string patientAge("015Y");

    {
        auto series = data::Series::New();
        series->setPatientAge(patientAge);
        CPPUNIT_ASSERT_EQUAL(patientAge, series->getPatientAge());
        CPPUNIT_ASSERT_EQUAL(patientAge, series->getByteValue(0x0010, 0x1010));
    }

    {
        auto series = data::Series::New();
        series->setByteValue(0x0010, 0x1010, patientAge);
        CPPUNIT_ASSERT_EQUAL(patientAge, series->getPatientAge());
        CPPUNIT_ASSERT_EQUAL(patientAge, series->getByteValue(0x0010, 0x1010));
    }
}

//------------------------------------------------------------------------------

void SeriesTest::patientSizeTest()
{
    static const double patientSize = 188.53;

    {
        auto series = data::Series::New();
        series->setPatientSize(patientSize);
        CPPUNIT_ASSERT_EQUAL(patientSize, *series->getPatientSize());
        CPPUNIT_ASSERT_DOUBLES_EQUAL(patientSize, std::stod(series->getByteValue(0x0010, 0x1020)), 0.00001);
    }

    {
        auto series = data::Series::New();
        series->setByteValue(0x0010, 0x1020, std::to_string(patientSize));
        CPPUNIT_ASSERT_EQUAL(patientSize, *series->getPatientSize());
        CPPUNIT_ASSERT_DOUBLES_EQUAL(patientSize, std::stod(series->getByteValue(0x0010, 0x1020)), 0.00001);
    }

    {
        // Test emtpy value
        auto series = data::Series::New();
        series->setPatientSize();

        CPPUNIT_ASSERT(!series->getPatientSize());
        CPPUNIT_ASSERT_EQUAL(std::string(), series->getByteValue(0x0010, 0x1020));
    }
}

//------------------------------------------------------------------------------

void SeriesTest::patientWeightTest()
{
    static const double patientWeight = 115.07;

    {
        auto series = data::Series::New();
        series->setPatientWeight(patientWeight);
        CPPUNIT_ASSERT_EQUAL(patientWeight, *series->getPatientWeight());
        CPPUNIT_ASSERT_DOUBLES_EQUAL(patientWeight, std::stod(series->getByteValue(0x0010, 0x1030)), 0.00001);
    }

    {
        auto series = data::Series::New();
        series->setByteValue(0x0010, 0x1030, std::to_string(patientWeight));
        CPPUNIT_ASSERT_EQUAL(patientWeight, *series->getPatientWeight());
        CPPUNIT_ASSERT_DOUBLES_EQUAL(patientWeight, std::stod(series->getByteValue(0x0010, 0x1030)), 0.00001);
    }

    {
        // Test emtpy value
        auto series = data::Series::New();
        series->setPatientWeight();

        CPPUNIT_ASSERT(!series->getPatientWeight());
        CPPUNIT_ASSERT_EQUAL(std::string(), series->getByteValue(0x0010, 0x1030));
    }
}

//------------------------------------------------------------------------------

void SeriesTest::acquisitionDateTest()
{
    static const std::string acquisitionDate("20011012");

    {
        auto series = data::ImageSeries::New();
        series->setAcquisitionDate(acquisitionDate);
        CPPUNIT_ASSERT_EQUAL(acquisitionDate, series->getAcquisitionDate());
        CPPUNIT_ASSERT_EQUAL(acquisitionDate, series->getByteValue(0x0008, 0x0022));
    }

    {
        auto series = data::ImageSeries::New();
        series->setByteValue(0x0008, 0x0022, acquisitionDate);
        CPPUNIT_ASSERT_EQUAL(acquisitionDate, series->getAcquisitionDate());
        CPPUNIT_ASSERT_EQUAL(acquisitionDate, series->getByteValue(0x0008, 0x0022));
    }
}

//------------------------------------------------------------------------------

void SeriesTest::acquisitionTimeTest()
{
    static const std::string acquisitionTime("224513.123456");

    {
        auto series = data::ImageSeries::New();
        series->setAcquisitionTime(acquisitionTime);
        CPPUNIT_ASSERT_EQUAL(acquisitionTime, series->getAcquisitionTime());
        CPPUNIT_ASSERT_EQUAL(acquisitionTime, series->getByteValue(0x0008, 0x0032));
    }

    {
        auto series = data::ImageSeries::New();
        series->setByteValue(0x0008, 0x0032, acquisitionTime);
        CPPUNIT_ASSERT_EQUAL(acquisitionTime, series->getAcquisitionTime());
        CPPUNIT_ASSERT_EQUAL(acquisitionTime, series->getByteValue(0x0008, 0x0032));
    }
}

//------------------------------------------------------------------------------

void SeriesTest::acquisitionNumberTest()
{
    static const std::int32_t acquisitionNumber = 0x00200012;

    {
        auto series = data::ImageSeries::New();
        series->setAcquisitionNumber(acquisitionNumber);
        CPPUNIT_ASSERT_EQUAL(acquisitionNumber, series->getAcquisitionNumber().value_or(0));
        CPPUNIT_ASSERT_EQUAL(std::to_string(acquisitionNumber), series->getByteValue(0x0020, 0x0012));
    }

    {
        auto series = data::ImageSeries::New();
        series->setByteValue(0x0020, 0x0012, std::to_string(acquisitionNumber));
        CPPUNIT_ASSERT_EQUAL(acquisitionNumber, series->getAcquisitionNumber().value_or(0));
        CPPUNIT_ASSERT_EQUAL(std::to_string(acquisitionNumber), series->getByteValue(0x0020, 0x0012));
    }
}

//------------------------------------------------------------------------------

void SeriesTest::instanceNumberTest()
{
    static const std::int32_t instanceNumber = 0x00200013;

    {
        auto series = data::ImageSeries::New();
        series->setInstanceNumber(instanceNumber);
        CPPUNIT_ASSERT_EQUAL(instanceNumber, series->getInstanceNumber().value_or(0));
        CPPUNIT_ASSERT_EQUAL(std::to_string(instanceNumber), series->getByteValue(0x0020, 0x0013));
    }

    {
        auto series = data::ImageSeries::New();
        series->setByteValue(0x0020, 0x0013, std::to_string(instanceNumber));
        CPPUNIT_ASSERT_EQUAL(instanceNumber, series->getInstanceNumber().value_or(0));
        CPPUNIT_ASSERT_EQUAL(std::to_string(instanceNumber), series->getByteValue(0x0020, 0x0013));
    }
}

//------------------------------------------------------------------------------

void SeriesTest::contentTimeTest()
{
    static const std::string contentTime("224513.123456");

    {
        auto series = data::ImageSeries::New();
        series->setContentTime(contentTime);
        CPPUNIT_ASSERT_EQUAL(contentTime, series->getContentTime());
        CPPUNIT_ASSERT_EQUAL(contentTime, series->getByteValue(0x0008, 0x0033));
    }

    {
        auto series = data::ImageSeries::New();
        series->setByteValue(0x0008, 0x0033, contentTime);
        CPPUNIT_ASSERT_EQUAL(contentTime, series->getContentTime());
        CPPUNIT_ASSERT_EQUAL(contentTime, series->getByteValue(0x0008, 0x0033));
    }
}

//------------------------------------------------------------------------------

void SeriesTest::contrastBolusAgentTest()
{
    static const std::string contrastBolusAgent(UUID::generateUUID());

    {
        auto series = data::ImageSeries::New();
        series->setContrastBolusAgent(contrastBolusAgent);
        CPPUNIT_ASSERT_EQUAL(contrastBolusAgent, series->getContrastBolusAgent());
        CPPUNIT_ASSERT_EQUAL(contrastBolusAgent, series->getByteValue(0x0018, 0x0010));
    }

    {
        auto series = data::ImageSeries::New();
        series->setByteValue(0x0018, 0x0010, contrastBolusAgent);
        CPPUNIT_ASSERT_EQUAL(contrastBolusAgent, series->getContrastBolusAgent());
        CPPUNIT_ASSERT_EQUAL(contrastBolusAgent, series->getByteValue(0x0018, 0x0010));
    }
}

//------------------------------------------------------------------------------

void SeriesTest::contrastBolusRouteTest()
{
    static const std::string contrastBolusRoute(UUID::generateUUID());

    {
        auto series = data::ImageSeries::New();
        series->setContrastBolusRoute(contrastBolusRoute);
        CPPUNIT_ASSERT_EQUAL(contrastBolusRoute, series->getContrastBolusRoute());
        CPPUNIT_ASSERT_EQUAL(contrastBolusRoute, series->getByteValue(0x0018, 0x1040));
    }

    {
        auto series = data::ImageSeries::New();
        series->setByteValue(0x0018, 0x1040, contrastBolusRoute);
        CPPUNIT_ASSERT_EQUAL(contrastBolusRoute, series->getContrastBolusRoute());
        CPPUNIT_ASSERT_EQUAL(contrastBolusRoute, series->getByteValue(0x0018, 0x1040));
    }
}

//------------------------------------------------------------------------------

void SeriesTest::contrastBolusVolumeTest()
{
    static const double contrastBolusVolume = 188.53;

    {
        auto series = data::ImageSeries::New();
        series->setContrastBolusVolume(contrastBolusVolume);
        CPPUNIT_ASSERT_EQUAL(contrastBolusVolume, *series->getContrastBolusVolume());
        CPPUNIT_ASSERT_DOUBLES_EQUAL(contrastBolusVolume, std::stod(series->getByteValue(0x0018, 0x1041)), 0.00001);
    }

    {
        auto series = data::ImageSeries::New();
        series->setByteValue(0x0018, 0x1041, std::to_string(contrastBolusVolume));
        CPPUNIT_ASSERT_EQUAL(contrastBolusVolume, *series->getContrastBolusVolume());
        CPPUNIT_ASSERT_DOUBLES_EQUAL(contrastBolusVolume, std::stod(series->getByteValue(0x0018, 0x1041)), 0.00001);
    }

    {
        // Test emtpy value
        auto series = data::ImageSeries::New();
        series->setContrastBolusVolume();

        CPPUNIT_ASSERT(!series->getContrastBolusVolume());
        CPPUNIT_ASSERT_EQUAL(std::string(), series->getByteValue(0x0018, 0x1041));
    }
}

//------------------------------------------------------------------------------

void SeriesTest::contrastBolusStartTimeTest()
{
    static const std::string contrastBolusStartTime("224513.123456");

    {
        auto series = data::ImageSeries::New();
        series->setContrastBolusStartTime(contrastBolusStartTime);
        CPPUNIT_ASSERT_EQUAL(contrastBolusStartTime, series->getContrastBolusStartTime());
        CPPUNIT_ASSERT_EQUAL(contrastBolusStartTime, series->getByteValue(0x0018, 0x1042));
    }

    {
        auto series = data::ImageSeries::New();
        series->setByteValue(0x0018, 0x1042, contrastBolusStartTime);
        CPPUNIT_ASSERT_EQUAL(contrastBolusStartTime, series->getContrastBolusStartTime());
        CPPUNIT_ASSERT_EQUAL(contrastBolusStartTime, series->getByteValue(0x0018, 0x1042));
    }
}

//------------------------------------------------------------------------------

void SeriesTest::contrastBolusStopTimeTest()
{
    static const std::string contrastBolusStopTime("224513.123456");

    {
        auto series = data::ImageSeries::New();
        series->setContrastBolusStopTime(contrastBolusStopTime);
        CPPUNIT_ASSERT_EQUAL(contrastBolusStopTime, series->getContrastBolusStopTime());
        CPPUNIT_ASSERT_EQUAL(contrastBolusStopTime, series->getByteValue(0x0018, 0x1043));
    }

    {
        auto series = data::ImageSeries::New();
        series->setByteValue(0x0018, 0x1043, contrastBolusStopTime);
        CPPUNIT_ASSERT_EQUAL(contrastBolusStopTime, series->getContrastBolusStopTime());
        CPPUNIT_ASSERT_EQUAL(contrastBolusStopTime, series->getByteValue(0x0018, 0x1043));
    }
}

//------------------------------------------------------------------------------

void SeriesTest::contrastBolusTotalDoseTest()
{
    static const double contrastBolusTotalDose = 123.45;

    {
        auto series = data::ImageSeries::New();
        series->setContrastBolusTotalDose(contrastBolusTotalDose);
        CPPUNIT_ASSERT_EQUAL(contrastBolusTotalDose, *series->getContrastBolusTotalDose());
        CPPUNIT_ASSERT_DOUBLES_EQUAL(
            contrastBolusTotalDose,
            std::stod(series->getByteValue(0x0018, 0x1044)),
            0.00001
        );
    }

    {
        auto series = data::ImageSeries::New();
        series->setByteValue(0x0018, 0x1044, std::to_string(contrastBolusTotalDose));
        CPPUNIT_ASSERT_EQUAL(contrastBolusTotalDose, *series->getContrastBolusTotalDose());
        CPPUNIT_ASSERT_DOUBLES_EQUAL(
            contrastBolusTotalDose,
            std::stod(series->getByteValue(0x0018, 0x1044)),
            0.00001
        );
    }

    {
        // Test emtpy value
        auto series = data::ImageSeries::New();
        series->setContrastBolusTotalDose();

        CPPUNIT_ASSERT(!series->getContrastBolusTotalDose());
        CPPUNIT_ASSERT_EQUAL(std::string(), series->getByteValue(0x0018, 0x1044));
    }
}

//------------------------------------------------------------------------------

void SeriesTest::contrastFlowRateTest()
{
    static const std::vector<std::string> contrastFlowRateStrings = {
        "11.1",
        "22.2",
        "33.3"
    };

    static const std::vector<double> contrastFlowRates = {
        std::stod(contrastFlowRateStrings[0]),
        std::stod(contrastFlowRateStrings[1]),
        std::stod(contrastFlowRateStrings[2])
    };

    static const std::string contrastFlowRate = boost::join(contrastFlowRateStrings, "\\");

    {
        auto series = data::ImageSeries::New();
        series->setContrastFlowRates(contrastFlowRates);
        CPPUNIT_ASSERT(core::tools::is_equal(contrastFlowRates, series->getContrastFlowRates()));
        CPPUNIT_ASSERT(contrastFlowRateStrings == series->getByteValues(0x0018, 0x1046));

        CPPUNIT_ASSERT_EQUAL(contrastFlowRate, series->getContrastFlowRate());
        CPPUNIT_ASSERT_EQUAL(contrastFlowRate, series->getByteValue(0x0018, 0x1046));
    }

    {
        auto series = data::ImageSeries::New();
        series->setByteValues(0x0018, 0x1046, contrastFlowRateStrings);
        CPPUNIT_ASSERT(contrastFlowRates == series->getContrastFlowRates());
        CPPUNIT_ASSERT(contrastFlowRateStrings == series->getByteValues(0x0018, 0x1046));

        CPPUNIT_ASSERT_EQUAL(contrastFlowRate, series->getContrastFlowRate());
        CPPUNIT_ASSERT_EQUAL(contrastFlowRate, series->getByteValue(0x0018, 0x1046));
    }

    {
        auto series = data::ImageSeries::New();
        series->setContrastFlowRate(contrastFlowRate);
        CPPUNIT_ASSERT(contrastFlowRates == series->getContrastFlowRates());
        CPPUNIT_ASSERT(contrastFlowRateStrings == series->getByteValues(0x0018, 0x1046));

        CPPUNIT_ASSERT_EQUAL(contrastFlowRate, series->getContrastFlowRate());
        CPPUNIT_ASSERT_EQUAL(contrastFlowRate, series->getByteValue(0x0018, 0x1046));
    }

    {
        auto series = data::ImageSeries::New();
        series->setByteValue(0x0018, 0x1046, contrastFlowRate);
        CPPUNIT_ASSERT(contrastFlowRates == series->getContrastFlowRates());
        CPPUNIT_ASSERT(contrastFlowRateStrings == series->getByteValues(0x0018, 0x1046));

        CPPUNIT_ASSERT_EQUAL(contrastFlowRate, series->getContrastFlowRate());
        CPPUNIT_ASSERT_EQUAL(contrastFlowRate, series->getByteValue(0x0018, 0x1046));
    }
}

//------------------------------------------------------------------------------

void SeriesTest::contrastFlowDurationTest()
{
    static const std::vector<std::string> contrastFlowDurationStrings = {
        "44.4",
        "55.5",
        "66.6"
    };

    static const std::vector<double> contrastFlowDurations = {
        std::stod(contrastFlowDurationStrings[0]),
        std::stod(contrastFlowDurationStrings[1]),
        std::stod(contrastFlowDurationStrings[2])
    };

    static const std::string contrastFlowDuration = boost::join(contrastFlowDurationStrings, "\\");

    {
        auto series = data::ImageSeries::New();
        series->setContrastFlowDurations(contrastFlowDurations);
        CPPUNIT_ASSERT(core::tools::is_equal(contrastFlowDurations, series->getContrastFlowDurations()));
        CPPUNIT_ASSERT(contrastFlowDurationStrings == series->getByteValues(0x0018, 0x1047));

        CPPUNIT_ASSERT_EQUAL(contrastFlowDuration, series->getContrastFlowDuration());
        CPPUNIT_ASSERT_EQUAL(contrastFlowDuration, series->getByteValue(0x0018, 0x1047));
    }

    {
        auto series = data::ImageSeries::New();
        series->setByteValues(0x0018, 0x1047, contrastFlowDurationStrings);
        CPPUNIT_ASSERT(contrastFlowDurations == series->getContrastFlowDurations());
        CPPUNIT_ASSERT(contrastFlowDurationStrings == series->getByteValues(0x0018, 0x1047));

        CPPUNIT_ASSERT_EQUAL(contrastFlowDuration, series->getContrastFlowDuration());
        CPPUNIT_ASSERT_EQUAL(contrastFlowDuration, series->getByteValue(0x0018, 0x1047));
    }

    {
        auto series = data::ImageSeries::New();
        series->setContrastFlowDuration(contrastFlowDuration);
        CPPUNIT_ASSERT(contrastFlowDurations == series->getContrastFlowDurations());
        CPPUNIT_ASSERT(contrastFlowDurationStrings == series->getByteValues(0x0018, 0x1047));

        CPPUNIT_ASSERT_EQUAL(contrastFlowDuration, series->getContrastFlowDuration());
        CPPUNIT_ASSERT_EQUAL(contrastFlowDuration, series->getByteValue(0x0018, 0x1047));
    }

    {
        auto series = data::ImageSeries::New();
        series->setByteValue(0x0018, 0x1047, contrastFlowDuration);
        CPPUNIT_ASSERT(contrastFlowDurations == series->getContrastFlowDurations());
        CPPUNIT_ASSERT(contrastFlowDurationStrings == series->getByteValues(0x0018, 0x1047));

        CPPUNIT_ASSERT_EQUAL(contrastFlowDuration, series->getContrastFlowDuration());
        CPPUNIT_ASSERT_EQUAL(contrastFlowDuration, series->getByteValue(0x0018, 0x1047));
    }
}

//------------------------------------------------------------------------------

void SeriesTest::contrastBolusIngredientTest()
{
    static const std::string contrastBolusIngredient("GADOLINIUM");

    {
        auto series = data::ImageSeries::New();
        series->setContrastBolusIngredient(contrastBolusIngredient);
        CPPUNIT_ASSERT_EQUAL(contrastBolusIngredient, series->getContrastBolusIngredient());
        CPPUNIT_ASSERT_EQUAL(contrastBolusIngredient, series->getByteValue(0x0018, 0x1048));
    }

    {
        auto series = data::ImageSeries::New();
        series->setByteValue(0x0018, 0x1048, contrastBolusIngredient);
        CPPUNIT_ASSERT_EQUAL(contrastBolusIngredient, series->getContrastBolusIngredient());
        CPPUNIT_ASSERT_EQUAL(contrastBolusIngredient, series->getByteValue(0x0018, 0x1048));
    }
}

//------------------------------------------------------------------------------

void SeriesTest::contrastBolusIngredientConcentrationTest()
{
    static const double contrastBolusIngredientConcentration = 123.45;

    {
        auto series = data::ImageSeries::New();
        series->setContrastBolusIngredientConcentration(contrastBolusIngredientConcentration);
        CPPUNIT_ASSERT_EQUAL(
            contrastBolusIngredientConcentration,
            *series->getContrastBolusIngredientConcentration()
        );
        CPPUNIT_ASSERT_DOUBLES_EQUAL(
            contrastBolusIngredientConcentration,
            std::stod(series->getByteValue(0x0018, 0x1049)),
            0.00001
        );
    }

    {
        auto series = data::ImageSeries::New();
        series->setByteValue(0x0018, 0x1049, std::to_string(contrastBolusIngredientConcentration));
        CPPUNIT_ASSERT_EQUAL(
            contrastBolusIngredientConcentration,
            *series->getContrastBolusIngredientConcentration()
        );
        CPPUNIT_ASSERT_DOUBLES_EQUAL(
            contrastBolusIngredientConcentration,
            std::stod(series->getByteValue(0x0018, 0x1049)),
            0.00001
        );
    }

    {
        // Test emtpy value
        auto series = data::ImageSeries::New();
        series->setContrastBolusIngredientConcentration();

        CPPUNIT_ASSERT(!series->getContrastBolusIngredientConcentration());
        CPPUNIT_ASSERT_EQUAL(std::string(), series->getByteValue(0x0018, 0x1049));
    }
}

//------------------------------------------------------------------------------

void SeriesTest::rowsTest()
{
    static const std::uint16_t rows = 256;
    static const std::string rows_bytes(reinterpret_cast<const char*>(&rows), sizeof(rows));

    {
        auto series = data::ImageSeries::New();
        series->setRows(rows);
        CPPUNIT_ASSERT_EQUAL(rows, series->getRows().value_or(0));
        CPPUNIT_ASSERT_EQUAL(rows_bytes, series->getByteValue(0x0028, 0x0010));
    }

    {
        auto series = data::ImageSeries::New();
        series->setByteValue(0x0028, 0x0010, rows_bytes);
        CPPUNIT_ASSERT_EQUAL(rows, series->getRows().value_or(0));
        CPPUNIT_ASSERT_EQUAL(rows_bytes, series->getByteValue(0x0028, 0x0010));
    }
}

//------------------------------------------------------------------------------

void SeriesTest::columnsTest()
{
    static const std::uint16_t columns = 256;
    static const std::string columns_bytes(reinterpret_cast<const char*>(&columns), sizeof(columns));

    {
        auto series = data::ImageSeries::New();
        series->setColumns(columns);
        CPPUNIT_ASSERT_EQUAL(columns, series->getColumns().value_or(0));
        CPPUNIT_ASSERT_EQUAL(columns_bytes, series->getByteValue(0x0028, 0x0011));
    }

    {
        auto series = data::ImageSeries::New();
        series->setByteValue(0x0028, 0x0011, columns_bytes);
        CPPUNIT_ASSERT_EQUAL(columns, series->getColumns().value_or(0));
        CPPUNIT_ASSERT_EQUAL(columns_bytes, series->getByteValue(0x0028, 0x0011));
    }
}

//------------------------------------------------------------------------------

void SeriesTest::windowCenterTest()
{
    static const double center = 333.33;

    {
        auto series = data::ImageSeries::New();
        series->setWindowCenter({center});
        CPPUNIT_ASSERT_EQUAL(center, series->getWindowCenter().front());
        CPPUNIT_ASSERT_EQUAL(center, std::stod(series->getByteValue(0x0028, 0x1050)));
    }

    {
        auto series = data::ImageSeries::New();
        series->setByteValue(0x0028, 0x1050, std::to_string(center));
        CPPUNIT_ASSERT_EQUAL(center, series->getWindowCenter().front());
        CPPUNIT_ASSERT_EQUAL(center, std::stod(series->getByteValue(0x0028, 0x1050)));
    }
}

//------------------------------------------------------------------------------

void SeriesTest::windowWidthTest()
{
    static const double width = 666.66;

    {
        auto series = data::ImageSeries::New();
        series->setWindowWidth({width});
        CPPUNIT_ASSERT_EQUAL(width, series->getWindowWidth().front());
        CPPUNIT_ASSERT_EQUAL(width, std::stod(series->getByteValue(0x0028, 0x1051)));
    }

    {
        auto series = data::ImageSeries::New();
        series->setByteValue(0x0028, 0x1051, std::to_string(width));
        CPPUNIT_ASSERT_EQUAL(width, series->getWindowWidth().front());
        CPPUNIT_ASSERT_EQUAL(width, std::stod(series->getByteValue(0x0028, 0x1051)));
    }
}

//------------------------------------------------------------------------------

void SeriesTest::imagePositionPatientTest()
{
    static const std::vector<double> position {111.111, 222.222, 333.333};

    {
        auto series = data::ImageSeries::New();
        series->setImagePositionPatient(position);
        CPPUNIT_ASSERT(position == series->getImagePositionPatient());

        const auto values = series->getByteValues(0x0020, 0x0032);
        const std::vector<double> actual {
            std::stod(values[0]),
            std::stod(values[1]),
            std::stod(values[2])
        };

        CPPUNIT_ASSERT(position == actual);
    }

    {
        auto series = data::ImageSeries::New();
        series->setByteValues(
            0x0020,
            0x0032,
            {
                std::to_string(position[0]),
                std::to_string(position[1]),
                std::to_string(position[2])
            });

        CPPUNIT_ASSERT(position == series->getImagePositionPatient());

        const auto values = series->getByteValues(0x0020, 0x0032);
        const std::vector<double> actual {
            std::stod(values[0]),
            std::stod(values[1]),
            std::stod(values[2])
        };

        CPPUNIT_ASSERT(position == actual);
    }
}

//------------------------------------------------------------------------------

void SeriesTest::imageOrientationPatientTest()
{
    static const std::vector<double> orientation {111.111, 222.222, 333.333, 444.444, 555.555, 666.666};

    {
        auto series = data::ImageSeries::New();
        series->setImageOrientationPatient(orientation);
        CPPUNIT_ASSERT(orientation == series->getImageOrientationPatient());

        const auto values = series->getByteValues(0x0020, 0x0037);
        const std::vector<double> actual {
            std::stod(values[0]),
            std::stod(values[1]),
            std::stod(values[2]),
            std::stod(values[3]),
            std::stod(values[4]),
            std::stod(values[5])
        };

        CPPUNIT_ASSERT(orientation == actual);
    }

    {
        auto series = data::ImageSeries::New();
        series->setByteValues(
            0x0020,
            0x0037,
            {
                std::to_string(orientation[0]),
                std::to_string(orientation[1]),
                std::to_string(orientation[2]),
                std::to_string(orientation[3]),
                std::to_string(orientation[4]),
                std::to_string(orientation[5])
            });

        CPPUNIT_ASSERT(orientation == series->getImageOrientationPatient());

        const auto values = series->getByteValues(0x0020, 0x0037);
        const std::vector<double> actual {
            std::stod(values[0]),
            std::stod(values[1]),
            std::stod(values[2]),
            std::stod(values[3]),
            std::stod(values[4]),
            std::stod(values[5])
        };

        CPPUNIT_ASSERT(orientation == actual);
    }
}

//------------------------------------------------------------------------------

void SeriesTest::imageTransformPatientTest()
{
    data::Matrix4 matrix = {
        0., 1., 0., 0.,
        1., 0., 0., 0.,
        0., 0., -1., 0.,
        0., 0., 0., 1.
    };

    {
        auto series = data::ImageSeries::New();
        series->setImageTransformPatient(matrix);
        CPPUNIT_ASSERT(matrix == series->getImageTransformPatient());
    }
}

//------------------------------------------------------------------------------

void SeriesTest::stringConversionTest()
{
    // Test binary to string and string to binary conversion for binary attributes

    // Unsigned Short (US)
    {
        static const uint16_t rows = 666;
        auto series                = data::ImageSeries::New();

        // Rows - "666" string (4) is bigger than short size (2)
        series->setStringValue(0x0018, 0x1049, std::to_string(rows));
        CPPUNIT_ASSERT_EQUAL(std::to_string(rows), series->getStringValue(0x0018, 0x1049));
    }
}

} //namespace sight::data::ut
