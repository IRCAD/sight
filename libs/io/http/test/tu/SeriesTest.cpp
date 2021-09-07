/************************************************************************
 *
 * Copyright (C) 2009-2021 IRCAD France
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

#include <data/DicomSeries.hpp>
#include <data/Equipment.hpp>
#include <data/Patient.hpp>
#include <data/Study.hpp>

#include <utest/Exception.hpp>

#include <cppunit/Exception.h>

CPPUNIT_TEST_SUITE_REGISTRATION(::sight::io::http::ut::SeriesTest);

namespace sight::io::http
{

namespace ut
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
    io::http::helper::Series::DicomSeriesContainer seriesVector =
        io::http::helper::Series::toFwMedData(m_json);
    CPPUNIT_ASSERT(seriesVector.size() == 1);
    data::Series::sptr series = seriesVector[0];
    CPPUNIT_ASSERT(series);

    data::DicomSeries::sptr dicomSeries = data::DicomSeries::dynamicCast(series);
    CPPUNIT_ASSERT(dicomSeries);
    CPPUNIT_ASSERT_EQUAL(dicomSeries->getInstanceUID(), seriesInstanceUID.toStdString());
    CPPUNIT_ASSERT_EQUAL(dicomSeries->getDate(), seriesDate.toStdString());
    CPPUNIT_ASSERT_EQUAL(dicomSeries->getTime(), seriesTime.toStdString());
    CPPUNIT_ASSERT_EQUAL(dicomSeries->getDescription(), seriesDescription.toStdString());
    CPPUNIT_ASSERT_EQUAL(dicomSeries->getModality(), modality.toStdString());
    CPPUNIT_ASSERT_EQUAL(
        dicomSeries->getNumberOfInstances(),
        static_cast<size_t>(numberOfSeriesRelatedInstances.toULong())
    );

    data::Patient::sptr patient = series->getPatient();
    CPPUNIT_ASSERT(patient);
    CPPUNIT_ASSERT_EQUAL(patient->getName(), patientName.toStdString());
    CPPUNIT_ASSERT_EQUAL(patient->getPatientId(), patientID.toStdString());
    CPPUNIT_ASSERT_EQUAL(patient->getBirthdate(), patientBirthDate.toStdString());
    CPPUNIT_ASSERT_EQUAL(patient->getSex(), patientSex.toStdString());

    data::Study::sptr study = series->getStudy();
    CPPUNIT_ASSERT(study);
    CPPUNIT_ASSERT_EQUAL(study->getInstanceUID(), studyInstanceUID.toStdString());
    CPPUNIT_ASSERT_EQUAL(study->getDate(), studyDate.toStdString());
    CPPUNIT_ASSERT_EQUAL(study->getTime(), studyTime.toStdString());
    CPPUNIT_ASSERT_EQUAL(study->getDescription(), studyDescription.toStdString());
    CPPUNIT_ASSERT_EQUAL(study->getPatientAge(), patientAge.toStdString());

    data::Equipment::sptr equipment = series->getEquipment();
    CPPUNIT_ASSERT(equipment);
    CPPUNIT_ASSERT_EQUAL(equipment->getInstitutionName(), institutionName.toStdString());

    io::http::helper::Series::InstanceUIDContainer instances =
        io::http::helper::Series::toSeriesInstanceUIDContainer(seriesVector);
    CPPUNIT_ASSERT(instances.size() == 1);
}

//------------------------------------------------------------------------------

} // namespace ut

} // namespace sight::io::http
