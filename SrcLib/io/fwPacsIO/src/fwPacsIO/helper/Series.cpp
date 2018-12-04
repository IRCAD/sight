/************************************************************************
 *
 * Copyright (C) 2009-2016 IRCAD France
 * Copyright (C) 2012-2016 IHU Strasbourg
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

#include "fwPacsIO/helper/Series.hpp"
#include "fwPacsIO/exceptions/TagMissing.hpp"

#include <fwCore/spyLog.hpp>
#include <fwMedData/DicomSeries.hpp>
#include <fwMedData/Equipment.hpp>
#include <fwMedData/ImageSeries.hpp>
#include <fwMedData/ModelSeries.hpp>
#include <fwMedData/Patient.hpp>
#include <fwMedData/Study.hpp>

#include <boost/foreach.hpp>

namespace fwPacsIO
{
namespace helper
{

void Series::releaseResponses(OFList< QRResponse* > responses)
{
    while (!responses.empty())
    {
        delete responses.front();
        responses.pop_front();
    }
}

// ----------------------------------------------------------------------------

Series::DicomSeriesContainer Series::toFwMedData(OFList< QRResponse* > responses)
{
    DicomSeriesContainer seriesContainer;

    OFIterator< QRResponse* > it;
    OFCondition result;

    // Every while loop run will get all image for a specific study
    for (it = responses.begin(); it != responses.end(); ++it)
    {
        // Be sure we are not in the last response which does not have a dataset
        if ((*it)->m_dataset != NULL)
        {
            OFString data;

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

            result = (*it)->m_dataset->findAndGetOFStringArray(DCM_SeriesInstanceUID, data);
            series->setInstanceUID(data.c_str());

            result = (*it)->m_dataset->findAndGetOFStringArray(DCM_Modality, data);
            series->setModality(data.c_str());

            result = (*it)->m_dataset->findAndGetOFStringArray(DCM_SeriesDate, data);
            series->setDate(data.c_str());

            result = (*it)->m_dataset->findAndGetOFStringArray(DCM_SeriesTime, data);
            series->setTime(data.c_str());

            result = (*it)->m_dataset->findAndGetOFStringArray(DCM_SeriesDescription, data);
            series->setDescription(data.c_str());

            result = (*it)->m_dataset->findAndGetOFStringArray(DCM_PerformingPhysicianName, data);
            ::fwMedData::DicomValuesType performingPhysiciansName;
            performingPhysiciansName.push_back(data.c_str());
            series->setPerformingPhysiciansName(performingPhysiciansName);

            // ==================================
            // Patient
            // ==================================
            result = (*it)->m_dataset->findAndGetOFStringArray(DCM_PatientName, data);
            patient->setName(data.c_str());

            result = (*it)->m_dataset->findAndGetOFStringArray(DCM_PatientID, data);
            patient->setPatientId(data.c_str());

            result = (*it)->m_dataset->findAndGetOFStringArray(DCM_PatientBirthDate, data);
            patient->setBirthdate(data.c_str());

            result = (*it)->m_dataset->findAndGetOFStringArray(DCM_PatientSex, data);
            patient->setSex(data.c_str());


            // ==================================
            // Study
            // ==================================
            result = (*it)->m_dataset->findAndGetOFStringArray(DCM_StudyInstanceUID, data);
            study->setInstanceUID(data.c_str());

            result = (*it)->m_dataset->findAndGetOFStringArray(DCM_StudyDate, data);
            study->setDate(data.c_str());

            result = (*it)->m_dataset->findAndGetOFStringArray(DCM_StudyTime, data);
            study->setTime(data.c_str());

            result = (*it)->m_dataset->findAndGetOFStringArray(DCM_ReferringPhysicianName, data);
            study->setReferringPhysicianName(data.c_str());

            result = (*it)->m_dataset->findAndGetOFStringArray(DCM_StudyDescription, data);
            study->setDescription(data.c_str());

            result = (*it)->m_dataset->findAndGetOFStringArray(DCM_PatientAge, data);
            study->setPatientAge(data.c_str());

            // ==================================
            // Equipment
            // ==================================
            (*it)->m_dataset->findAndGetOFStringArray(DCM_InstitutionName, data);
            equipment->setInstitutionName(data.c_str());



            // ==================================
            // Number of instances
            // ==================================
            long int nbinstances;
            (*it)->m_dataset->findAndGetLongInt(DCM_NumberOfSeriesRelatedInstances, nbinstances);
            series->setNumberOfInstances(nbinstances);

            // Add series to container
            seriesContainer.push_back(series);

        }

    }

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

// ----------------------------------------------------------------------------

Series::InstanceUIDContainer Series::toSeriesInstanceUIDContainer(OFList< QRResponse* > responses)
{
    InstanceUIDContainer instanceUIDContainer;

    OFIterator< QRResponse* > it;
    DcmDataset dataset;
    OFCondition result;
    // Every while loop run will get all image for a specific study
    for (it = responses.begin(); it != responses.end(); ++it)
    {
        // Be sure we are not in the last response which does not have a dataset
        if ((*it)->m_dataset != NULL)
        {
            OFString seriesInstanceUID;
            result = (*it)->m_dataset->findAndGetOFStringArray(DCM_SeriesInstanceUID, seriesInstanceUID);
            // Only try to get study if we actually have study instance uid, otherwise skip it
            if (result.good())
            {
                instanceUIDContainer.push_back(seriesInstanceUID.c_str());
            }
            else
            {
                const std::string msg = "There is no \"SeriersInstanceUID\" tag in the selected series :"
                                        + std::string(result.text());
                throw ::fwPacsIO::exceptions::TagMissing(msg);
            }
        }
    }


    return instanceUIDContainer;
}

} //helper
} //fwPacsIO
