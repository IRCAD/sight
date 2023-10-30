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

CPPUNIT_TEST_SUITE_REGISTRATION(sight::io::http::ut::series_test);

namespace sight::io::http::ut
{

const QString SERIES_INSTANCE_UID("1.2.250.317.4553866982239972067453446023029080307");
const QString SERIES_DATE("20171028");
const QString SERIES_TIME("180156.734");
const QString SERIES_DESCRIPTION("OS 0.5");
const QString MODALITY("CT");
const QString NUMBER_OF_SERIES_RELATED_INSTANCES(403);
const QString PATIENT_NAME("ANONYMIZED^ANONYMIZED");
const QString PATIENT_ID("ANONYMIZED");
const QString PATIENT_BIRTH_DATE("19000101");
const QString PATIENT_SEX("O");
const QString STUDY_INSTANCE_UID("1.2.826.0.1.3680043.2.1143.3387620735642715927946151392447602822");
const QString STUDY_DATE("19000101");
const QString STUDY_TIME("000000.000000");
const QString STUDY_DESCRIPTION("");
const QString PATIENT_AGE("");
const QString INSTITUTION_NAME("IRCAD IHU");

//------------------------------------------------------------------------------

void series_test::setUp()
{
    // ==================================
    // Series
    // ==================================
    m_json.insert("SeriesInstanceUID", SERIES_INSTANCE_UID);
    m_json.insert("SeriesDate", SERIES_DATE);
    m_json.insert("SeriesTime", SERIES_TIME);
    m_json.insert("SeriesDescription", SERIES_DESCRIPTION);
    m_json.insert("Modality", MODALITY);
    m_json.insert("NumberOfSeriesRelatedInstances", NUMBER_OF_SERIES_RELATED_INSTANCES);

    // ==================================
    // Patient
    // ==================================
    m_json.insert("PatientName", PATIENT_NAME);
    m_json.insert("PatientID", PATIENT_ID);
    m_json.insert("PatientBirthDate", PATIENT_BIRTH_DATE);
    m_json.insert("PatientSex", PATIENT_SEX);

    // ==================================
    // Study
    // ==================================
    m_json.insert("StudyInstanceUID", STUDY_INSTANCE_UID);
    m_json.insert("StudyDate", STUDY_DATE);
    m_json.insert("StudyTime", STUDY_TIME);
    m_json.insert("StudyDescription", STUDY_DESCRIPTION);
    m_json.insert("PatientAge", PATIENT_AGE);

    // ==================================
    // Equipment
    // ==================================
    m_json.insert("InstitutionName", INSTITUTION_NAME);
}

//------------------------------------------------------------------------------

void series_test::tearDown()
{
}

//------------------------------------------------------------------------------

void series_test::test_series()
{
    io::http::helper::series::DicomSeriesContainer series_vector =
        io::http::helper::series::to_fw_med_data(m_json);
    CPPUNIT_ASSERT(series_vector.size() == 1);
    const auto& series = series_vector[0];
    CPPUNIT_ASSERT(series);

    data::dicom_series::sptr dicom_series = std::dynamic_pointer_cast<data::dicom_series>(series);
    CPPUNIT_ASSERT(dicom_series);
    CPPUNIT_ASSERT_EQUAL(dicom_series->get_series_instance_uid(), SERIES_INSTANCE_UID.toStdString());
    CPPUNIT_ASSERT_EQUAL(dicom_series->get_series_date(), SERIES_DATE.toStdString());
    CPPUNIT_ASSERT_EQUAL(dicom_series->get_series_time(), SERIES_TIME.toStdString());
    CPPUNIT_ASSERT_EQUAL(dicom_series->get_series_description(), SERIES_DESCRIPTION.toStdString());
    CPPUNIT_ASSERT_EQUAL(dicom_series->get_modality(), MODALITY.toStdString());
    CPPUNIT_ASSERT_EQUAL(
        dicom_series->num_instances(),
        static_cast<std::size_t>(NUMBER_OF_SERIES_RELATED_INSTANCES.toULong())
    );

    CPPUNIT_ASSERT_EQUAL(series->get_patient_name(), PATIENT_NAME.toStdString());
    CPPUNIT_ASSERT_EQUAL(series->get_patient_id(), PATIENT_ID.toStdString());
    CPPUNIT_ASSERT_EQUAL(series->get_patient_birth_date(), PATIENT_BIRTH_DATE.toStdString());
    CPPUNIT_ASSERT_EQUAL(series->get_patient_sex(), PATIENT_SEX.toStdString());

    CPPUNIT_ASSERT_EQUAL(series->get_study_instance_uid(), STUDY_INSTANCE_UID.toStdString());
    CPPUNIT_ASSERT_EQUAL(series->get_study_date(), STUDY_DATE.toStdString());
    CPPUNIT_ASSERT_EQUAL(series->get_study_time(), STUDY_TIME.toStdString());
    CPPUNIT_ASSERT_EQUAL(series->get_study_description(), STUDY_DESCRIPTION.toStdString());
    CPPUNIT_ASSERT_EQUAL(series->get_patient_age(), PATIENT_AGE.toStdString());

    CPPUNIT_ASSERT_EQUAL(series->get_institution_name(), INSTITUTION_NAME.toStdString());

    io::http::helper::series::InstanceUIDContainer instances =
        io::http::helper::series::to_series_instance_uid_container(series_vector);
    CPPUNIT_ASSERT(instances.size() == 1);
}

//------------------------------------------------------------------------------

} // namespace sight::io::http::ut
