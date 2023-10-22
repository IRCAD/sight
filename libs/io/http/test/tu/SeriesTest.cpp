/************************************************************************
 *
 * Copyright (C) 2009-2023 IRCAD France
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

#include "SeriesTest.hpp"

#include <data/dicom_series.hpp>

#include <utest/exception.hpp>

#include <cppunit/Exception.h>

CPPUNIT_TEST_SUITE_REGISTRATION(sight::io::http::ut::SeriesTest);

namespace sight::io::http::ut
{

const QString seriesInstanceUID("1.2.250.317.4553866982239972067453446023029080307");
const QString seriesDate("20171028");
const QString seriesTime("180156.734");
const QString seriesDescription("OS 0.5");
const QString modality("CT");
const QString numberOfSeriesRelatedInstances(403);
const QString patientName("ANONYMIZED^ANONYMIZED");
const QString patientID("ANONYMIZED");
const QString patientBirthDate("19000101");
const QString patientSex("O");
const QString studyInstanceUID("1.2.826.0.1.3680043.2.1143.3387620735642715927946151392447602822");
const QString studyDate("19000101");
const QString studyTime("000000.000000");
const QString studyDescription("");
const QString patientAge("");
const QString institutionName("IRCAD IHU");

//------------------------------------------------------------------------------

void SeriesTest::setUp()
{
    // ==================================
    // Series
    // ==================================
    m_json.insert("SeriesInstanceUID", seriesInstanceUID);
    m_json.insert("SeriesDate", seriesDate);
    m_json.insert("SeriesTime", seriesTime);
    m_json.insert("SeriesDescription", seriesDescription);
    m_json.insert("Modality", modality);
    m_json.insert("NumberOfSeriesRelatedInstances", numberOfSeriesRelatedInstances);

    // ==================================
    // Patient
    // ==================================
    m_json.insert("PatientName", patientName);
    m_json.insert("PatientID", patientID);
    m_json.insert("PatientBirthDate", patientBirthDate);
    m_json.insert("PatientSex", patientSex);

    // ==================================
    // Study
    // ==================================
    m_json.insert("StudyInstanceUID", studyInstanceUID);
    m_json.insert("StudyDate", studyDate);
    m_json.insert("StudyTime", studyTime);
    m_json.insert("StudyDescription", studyDescription);
    m_json.insert("PatientAge", patientAge);

    // ==================================
    // Equipment
    // ==================================
    m_json.insert("InstitutionName", institutionName);
}

//------------------------------------------------------------------------------

void SeriesTest::tearDown()
{
}

//------------------------------------------------------------------------------

void SeriesTest::testSeries()
{
    io::http::helper::Series::DicomSeriesContainer series_vector =
        io::http::helper::Series::toFwMedData(m_json);
    CPPUNIT_ASSERT(series_vector.size() == 1);
    const auto& series = series_vector[0];
    CPPUNIT_ASSERT(series);

    data::dicom_series::sptr dicom_series = std::dynamic_pointer_cast<data::dicom_series>(series);
    CPPUNIT_ASSERT(dicom_series);
    CPPUNIT_ASSERT_EQUAL(dicom_series->getSeriesInstanceUID(), seriesInstanceUID.toStdString());
    CPPUNIT_ASSERT_EQUAL(dicom_series->getSeriesDate(), seriesDate.toStdString());
    CPPUNIT_ASSERT_EQUAL(dicom_series->getSeriesTime(), seriesTime.toStdString());
    CPPUNIT_ASSERT_EQUAL(dicom_series->getSeriesDescription(), seriesDescription.toStdString());
    CPPUNIT_ASSERT_EQUAL(dicom_series->getModality(), modality.toStdString());
    CPPUNIT_ASSERT_EQUAL(
        dicom_series->numInstances(),
        static_cast<std::size_t>(numberOfSeriesRelatedInstances.toULong())
    );

    CPPUNIT_ASSERT_EQUAL(series->getPatientName(), patientName.toStdString());
    CPPUNIT_ASSERT_EQUAL(series->getPatientID(), patientID.toStdString());
    CPPUNIT_ASSERT_EQUAL(series->getPatientBirthDate(), patientBirthDate.toStdString());
    CPPUNIT_ASSERT_EQUAL(series->getPatientSex(), patientSex.toStdString());

    CPPUNIT_ASSERT_EQUAL(series->getStudyInstanceUID(), studyInstanceUID.toStdString());
    CPPUNIT_ASSERT_EQUAL(series->getStudyDate(), studyDate.toStdString());
    CPPUNIT_ASSERT_EQUAL(series->getStudyTime(), studyTime.toStdString());
    CPPUNIT_ASSERT_EQUAL(series->getStudyDescription(), studyDescription.toStdString());
    CPPUNIT_ASSERT_EQUAL(series->getPatientAge(), patientAge.toStdString());

    CPPUNIT_ASSERT_EQUAL(series->getInstitutionName(), institutionName.toStdString());

    io::http::helper::Series::InstanceUIDContainer instances =
        io::http::helper::Series::toSeriesInstanceUIDContainer(series_vector);
    CPPUNIT_ASSERT(instances.size() == 1);
}

//------------------------------------------------------------------------------

} // namespace sight::io::http::ut
