/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2018.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "fwNetworkIO/helper/Series.hpp"

#include "fwNetworkIO/exceptions/TagMissing.hpp"

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
