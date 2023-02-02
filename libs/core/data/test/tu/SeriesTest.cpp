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

#include "SeriesTest.hpp"

#include <core/tools/compare.hpp>
#include <core/tools/UUID.hpp>

#include <data/dicom/Attribute.hpp>

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

    series1->setSOPKeyword(dicom::sop::Keyword::CTImageStorage);
    CPPUNIT_ASSERT(*series1 != *series2 && !(*series1 == *series2));
    series2->setSOPKeyword(series1->getSOPKeyword());
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

    series1->setFrameAcquisitionDateTime("57");
    CPPUNIT_ASSERT(*series1 != *series2 && !(*series1 == *series2));
    series2->setFrameAcquisitionDateTime(series1->getFrameAcquisitionDateTime());
    CPPUNIT_ASSERT(*series1 == *series2 && !(*series1 != *series2));

    series1->setFrameComments("58");
    CPPUNIT_ASSERT(*series1 != *series2 && !(*series1 == *series2));
    series2->setFrameComments(series1->getFrameComments());
    CPPUNIT_ASSERT(*series1 == *series2 && !(*series1 != *series2));

    series1->setFrameLabel("59");
    CPPUNIT_ASSERT(*series1 != *series2 && !(*series1 == *series2));
    series2->setFrameLabel(series1->getFrameLabel());
    CPPUNIT_ASSERT(*series1 == *series2 && !(*series1 != *series2));

    // Test also deepcopy, just for fun
    auto series3 = data::Series::New();
    series3->deepCopy(series1);
    CPPUNIT_ASSERT(*series1 == *series3 && !(*series1 != *series3));
}

//------------------------------------------------------------------------------

void SeriesTest::sopClassUIDTest()
{
    static const dicom::sop::Keyword keyword = dicom::sop::Keyword::CTImageStorage;

    {
        auto series = data::Series::New();
        series->setSOPKeyword(keyword);
        CPPUNIT_ASSERT_EQUAL(keyword, series->getSOPKeyword());
        CPPUNIT_ASSERT_EQUAL(
            keyword,
            dicom::sop::get(series->getByteValue(dicom::attribute::Keyword::SOPClassUID)).m_keyword
        );
    }

    {
        auto series = data::Series::New();
        series->setByteValue(dicom::attribute::Keyword::SOPClassUID, std::string(dicom::sop::get(keyword).m_uid));
        CPPUNIT_ASSERT_EQUAL(keyword, series->getSOPKeyword());
        CPPUNIT_ASSERT_EQUAL(
            keyword,
            dicom::sop::get(series->getByteValue(dicom::attribute::Keyword::SOPClassUID)).m_keyword
        );
    }
}

//------------------------------------------------------------------------------

void SeriesTest::sopInstanceUIDTest()
{
    static const std::string sopInstanceUID(UUID::generateUUID());

    {
        auto series = data::Series::New();
        series->setSOPInstanceUID(sopInstanceUID);
        CPPUNIT_ASSERT_EQUAL(sopInstanceUID, series->getSOPInstanceUID());
        CPPUNIT_ASSERT_EQUAL(sopInstanceUID, series->getByteValue(data::dicom::attribute::Keyword::SOPInstanceUID));
    }

    {
        auto series = data::Series::New();
        series->setByteValue(data::dicom::attribute::Keyword::SOPInstanceUID, sopInstanceUID);
        CPPUNIT_ASSERT_EQUAL(sopInstanceUID, series->getSOPInstanceUID());
        CPPUNIT_ASSERT_EQUAL(sopInstanceUID, series->getByteValue(data::dicom::attribute::Keyword::SOPInstanceUID));
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
        CPPUNIT_ASSERT_EQUAL(
            specificCharacterSet,
            series->getByteValue(data::dicom::attribute::Keyword::SpecificCharacterSet)
        );
    }

    {
        auto series = data::Series::New();
        series->setByteValue(data::dicom::attribute::Keyword::SpecificCharacterSet, specificCharacterSet);
        CPPUNIT_ASSERT_EQUAL(specificCharacterSet, series->getSpecificCharacterSet());
        CPPUNIT_ASSERT_EQUAL(
            specificCharacterSet,
            series->getByteValue(data::dicom::attribute::Keyword::SpecificCharacterSet)
        );
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
        CPPUNIT_ASSERT_EQUAL(seriesDate, series->getByteValue(data::dicom::attribute::Keyword::SeriesDate));
    }

    {
        auto series = data::Series::New();
        series->setByteValue(data::dicom::attribute::Keyword::SeriesDate, seriesDate);
        CPPUNIT_ASSERT_EQUAL(seriesDate, series->getSeriesDate());
        CPPUNIT_ASSERT_EQUAL(seriesDate, series->getByteValue(data::dicom::attribute::Keyword::SeriesDate));
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
        CPPUNIT_ASSERT_EQUAL(seriesTime, series->getByteValue(data::dicom::attribute::Keyword::SeriesTime));
    }

    {
        auto series = data::Series::New();
        series->setByteValue(data::dicom::attribute::Keyword::SeriesTime, seriesTime);
        CPPUNIT_ASSERT_EQUAL(seriesTime, series->getSeriesTime());
        CPPUNIT_ASSERT_EQUAL(seriesTime, series->getByteValue(data::dicom::attribute::Keyword::SeriesTime));
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
        CPPUNIT_ASSERT_EQUAL(modality, series->getByteValue(data::dicom::attribute::Keyword::Modality));
    }

    {
        auto series = data::Series::New();
        series->setByteValue(data::dicom::attribute::Keyword::Modality, modality);
        CPPUNIT_ASSERT_EQUAL(modality, series->getModality());
        CPPUNIT_ASSERT_EQUAL(modality, series->getByteValue(data::dicom::attribute::Keyword::Modality));
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
        CPPUNIT_ASSERT_EQUAL(
            seriesDescription,
            series->getByteValue(data::dicom::attribute::Keyword::SeriesDescription)
        );
    }

    {
        auto series = data::Series::New();
        series->setByteValue(data::dicom::attribute::Keyword::SeriesDescription, seriesDescription);
        CPPUNIT_ASSERT_EQUAL(seriesDescription, series->getSeriesDescription());
        CPPUNIT_ASSERT_EQUAL(
            seriesDescription,
            series->getByteValue(data::dicom::attribute::Keyword::SeriesDescription)
        );
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
        CPPUNIT_ASSERT(
            performingPhysicianNames
            == series->getByteValues(data::dicom::attribute::Keyword::PerformingPhysicianName)
        );

        CPPUNIT_ASSERT_EQUAL(performingPhysicianName, series->getPerformingPhysicianName());
        CPPUNIT_ASSERT_EQUAL(
            performingPhysicianName,
            series->getByteValue(data::dicom::attribute::Keyword::PerformingPhysicianName)
        );
    }

    {
        auto series = data::Series::New();
        series->setByteValues(data::dicom::attribute::Keyword::PerformingPhysicianName, performingPhysicianNames);
        CPPUNIT_ASSERT(performingPhysicianNames == series->getPerformingPhysicianNames());
        CPPUNIT_ASSERT(
            performingPhysicianNames
            == series->getByteValues(data::dicom::attribute::Keyword::PerformingPhysicianName)
        );

        CPPUNIT_ASSERT_EQUAL(performingPhysicianName, series->getPerformingPhysicianName());
        CPPUNIT_ASSERT_EQUAL(
            performingPhysicianName,
            series->getByteValue(data::dicom::attribute::Keyword::PerformingPhysicianName)
        );
    }

    {
        auto series = data::Series::New();
        series->setPerformingPhysicianName(performingPhysicianName);
        CPPUNIT_ASSERT(performingPhysicianNames == series->getPerformingPhysicianNames());
        CPPUNIT_ASSERT(
            performingPhysicianNames
            == series->getByteValues(data::dicom::attribute::Keyword::PerformingPhysicianName)
        );

        CPPUNIT_ASSERT_EQUAL(performingPhysicianName, series->getPerformingPhysicianName());
        CPPUNIT_ASSERT_EQUAL(
            performingPhysicianName,
            series->getByteValue(data::dicom::attribute::Keyword::PerformingPhysicianName)
        );
    }

    {
        auto series = data::Series::New();
        series->setByteValue(data::dicom::attribute::Keyword::PerformingPhysicianName, performingPhysicianName);
        CPPUNIT_ASSERT(performingPhysicianNames == series->getPerformingPhysicianNames());
        CPPUNIT_ASSERT(
            performingPhysicianNames
            == series->getByteValues(data::dicom::attribute::Keyword::PerformingPhysicianName)
        );

        CPPUNIT_ASSERT_EQUAL(performingPhysicianName, series->getPerformingPhysicianName());
        CPPUNIT_ASSERT_EQUAL(
            performingPhysicianName,
            series->getByteValue(data::dicom::attribute::Keyword::PerformingPhysicianName)
        );
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
        CPPUNIT_ASSERT_EQUAL(
            anatomicalOrientationType,
            series->getByteValue(data::dicom::attribute::Keyword::AnatomicalOrientationType)
        );
    }

    {
        auto series = data::Series::New();
        series->setByteValue(data::dicom::attribute::Keyword::AnatomicalOrientationType, anatomicalOrientationType);
        CPPUNIT_ASSERT_EQUAL(anatomicalOrientationType, series->getAnatomicalOrientationType());
        CPPUNIT_ASSERT_EQUAL(
            anatomicalOrientationType,
            series->getByteValue(data::dicom::attribute::Keyword::AnatomicalOrientationType)
        );
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
        CPPUNIT_ASSERT_EQUAL(bodyPartExamined, series->getByteValue(data::dicom::attribute::Keyword::BodyPartExamined));
    }

    {
        auto series = data::Series::New();
        series->setByteValue(data::dicom::attribute::Keyword::BodyPartExamined, bodyPartExamined);
        CPPUNIT_ASSERT_EQUAL(bodyPartExamined, series->getBodyPartExamined());
        CPPUNIT_ASSERT_EQUAL(bodyPartExamined, series->getByteValue(data::dicom::attribute::Keyword::BodyPartExamined));
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
        CPPUNIT_ASSERT_EQUAL(protocolName, series->getByteValue(data::dicom::attribute::Keyword::ProtocolName));
    }

    {
        auto series = data::Series::New();
        series->setByteValue(data::dicom::attribute::Keyword::ProtocolName, protocolName);
        CPPUNIT_ASSERT_EQUAL(protocolName, series->getProtocolName());
        CPPUNIT_ASSERT_EQUAL(protocolName, series->getByteValue(data::dicom::attribute::Keyword::ProtocolName));
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
        CPPUNIT_ASSERT_EQUAL(patientPosition, series->getByteValue(data::dicom::attribute::Keyword::PatientPosition));
    }

    {
        auto series = data::Series::New();
        series->setByteValue(data::dicom::attribute::Keyword::PatientPosition, patientPosition);
        CPPUNIT_ASSERT_EQUAL(patientPosition, series->getPatientPosition());
        CPPUNIT_ASSERT_EQUAL(patientPosition, series->getByteValue(data::dicom::attribute::Keyword::PatientPosition));
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
        CPPUNIT_ASSERT_EQUAL(
            seriesInstanceUID,
            series->getByteValue(data::dicom::attribute::Keyword::SeriesInstanceUID)
        );
    }

    {
        auto series = data::Series::New();
        series->setByteValue(data::dicom::attribute::Keyword::SeriesInstanceUID, seriesInstanceUID);
        CPPUNIT_ASSERT_EQUAL(seriesInstanceUID, series->getSeriesInstanceUID());
        CPPUNIT_ASSERT_EQUAL(
            seriesInstanceUID,
            series->getByteValue(data::dicom::attribute::Keyword::SeriesInstanceUID)
        );
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
        CPPUNIT_ASSERT_EQUAL(
            std::to_string(seriesNumber),
            series->getByteValue(data::dicom::attribute::Keyword::SeriesNumber)
        );
    }

    {
        auto series = data::Series::New();
        series->setByteValue(data::dicom::attribute::Keyword::SeriesNumber, std::to_string(seriesNumber));
        CPPUNIT_ASSERT_EQUAL(seriesNumber, *series->getSeriesNumber());
        CPPUNIT_ASSERT_EQUAL(
            std::to_string(seriesNumber),
            series->getByteValue(data::dicom::attribute::Keyword::SeriesNumber)
        );
    }

    {
        // Test emtpy value
        auto series = data::Series::New();
        series->setSeriesNumber();

        CPPUNIT_ASSERT(!series->getSeriesNumber());
        CPPUNIT_ASSERT_EQUAL(std::string(), series->getByteValue(data::dicom::attribute::Keyword::SeriesNumber));
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
        CPPUNIT_ASSERT_EQUAL(laterality, series->getByteValue(data::dicom::attribute::Keyword::Laterality));
    }

    {
        auto series = data::Series::New();
        series->setByteValue(data::dicom::attribute::Keyword::Laterality, laterality);
        CPPUNIT_ASSERT_EQUAL(laterality, series->getLaterality());
        CPPUNIT_ASSERT_EQUAL(laterality, series->getByteValue(data::dicom::attribute::Keyword::Laterality));
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
        CPPUNIT_ASSERT_EQUAL(
            performedProcedureStepStartDate,
            series->getByteValue(data::dicom::attribute::Keyword::PerformedProcedureStepStartDate)
        );
    }

    {
        auto series = data::Series::New();
        series->setByteValue(
            data::dicom::attribute::Keyword::PerformedProcedureStepStartDate,
            performedProcedureStepStartDate
        );
        CPPUNIT_ASSERT_EQUAL(performedProcedureStepStartDate, series->getPerformedProcedureStepStartDate());
        CPPUNIT_ASSERT_EQUAL(
            performedProcedureStepStartDate,
            series->getByteValue(data::dicom::attribute::Keyword::PerformedProcedureStepStartDate)
        );
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
        CPPUNIT_ASSERT_EQUAL(
            performedProcedureStepStartTime,
            series->getByteValue(data::dicom::attribute::Keyword::PerformedProcedureStepStartTime)
        );
    }

    {
        auto series = data::Series::New();
        series->setByteValue(
            data::dicom::attribute::Keyword::PerformedProcedureStepStartTime,
            performedProcedureStepStartTime
        );
        CPPUNIT_ASSERT_EQUAL(performedProcedureStepStartTime, series->getPerformedProcedureStepStartTime());
        CPPUNIT_ASSERT_EQUAL(
            performedProcedureStepStartTime,
            series->getByteValue(data::dicom::attribute::Keyword::PerformedProcedureStepStartTime)
        );
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
        CPPUNIT_ASSERT_EQUAL(
            performedProcedureStepEndDate,
            series->getByteValue(data::dicom::attribute::Keyword::PerformedProcedureStepEndDate)
        );
    }

    {
        auto series = data::Series::New();
        series->setByteValue(
            data::dicom::attribute::Keyword::PerformedProcedureStepEndDate,
            performedProcedureStepEndDate
        );
        CPPUNIT_ASSERT_EQUAL(performedProcedureStepEndDate, series->getPerformedProcedureStepEndDate());
        CPPUNIT_ASSERT_EQUAL(
            performedProcedureStepEndDate,
            series->getByteValue(data::dicom::attribute::Keyword::PerformedProcedureStepEndDate)
        );
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
        CPPUNIT_ASSERT_EQUAL(
            performedProcedureStepEndTime,
            series->getByteValue(data::dicom::attribute::Keyword::PerformedProcedureStepEndTime)
        );
    }

    {
        auto series = data::Series::New();
        series->setByteValue(
            data::dicom::attribute::Keyword::PerformedProcedureStepEndTime,
            performedProcedureStepEndTime
        );
        CPPUNIT_ASSERT_EQUAL(performedProcedureStepEndTime, series->getPerformedProcedureStepEndTime());
        CPPUNIT_ASSERT_EQUAL(
            performedProcedureStepEndTime,
            series->getByteValue(data::dicom::attribute::Keyword::PerformedProcedureStepEndTime)
        );
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
        CPPUNIT_ASSERT_EQUAL(
            performedProcedureStepID,
            series->getByteValue(data::dicom::attribute::Keyword::PerformedProcedureStepID)
        );
    }

    {
        auto series = data::Series::New();
        series->setByteValue(data::dicom::attribute::Keyword::PerformedProcedureStepID, performedProcedureStepID);
        CPPUNIT_ASSERT_EQUAL(performedProcedureStepID, series->getPerformedProcedureStepID());
        CPPUNIT_ASSERT_EQUAL(
            performedProcedureStepID,
            series->getByteValue(data::dicom::attribute::Keyword::PerformedProcedureStepID)
        );
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
        CPPUNIT_ASSERT_EQUAL(
            performedProcedureStepDescription,
            series->getByteValue(data::dicom::attribute::Keyword::PerformedProcedureStepDescription)
        );
    }

    {
        auto series = data::Series::New();
        series->setByteValue(
            data::dicom::attribute::Keyword::PerformedProcedureStepDescription,
            performedProcedureStepDescription
        );
        CPPUNIT_ASSERT_EQUAL(performedProcedureStepDescription, series->getPerformedProcedureStepDescription());
        CPPUNIT_ASSERT_EQUAL(
            performedProcedureStepDescription,
            series->getByteValue(data::dicom::attribute::Keyword::PerformedProcedureStepDescription)
        );
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
        CPPUNIT_ASSERT_EQUAL(
            commentsOnThePerformedProcedureStep,
            series->getByteValue(data::dicom::attribute::Keyword::CommentsOnThePerformedProcedureStep)
        );
    }

    {
        auto series = data::Series::New();
        series->setByteValue(
            data::dicom::attribute::Keyword::CommentsOnThePerformedProcedureStep,
            commentsOnThePerformedProcedureStep
        );
        CPPUNIT_ASSERT_EQUAL(commentsOnThePerformedProcedureStep, series->getCommentsOnThePerformedProcedureStep());
        CPPUNIT_ASSERT_EQUAL(
            commentsOnThePerformedProcedureStep,
            series->getByteValue(data::dicom::attribute::Keyword::CommentsOnThePerformedProcedureStep)
        );
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
        CPPUNIT_ASSERT_EQUAL(institutionName, series->getByteValue(data::dicom::attribute::Keyword::InstitutionName));
    }

    {
        auto series = data::Series::New();
        series->setByteValue(data::dicom::attribute::Keyword::InstitutionName, institutionName);
        CPPUNIT_ASSERT_EQUAL(institutionName, series->getInstitutionName());
        CPPUNIT_ASSERT_EQUAL(institutionName, series->getByteValue(data::dicom::attribute::Keyword::InstitutionName));
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
        CPPUNIT_ASSERT_EQUAL(patientName, series->getByteValue(data::dicom::attribute::Keyword::PatientName));
    }

    {
        auto series = data::Series::New();
        series->setByteValue(data::dicom::attribute::Keyword::PatientName, patientName);
        CPPUNIT_ASSERT_EQUAL(patientName, series->getPatientName());
        CPPUNIT_ASSERT_EQUAL(patientName, series->getByteValue(data::dicom::attribute::Keyword::PatientName));
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
        CPPUNIT_ASSERT_EQUAL(patientID, series->getByteValue(data::dicom::attribute::Keyword::PatientID));
    }

    {
        auto series = data::Series::New();
        series->setByteValue(data::dicom::attribute::Keyword::PatientID, patientID);
        CPPUNIT_ASSERT_EQUAL(patientID, series->getPatientID());
        CPPUNIT_ASSERT_EQUAL(patientID, series->getByteValue(data::dicom::attribute::Keyword::PatientID));
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
        CPPUNIT_ASSERT_EQUAL(patientBirthDate, series->getByteValue(data::dicom::attribute::Keyword::PatientBirthDate));
    }

    {
        auto series = data::Series::New();
        series->setByteValue(data::dicom::attribute::Keyword::PatientBirthDate, patientBirthDate);
        CPPUNIT_ASSERT_EQUAL(patientBirthDate, series->getPatientBirthDate());
        CPPUNIT_ASSERT_EQUAL(patientBirthDate, series->getByteValue(data::dicom::attribute::Keyword::PatientBirthDate));
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
        CPPUNIT_ASSERT_EQUAL(patientSex, series->getByteValue(data::dicom::attribute::Keyword::PatientSex));
    }

    {
        auto series = data::Series::New();
        series->setByteValue(data::dicom::attribute::Keyword::PatientSex, patientSex);
        CPPUNIT_ASSERT_EQUAL(patientSex, series->getPatientSex());
        CPPUNIT_ASSERT_EQUAL(patientSex, series->getByteValue(data::dicom::attribute::Keyword::PatientSex));
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
        CPPUNIT_ASSERT_EQUAL(studyDate, series->getByteValue(data::dicom::attribute::Keyword::StudyDate));
    }

    {
        auto series = data::Series::New();
        series->setByteValue(data::dicom::attribute::Keyword::StudyDate, studyDate);
        CPPUNIT_ASSERT_EQUAL(studyDate, series->getStudyDate());
        CPPUNIT_ASSERT_EQUAL(studyDate, series->getByteValue(data::dicom::attribute::Keyword::StudyDate));
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
        CPPUNIT_ASSERT_EQUAL(studyTime, series->getByteValue(data::dicom::attribute::Keyword::StudyTime));
    }

    {
        auto series = data::Series::New();
        series->setByteValue(data::dicom::attribute::Keyword::StudyTime, studyTime);
        CPPUNIT_ASSERT_EQUAL(studyTime, series->getStudyTime());
        CPPUNIT_ASSERT_EQUAL(studyTime, series->getByteValue(data::dicom::attribute::Keyword::StudyTime));
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
        CPPUNIT_ASSERT_EQUAL(
            referringPhysicianName,
            series->getByteValue(data::dicom::attribute::Keyword::ReferringPhysicianName)
        );
    }

    {
        auto series = data::Series::New();
        series->setByteValue(data::dicom::attribute::Keyword::ReferringPhysicianName, referringPhysicianName);
        CPPUNIT_ASSERT_EQUAL(referringPhysicianName, series->getReferringPhysicianName());
        CPPUNIT_ASSERT_EQUAL(
            referringPhysicianName,
            series->getByteValue(data::dicom::attribute::Keyword::ReferringPhysicianName)
        );
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
        CPPUNIT_ASSERT_EQUAL(studyDescription, series->getByteValue(data::dicom::attribute::Keyword::StudyDescription));
    }

    {
        auto series = data::Series::New();
        series->setByteValue(data::dicom::attribute::Keyword::StudyDescription, studyDescription);
        CPPUNIT_ASSERT_EQUAL(studyDescription, series->getStudyDescription());
        CPPUNIT_ASSERT_EQUAL(studyDescription, series->getByteValue(data::dicom::attribute::Keyword::StudyDescription));
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
        CPPUNIT_ASSERT_EQUAL(studyInstanceUID, series->getByteValue(data::dicom::attribute::Keyword::StudyInstanceUID));
    }

    {
        auto series = data::Series::New();
        series->setByteValue(data::dicom::attribute::Keyword::StudyInstanceUID, studyInstanceUID);
        CPPUNIT_ASSERT_EQUAL(studyInstanceUID, series->getStudyInstanceUID());
        CPPUNIT_ASSERT_EQUAL(studyInstanceUID, series->getByteValue(data::dicom::attribute::Keyword::StudyInstanceUID));
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
        CPPUNIT_ASSERT_EQUAL(studyID, series->getByteValue(data::dicom::attribute::Keyword::StudyID));
    }

    {
        auto series = data::Series::New();
        series->setByteValue(data::dicom::attribute::Keyword::StudyID, studyID);
        CPPUNIT_ASSERT_EQUAL(studyID, series->getStudyID());
        CPPUNIT_ASSERT_EQUAL(studyID, series->getByteValue(data::dicom::attribute::Keyword::StudyID));
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
        CPPUNIT_ASSERT_EQUAL(patientAge, series->getByteValue(data::dicom::attribute::Keyword::PatientAge));
    }

    {
        auto series = data::Series::New();
        series->setByteValue(data::dicom::attribute::Keyword::PatientAge, patientAge);
        CPPUNIT_ASSERT_EQUAL(patientAge, series->getPatientAge());
        CPPUNIT_ASSERT_EQUAL(patientAge, series->getByteValue(data::dicom::attribute::Keyword::PatientAge));
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
        CPPUNIT_ASSERT_DOUBLES_EQUAL(
            patientSize,
            std::stod(series->getByteValue(data::dicom::attribute::Keyword::PatientSize)),
            0.00001
        );
    }

    {
        auto series = data::Series::New();
        series->setByteValue(data::dicom::attribute::Keyword::PatientSize, std::to_string(patientSize));
        CPPUNIT_ASSERT_EQUAL(patientSize, *series->getPatientSize());
        CPPUNIT_ASSERT_DOUBLES_EQUAL(
            patientSize,
            std::stod(series->getByteValue(data::dicom::attribute::Keyword::PatientSize)),
            0.00001
        );
    }

    {
        // Test emtpy value
        auto series = data::Series::New();
        series->setPatientSize();

        CPPUNIT_ASSERT(!series->getPatientSize());
        CPPUNIT_ASSERT_EQUAL(std::string(), series->getByteValue(data::dicom::attribute::Keyword::PatientSize));
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
        CPPUNIT_ASSERT_DOUBLES_EQUAL(
            patientWeight,
            std::stod(series->getByteValue(data::dicom::attribute::Keyword::PatientWeight)),
            0.00001
        );
    }

    {
        auto series = data::Series::New();
        series->setByteValue(data::dicom::attribute::Keyword::PatientWeight, std::to_string(patientWeight));
        CPPUNIT_ASSERT_EQUAL(patientWeight, *series->getPatientWeight());
        CPPUNIT_ASSERT_DOUBLES_EQUAL(
            patientWeight,
            std::stod(series->getByteValue(data::dicom::attribute::Keyword::PatientWeight)),
            0.00001
        );
    }

    {
        // Test emtpy value
        auto series = data::Series::New();
        series->setPatientWeight();

        CPPUNIT_ASSERT(!series->getPatientWeight());
        CPPUNIT_ASSERT_EQUAL(std::string(), series->getByteValue(data::dicom::attribute::Keyword::PatientWeight));
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
        CPPUNIT_ASSERT_EQUAL(acquisitionDate, series->getByteValue(data::dicom::attribute::Keyword::AcquisitionDate));
    }

    {
        auto series = data::ImageSeries::New();
        series->setByteValue(data::dicom::attribute::Keyword::AcquisitionDate, acquisitionDate);
        CPPUNIT_ASSERT_EQUAL(acquisitionDate, series->getAcquisitionDate());
        CPPUNIT_ASSERT_EQUAL(acquisitionDate, series->getByteValue(data::dicom::attribute::Keyword::AcquisitionDate));
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
        CPPUNIT_ASSERT_EQUAL(acquisitionTime, series->getByteValue(data::dicom::attribute::Keyword::AcquisitionTime));
    }

    {
        auto series = data::ImageSeries::New();
        series->setByteValue(data::dicom::attribute::Keyword::AcquisitionTime, acquisitionTime);
        CPPUNIT_ASSERT_EQUAL(acquisitionTime, series->getAcquisitionTime());
        CPPUNIT_ASSERT_EQUAL(acquisitionTime, series->getByteValue(data::dicom::attribute::Keyword::AcquisitionTime));
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
        CPPUNIT_ASSERT_EQUAL(
            std::to_string(acquisitionNumber),
            series->getByteValue(data::dicom::attribute::Keyword::AcquisitionNumber)
        );
    }

    {
        auto series = data::ImageSeries::New();
        series->setByteValue(data::dicom::attribute::Keyword::AcquisitionNumber, std::to_string(acquisitionNumber));
        CPPUNIT_ASSERT_EQUAL(acquisitionNumber, series->getAcquisitionNumber().value_or(0));
        CPPUNIT_ASSERT_EQUAL(
            std::to_string(acquisitionNumber),
            series->getByteValue(data::dicom::attribute::Keyword::AcquisitionNumber)
        );
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
        CPPUNIT_ASSERT_EQUAL(
            std::to_string(instanceNumber),
            series->getByteValue(data::dicom::attribute::Keyword::InstanceNumber)
        );
    }

    {
        auto series = data::ImageSeries::New();
        series->setByteValue(data::dicom::attribute::Keyword::InstanceNumber, std::to_string(instanceNumber));
        CPPUNIT_ASSERT_EQUAL(instanceNumber, series->getInstanceNumber().value_or(0));
        CPPUNIT_ASSERT_EQUAL(
            std::to_string(instanceNumber),
            series->getByteValue(data::dicom::attribute::Keyword::InstanceNumber)
        );
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
        CPPUNIT_ASSERT_EQUAL(contentTime, series->getByteValue(data::dicom::attribute::Keyword::ContentTime));
    }

    {
        auto series = data::ImageSeries::New();
        series->setByteValue(data::dicom::attribute::Keyword::ContentTime, contentTime);
        CPPUNIT_ASSERT_EQUAL(contentTime, series->getContentTime());
        CPPUNIT_ASSERT_EQUAL(contentTime, series->getByteValue(data::dicom::attribute::Keyword::ContentTime));
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
        CPPUNIT_ASSERT_EQUAL(
            contrastBolusAgent,
            series->getByteValue(data::dicom::attribute::Keyword::ContrastBolusAgent)
        );
    }

    {
        auto series = data::ImageSeries::New();
        series->setByteValue(data::dicom::attribute::Keyword::ContrastBolusAgent, contrastBolusAgent);
        CPPUNIT_ASSERT_EQUAL(contrastBolusAgent, series->getContrastBolusAgent());
        CPPUNIT_ASSERT_EQUAL(
            contrastBolusAgent,
            series->getByteValue(data::dicom::attribute::Keyword::ContrastBolusAgent)
        );
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
        CPPUNIT_ASSERT_EQUAL(
            contrastBolusRoute,
            series->getByteValue(data::dicom::attribute::Keyword::ContrastBolusRoute)
        );
    }

    {
        auto series = data::ImageSeries::New();
        series->setByteValue(data::dicom::attribute::Keyword::ContrastBolusRoute, contrastBolusRoute);
        CPPUNIT_ASSERT_EQUAL(contrastBolusRoute, series->getContrastBolusRoute());
        CPPUNIT_ASSERT_EQUAL(
            contrastBolusRoute,
            series->getByteValue(data::dicom::attribute::Keyword::ContrastBolusRoute)
        );
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
        CPPUNIT_ASSERT_DOUBLES_EQUAL(
            contrastBolusVolume,
            std::stod(series->getByteValue(data::dicom::attribute::Keyword::ContrastBolusVolume)),
            0.00001
        );
    }

    {
        auto series = data::ImageSeries::New();
        series->setByteValue(data::dicom::attribute::Keyword::ContrastBolusVolume, std::to_string(contrastBolusVolume));
        CPPUNIT_ASSERT_EQUAL(contrastBolusVolume, *series->getContrastBolusVolume());
        CPPUNIT_ASSERT_DOUBLES_EQUAL(
            contrastBolusVolume,
            std::stod(series->getByteValue(data::dicom::attribute::Keyword::ContrastBolusVolume)),
            0.00001
        );
    }

    {
        // Test emtpy value
        auto series = data::ImageSeries::New();
        series->setContrastBolusVolume();

        CPPUNIT_ASSERT(!series->getContrastBolusVolume());
        CPPUNIT_ASSERT_EQUAL(std::string(), series->getByteValue(data::dicom::attribute::Keyword::ContrastBolusVolume));
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
        CPPUNIT_ASSERT_EQUAL(
            contrastBolusStartTime,
            series->getByteValue(data::dicom::attribute::Keyword::ContrastBolusStartTime)
        );
    }

    {
        auto series = data::ImageSeries::New();
        series->setByteValue(data::dicom::attribute::Keyword::ContrastBolusStartTime, contrastBolusStartTime);
        CPPUNIT_ASSERT_EQUAL(contrastBolusStartTime, series->getContrastBolusStartTime());
        CPPUNIT_ASSERT_EQUAL(
            contrastBolusStartTime,
            series->getByteValue(data::dicom::attribute::Keyword::ContrastBolusStartTime)
        );
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
        CPPUNIT_ASSERT_EQUAL(
            contrastBolusStopTime,
            series->getByteValue(data::dicom::attribute::Keyword::ContrastBolusStopTime)
        );
    }

    {
        auto series = data::ImageSeries::New();
        series->setByteValue(data::dicom::attribute::Keyword::ContrastBolusStopTime, contrastBolusStopTime);
        CPPUNIT_ASSERT_EQUAL(contrastBolusStopTime, series->getContrastBolusStopTime());
        CPPUNIT_ASSERT_EQUAL(
            contrastBolusStopTime,
            series->getByteValue(data::dicom::attribute::Keyword::ContrastBolusStopTime)
        );
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
            std::stod(series->getByteValue(data::dicom::attribute::Keyword::ContrastBolusTotalDose)),
            0.00001
        );
    }

    {
        auto series = data::ImageSeries::New();
        series->setByteValue(
            data::dicom::attribute::Keyword::ContrastBolusTotalDose,
            std::to_string(contrastBolusTotalDose)
        );
        CPPUNIT_ASSERT_EQUAL(contrastBolusTotalDose, *series->getContrastBolusTotalDose());
        CPPUNIT_ASSERT_DOUBLES_EQUAL(
            contrastBolusTotalDose,
            std::stod(series->getByteValue(data::dicom::attribute::Keyword::ContrastBolusTotalDose)),
            0.00001
        );
    }

    {
        // Test emtpy value
        auto series = data::ImageSeries::New();
        series->setContrastBolusTotalDose();

        CPPUNIT_ASSERT(!series->getContrastBolusTotalDose());
        CPPUNIT_ASSERT_EQUAL(
            std::string(),
            series->getByteValue(data::dicom::attribute::Keyword::ContrastBolusTotalDose)
        );
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
        CPPUNIT_ASSERT(
            contrastFlowRateStrings
            == series->getByteValues(data::dicom::attribute::Keyword::ContrastFlowRate)
        );

        CPPUNIT_ASSERT_EQUAL(contrastFlowRate, series->getContrastFlowRate());
        CPPUNIT_ASSERT_EQUAL(contrastFlowRate, series->getByteValue(data::dicom::attribute::Keyword::ContrastFlowRate));
    }

    {
        auto series = data::ImageSeries::New();
        series->setByteValues(data::dicom::attribute::Keyword::ContrastFlowRate, contrastFlowRateStrings);
        CPPUNIT_ASSERT(contrastFlowRates == series->getContrastFlowRates());
        CPPUNIT_ASSERT(
            contrastFlowRateStrings
            == series->getByteValues(data::dicom::attribute::Keyword::ContrastFlowRate)
        );

        CPPUNIT_ASSERT_EQUAL(contrastFlowRate, series->getContrastFlowRate());
        CPPUNIT_ASSERT_EQUAL(contrastFlowRate, series->getByteValue(data::dicom::attribute::Keyword::ContrastFlowRate));
    }

    {
        auto series = data::ImageSeries::New();
        series->setContrastFlowRate(contrastFlowRate);
        CPPUNIT_ASSERT(contrastFlowRates == series->getContrastFlowRates());
        CPPUNIT_ASSERT(
            contrastFlowRateStrings
            == series->getByteValues(data::dicom::attribute::Keyword::ContrastFlowRate)
        );

        CPPUNIT_ASSERT_EQUAL(contrastFlowRate, series->getContrastFlowRate());
        CPPUNIT_ASSERT_EQUAL(contrastFlowRate, series->getByteValue(data::dicom::attribute::Keyword::ContrastFlowRate));
    }

    {
        auto series = data::ImageSeries::New();
        series->setByteValue(data::dicom::attribute::Keyword::ContrastFlowRate, contrastFlowRate);
        CPPUNIT_ASSERT(contrastFlowRates == series->getContrastFlowRates());
        CPPUNIT_ASSERT(
            contrastFlowRateStrings
            == series->getByteValues(data::dicom::attribute::Keyword::ContrastFlowRate)
        );

        CPPUNIT_ASSERT_EQUAL(contrastFlowRate, series->getContrastFlowRate());
        CPPUNIT_ASSERT_EQUAL(contrastFlowRate, series->getByteValue(data::dicom::attribute::Keyword::ContrastFlowRate));
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
        CPPUNIT_ASSERT(
            contrastFlowDurationStrings
            == series->getByteValues(data::dicom::attribute::Keyword::ContrastFlowDuration)
        );

        CPPUNIT_ASSERT_EQUAL(contrastFlowDuration, series->getContrastFlowDuration());
        CPPUNIT_ASSERT_EQUAL(
            contrastFlowDuration,
            series->getByteValue(data::dicom::attribute::Keyword::ContrastFlowDuration)
        );
    }

    {
        auto series = data::ImageSeries::New();
        series->setByteValues(data::dicom::attribute::Keyword::ContrastFlowDuration, contrastFlowDurationStrings);
        CPPUNIT_ASSERT(contrastFlowDurations == series->getContrastFlowDurations());
        CPPUNIT_ASSERT(
            contrastFlowDurationStrings
            == series->getByteValues(data::dicom::attribute::Keyword::ContrastFlowDuration)
        );

        CPPUNIT_ASSERT_EQUAL(contrastFlowDuration, series->getContrastFlowDuration());
        CPPUNIT_ASSERT_EQUAL(
            contrastFlowDuration,
            series->getByteValue(data::dicom::attribute::Keyword::ContrastFlowDuration)
        );
    }

    {
        auto series = data::ImageSeries::New();
        series->setContrastFlowDuration(contrastFlowDuration);
        CPPUNIT_ASSERT(contrastFlowDurations == series->getContrastFlowDurations());
        CPPUNIT_ASSERT(
            contrastFlowDurationStrings
            == series->getByteValues(data::dicom::attribute::Keyword::ContrastFlowDuration)
        );

        CPPUNIT_ASSERT_EQUAL(contrastFlowDuration, series->getContrastFlowDuration());
        CPPUNIT_ASSERT_EQUAL(
            contrastFlowDuration,
            series->getByteValue(data::dicom::attribute::Keyword::ContrastFlowDuration)
        );
    }

    {
        auto series = data::ImageSeries::New();
        series->setByteValue(data::dicom::attribute::Keyword::ContrastFlowDuration, contrastFlowDuration);
        CPPUNIT_ASSERT(contrastFlowDurations == series->getContrastFlowDurations());
        CPPUNIT_ASSERT(
            contrastFlowDurationStrings
            == series->getByteValues(data::dicom::attribute::Keyword::ContrastFlowDuration)
        );

        CPPUNIT_ASSERT_EQUAL(contrastFlowDuration, series->getContrastFlowDuration());
        CPPUNIT_ASSERT_EQUAL(
            contrastFlowDuration,
            series->getByteValue(data::dicom::attribute::Keyword::ContrastFlowDuration)
        );
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
        CPPUNIT_ASSERT_EQUAL(
            contrastBolusIngredient,
            series->getByteValue(data::dicom::attribute::Keyword::ContrastBolusIngredient)
        );
    }

    {
        auto series = data::ImageSeries::New();
        series->setByteValue(data::dicom::attribute::Keyword::ContrastBolusIngredient, contrastBolusIngredient);
        CPPUNIT_ASSERT_EQUAL(contrastBolusIngredient, series->getContrastBolusIngredient());
        CPPUNIT_ASSERT_EQUAL(
            contrastBolusIngredient,
            series->getByteValue(data::dicom::attribute::Keyword::ContrastBolusIngredient)
        );
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
            std::stod(series->getByteValue(data::dicom::attribute::Keyword::ContrastBolusIngredientConcentration)),
            0.00001
        );
    }

    {
        auto series = data::ImageSeries::New();
        series->setByteValue(
            data::dicom::attribute::Keyword::ContrastBolusIngredientConcentration,
            std::to_string(contrastBolusIngredientConcentration)
        );
        CPPUNIT_ASSERT_EQUAL(
            contrastBolusIngredientConcentration,
            *series->getContrastBolusIngredientConcentration()
        );
        CPPUNIT_ASSERT_DOUBLES_EQUAL(
            contrastBolusIngredientConcentration,
            std::stod(series->getByteValue(data::dicom::attribute::Keyword::ContrastBolusIngredientConcentration)),
            0.00001
        );
    }

    {
        // Test emtpy value
        auto series = data::ImageSeries::New();
        series->setContrastBolusIngredientConcentration();

        CPPUNIT_ASSERT(!series->getContrastBolusIngredientConcentration());
        CPPUNIT_ASSERT_EQUAL(
            std::string(),
            series->getByteValue(data::dicom::attribute::Keyword::ContrastBolusIngredientConcentration)
        );
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
        CPPUNIT_ASSERT_EQUAL(rows_bytes, series->getByteValue(data::dicom::attribute::Keyword::Rows));
    }

    {
        auto series = data::ImageSeries::New();
        series->setByteValue(data::dicom::attribute::Keyword::Rows, rows_bytes);
        CPPUNIT_ASSERT_EQUAL(rows, series->getRows().value_or(0));
        CPPUNIT_ASSERT_EQUAL(rows_bytes, series->getByteValue(data::dicom::attribute::Keyword::Rows));
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
        CPPUNIT_ASSERT_EQUAL(columns_bytes, series->getByteValue(data::dicom::attribute::Keyword::Columns));
    }

    {
        auto series = data::ImageSeries::New();
        series->setByteValue(data::dicom::attribute::Keyword::Columns, columns_bytes);
        CPPUNIT_ASSERT_EQUAL(columns, series->getColumns().value_or(0));
        CPPUNIT_ASSERT_EQUAL(columns_bytes, series->getByteValue(data::dicom::attribute::Keyword::Columns));
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
        CPPUNIT_ASSERT_EQUAL(center, std::stod(series->getByteValue(data::dicom::attribute::Keyword::WindowCenter)));
    }

    {
        auto series = data::ImageSeries::New();
        series->setByteValue(data::dicom::attribute::Keyword::WindowCenter, std::to_string(center));
        CPPUNIT_ASSERT_EQUAL(center, series->getWindowCenter().front());
        CPPUNIT_ASSERT_EQUAL(center, std::stod(series->getByteValue(data::dicom::attribute::Keyword::WindowCenter)));
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
        CPPUNIT_ASSERT_EQUAL(width, std::stod(series->getByteValue(data::dicom::attribute::Keyword::WindowWidth)));
    }

    {
        auto series = data::ImageSeries::New();
        series->setByteValue(data::dicom::attribute::Keyword::WindowWidth, std::to_string(width));
        CPPUNIT_ASSERT_EQUAL(width, series->getWindowWidth().front());
        CPPUNIT_ASSERT_EQUAL(width, std::stod(series->getByteValue(data::dicom::attribute::Keyword::WindowWidth)));
    }
}

//------------------------------------------------------------------------------

void SeriesTest::imagePositionPatientTest()
{
    static const std::vector<double> position {111.111, 222.222, 333.333};

    {
        auto series = data::ImageSeries::New();

        // We need to set the SOP class UID to let Sight know how to store the image position patient
        series->setSOPKeyword(data::dicom::sop::Keyword::CTImageStorage);

        series->setImagePositionPatient(position);
        CPPUNIT_ASSERT(position == series->getImagePositionPatient());

        const auto values = series->getByteValues(data::dicom::attribute::Keyword::ImagePositionPatient);
        const std::vector<double> actual {
            std::stod(values[0]),
            std::stod(values[1]),
            std::stod(values[2])
        };

        CPPUNIT_ASSERT(position == actual);
    }

    {
        auto series = data::ImageSeries::New();

        // We need to set the SOP class UID to let Sight know how to read the image position patient
        series->setSOPKeyword(data::dicom::sop::Keyword::CTImageStorage);

        series->setByteValues(
            data::dicom::attribute::Keyword::ImagePositionPatient,
            {
                std::to_string(position[0]),
                std::to_string(position[1]),
                std::to_string(position[2])
            });

        CPPUNIT_ASSERT(position == series->getImagePositionPatient());

        const auto values = series->getByteValues(data::dicom::attribute::Keyword::ImagePositionPatient);
        const std::vector<double> actual {
            std::stod(values[0]),
            std::stod(values[1]),
            std::stod(values[2])
        };

        CPPUNIT_ASSERT(position == actual);
    }

    {
        auto series = data::ImageSeries::New();

        // We need to set the SOP class UID to let Sight know how to read the image position patient
        series->setSOPKeyword(data::dicom::sop::Keyword::EnhancedUSVolumeStorage);

        // test Multi-Frame image
        series->setImagePositionPatient(position);
        CPPUNIT_ASSERT(position == series->getImagePositionPatient());
    }
}

//------------------------------------------------------------------------------

void SeriesTest::imageOrientationPatientTest()
{
    static const std::vector<double> orientation {111.111, 222.222, 333.333, 444.444, 555.555, 666.666};

    {
        auto series = data::ImageSeries::New();

        // We need to set the SOP class UID to let Sight know how to store the image orientation patient
        series->setSOPKeyword(data::dicom::sop::Keyword::CTImageStorage);

        series->setImageOrientationPatient(orientation);
        CPPUNIT_ASSERT(orientation == series->getImageOrientationPatient());

        const auto values = series->getByteValues(data::dicom::attribute::Keyword::ImageOrientationPatient);
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

        // We need to set the SOP class UID to let Sight know how to read the image orientation patient
        series->setSOPKeyword(data::dicom::sop::Keyword::CTImageStorage);

        series->setByteValues(
            data::dicom::attribute::Keyword::ImageOrientationPatient,
            {
                std::to_string(orientation[0]),
                std::to_string(orientation[1]),
                std::to_string(orientation[2]),
                std::to_string(orientation[3]),
                std::to_string(orientation[4]),
                std::to_string(orientation[5])
            });

        CPPUNIT_ASSERT(orientation == series->getImageOrientationPatient());

        const auto values = series->getByteValues(data::dicom::attribute::Keyword::ImageOrientationPatient);
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

        // We need to set the SOP class UID to let Sight know how to read the image orientation patient
        series->setSOPKeyword(data::dicom::sop::Keyword::EnhancedUSVolumeStorage);

        // test Multi-Frame image
        series->setImageOrientationPatient(orientation);
        CPPUNIT_ASSERT(orientation == series->getImageOrientationPatient());
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

        // We need to set the SOP class UID to let Sight know how to read the image orientation/position patient
        series->setSOPKeyword(data::dicom::sop::Keyword::CTImageStorage);

        series->setImageTransformPatient(matrix);
        CPPUNIT_ASSERT(matrix == series->getImageTransformPatient());
    }
}

//------------------------------------------------------------------------------

void SeriesTest::frameAcquisitionDateTimeTest()
{
    // DICOM DT format is  "YYYYMMDDHHMMSS.FFFFFF"
    static const std::string expected_frameAcquisitionDateTime0("20221026150703.000000");
    static const std::string expected_frameAcquisitionDateTime1("20221026150703.000001");
    static const std::string expected_frameAcquisitionDateTime2("20221026150703.000002");

    {
        auto series = data::ImageSeries::New();

        series->setFrameAcquisitionDateTime(expected_frameAcquisitionDateTime0, 0);
        series->setFrameAcquisitionDateTime(expected_frameAcquisitionDateTime1, 1);
        series->setFrameAcquisitionDateTime(expected_frameAcquisitionDateTime2, 2);

        const std::string actual_frameAcquisitionDateTime0(*(series->getFrameAcquisitionDateTime(0)));
        const std::string actual_frameAcquisitionDateTime1(*(series->getFrameAcquisitionDateTime(1)));
        const std::string actual_frameAcquisitionDateTime2(*(series->getFrameAcquisitionDateTime(2)));

        CPPUNIT_ASSERT_EQUAL(expected_frameAcquisitionDateTime0, actual_frameAcquisitionDateTime0);
        CPPUNIT_ASSERT_EQUAL(expected_frameAcquisitionDateTime1, actual_frameAcquisitionDateTime1);
        CPPUNIT_ASSERT_EQUAL(expected_frameAcquisitionDateTime2, actual_frameAcquisitionDateTime2);
    }
}

//------------------------------------------------------------------------------

void SeriesTest::frameCommentsTest()
{
    static const std::string expected_frameComments0("Comments0");
    static const std::string expected_frameComments1("Comments1");
    static const std::string expected_frameComments2("Comments2");

    {
        auto series = data::ImageSeries::New();

        series->setFrameComments(expected_frameComments0, 0);
        series->setFrameComments(expected_frameComments1, 1);
        series->setFrameComments(expected_frameComments2, 2);

        const std::string actual_frameComments0(*(series->getFrameComments(0)));
        const std::string actual_frameComments1(*(series->getFrameComments(1)));
        const std::string actual_frameComments2(*(series->getFrameComments(2)));

        CPPUNIT_ASSERT_EQUAL(expected_frameComments0, actual_frameComments0);
        CPPUNIT_ASSERT_EQUAL(expected_frameComments1, actual_frameComments1);
        CPPUNIT_ASSERT_EQUAL(expected_frameComments2, actual_frameComments2);
    }
}

//------------------------------------------------------------------------------

void SeriesTest::frameLabelTest()
{
    static const std::string expected_frameLabel0("Label0");
    static const std::string expected_frameLabel1("Label1");
    static const std::string expected_frameLabel2("Label2");

    {
        auto series = data::ImageSeries::New();

        series->setFrameLabel(expected_frameLabel0, 0);
        series->setFrameLabel(expected_frameLabel1, 1);
        series->setFrameLabel(expected_frameLabel2, 2);

        const std::string actual_frameLabel0(*(series->getFrameLabel(0)));
        const std::string actual_frameLabel1(*(series->getFrameLabel(1)));
        const std::string actual_frameLabel2(*(series->getFrameLabel(2)));

        CPPUNIT_ASSERT_EQUAL(expected_frameLabel0, actual_frameLabel0);
        CPPUNIT_ASSERT_EQUAL(expected_frameLabel1, actual_frameLabel1);
        CPPUNIT_ASSERT_EQUAL(expected_frameLabel2, actual_frameLabel2);
    }
}

//------------------------------------------------------------------------------

void SeriesTest::frameAcquisitionTimePointTest()
{
    // DICOM DT format is  "YYYYMMDDHHMMSS.FFFFFF"
    static const std::string expected_0("20221026150703.000000");
    static const std::string expected_1("20221026150703.000001");
    static const std::string expected_2("20221026150703.000002");
    static const std::string expected_3("2023");
    static const std::string expected_3b("20230101000000.000000");
    static const std::string expected_4("20221026150703.100000");
    static const std::string expected_5("20221026150703");
    static const std::string expected_5b("20221026150703.000000");

    auto series = data::ImageSeries::New();

    series->setFrameAcquisitionDateTime(expected_0, 0);
    series->setFrameAcquisitionDateTime(expected_1, 1);
    series->setFrameAcquisitionDateTime(expected_2, 2);
    series->setFrameAcquisitionDateTime(expected_3, 3);
    series->setFrameAcquisitionDateTime(expected_4, 4);
    series->setFrameAcquisitionDateTime(expected_5, 5);

    const auto timePoint0 = *series->getFrameAcquisitionTimePoint(0);
    const auto timePoint1 = *series->getFrameAcquisitionTimePoint(1);
    const auto timePoint2 = *series->getFrameAcquisitionTimePoint(2);
    const auto timePoint3 = *series->getFrameAcquisitionTimePoint(3);
    const auto timePoint4 = *series->getFrameAcquisitionTimePoint(4);
    const auto timePoint5 = *series->getFrameAcquisitionTimePoint(5);

    CPPUNIT_ASSERT_EQUAL(expected_0, data::Series::timePointToDateTime(timePoint0));
    CPPUNIT_ASSERT_EQUAL(expected_1, data::Series::timePointToDateTime(timePoint1));
    CPPUNIT_ASSERT_EQUAL(expected_2, data::Series::timePointToDateTime(timePoint2));
    CPPUNIT_ASSERT_EQUAL(expected_4, data::Series::timePointToDateTime(timePoint4));

    // Using time point API will force "YYYYMMDDHHMMSS.FFFFFF" format
    CPPUNIT_ASSERT_EQUAL(expected_3b, data::Series::timePointToDateTime(timePoint3));
    CPPUNIT_ASSERT_EQUAL(expected_5b, data::Series::timePointToDateTime(timePoint5));

    // Test setting time point directly, with overwrite
    series->setFrameAcquisitionTimePoint(timePoint1, 0);
    series->setFrameAcquisitionTimePoint(timePoint2, 1);
    series->setFrameAcquisitionTimePoint(timePoint3, 2);
    series->setFrameAcquisitionTimePoint(timePoint4, 3);
    series->setFrameAcquisitionTimePoint(timePoint5, 4);
    series->setFrameAcquisitionTimePoint(timePoint0, 5);

    const std::string actual_1(*(series->getFrameAcquisitionDateTime(0)));
    const std::string actual_2(*(series->getFrameAcquisitionDateTime(1)));
    const std::string actual_3(*(series->getFrameAcquisitionDateTime(2)));
    const std::string actual_4(*(series->getFrameAcquisitionDateTime(3)));
    const std::string actual_5(*(series->getFrameAcquisitionDateTime(4)));
    const std::string actual_0(*(series->getFrameAcquisitionDateTime(5)));

    CPPUNIT_ASSERT_EQUAL(expected_0, actual_0);
    CPPUNIT_ASSERT_EQUAL(expected_1, actual_1);
    CPPUNIT_ASSERT_EQUAL(expected_2, actual_2);
    CPPUNIT_ASSERT_EQUAL(expected_4, actual_4);

    // Using time point API will force "YYYYMMDDHHMMSS.FFFFFF" format
    CPPUNIT_ASSERT_EQUAL(expected_3b, actual_3);
    CPPUNIT_ASSERT_EQUAL(expected_5b, actual_5);

    // Test DeepCopy
    {
        auto destination      = data::ImageSeries::New();
        auto destination_lock = destination->dump_lock();

        {
            auto source      = data::ImageSeries::New();
            auto source_lock = source->dump_lock();

            source->setFrameAcquisitionDateTime(expected_0, 0);
            source->setFrameAcquisitionDateTime(expected_1, 1);
            source->setFrameAcquisitionDateTime(expected_2, 2);
            source->setFrameAcquisitionDateTime(expected_3, 3);
            source->setFrameAcquisitionDateTime(expected_4, 4);
            source->setFrameAcquisitionDateTime(expected_5, 5);

            destination->deepCopy(source);

            // This should not change destination
            source->setFrameAcquisitionDateTime(expected_5, 0);
            source->setFrameAcquisitionDateTime(expected_0, 1);
            source->setFrameAcquisitionDateTime(expected_1, 2);
            source->setFrameAcquisitionDateTime(expected_2, 3);
            source->setFrameAcquisitionDateTime(expected_3, 4);
            source->setFrameAcquisitionDateTime(expected_4, 5);
        }

        const std::string destination_0(*(destination->getFrameAcquisitionDateTime(0)));
        const std::string destination_1(*(destination->getFrameAcquisitionDateTime(1)));
        const std::string destination_2(*(destination->getFrameAcquisitionDateTime(2)));
        const std::string destination_3(*(destination->getFrameAcquisitionDateTime(3)));
        const std::string destination_4(*(destination->getFrameAcquisitionDateTime(4)));
        const std::string destination_5(*(destination->getFrameAcquisitionDateTime(5)));

        CPPUNIT_ASSERT_EQUAL(expected_0, destination_0);
        CPPUNIT_ASSERT_EQUAL(expected_1, destination_1);
        CPPUNIT_ASSERT_EQUAL(expected_2, destination_2);
        CPPUNIT_ASSERT_EQUAL(expected_4, destination_4);

        // Using time point API will force "YYYYMMDDHHMMSS.FFFFFF" format
        CPPUNIT_ASSERT_EQUAL(expected_3b, actual_3);
        CPPUNIT_ASSERT_EQUAL(expected_5b, actual_5);
    }

    // test shrinking
    {
        auto destination      = data::ImageSeries::New();
        auto destination_lock = destination->dump_lock();

        {
            auto source      = data::ImageSeries::New();
            auto source_lock = source->dump_lock();

            source->setFrameAcquisitionDateTime(expected_0, 0);
            source->setFrameAcquisitionDateTime(expected_1, 1);
            source->setFrameAcquisitionDateTime(expected_2, 2);

            destination->deepCopy(source);
        }

        CPPUNIT_ASSERT_EQUAL(expected_0, *destination->getFrameAcquisitionDateTime(0));
        CPPUNIT_ASSERT_EQUAL(expected_1, *destination->getFrameAcquisitionDateTime(1));
        CPPUNIT_ASSERT_EQUAL(expected_2, *destination->getFrameAcquisitionDateTime(2));

        // Now resize the destination to 2 elements
        destination->shrinkFrames(2);

        CPPUNIT_ASSERT_EQUAL(expected_0, *destination->getFrameAcquisitionDateTime(0));
        CPPUNIT_ASSERT_EQUAL(expected_1, *destination->getFrameAcquisitionDateTime(1));
        CPPUNIT_ASSERT(!destination->getFrameAcquisitionDateTime(2));
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

//------------------------------------------------------------------------------

void SeriesTest::privateTagTest()
{
    const std::string expected1 {UUID::generateUUID()};
    const std::string expected2 {UUID::generateUUID()};
    const std::string expected3 {UUID::generateUUID()};

    {
        auto series = data::ImageSeries::New();
        series->setPrivateValue(0x00, expected1);
        series->setPrivateValue(0x11, expected2);

        const auto& actual1 = series->getPrivateValue(0x0);
        CPPUNIT_ASSERT(actual1);
        CPPUNIT_ASSERT_EQUAL(expected1, *actual1);

        const auto& actual2 = series->getPrivateValue(0x11);
        CPPUNIT_ASSERT(actual2);
        CPPUNIT_ASSERT_EQUAL(expected2, *actual2);

        // test that setPrivateValue(0x00) == setPrivateValue(0x10)
        series->setPrivateValue(0x10, expected3);
        const auto& actual3 = series->getPrivateValue(0x0);
        CPPUNIT_ASSERT_EQUAL(expected3, *actual3);
        const auto& actual4 = series->getPrivateValue(0x10);
        CPPUNIT_ASSERT_EQUAL(expected3, *actual4);

        // test removing the tag
        series->setPrivateValue(0x00, std::nullopt);
        const auto& actual5 = series->getPrivateValue(0x00);
        CPPUNIT_ASSERT(!actual5.has_value());
    }
}

//------------------------------------------------------------------------------

void SeriesTest::copyPatientModuleTest()
{
    auto series1 = data::Series::New();
    series1->setPatientName("1");
    series1->setPatientID("2");
    series1->setPatientBirthDate("5");
    series1->setPatientSex("6");

    auto series2 = data::Series::New();
    series2->copyPatientModule(series1);
    CPPUNIT_ASSERT(*series1 == *series2);
}

//------------------------------------------------------------------------------

void SeriesTest::copyGeneralStudyModuleTest()
{
    auto series1 = data::Series::New();
    series1->setStudyInstanceUID("1");
    series1->setStudyDate("2");
    series1->setStudyTime("3");
    series1->setReferringPhysicianName("4");
    series1->setStudyID("6");
    series1->setStudyDescription("9");

    auto series2 = data::Series::New();
    series2->copyGeneralStudyModule(series1);
    CPPUNIT_ASSERT(*series1 == *series2);
}

//------------------------------------------------------------------------------

void SeriesTest::copyPatientStudyModuleTest()
{
    auto series1 = data::Series::New();
    series1->setPatientAge("3");
    series1->setPatientSize(4);
    series1->setPatientWeight(5);

    auto series2 = data::Series::New();
    series2->copyPatientStudyModule(series1);
    CPPUNIT_ASSERT(*series1 == *series2);
}

//------------------------------------------------------------------------------

void SeriesTest::copyGeneralSeriesModuleTest()
{
    auto series1 = data::Series::New();
    series1->setModality("1");
    series1->setSeriesInstanceUID("2");
    series1->setSeriesNumber(3);
    series1->setLaterality("4");
    series1->setSeriesDate("5");
    series1->setSeriesTime("6");
    series1->setPerformingPhysicianName("7");
    series1->setProtocolName("9");
    series1->setSeriesDescription("10");
    series1->setBodyPartExamined("16");
    series1->setPatientPosition("17");
    series1->setPerformedProcedureStepID("21");
    series1->setPerformedProcedureStepStartDate("22");
    series1->setPerformedProcedureStepStartTime("23");
    series1->setPerformedProcedureStepEndDate("24");
    series1->setPerformedProcedureStepEndTime("25");
    series1->setPerformedProcedureStepDescription("26");
    series1->setCommentsOnThePerformedProcedureStep("28");
    series1->setAnatomicalOrientationType("29");

    auto series2 = data::Series::New();
    series2->copyGeneralSeriesModule(series1);
    CPPUNIT_ASSERT(*series1 == *series2);
}

//------------------------------------------------------------------------------

void SeriesTest::copyGeneralEquipmentModuleTest()
{
    auto series1 = data::Series::New();
    series1->setInstitutionName("2");

    auto series2 = data::Series::New();
    series2->copyGeneralEquipmentModule(series1);
    CPPUNIT_ASSERT(*series1 == *series2);
}

//------------------------------------------------------------------------------

void SeriesTest::copySOPCommonModuleTest()
{
    auto series1 = data::Series::New();
    series1->setSOPKeyword(data::dicom::sop::Keyword::ComputedRadiographyImageStorage);
    series1->setSOPInstanceUID("2");
    series1->setSpecificCharacterSet("3");
    series1->setInstanceNumber(13);

    auto series2 = data::Series::New();
    series2->copySOPCommonModule(series1);
    CPPUNIT_ASSERT(*series1 == *series2);
}

//------------------------------------------------------------------------------

void SeriesTest::copyGeneralImageModuleTest()
{
    auto series1 = data::Series::New();
    series1->setInstanceNumber(1);
    series1->setContentTime("4");
    series1->setAcquisitionNumber(6);
    series1->setAcquisitionDate("7");
    series1->setAcquisitionTime("8");

    auto series2 = data::Series::New();
    series2->copyGeneralImageModule(series1);
    CPPUNIT_ASSERT(*series1 == *series2);
}

//------------------------------------------------------------------------------

void SeriesTest::getPatientPositionStringTest()
{
    static const std::map<std::string, std::string> shortToLong {
        {"HFP", "Head First-Prone"},
        {"HFS", "Head First-Supine"},
        {"HFDR", "Head First-Decubitus Right"},
        {"HFDL", "Head First-Decubitus Left"},
        {"FFDR", "Feet First-Decubitus Right"},
        {"FFDL", "Feet First-Decubitus Left"},
        {"FFP", "Feet First-Prone"},
        {"FFS", "Feet First-Supine"},
        {"LFP", "Left First-Prone"},
        {"LFS", "Left First-Supine"},
        {"RFP", "Right First-Prone"},
        {"RFS", "Right First-Supine"},
        {"AFDR", "Anterior First-Decubitus Right"},
        {"AFDL", "Anterior First-Decubitus Left"},
        {"PFDR", "Posterior First-Decubitus Right"},
        {"PFDL", "Posterior First-Decubitus Left"},
        {"NOT_A_POSITION", "NOT_A_POSITION"}
    };
    auto series = data::Series::New();
    for(auto&& [key, value] : shortToLong)
    {
        series->setPatientPosition(key);
        CPPUNIT_ASSERT_EQUAL_MESSAGE(key, value, series->getPatientPositionString());
    }
}

} //namespace sight::data::ut
