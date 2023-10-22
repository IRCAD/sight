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

#include "io/http/helper/Series.hpp"

#include <core/spy_log.hpp>

#include <data/dicom_series.hpp>
#include <data/image_series.hpp>
#include <data/model_series.hpp>

namespace sight::io::http::helper
{

// ----------------------------------------------------------------------------

Series::DicomSeriesContainer Series::toFwMedData(const QJsonObject& _series_json)
{
    DicomSeriesContainer series_container;

    // Create series
    data::dicom_series::sptr series = std::make_shared<data::dicom_series>();

    // ==================================
    // Series
    // ==================================

    series->setSeriesInstanceUID(_series_json["SeriesInstanceUID"].toString().toStdString());
    series->setModality(_series_json["Modality"].toString().toStdString());
    series->setSeriesDate(_series_json["SeriesDate"].toString().toStdString());
    series->setSeriesTime(_series_json["SeriesTime"].toString().toStdString());
    series->setSeriesDescription(_series_json["SeriesDescription"].toString().toStdString());

    // ==================================
    // Patient
    // ==================================
    series->setPatientName(_series_json["PatientName"].toString().toStdString());
    series->setPatientID(_series_json["PatientID"].toString().toStdString());
    series->setPatientBirthDate(_series_json["PatientBirthDate"].toString().toStdString());
    series->setPatientSex(_series_json["PatientSex"].toString().toStdString());

    // ==================================
    // Study
    // ==================================
    series->setStudyInstanceUID(_series_json["StudyInstanceUID"].toString().toStdString());
    series->setStudyDate(_series_json["StudyDate"].toString().toStdString());
    series->setStudyTime(_series_json["StudyTime"].toString().toStdString());
    series->setStudyDescription(_series_json["StudyDescription"].toString().toStdString());
    series->setPatientAge(_series_json["PatientAge"].toString().toStdString());

    // ==================================
    // Equipment
    // ==================================
    series->setInstitutionName(_series_json["InstitutionName"].toString().toStdString());

    // ==================================
    // Number of instances
    // ==================================
    series->setNumberOfInstances(static_cast<std::size_t>(_series_json["NumberOfSeriesRelatedInstances"].toInt()));

    // Add series to container
    series_container.push_back(series);

    return series_container;
}

// ----------------------------------------------------------------------------

Series::InstanceUIDContainer Series::toSeriesInstanceUIDContainer(DicomSeriesContainer _series)
{
    InstanceUIDContainer result;

    for(const data::series::sptr& s : _series)
    {
        result.push_back(s->getSeriesInstanceUID());
    }

    return result;
}

} // namespace sight::io::http::helper
