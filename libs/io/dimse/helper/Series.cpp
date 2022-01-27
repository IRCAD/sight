/************************************************************************
 *
 * Copyright (C) 2009-2022 IRCAD France
 * Copyright (C) 2012-2020 IHU Strasbourg
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

#include "io/dimse/helper/Series.hpp"

#include "io/dimse/exceptions/TagMissing.hpp"

#include <core/spyLog.hpp>

#include <data/DicomSeries.hpp>
#include <data/Equipment.hpp>
#include <data/ImageSeries.hpp>
#include <data/ModelSeries.hpp>
#include <data/Patient.hpp>
#include <data/Study.hpp>

#include <boost/foreach.hpp>

namespace sight::io::dimse
{

namespace helper
{

//------------------------------------------------------------------------------

void Series::releaseResponses(OFList<QRResponse*> _responses)
{
    while(!_responses.empty())
    {
        delete _responses.front();
        _responses.pop_front();
    }
}

// ----------------------------------------------------------------------------

Series::DicomSeriesContainer Series::toFwMedData(OFList<QRResponse*> _responses)
{
    DicomSeriesContainer seriesContainer;

    OFListIterator(QRResponse*) it;
    OFCondition result;

    // Every while loop run will get all image for a specific study
    for(it = _responses.begin() ; it != _responses.end() ; ++it)
    {
        // Be sure we are not in the last response which does not have a dataset
        if((*it)->m_dataset != NULL)
        {
            OFString data;

            // Create series and get informations.
            sight::data::DicomSeries::sptr series  = sight::data::DicomSeries::New();
            sight::data::Patient::sptr patient     = series->getPatient();
            sight::data::Study::sptr study         = series->getStudy();
            sight::data::Equipment::sptr equipment = series->getEquipment();

            // Series
            result = (*it)->m_dataset->findAndGetOFStringArray(DCM_Modality, data);
            series->setModality(data.c_str());

            result = (*it)->m_dataset->findAndGetOFStringArray(DCM_SeriesInstanceUID, data);
            series->setInstanceUID(data.c_str());

            result = (*it)->m_dataset->findAndGetOFStringArray(DCM_SeriesNumber, data);
            series->setNumber(data.c_str());

            result = (*it)->m_dataset->findAndGetOFStringArray(DCM_Laterality, data);
            series->setLaterality(data.c_str());

            result = (*it)->m_dataset->findAndGetOFStringArray(DCM_SeriesDate, data);
            series->setDate(data.c_str());

            result = (*it)->m_dataset->findAndGetOFStringArray(DCM_SeriesTime, data);
            series->setTime(data.c_str());

            result = (*it)->m_dataset->findAndGetOFStringArray(DCM_PerformingPhysicianName, data);
            sight::data::DicomValuesType performingPhysiciansName;
            performingPhysiciansName.push_back(data.c_str());
            series->setPerformingPhysiciansName(performingPhysiciansName);

            result = (*it)->m_dataset->findAndGetOFStringArray(DCM_ProtocolName, data);
            series->setProtocolName(data.c_str());

            result = (*it)->m_dataset->findAndGetOFStringArray(DCM_SeriesDescription, data);
            series->setDescription(data.c_str());

            result = (*it)->m_dataset->findAndGetOFStringArray(DCM_BodyPartExamined, data);
            series->setBodyPartExamined(data.c_str());

            result = (*it)->m_dataset->findAndGetOFStringArray(DCM_PatientPosition, data);
            series->setPatientPosition(data.c_str());

            result = (*it)->m_dataset->findAndGetOFStringArray(DCM_AnatomicalOrientationType, data);
            series->setAnatomicalOrientationType(data.c_str());

            result = (*it)->m_dataset->findAndGetOFStringArray(DCM_PerformedProcedureStepID, data);
            series->setPerformedProcedureStepID(data.c_str());

            result = (*it)->m_dataset->findAndGetOFStringArray(DCM_PerformedProcedureStepStartDate, data);
            series->setPerformedProcedureStepStartDate(data.c_str());

            result = (*it)->m_dataset->findAndGetOFStringArray(DCM_PerformedProcedureStepStartTime, data);
            series->setPerformedProcedureStepStartTime(data.c_str());

            result = (*it)->m_dataset->findAndGetOFStringArray(DCM_PerformedProcedureStepEndDate, data);
            series->setPerformedProcedureStepEndDate(data.c_str());

            result = (*it)->m_dataset->findAndGetOFStringArray(DCM_PerformedProcedureStepEndTime, data);
            series->setPerformedProcedureStepEndTime(data.c_str());

            result = (*it)->m_dataset->findAndGetOFStringArray(DCM_PerformedProcedureStepDescription, data);
            series->setPerformedProcedureStepDescription(data.c_str());

            result = (*it)->m_dataset->findAndGetOFStringArray(DCM_CommentsOnThePerformedProcedureStep, data);
            series->setPerformedProcedureComments(data.c_str());

            // Study
            result = (*it)->m_dataset->findAndGetOFStringArray(DCM_StudyInstanceUID, data);
            study->setInstanceUID(data.c_str());

            result = (*it)->m_dataset->findAndGetOFStringArray(DCM_StudyID, data);
            study->setStudyID(data.c_str());

            result = (*it)->m_dataset->findAndGetOFStringArray(DCM_StudyDate, data);
            study->setDate(data.c_str());

            result = (*it)->m_dataset->findAndGetOFStringArray(DCM_StudyTime, data);
            study->setTime(data.c_str());

            result = (*it)->m_dataset->findAndGetOFStringArray(DCM_ReferringPhysicianName, data);
            study->setReferringPhysicianName(data.c_str());

            result = (*it)->m_dataset->findAndGetOFStringArray(DCM_ConsultingPhysicianName, data);
            study->setConsultingPhysicianName(data.c_str());

            result = (*it)->m_dataset->findAndGetOFStringArray(DCM_StudyDescription, data);
            study->setDescription(data.c_str());

            result = (*it)->m_dataset->findAndGetOFStringArray(DCM_PatientAge, data);
            study->setPatientAge(data.c_str());

            result = (*it)->m_dataset->findAndGetOFStringArray(DCM_PatientSize, data);
            study->setPatientSize(data.c_str());

            result = (*it)->m_dataset->findAndGetOFStringArray(DCM_PatientWeight, data);
            study->setPatientWeight(data.c_str());

            result = (*it)->m_dataset->findAndGetOFStringArray(DCM_PatientBodyMassIndex, data);
            study->setPatientBodyMassIndex(data.c_str());

            // Patient
            result = (*it)->m_dataset->findAndGetOFStringArray(DCM_PatientName, data);
            patient->setName(data.c_str());

            result = (*it)->m_dataset->findAndGetOFStringArray(DCM_PatientID, data);
            patient->setPatientId(data.c_str());

            result = (*it)->m_dataset->findAndGetOFStringArray(DCM_PatientBirthDate, data);
            patient->setBirthdate(data.c_str());

            result = (*it)->m_dataset->findAndGetOFStringArray(DCM_PatientSex, data);
            patient->setSex(data.c_str());

            // Equipment
            (*it)->m_dataset->findAndGetOFStringArray(DCM_InstitutionName, data);
            equipment->setInstitutionName(data.c_str());

            // Number of instances
            long int nb_instances;
            (*it)->m_dataset->findAndGetLongInt(DCM_NumberOfSeriesRelatedInstances, nb_instances);
            series->setNumberOfInstances(nb_instances);

            // Add series to container
            seriesContainer.push_back(series);
        }
    }

    return seriesContainer;
}

// ----------------------------------------------------------------------------

Series::InstanceUIDContainer Series::toSeriesInstanceUIDContainer(DicomSeriesContainer _series)
{
    InstanceUIDContainer result;

    for(const auto& s : _series)
    {
        result.push_back(s->getInstanceUID());
    }

    return result;
}

// ----------------------------------------------------------------------------

Series::InstanceUIDContainer Series::toSeriesInstanceUIDContainer(OFList<QRResponse*> _responses)
{
    InstanceUIDContainer instanceUIDContainer;

    OFListIterator(QRResponse*) it;
    DcmDataset dataset;
    OFCondition result;
    // Every while loop run will get all image for a specific study
    for(it = _responses.begin() ; it != _responses.end() ; ++it)
    {
        // Be sure we are not in the last response which does not have a dataset
        if((*it)->m_dataset != NULL)
        {
            OFString seriesInstanceUID;
            result = (*it)->m_dataset->findAndGetOFStringArray(DCM_SeriesInstanceUID, seriesInstanceUID);
            // Only try to get study if we actually have study instance uid, otherwise skip it
            if(result.good())
            {
                instanceUIDContainer.push_back(seriesInstanceUID.c_str());
            }
            else
            {
                const std::string msg = "There is no \"SeriersInstanceUID\" tag in the selected series :"
                                        + std::string(result.text());
                throw io::dimse::exceptions::TagMissing(msg);
            }
        }
    }

    return instanceUIDContainer;
}

} //helper

} //fwPacsIO
