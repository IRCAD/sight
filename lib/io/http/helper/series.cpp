/************************************************************************
 *
 * Copyright (C) 2009-2025 IRCAD France
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

#include "series.hpp"

#include <data/image_series.hpp>
#include <data/model_series.hpp>

namespace sight::io::http::helper
{

// ----------------------------------------------------------------------------

series::dicom_series_container_t series::to_fw_med_data(const QJsonObject& _series_json)
{
    dicom_series_container_t series_container;

    // Create series
    data::series::sptr series = std::make_shared<data::series>();

    // ==================================
    // Series
    // ==================================

    series->set_sop_class_uid(_series_json["SOPClassUID"].toString().toStdString());
    series->set_series_instance_uid(_series_json["SeriesInstanceUID"].toString().toStdString());
    series->set_sop_instance_uid(_series_json["SOPInstanceUID"].toString().toStdString());
    series->set_modality(_series_json["Modality"].toString().toStdString());
    series->set_series_date(_series_json["SeriesDate"].toString().toStdString());
    series->set_series_time(_series_json["SeriesTime"].toString().toStdString());
    series->set_series_description(_series_json["SeriesDescription"].toString().toStdString());

    // ==================================
    // Patient
    // ==================================
    series->set_patient_name(_series_json["PatientName"].toString().toStdString());
    series->set_patient_id(_series_json["PatientID"].toString().toStdString());
    series->set_patient_birth_date(_series_json["PatientBirthDate"].toString().toStdString());
    series->set_patient_sex(_series_json["PatientSex"].toString().toStdString());

    // ==================================
    // Study
    // ==================================
    series->set_study_instance_uid(_series_json["StudyInstanceUID"].toString().toStdString());
    series->set_study_date(_series_json["StudyDate"].toString().toStdString());
    series->set_study_time(_series_json["StudyTime"].toString().toStdString());
    series->set_study_description(_series_json["StudyDescription"].toString().toStdString());
    series->set_patient_age(_series_json["PatientAge"].toString().toStdString());

    // ==================================
    // Equipment
    // ==================================
    series->set_institution_name(_series_json["InstitutionName"].toString().toStdString());

    // ==================================
    // Number of instances
    // ==================================

    const auto& num_instances_json  = _series_json["NumberOfSeriesRelatedInstances"];
    const std::size_t num_instances = num_instances_json.isString()
                                      ? num_instances_json.toString("0").toULongLong()
                                      : std::size_t(num_instances_json.toInt());

    // A bit abusive, but this is a temporary series until it is downloaded,
    // so we use this field to store the number of instances.
    series->set_instance_number(int(num_instances));

    // Add series to container
    series_container.push_back(series);

    return series_container;
}

// ----------------------------------------------------------------------------

series::instance_uid_container_t series::to_series_instance_uid_container(dicom_series_container_t _series)
{
    instance_uid_container_t result;

    for(const data::series::sptr& s : _series)
    {
        result.push_back(s->get_series_instance_uid());
    }

    return result;
}

} // namespace sight::io::http::helper
