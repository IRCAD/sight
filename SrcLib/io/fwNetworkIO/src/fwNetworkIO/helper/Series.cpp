/************************************************************************
 *
 * Copyright (C) 2009-2018 IRCAD France
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

#include "fwNetworkIO/helper/Series.hpp"

#include <fwCore/spyLog.hpp>

#include <fwMedData/DicomSeries.hpp>
#include <fwMedData/Equipment.hpp>
#include <fwMedData/ImageSeries.hpp>
#include <fwMedData/ModelSeries.hpp>
#include <fwMedData/Patient.hpp>
#include <fwMedData/Study.hpp>

#include <boost/foreach.hpp>

namespace fwNetworkIO
{
namespace helper
{

// ----------------------------------------------------------------------------

Series::DicomSeriesContainer Series::toFwMedData(const QJsonObject& seriesJson)
{
    DicomSeriesContainer seriesContainer;

    // Create series
    ::fwMedData::DicomSeries::sptr series  = ::fwMedData::DicomSeries::New();
    ::fwMedData::Patient::sptr patient     = ::fwMedData::Patient::New();
    ::fwMedData::Study::sptr study         = ::fwMedData::Study::New();
    ::fwMedData::Equipment::sptr equipment = ::fwMedData::Equipment::New();

    // Set informations to series
    series->setPatient(patient);
    series->setStudy(study);
    series->setEquipment(equipment);

    // ==================================
    // Series
    // ==================================

    series->setInstanceUID(seriesJson["SeriesInstanceUID"].toString().toStdString());
    series->setModality(seriesJson["Modality"].toString().toStdString());
    series->setDate(seriesJson["SeriesDate"].toString().toStdString());
    series->setTime(seriesJson["SeriesTime"].toString().toStdString());
    series->setDescription(seriesJson["SeriesDescription"].toString().toStdString());

    // ==================================
    // Patient
    // ==================================
    patient->setName(seriesJson["PatientName"].toString().toStdString());
    patient->setPatientId(seriesJson["PatientID"].toString().toStdString());
    patient->setBirthdate(seriesJson["PatientBirthDate"].toString().toStdString());
    patient->setSex(seriesJson["PatientSex"].toString().toStdString());

    // ==================================
    // Study
    // ==================================
    study->setInstanceUID(seriesJson["StudyInstanceUID"].toString().toStdString());
    study->setDate(seriesJson["StudyDate"].toString().toStdString());
    study->setTime(seriesJson["StudyTime"].toString().toStdString());
    study->setDescription(seriesJson["StudyDescription"].toString().toStdString());
    study->setPatientAge(seriesJson["PatientAge"].toString().toStdString());

    // ==================================
    // Equipment
    // ==================================
    equipment->setInstitutionName(seriesJson["InstitutionName"].toString().toStdString());

    // ==================================
    // Number of instances
    // ==================================
    series->setNumberOfInstances(seriesJson["NumberOfSeriesRelatedInstances"].toInt());

    // Add series to container
    seriesContainer.push_back(series);

    return seriesContainer;
}

// ----------------------------------------------------------------------------

Series::InstanceUIDContainer Series::toSeriesInstanceUIDContainer(DicomSeriesContainer series)
{
    InstanceUIDContainer result;

    for(const ::fwMedData::Series::sptr& s: series )
    {
        result.push_back(s->getInstanceUID());
    }

    return result;
}

} //helper
} //fwNetworkIO
